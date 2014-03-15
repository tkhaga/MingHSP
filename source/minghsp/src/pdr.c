/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2006 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "blocks/shape.h"
#include "blocks/matrix.h"
#include "blocks/block.h"
#include "blocks/character.h"
#include "blocks/input.h"

#include "pdr.h"

#if !defined(SWFFILL_LINEAR_GRADIENT) || !defined(SWFFILL_RADIAL_GRADIENT)
#define MING_02A
#endif

#ifdef MING_02A
#define SWFFILL_LINEAR_GRADIENT 0x10
#define SWFFILL_RADIAL_GRADIENT 0x12
extern float Ming_scale;
void SWFShape_hideLine(SWFShape shape);
#else
float Ming_getScale();
#endif

void SWFShape_newStyles(SWFShape shape);
void SWFShape_setWhole(SWFShape shape);

#define LINEMAX_INC     256
#define LINEMAX_DEFAULT 255

typedef struct {
	int flgAnchor;
	int x, y;
} ANCHOR;

typedef struct {
	int PositionRatio;
	int Color;
} GRADENTRY;

typedef struct {
	byte flgClosePath;
	int LineWidth, LineMode, LineColor, FillMode, FillColor;
	int GradCenterX, GradCenterY, GradAngle, GradColor;
	double GradRatioX, GradRatioY;
	byte flgCompounded;
	int CompoID;
	int NumAnc;
	int numentry;
	byte processed;
	ANCHOR *anchor;
	GRADENTRY *grad;
} PATH;

typedef struct {
	int max;
	PATH *path;
} PATHLIST;

typedef struct {
	int compoid;	/* �����p�X��ID */
	int fillid;		/* �����p�X�Ɋ֘A�t������h����̂���p�X�ԍ� */
	int numpath;
	int *pathlist;	/* �����p�X�Ɋ֘A�t������p�X�ԍ��̔z�� */
} COMPO;

typedef struct {
	int max;
	COMPO *compo;
} COMPOLIST;

typedef struct {
	int NumPath;
	int ClipX, ClipY;
	int flgmorph;
	int ispdclip;
	int linemax;
	int size[2];	/* PdrReadHeader���ł͒P�ʂ̓s�N�Z���APdrWriteShape���ł�Twip */
	int version;
	char *linebuf;
#ifdef PDR_USE_SWFINPUT
	SWFInput input;
#else
	void **src;
#endif
	SWFShape shape;	/* == NULL �Ȃ�T�C�Y�擾�̂� */
	PATHLIST pathlist;
	COMPOLIST compolist;
} PDR;

typedef int (* GETCHAR)(void **);
typedef void (* FREELINE)(char *, void *);

#ifndef PDR_USE_SWFINPUT
static GETCHAR GetChar;
static FREELINE FreeLine;

static int pdr_limit;	/* �ǂݍ��߂�c��̃o�C�g��   */
#endif
static int pdr_eol;		/* 0�łȂ��Ƃ��s��            */
static int pdr_error;	/* 0�łȂ��Ƃ��G���[������    */

static int FindCompo(COMPOLIST *list, int key)
{
	int i = 0;
	int max = list->max;
	COMPO *compo = list->compo;

	while (i <= max) {
		if (i == max) {
			i= -1;
			break;
		}
		if (key == compo[i].compoid) {
			break;
		}
		i++;
	}

	return i;
}

#ifndef PDR_USE_SWFINPUT
static void FreeLineBuf(char *linebuf, void *ptr)
{
	free(linebuf);
}

static void FreeLineFile(char *linebuf, void *ptr)
{
	fclose((FILE *)ptr);
	free(linebuf);
}
#endif

