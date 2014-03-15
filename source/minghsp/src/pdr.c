/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2006 THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Mingの低レベルAPIを用いることで誤差を小さくできる */
#include "blocks/shape.h"
#include "blocks/matrix.h"
#include "blocks/block.h"
#include "blocks/character.h"

#include "pdr.h"

static int pdrlimit;	/* 読み込める残りのバイト数   */
static int pdreol;		/* 0でないとき行末            */
static int pdrerror;	/* 0でないときエラーが存在    */

static GETCHAR GetChar;
static FREELINE FreeLine;

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

static void FreeLineBuf(char *linebuf, void *ptr)
{
	free(linebuf);
}

static void FreeLineFile(char *linebuf, void *ptr)
{
	fclose((FILE *)ptr);
	free(linebuf);
}

/* PDR構造体に関連するメモリ等の解放。使い方を限定しているので基本的にエラーチェックをしない */
static void FreePdr(PDR *pdr)
{
	int i;
	int max;

	FreeLine(pdr->linebuf, *pdr->src);

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
		if (**p == '\r' || **p == '\0') {
			pdreol = 1;
			break;
		}
		(*p)++;
	}
}

/* 第二引数が0でなければ省略可能(以下同じ) */
static int GetNextValue(char **p, int skip)
{
	int val;

	if (pdreol) {
		if (skip == 0) {
			pdrerror = 1;
		}
		return 0;
	}

	val = atoi(*p);
	IncPtr(p);

	return val;
}

static byte GetNextFlgValue(char **p, int skip)
{
	byte val;

	if (pdreol) {
		if (skip == 0) {
			pdrerror = 1;
		}
		return 0;
	}

	if (!strncmp(*p, "#TRUE#", 6)) {
		val = 1;
	}
	else if (!strncmp(*p, "#FALSE#", 7)){
		val = 0;
	}
	else {
		pdrerror = 1;
		return 0;
	}

	IncPtr(p);

	return val;
}

static double GetNextFloatValue(char **p, int skip)
{
	double val;

	if (pdreol) {
		if (skip == 0) {
			pdrerror = 1;
		}
		return 0;
	}

	val = atof(*p);
	IncPtr(p);

	return val;
}

/* 一文字読み込み */
static int GetCharFromBuf(void **ptr)
{
	if (pdrlimit-- <= 0) {
		return EOF;
	}

	return *(*(char **)ptr)++;
}

static int GetCharFromFile(void **ptr)
{
	return fgetc((FILE *)*ptr);
}

