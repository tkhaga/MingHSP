#ifndef _MINGHSP_PDR_H_
#define _MINGHSP_PDR_H_

#if !defined(SWFFILL_LINEAR_GRADIENT) || !defined(SWFFILL_RADIAL_GRADIENT)
#define MING_02A
#endif

/* Ming 0.2a�ł͕K�v */
#ifdef MING_02A
#undef SWFFILL_LINEAR_GRADIENT
#define SWFFILL_LINEAR_GRADIENT 0x10
#undef SWFFILL_RADIAL_GRADIENT
#define SWFFILL_RADIAL_GRADIENT 0x12
void SWFShape_hideLine(SWFShape shape);
#endif

void SWFShape_newStyles(SWFShape shape);

#define LINEMAX_INC     256
#define LINEMAX_DEFAULT 255

#define PDR_MORPH       0x01	/* StateNewStyles���g�p���Ȃ��A�G�b�W�̏ȗ����s��Ȃ� */
#define PDR_GETRECTONLY 0x02	/* �T�C�Y�̎擾�̂� */
#define PDR_ASCLIP      0x04	/* PDCLIP�`�����N���b�v�Ƃ��Ĉ��� */

typedef struct {
	int flgAnchor;
	int x, y;
} ANCHOR;

typedef struct {
	byte flgClosePath;
	int LineWidth, LineMode, LineColor, FillMode, FillColor;
	int GradCenterX, GradCenterY, GradAngle, GradColor;
	double GradRatio;
	byte flgCompounded;
	int CompoID;
	int NumAnc;
	byte processed;
	ANCHOR *anchor;
} PATH;

typedef struct {
	int max;
	PATH *path;
} PATHLIST;

typedef struct {
	/* �����p�X��ID */
	int compoid;
	/* �����p�X�Ɋ֘A�t������h����̂���p�X�ԍ� */
	int fillid;
	int numpath;
	/* �����p�X�Ɋ֘A�t������p�X�ԍ��̔z�� */
	int *pathlist;
} COMPO;

typedef struct {
	int max;
	int pathid;
	int index;
	COMPO *compo;
} COMPOLIST;

typedef struct {
	int NumPath;
	int ClipX, ClipY;
	int flgmorph;
	int ispdclip;
	int linemax;
	int *rect;
	char *linebuf;
	void **src;
	SWFShape shape;
	PATHLIST pathlist;
	COMPOLIST compolist;
} PDR;

typedef int (* GETCHAR)(void **);
typedef void (* FREELINE)(char *, void *);

int SWFShape_loadPdr(SWFShape shape, int rect[], char *filename, int flag, int size);

#endif