/* PDR�\���̂Ɋ֘A���郁�������̉���B�g���������肵�Ă���̂Ŋ�{�I�ɃG���[�`�F�b�N�����Ȃ� */
static void FreePdr(PDR *pdr)
{
	int i;
	int max;

#ifdef PDR_USE_SWFINPUT
	free(pdr->linebuf);
#else
	FreeLine(pdr->linebuf, *pdr->src);
#endif

	if (pdr->compolist.compo) {
		max = pdr->compolist.max;
		for (i = 0; i < max; i++) {
			free(pdr->compolist.compo[i].pathlist);
		}
		free(pdr->compolist.compo);
	}

	if (pdr->pathlist.path) {
		max = pdr->pathlist.max;
		for (i = 0; i < max; i++) {
			free(pdr->pathlist.path[i].anchor);
			free(pdr->pathlist.path[i].grad);	/* NULL�������͗L���ȃA�h���X */
		}
		free(pdr->pathlist.path);
	}
}

static void IncPtr(char **p)
{
	for (;;) {
		if (**p == ',') {
			(*p)++;
			break;
		}
		if (**p == '\r'  ||  **p == '\0') {
			pdr_eol = 1;
			break;
		}
		(*p)++;
	}
}

/* ��������0�łȂ���Ώȗ��\(�ȉ�����) */
static int GetNextValue(char **p)
{
	int val;

	if (pdr_eol) {
		pdr_error = 1;
		return 0;
	}

	val = atoi(*p);
	IncPtr(p);

	return val;
}

static byte GetNextFlgValue(char **p)
{
	byte val;

	if (pdr_eol) {
		pdr_error = 1;
		return 0;
	}

	if (!strncmp(*p, "#TRUE#", 6)) {
		val = 1;
	}
	else if (!strncmp(*p, "#FALSE#", 7)){
		val = 0;
	}
	else {
		pdr_error = 1;
		return 0;
	}

	IncPtr(p);

	return val;
}

static double GetNextFloatValue(char **p)
{
	double val;

	if (pdr_eol) {
		pdr_error = 1;
		return 0;
	}

	val = atof(*p);
	IncPtr(p);

	return val;
}

#ifndef PDR_USE_SWFINPUT
/* �ꕶ���ǂݍ��� */
static int GetCharFromBuf(void **ptr)
{
	if (pdr_limit-- <= 0) {
		return EOF;
	}

	return *(*(char **)ptr)++;
}

static int GetCharFromFile(void **ptr)
{
	return fgetc((FILE *)*ptr);
}
#endif

/* ��s�ǂݍ��� */
static int GetLine(PDR *pdr)
{
	char c;
	char *dest = pdr->linebuf;
	int i;
	int ret = 0;

	pdr_eol = 0;

	for (i = 0; ; i++) {
#ifdef PDR_USE_SWFINPUT
		c = SWFInput_getChar(pdr->input);
#else
		c = GetChar(pdr->src);
#endif
		if (c == EOF || c == '\0') {
			break;
		}
		if (c == '\r') {
			if (i == 0) {
				pdr_eol = 1;
			}
			ret = 1;
		}
		else {
			if (ret) {
				if (c == '\n') {
					/*
					*dest = c;
					dest++;
					*/
					break;
				}
				else {
					return 0;
				}
			} else if (c == '\n') {
				return 0;
			}
		}
		if (i == pdr->linemax) {
			int size = (dest - pdr->linebuf);
			pdr->linemax += LINEMAX_INC;
			pdr->linebuf = realloc(pdr->linebuf, pdr->linemax + 1);
			dest = pdr->linebuf + size;
		}
		*dest = c;
		dest++;
	}
	*dest = '\0';

	return i;
}

