#ifndef _MINGHSP_PDR_H_
#define _MINGHSP_PDR_H_

#if !defined(SWFFILL_LINEAR_GRADIENT) || !defined(SWFFILL_RADIAL_GRADIENT)
#define MING_02A
#endif

/* Ming 0.2aでは必要 */
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

#define PDR_MORPH       0x01	/* StateNewStylesを使用しない、エッジの省略を行わない */
#define PDR_GETRECTONLY 0x02	/* サイズの取得のみ */
#define PDR_ASCLIP      0x04	/* PDCLIP形式をクリップとして扱う */

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
	/* 複合パスのID */
	int compoid;
	/* 複合パスに関連付けられる塗り情報のあるパス番号 */
	int fillid;
	int numpath;
	/* 複合パスに関連付けられるパス番号の配列 */
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