/* 一行読み込み */
static int GetLine(PDR *pdr)
{
	char c;
	char *dest = pdr->linebuf;
	int i;
	int ret = 0;

	pdreol = 0;

	for (i = 0; ; i++) {
		c = GetChar(pdr->src);
		if (c == EOF || c == '\0') {
			break;
		}
		if (c == '\r') {
			if (i == 0) {
				pdreol = 1;
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

	/* ヘッダの読み込み */
	for (i = 0; i < 4; i++) {
		if (pdrerror != 0 || !GetLine(pdr)) {
			return -1;
		}
		if (i == 0) {
			/* PDRかPDCLIPか。PDCLIPならサイズ取得は別処理 */
			if (pdr->linebuf[0] == 'P' && pdr->linebuf[1] == 'D') {
				if (pdr->linebuf[2] == 'R') {
				}
				else if (pdr->linebuf[2] == 'C' && pdr->linebuf[3] == 'L' &&
					 pdr->linebuf[4] == 'I' && pdr->linebuf[5] == 'P')
				{
					pdr->ispdclip = 1;
				}
				else {
					return -1;
				}
			}
			else {
				return -1;
			}
		}
		if (i == 1) {
			char *ptok = pdr->linebuf;
			if (pdr->ispdclip != 0) {
				pdr->NumPath = GetNextValue(&ptok, 0);
				pdr->ClipX = GetNextValue(&ptok, 0);
				pdr->ClipY = GetNextValue(&ptok, 0);
				if (pdrerror != 0) {
					return -1;
				}
				break;	/* PDCLIPならヘッダは二行なのでここでループを抜ける */
			}
			else {
				pdr->rect[0] = GetNextValue(&ptok, 0);
				pdr->rect[1] = GetNextValue(&ptok, 0);
			}
		}
		if (i == 3) {
			char *ptok = pdr->linebuf;
			pdr->NumPath = GetNextValue(&ptok, 0);
		}
	}

	return 0;
}

static int PdrReadData(PDR *pdr)
{
	int i, j;

	PATH *ppath = pdr->pathlist.path;

	/* パス情報の読み込み */
	for (i = 0; i < pdr->NumPath; i++) {
		char *ptok;
		ANCHOR *panchor;

		pdr->pathlist.max++;

		if (pdrerror != 0 || !GetLine(pdr)) {
			FreePdr(pdr);
			return -1;
		}
		ptok = pdr->linebuf;

		ppath->NumAnc        = GetNextValue(&ptok, 0);
		ppath->flgClosePath  = GetNextFlgValue(&ptok, 0);
		ppath->LineWidth     = GetNextValue(&ptok, 0);
		ppath->LineMode      = GetNextValue(&ptok, 0);
		ppath->LineColor     = GetNextValue(&ptok, 0);
		ppath->FillMode      = GetNextValue(&ptok, 0);
		ppath->FillColor     = GetNextValue(&ptok, 0);
		ppath->GradCenterX   = GetNextValue(&ptok, 0);
		ppath->GradCenterY   = GetNextValue(&ptok, 0);
		ppath->GradAngle     = GetNextValue(&ptok, 0);
		ppath->GradRatio     = GetNextFloatValue(&ptok, 0);
		ppath->GradColor     = GetNextValue(&ptok, 0);
		GetNextFlgValue(&ptok, 0);
		GetNextValue(&ptok, 0);

		/* PDR Ver0.1には存在しない */
		ppath->flgCompounded = GetNextFlgValue(&ptok, 1);
		ppath->CompoID       = GetNextValue(&ptok, 1);

		ppath->processed = 0;

		/* アンカー情報の読み込み */
		panchor = ppath->anchor = (ANCHOR *)malloc(sizeof(ANCHOR) * (ppath->NumAnc + 3));
		if (!panchor) {
			FreePdr(pdr);
			return -1;
		}

		for (j = 0; j <= ppath->NumAnc; j++) {
			panchor++;
			if (pdrerror != 0 || !GetLine(pdr)) {
				FreePdr(pdr);
				return -1;
			}
			ptok = pdr->linebuf;
			panchor->flgAnchor = GetNextValue(&ptok, 0);
			panchor->x = GetNextValue(&ptok, 0);
			panchor->y = GetNextValue(&ptok, 0);
		}
		if (ppath->flgClosePath == 0) {
			/* パスが閉じていなければ両端はオンカーブアンカー */
			ppath->anchor[1].flgAnchor = 0;
			ppath->anchor[ppath->NumAnc + 1].flgAnchor = 0;
			ppath->FillMode = 0;	/* パスが閉じていなければ塗りはない(Flashの仕様ではあっても良い) */
		}
		ppath->anchor[ppath->NumAnc + 2] = ppath->anchor[1];
		ppath->anchor[0]                 = ppath->anchor[ppath->NumAnc + 1];

		/* 複合パスのリストを作成 */
		if (ppath->flgCompounded != 0) {
			int index;

			index = FindCompo(&pdr->compolist, ppath->CompoID);
			if (index < 0) {
				index = pdr->compolist.max;
				/* pdr->compolist.compo[compolist.max]が新しい複合パス情報 */
				pdr->compolist.compo[index].compoid     = ppath->CompoID;
				pdr->compolist.compo[index].numpath     = 1;
				pdr->compolist.compo[index].fillid      = -1;
				pdr->compolist.compo[index].pathlist    = (int *)malloc(sizeof(int) * pdr->NumPath);
				pdr->compolist.compo[index].pathlist[0] = i;
				pdr->compolist.max++;
				if (!pdr->compolist.compo[index].pathlist) {
					FreePdr(pdr);
					return -1;
				}
			}
			else {
				pdr->compolist.compo[index].pathlist[pdr->compolist.compo[index].numpath] = i;
				pdr->compolist.compo[index].numpath++;
			}
			if (pdr->compolist.compo[index].fillid == -1 && ppath->FillMode != 0) {
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

	SWFFillStyle compofill = NULL;

	PATH *ppath;

	/* パス情報をセット */
	for (i = 0; i < pdr->NumPath; i++) {
		int flgClosePath, LineWidth, LineMode, LineColor, FillMode, FillColor;
		int GradCenterX, GradCenterY, GradAngle, GradColor;
		double GradRatio;
		int flgCompounded, CompoID;
		int NumAnc;

		SWFGradient gradient;
		SWFFillStyle fill;
		ANCHOR *anchor;

		ppath = &pdr->pathlist.path[i];

		/* 処理済みのパスはスキップ(複合パス対策) */
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
		GradRatio     = ppath->GradRatio;
		GradColor     = ppath->GradColor;
		flgCompounded = ppath->flgCompounded;
		CompoID       = ppath->CompoID;

		anchor = ppath->anchor;

		/* 複合パスの途中でなければStateNewStylesを立てる(連続してたら新しくShapeRecordは作られない) */
		if (pdr->flgmorph == 0 && pdr->compolist.index == 0) {
			SWFShape_newStyles(pdr->shape);
		}

		if (flgCompounded != 0) {
			int index;

			index = FindCompo(&pdr->compolist, CompoID);
			if (index < 0) {
				FreePdr(pdr);
				return -1;
			}
			if (pdr->compolist.compo[index].fillid != i && FillMode != 0) {
				FillMode = -1;
			}
			if (pdr->compolist.index == 0) {
				pdr->compolist.pathid = i;
			}
			if (pdr->compolist.index == pdr->compolist.compo[index].numpath - 1) {
				i = pdr->compolist.pathid;
				pdr->compolist.pathid = -1;
				pdr->compolist.index = 0;
			}
			else {
				i = pdr->compolist.compo[index].pathlist[++pdr->compolist.index] - 1;	/* 次の複合パス */
			}
		}

		/* 塗りを設定 */
		if (FillMode > 0) {
			if (FillMode == 1) {	/* ベタ */
				fill = SWFShape_addSolidFillStyle(
						pdr->shape,
						(byte)( FillColor        & 0xff),
						(byte)((FillColor >> 8 ) & 0xff),
						(byte)((FillColor >> 16) & 0xff),
						(byte)((FillColor >> 24) & 0xff)
					);
			}
			else {	/* グラデーション */
				byte flags = 0;
				double rot1 = cos(GradAngle * M_PI / 180);
				double rot2 = sin(GradAngle * M_PI / 180);

				gradient = newSWFGradient();
				SWFGradient_addEntry(
					gradient,
					0,
					(byte)( FillColor        & 0xff),
					(byte)((FillColor >> 8 ) & 0xff),
					(byte)((FillColor >> 16) & 0xff),
					(byte)((FillColor >> 24) & 0xff)
				);
				if (FillMode == 2) {	/* 線形グラデ1 */
					flags = SWFFILL_LINEAR_GRADIENT;
					SWFGradient_addEntry(
						gradient,
						1.0,
						(byte)( GradColor        & 0xff),
						(byte)((GradColor >> 8 ) & 0xff),
						(byte)((GradColor >> 16) & 0xff),
						(byte)((GradColor >> 24) & 0xff)
					);
				}
				if (FillMode == 3) {	/* 線形グラデ2 */
					flags = SWFFILL_LINEAR_GRADIENT;
					SWFGradient_addEntry(
						gradient,
						0.5,
						(byte)( GradColor        & 0xff),
						(byte)((GradColor >> 8 ) & 0xff),
						(byte)((GradColor >> 16) & 0xff),
						(byte)((GradColor >> 24) & 0xff)
					);
					SWFGradient_addEntry(
						gradient,
						1.0,
						(byte)( FillColor        & 0xff),
						(byte)((FillColor >> 8 ) & 0xff),
						(byte)((FillColor >> 16) & 0xff),
						(byte)((FillColor >> 24) & 0xff)
					);
				}
				if (FillMode == 4) {	/* 円形グラデ */
					flags = SWFFILL_RADIAL_GRADIENT;
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
				/* 変換行列の設定 */
				SWFMatrix_set(
					SWFFillStyle_getMatrix(fill),
					GradRatio * rot1,
					GradRatio * rot2,
					GradRatio * -rot2,
					GradRatio * rot1,
					GradCenterX - pdr->ClipX,
					GradCenterY - pdr->ClipY
				);
			}
			compofill = fill;
		}
		else if (FillMode < 0) {
			fill = compofill;
		}
		else {
			fill = NULL;
		}
		SWFShape_setLeftFillStyle(pdr->shape, fill);

		/* 線の色を設定(閉じていない時は非表示フラグが立っていても表示する仕様らしい) */
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

		/* アンカー情報をセット */
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
				SWFShape_moveScaledPenTo(pdr->shape, dx - pdr->ClipX, dy - pdr->ClipY);
				/*printf("moveTo(%d, %d)\n", dx, dy);*/
			}
			else if (j != NumAnc + 1 || flgClosePath != 0) {
				if (flgAnchor1 == 0) {
					if (flgAnchor2 == 0) {
						dx = x2;
						dy = y2;
					}
					else {
						dx = x1;
						dy = y1;
					}
					SWFShape_drawScaledLineTo(pdr->shape, dx - pdr->ClipX, dy - pdr->ClipY);
					/*printf("lineTo(%d, %d)\n", dx, dy);*/
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
					SWFShape_drawScaledCurveTo(pdr->shape, x1 - pdr->ClipX, y1 - pdr->ClipY, dx - pdr->ClipX, dy - pdr->ClipY);
					/*printf("curveTo(%d, %d, %d, %d)\n", x1, y1, dx, dy);*/
				}
			}
		}

		ppath->processed = 1;	/* 処理済み */
	}

	return 0;
}

int SWFShape_loadPdr(SWFShape shape, int rect[], char *filename, int flag, int size)
{
	PDR pdr;

	pdreol   = 0;
	pdrerror = 0;

	memset(&pdr, 0, sizeof(PDR));
	pdr.flgmorph = flag & PDR_MORPH;
	pdr.linemax  = LINEMAX_DEFAULT;
	pdr.rect     = rect;
	pdr.shape    = shape;

	if (size) {
		pdr.src = (void *)&filename;
		GetChar = GetCharFromBuf;
		FreeLine = FreeLineBuf;
		pdrlimit = size;
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

	pdr.linebuf = malloc(pdr.linemax + 1);
	if (!pdr.linebuf) {
		return -1;
	}
	pdr.linebuf[pdr.linemax] = '\0';	/* IncPtr関数が必ず終了するようにする */

	if (PdrReadHeader(&pdr) != 0) {
		FreePdr(&pdr);
		return -1;
	}

	if (flag & PDR_GETRECTONLY) {
		if (pdr.ispdclip != 0) {
			pdr.shape = newSWFShape();
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

	/* 複合パスの情報を管理する。要素数は高々NumPath個 */
	pdr.compolist.max = 0;
	pdr.compolist.pathid = -1;
	pdr.compolist.index = 0;
	pdr.compolist.compo = (COMPO *)malloc(sizeof(COMPO) * pdr.NumPath);

	pdr.pathlist.max = 0;
	pdr.pathlist.path = (PATH *)malloc(sizeof(PATH) * pdr.NumPath);

	if (!pdr.compolist.compo || !pdr.pathlist.path) {
		FreePdr(&pdr);
		return -1;
	}

	if (PdrReadData(&pdr) != 0) {
		FreePdr(&pdr);
		return -1;
	}

	if (PdrWriteShape(&pdr) != 0) {
		FreePdr(&pdr);
		return -1;
	}

	FreePdr(&pdr);

	if (pdr.ispdclip != 0) {
		rect[0] = SWFCharacter_getScaledWidth(CHARACTER(pdr.shape))  / 20.0;
		rect[1] = SWFCharacter_getScaledHeight(CHARACTER(pdr.shape)) / 20.0;
		if (flag & PDR_GETRECTONLY) {
#ifdef MING_02A
			destroySWFShape(BLOCK(pdr.shape));
#else
			destroySWFShape(pdr.shape);
#endif
		}
	}

	return 0;
}