static int PdrReadHeader(PDR *pdr)
{
	int i;
	char *ptok;

	/* �w�b�_�̓ǂݍ��� */
	for (i = 0; i < 4; i++) {
		if (pdr_error != 0 || !GetLine(pdr)) {
			return -1;
		}
		if (i == 0) {
			/* PDR��PDCLIP���BPDCLIP�Ȃ�T�C�Y�擾�͕ʏ��� */
			ptok = pdr->linebuf;
			if (*ptok++ == 'P' && *ptok++ == 'D') {
				if (*ptok == 'R') {
					ptok++;
				}
				else if (*ptok++ == 'C' && *ptok++ == 'L' &&
					 *ptok++ == 'I' && *ptok++ == 'P')
				{
					pdr->ispdclip = 1;
				}
				else {
					return -1;
				}
				pdr->version = GetNextValue(&ptok);
			}
			else {
				return -1;
			}
		}
		if (i == 1) {
			ptok = pdr->linebuf;
			if (pdr->ispdclip != 0) {
				pdr->NumPath = GetNextValue(&ptok);
				pdr->ClipX = GetNextValue(&ptok);
				pdr->ClipY = GetNextValue(&ptok);
				if (pdr_error != 0) {
					return -1;
				}
				break;	/* PDCLIP�Ȃ�w�b�_�͓�s�Ȃ̂ł����Ń��[�v�𔲂��� */
			}
			else {
				pdr->size[0] = GetNextValue(&ptok);
				pdr->size[1] = GetNextValue(&ptok);
			}
		}
		if (i == 3) {
			ptok = pdr->linebuf;
			pdr->NumPath = GetNextValue(&ptok);
		}
	}

	return 0;
}

static int PdrReadData(PDR *pdr)
{
	int i, j;
	char *ptok;

	PATH *ppath;
	ANCHOR *panchor;
	GRADENTRY *pgrad;

	/* �����p�X�̏����Ǘ�����B�v�f���͍��XNumPath�� */
	pdr->compolist.max = 0;
	if ( !(pdr->compolist.compo = (COMPO *)calloc(pdr->NumPath, sizeof(COMPO))) ) {
		return -1;
	}

	pdr->pathlist.max = 0;
	if ( !(pdr->pathlist.path = (PATH *)calloc(pdr->NumPath, sizeof(PATH))) ) {
		return -1;
	}

	ppath = pdr->pathlist.path;

	/* �p�X���̓ǂݍ��� */
	for (i = 0; i < pdr->NumPath; i++) {

		pdr->pathlist.max++;

		if (pdr_error != 0  ||  !GetLine(pdr)) {
			return -1;
		}
		ptok = pdr->linebuf;

		ppath->NumAnc        = GetNextValue(&ptok);
		ppath->flgClosePath  = GetNextFlgValue(&ptok);
		ppath->LineWidth     = GetNextValue(&ptok);
		ppath->LineMode      = GetNextValue(&ptok);
		ppath->LineColor     = GetNextValue(&ptok);
		ppath->FillMode      = GetNextValue(&ptok);
		ppath->FillColor     = GetNextValue(&ptok);
		ppath->GradCenterX   = GetNextValue(&ptok);
		ppath->GradCenterY   = GetNextValue(&ptok);
		ppath->GradAngle     = GetNextValue(&ptok);
		ppath->GradRatioX    = GetNextFloatValue(&ptok);
		if (pdr->version >= 30) {
			ppath->GradRatioY = GetNextFloatValue(&ptok);
		}
		else {
			ppath->GradRatioY = ppath->GradRatioX;
		}
		ppath->GradColor     = GetNextValue(&ptok);
		GetNextFlgValue(&ptok);
		GetNextValue(&ptok);
		if (pdr->version >= 20) {
			ppath->flgCompounded = GetNextFlgValue(&ptok);
			ppath->CompoID       = GetNextValue(&ptok);
		}

		ppath->processed = 0;

		ppath->grad = NULL;	/* NULL�ɂȂ��Ă���ۏ؂͂Ȃ��̂ňꉞ */

		/* �O���f�[�V�����u���b�N�̓ǂݍ���(PDR Ver0.3�ȏ�̂�) */
		if (pdr->version >= 30) {
			if (pdr_error != 0  ||  !GetLine(pdr)) {
				return -1;
			}
			ptok = pdr->linebuf;
			ppath->numentry = GetNextValue(&ptok);
			if (ppath->numentry > 0) {
				pgrad = ppath->grad = (GRADENTRY *)calloc(ppath->numentry, sizeof(GRADENTRY));
				if (!pgrad) {
					return -1;
				}
				for (j = 0; j < ppath->numentry; j++) {
					if (pdr_error != 0  ||  !GetLine(pdr)) {
						return -1;
					}
					ptok = pdr->linebuf;
					pgrad->PositionRatio = GetNextValue(&ptok);
					pgrad->Color         = GetNextValue(&ptok);
					pgrad++;
				}
			}
		}
		
		/* �A���J�[�f�[�^�̓ǂݍ��� */
		panchor = ppath->anchor = (ANCHOR *)calloc(ppath->NumAnc + 3, sizeof(ANCHOR));
		if (!panchor) {
			return -1;
		}

		for (j = 0; j <= ppath->NumAnc; j++) {
			panchor++;
			if (pdr_error != 0 || !GetLine(pdr)) {
				return -1;
			}
			ptok = pdr->linebuf;
			panchor->flgAnchor = GetNextValue(&ptok);
			panchor->x = GetNextValue(&ptok);
			panchor->y = GetNextValue(&ptok);
		}
		if (ppath->flgClosePath == 0) {
			/* �p�X�����Ă��Ȃ���Η��[�̓I���J�[�u�A���J�[ */
			ppath->anchor[1].flgAnchor = 0;
			ppath->anchor[ppath->NumAnc + 1].flgAnchor = 0;
			ppath->FillMode = 0;	/* �p�X�����Ă��Ȃ���Γh��͂Ȃ�(Flash�̎d�l�ł͂����Ă��ǂ�) */
		}
		ppath->anchor[ppath->NumAnc + 2] = ppath->anchor[1];
		ppath->anchor[0]                 = ppath->anchor[ppath->NumAnc + 1];

		/* �����p�X�̃��X�g���쐬 */
		if (ppath->flgCompounded != 0) {
			int index;

			index = FindCompo(&pdr->compolist, ppath->CompoID);
			if (index < 0) {
				index = pdr->compolist.max;
				/* pdr->compolist.compo[compolist.max]���V���������p�X��� */
				pdr->compolist.compo[index].compoid     = ppath->CompoID;
				pdr->compolist.compo[index].numpath     = 1;
				pdr->compolist.compo[index].fillid      = -1;
				pdr->compolist.compo[index].pathlist    = (int *)calloc(pdr->NumPath, sizeof(int));
				pdr->compolist.compo[index].pathlist[0] = i;
				pdr->compolist.max++;
				if (!pdr->compolist.compo[index].pathlist) {
					return -1;
				}
			}
			else {
				pdr->compolist.compo[index].pathlist[pdr->compolist.compo[index].numpath] = i;
				pdr->compolist.compo[index].numpath++;
			}
			/* ���������p�XID�ň�Ԏ�O�ɂ���h�肪�A���������p�X���œh��̂�����̂̓h��ɂȂ� */
			if (pdr->compolist.compo[index].fillid == -1  &&  ppath->FillMode != 0) {
				pdr->compolist.compo[index].fillid = i;
			}
		}

		ppath++;
	}

	return 0;
}

static int PdrWriteShape(PDR *pdr)
{
	int i, j;

	int flgClosePath, LineWidth, LineMode, LineColor, FillMode, FillColor;
	int GradCenterX, GradCenterY, GradAngle, GradColor;
	double GradRatioX, GradRatioY;
	int flgCompounded, CompoID;
	int NumAnc;
	int compo_pathid = -1, compo_index = 0;

	SWFGradient gradient;
	SWFFillStyle fill;
	SWFFillStyle compofill = NULL;

	PATH *ppath;
	ANCHOR *anchor;
	GRADENTRY *grad;

	/* SWFShape�I�u�W�F�N�g�폜���Ɋ֘A�I�u�W�F�N�g���폜���邱�Ƃ��w�� */
	SWFShape_setWhole(pdr->shape);

	/* �p�X�����Z�b�g */
	for (i = 0; i < pdr->NumPath; i++) {
		ppath = &pdr->pathlist.path[i];

		/* �����ς݂̃p�X�̓X�L�b�v(�����p�X�΍�) */
		if (ppath->processed != 0) {
			continue;
		}

		NumAnc        = ppath->NumAnc;
		flgClosePath  = ppath->flgClosePath;
		LineWidth     = ppath->LineWidth;
		LineMode      = ppath->LineMode;
		LineColor     = ppath->LineColor;
		FillMode      = ppath->FillMode;
		FillColor     = ppath->FillColor;
		GradCenterX   = ppath->GradCenterX;
		GradCenterY   = ppath->GradCenterY;
		GradAngle     = ppath->GradAngle;
		GradRatioX    = ppath->GradRatioX;
		GradRatioY    = ppath->GradRatioY;
		GradColor     = ppath->GradColor;
		flgCompounded = ppath->flgCompounded;
		CompoID       = ppath->CompoID;

		anchor = ppath->anchor;

		/* �����p�X�̓r���łȂ����StateNewStyles�𗧂Ă�(�A�����Ă���V����ShapeRecord�͍���Ȃ�) */
		if (pdr->shape  &&  pdr->flgmorph == 0  &&  compo_index == 0) {
			SWFShape_newStyles(pdr->shape);
		}

		if (flgCompounded != 0) {
			int index;

			index = FindCompo(&pdr->compolist, CompoID);
			if (index < 0) {
				return -1;
			}
			if (pdr->compolist.compo[index].fillid != i  &&  FillMode != 0) {
				FillMode = -1;
			}
			if (compo_index == 0) {
				compo_pathid = i;
			}
			if (compo_index == pdr->compolist.compo[index].numpath - 1) {
				i = compo_pathid;
				compo_pathid = -1;
				compo_index = 0;
			}
			else {
				i = pdr->compolist.compo[index].pathlist[++compo_index] - 1;	/* ���̕����p�X */
			}
		}

		if (pdr->shape) {
			/* �h���ݒ� */
			if (FillMode > 0) {
				if (FillMode == 1) {	/* �x�^ */
					fill = SWFShape_addSolidFillStyle(
							pdr->shape,
							(byte)( FillColor        & 0xff),
							(byte)((FillColor >> 8 ) & 0xff),
							(byte)((FillColor >> 16) & 0xff),
							(byte)((FillColor >> 24) & 0xff)
						);
				}
				else {	/* �O���f�[�V���� */
					byte flags = 0;
					double rot1, rot2;
					rot1 = cos(GradAngle * M_PI / 180);
					rot2 = sin(GradAngle * M_PI / 180);

					gradient = newSWFGradient();
					SWFGradient_addEntry(
						gradient,
						0,
						(byte)( FillColor        & 0xff),
						(byte)((FillColor >> 8 ) & 0xff),
						(byte)((FillColor >> 16) & 0xff),
						(byte)((FillColor >> 24) & 0xff)
					);
					if (FillMode == 3) {	/* ���`�O���f2 */
						flags = SWFFILL_LINEAR_GRADIENT;
						grad = ppath->grad;
						for (j = 0; j < ppath->numentry; j++) {
							SWFGradient_addEntry(
								gradient,
								(float)(grad->PositionRatio / 2) / 255,
								(byte)( grad->Color        & 0xff),
								(byte)((grad->Color >> 8 ) & 0xff),
								(byte)((grad->Color >> 16) & 0xff),
								(byte)((grad->Color >> 24) & 0xff)
							);
							grad++;
						}
						SWFGradient_addEntry(
							gradient,
							(float)128 / 255,
							(byte)( GradColor        & 0xff),
							(byte)((GradColor >> 8 ) & 0xff),
							(byte)((GradColor >> 16) & 0xff),
							(byte)((GradColor >> 24) & 0xff)
						);
						grad--;
						for (j = 0; j < ppath->numentry; j++) {
							SWFGradient_addEntry(
								gradient,
								(float)(255 - grad->PositionRatio / 2) / 255,
								(byte)( grad->Color        & 0xff),
								(byte)((grad->Color >> 8 ) & 0xff),
								(byte)((grad->Color >> 16) & 0xff),
								(byte)((grad->Color >> 24) & 0xff)
							);
							grad--;
						}
						SWFGradient_addEntry(
							gradient,
							1.0,
							(byte)( FillColor        & 0xff),
							(byte)((FillColor >> 8 ) & 0xff),
							(byte)((FillColor >> 16) & 0xff),
							(byte)((FillColor >> 24) & 0xff)
						);
					}
					else {
						if (FillMode == 2) {	/* ���`�O���f1 */
							flags = SWFFILL_LINEAR_GRADIENT;
						}
						else if (FillMode == 4) {	/* �~�`�O���f */
							flags = SWFFILL_RADIAL_GRADIENT;
						}
						grad = ppath->grad;
						for (j = 0; j < ppath->numentry; j++) {
							SWFGradient_addEntry(
								gradient,
								(float)grad->PositionRatio / 255,
								(byte)( grad->Color        & 0xff),
								(byte)((grad->Color >> 8 ) & 0xff),
								(byte)((grad->Color >> 16) & 0xff),
								(byte)((grad->Color >> 24) & 0xff)
							);
							grad++;
						}
						SWFGradient_addEntry(
							gradient,
							1.0,
							(byte)( GradColor        & 0xff),
							(byte)((GradColor >> 8 ) & 0xff),
							(byte)((GradColor >> 16) & 0xff),
							(byte)((GradColor >> 24) & 0xff)
						);
					}
					fill = SWFShape_addGradientFillStyle(pdr->shape, gradient, flags);
					/* �ϊ��s��̐ݒ� */
					SWFMatrix_set(
						SWFFillStyle_getMatrix(fill),
						GradRatioX * rot1,
						GradRatioX * rot2,
						GradRatioY * -rot2,
						GradRatioY * rot1,
						GradCenterX - pdr->ClipX,
						GradCenterY - pdr->ClipY
					);
				}
				compofill = fill;
			}
			else if (FillMode < 0) {
				fill = compofill;
			}
			else if (FillMode == 0) {
				fill = NULL;
			}
			SWFShape_setLeftFillStyle(pdr->shape, fill);

			/* ���̐F��ݒ�(���Ă��Ȃ����͔�\���t���O�������Ă��Ă��\������d�l�炵��) */
			if (LineMode != 0 || flgClosePath == 0) {
				SWFShape_setLineStyle(
					pdr->shape,
					LineWidth,
					(byte)( LineColor        & 0xff),
					(byte)((LineColor >> 8 ) & 0xff),
					(byte)((LineColor >> 16) & 0xff),
					(byte)((LineColor >> 24) & 0xff)
				);
			}
			else {
				SWFShape_hideLine(pdr->shape);
			}
		}

		/* �A���J�[�����Z�b�g */
		for (j = 0; j <= NumAnc + 1; j++) {
			int k = j + 1;
			int flgAnchor1 = anchor[j].flgAnchor;
			int flgAnchor2 = anchor[k].flgAnchor;
			int x1 = anchor[j].x, y1 = anchor[j].y;
			int x2 = anchor[k].x, y2 = anchor[k].y;
			int dx, dy;

			if (j == 0) {
				if (flgAnchor2 != 0) {
					if (flgAnchor1 == 0) {
						dx = x1;
						dy = y1;
					}
					else {
						dx = (x1 + x2 + 1) / 2;
						dy = (y1 + y2 + 1) / 2;
					}
				}
				else {
					dx = x2;
					dy = y2;
				}

				if (pdr->shape) {
					SWFShape_moveScaledPenTo(pdr->shape, dx - pdr->ClipX, dy - pdr->ClipY);
					/*printf("moveTo(%d, %d)\n", dx, dy);*/
				}
				else {
					pdr->size[0] = max(dx - pdr->ClipX, pdr->size[0]);
					pdr->size[1] = max(dy - pdr->ClipY, pdr->size[1]);
				}
			}
			else if (j != NumAnc + 1  ||  flgClosePath != 0) {
				if (flgAnchor1 == 0) {
					if (flgAnchor2 == 0) {
						dx = x2;
						dy = y2;
					}
					else {
						dx = x1;
						dy = y1;
					}

					if (pdr->shape) {
						SWFShape_drawScaledLineTo(pdr->shape, dx - pdr->ClipX, dy - pdr->ClipY);
						/*printf("lineTo(%d, %d)\n", dx, dy);*/
					}
					else {
						pdr->size[0] = max(dx - pdr->ClipX + LineWidth, pdr->size[0]);
						pdr->size[1] = max(dy - pdr->ClipY + LineWidth, pdr->size[1]);
					}
				}
				else {
					if (flgAnchor2 == 0) {
						dx = x2;
						dy = y2;
					}
					else {
						dx = (x1 + x2 + 1) / 2;
						dy = (y1 + y2 + 1) / 2;
					}

					if (pdr->shape) {
						SWFShape_drawScaledCurveTo(pdr->shape, x1 - pdr->ClipX, y1 - pdr->ClipY, dx - pdr->ClipX, dy - pdr->ClipY);
						/*printf("curveTo(%d, %d, %d, %d)\n", x1, y1, dx, dy);*/
					}
					else {
						pdr->size[0] = max(x1 - pdr->ClipX + LineWidth, max(dx - pdr->ClipX + LineWidth, pdr->size[0]));
						pdr->size[1] = max(y1 - pdr->ClipY + LineWidth, max(dy - pdr->ClipY + LineWidth, pdr->size[1]));
					}
				}
			}
		}

		ppath->processed = 1;	/* �����ς� */
	}

	return 0;
}

#ifdef PDR_USE_SWFINPUT
int SWFShape_loadPdr(SWFShape shape, int size[], SWFInput input, int flag, float orgX, float orgY)
#else
int SWFShape_loadPdr(SWFShape shape, int size[], char *filename, int flag, int pdrsize, float orgX, float orgY)
#endif
{
#ifdef MING_02A
	float scale = Ming_scale;
#else
	float scale = Ming_getScale();
#endif
	PDR pdr;

	pdr_eol   = 0;
	pdr_error = 0;

	memset(&pdr, 0, sizeof(PDR));
	pdr.flgmorph = flag & PDR_MORPH;
	pdr.linemax  = LINEMAX_DEFAULT;
	pdr.shape    = shape;

#ifdef PDR_USE_SWFINPUT
	pdr.input = input;
#else
	if (pdrsize) {
		pdr.src = (void *)&filename;
		GetChar = GetCharFromBuf;
		FreeLine = FreeLineBuf;
		pdr_limit = pdrsize;
	}
	else {
		FILE *fp;
		fp = fopen(filename, "rb");
		if (!fp) {
			return -1;
		}
		pdr.src = (void *)&fp;
		GetChar = GetCharFromFile;
		FreeLine = FreeLineFile;
	}
#endif

	pdr.linebuf = malloc(pdr.linemax + 1);
	if (!pdr.linebuf) {
		return -1;
	}
	pdr.linebuf[pdr.linemax] = '\0';	/* IncPtr�֐����K���I������悤�ɂ��� */

	if (PdrReadHeader(&pdr) != 0) {
		FreePdr(&pdr);
		return -1;
	}

	size[0] = pdr.size[0];
	size[1] = pdr.size[1];

	if (flag & PDR_GETSIZEONLY) {
		if (pdr.ispdclip != 0) {
			pdr.shape = NULL;
		}
		else {
			FreePdr(&pdr);
			return 0;
		}
	}

	if (flag & PDR_ASCLIP) {
		pdr.ClipX = 0;
		pdr.ClipY = 0;
	}

	pdr.ClipX += orgX * scale;
	pdr.ClipY += orgY * scale;

	if (PdrReadData(&pdr) != 0  ||  PdrWriteShape(&pdr) != 0) {
		FreePdr(&pdr);
		return -1;
	}

	FreePdr(&pdr);

	if (pdr.ispdclip != 0) {
		if (pdr.shape) {
			size[0] = SWFCharacter_getScaledWidth(CHARACTER(pdr.shape))  / scale;
			size[1] = SWFCharacter_getScaledHeight(CHARACTER(pdr.shape)) / scale;
		}
		else {
			size[0] = pdr.size[0] / scale;
			size[1] = pdr.size[1] / scale;
		}
	}

	return 0;
}
