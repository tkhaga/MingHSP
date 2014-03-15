/*
    MingHSP - Ming wrapper for HSP
    THAGA http://www.h5.dion.ne.jp/~markent/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ming.h"
#include "utf8.h"
#include "formats.h"
#include "hspdll.h"

#undef  EXPORT
#define EXPORT __declspec(dllexport)

SWFShape mhsp_shape = NULL;
SWFMovie mhsp_movie = NULL;
SWFDisplayItem mhsp_item = NULL;
SWFMovieClip mhsp_movieclip = NULL;
SWFGradient mhsp_gradient = NULL;
SWFBitmap mhsp_bitmap = NULL;
SWFFill mhsp_fill = NULL;
SWFMorph mhsp_morph = NULL;
SWFText mhsp_text = NULL;
SWFFont mhsp_font = NULL;
SWFTextField mhsp_field = NULL;
SWFButton mhsp_button = NULL;
#ifndef JAMING
SWFVideoStream = mhsp_stream;
#endif

extern char funcname[];
extern char *warnbuf;
extern int warnlimit;
char *bmptmp = NULL;
SWFInput input_bmptmp;

char setLine_flag = 0;
unsigned char l;

char addSolidFill_flag = 0;
unsigned char sf;

char drawCubicTo_flag = 0;
float ct1, ct2;

char drawCubic_flag = 0;
float c1, c2;

char setMatrix_flag = 0;
float m1, m2;

char addEntry_flag = 0;
unsigned char e;

int idx, bufsize;

void mhsp_method(byte b, byte *data)
{
	if (idx < bufsize)
	{
		data[idx++] = b;
	}
}

EXPORT BOOL WINAPI ming_setCubicThreshold(int num, int p2, int p3, int p4)
{
	strcpy(funcname, "ming_setCubicThreshold");
	Ming_setCubicThreshold(num);
	return 0;
}

EXPORT BOOL WINAPI ming_setScale(float scale, int p2, int p3, int p4)
{
	strcpy(funcname, "ming_setScale");
	Ming_setScale(scale);
	return 0;
}

EXPORT BOOL WINAPI ming_useSWFVersion(int version, int p2, int p3, int p4)
{
	strcpy(funcname, "Ming_useSWFVersion");
	Ming_useSWFVersion(version);
	return 0;
}

EXPORT BOOL WINAPI swfbutton_keypress(int *c, char *key, int p3, int p4)
{
	*c = (key[0] & 0x7f) << 9;
	return 0;
}

#ifndef JAMING
EXPORT BOOL WINAPI ming_useConstants(int flag, int p2, int p3, int p4)
{
	strcpy(funcname, "ming_useConstants");
	Ming_useConstants(flag);
}
#endif

//SWFMovie

EXPORT BOOL WINAPI mhsp_SWFMovie(SWFShape *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "SWFMovie");
	*p1 = newSWFMovie();
	if (!mhsp_movie) mhsp_movie = *p1;
	return 0;
}

#ifdef JAMING
EXPORT BOOL WINAPI m_save(void *p1, char *filename, int p3, int p4)
{
	strcpy(funcname, "m_save");
	return -SWFMovie_save(mhsp_movie, filename);
}
#else
EXPORT BOOL WINAPI m_save(void *p1, char *filename, int level, int p4)
{
	strcpy(funcname, "m_save");
	return -SWFMovie_save(mhsp_movie, filename, level);
}
#endif

EXPORT BOOL WINAPI m_add(SWFDisplayItem *p1, SWFBlock block, int p3, int p4)
{
	strcpy(funcname, "m_add");
	*p1 = SWFMovie_add(mhsp_movie, block);
	if (!mhsp_item) mhsp_item = *p1;
	return 0;
}

#ifdef JAMING
EXPORT BOOL WINAPI m_output(void *data, int p2, int p3, int p4)
{
	strcpy(funcname, "m_output");
	bufsize = p2;
	idx = 0;
	return -SWFMovie_output(mhsp_movie, (SWFByteOutputMethod)mhsp_method, data);
}
#else
EXPORT BOOL WINAPI m_output(void *data, int level, int p3, int p4)
{
	strcpy(funcname, "m_output");
	bufsize = p2;
	idx = 0;
	return -SWFMovie_output(mhsp_movie, (SWFByteOutputMethod)mhsp_method, data, level);
}
#endif

EXPORT BOOL WINAPI m_remove(SWFDisplayItem item, int p2, int p3, int p4)
{
	strcpy(funcname, "m_remove");
	SWFMovie_remove(mhsp_movie, item);
	return 0;
}

EXPORT BOOL WINAPI m_nextFrame(int p1, int p2, int p3, int p4)
{
	strcpy(funcname, "m_nextFrame");
	SWFMovie_nextFrame(mhsp_movie);
	return 0;
}

EXPORT BOOL WINAPI m_labelFrame(void *p1, char *label, int p3, int p4)
{
	strcpy(funcname, "m_labelFrame");
	SWFMovie_labelFrame(mhsp_movie, label);
	return 0;
}

EXPORT BOOL WINAPI m_setBackground(int r, int g, int b, int p4)
{
	strcpy(funcname, "m_setBackground");
	SWFMovie_setBackground(mhsp_movie, r, g, b);
	return 0;
}

EXPORT BOOL WINAPI m_setRate(float rate, int p2, int p3, int p4)
{
	strcpy(funcname, "m_setRate");
	SWFMovie_setRate(mhsp_movie, rate);
	return 0;
}

EXPORT BOOL WINAPI m_setDimension(float width, float height, int p3, int p4)
{
	strcpy(funcname, "m_setDimention");
	SWFMovie_setDimension(mhsp_movie, width, height);
	return 0;
}

EXPORT BOOL WINAPI m_setFrames(int totalFrames, int p2, int p3, int p4)
{
	strcpy(funcname, "m_setFrames");
	SWFMovie_setNumberOfFrames(mhsp_movie, totalFrames);
	return 0;
}

EXPORT BOOL WINAPI m_streamMp3(void *p1, char *mp3FileName, int p3, int p4)
{
	FILE *fp = fopen(mp3FileName, "rb");
	if (!fp) return -1;
	strcpy(funcname, "m_streamMp3");
	SWFMovie_setSoundStream(mhsp_movie, newSWFSoundStream(fp));
	return 0;
}

EXPORT BOOL WINAPI m_streamMp3_buf(PVAL2 *p1, int p2, int p3, int p4)
{
	char *buf;
	strcpy(funcname, "m_streamMp3_buf");
	buf = malloc(p2);
	memcpy(buf, p1->pt, p2);
	p1->flag = 4;
	SWFMovie_setSoundStream(mhsp_movie, newSWFSoundStream_fromInput(newSWFInput_allocedBuffer(buf, p2)));
	return 0;
}

#ifdef JAMING
EXPORT BOOL WINAPI m_addSound(SWFDisplayItem *item, char *file, int flags, int p4)
{
	FILE *fp = fopen(file, "rb");
	if (!fp) return -1;
	strcpy(funcname, "m_addSound");
	*item = SWFMovie_add(mhsp_movie, newSWFSound(fp, (byte)flags));
	if (!mhsp_item) mhsp_item = *item;
	return 0;
}

EXPORT BOOL WINAPI m_addSound_buf(PVAL2 *p1, int p2, int flags, int p4)
{
	char *buf;
	strcpy(funcname, "m_addSound_buf");
	buf = malloc(p2);
	memcpy(buf, p1->pt, p2);
	p1->flag = 4;
	*(SWFDisplayItem *)p1->pt = SWFMovie_add(mhsp_movie, newSWFSound_fromInput(newSWFInput_allocedBuffer(buf, p2), (byte)flags));
	if (!mhsp_item) mhsp_item = *(SWFDisplayItem *)p1->pt;
	return 0;
}

EXPORT BOOL WINAPI m_startSound(SWFDisplayItem item, int p2, int p3, int p4)
{
	strcpy(funcname, "m_startSound");
	SWFMovie_startSound(mhsp_movie, item);
	return 0;
}

EXPORT BOOL WINAPI m_stopSound(SWFDisplayItem item, int p2, int p3, int p4)
{
	strcpy(funcname, "m_stopSound");
	SWFMovie_stopSound(mhsp_movie, item);
	return 0;
}

EXPORT BOOL WINAPI m_setButtonSound(int *p1, SWFDisplayItem ibutton, int p3, int p4)
{
	SWFDisplayItem item[4];
	strcpy(funcname, "m_setButtonSound");
	int i;
	for (i = 0; i < 4; i++)
		item[i] = (SWFDisplayItem)*(p1 + i);
	SWFMovie_setButtonSound(mhsp_movie, ibutton, item);
	return 0;
}
#else
EXPORT BOOL WINAPI m_stopSound(SWFDisplayItem item, int p2, int p3, int p4)
{
	strcpy(funcname, "m_stopSound");
	SWFMovie_stopSound(mhsp_movie, item);
	return 0;
}
#endif

/* SWFShape */

EXPORT BOOL WINAPI mhsp_SWFShape(SWFShape *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "SWFShape");
	*p1 = newSWFShape();
	if (!mhsp_shape) mhsp_shape = *p1;
	return 0;
}

EXPORT BOOL WINAPI s_setLine(int width, int p2, int p3, int p4)
{
	if (setLine_flag)
	{
		strcpy(funcname, "s_setLine");
		SWFShape_setLine(mhsp_shape, width, p2 & 0xff, p3 & 0xff, p4 & 0xff, l);
		setLine_flag = 0;
	}else{
		l = width;
		setLine_flag = 1;
	}
	return 0;
}

EXPORT BOOL WINAPI s_addSolidFill(SWFFill *fill, int p2, int p3, int p4)
{
	if (addSolidFill_flag)
	{
		strcpy(funcname, "s_addSolidFill");
		*fill = SWFShape_addSolidFill(mhsp_shape, p2 & 0xff, p3 & 0xff, p4 & 0xff, sf);
		if (!mhsp_fill) mhsp_fill = *fill;
		addSolidFill_flag = 0;
	}else{
		sf = p2;
		addSolidFill_flag = 1;
	}
	return 0;
}

EXPORT BOOL WINAPI s_addBitmapFill(SWFFill *fill, SWFBitmap bitmap, int flags, int p4)
{
	strcpy(funcname, "s_addBitmapFill");
	*fill = SWFShape_addBitmapFill(mhsp_shape, bitmap, flags);
	if (!mhsp_fill) mhsp_fill = *fill;
	return 0;
}

EXPORT BOOL WINAPI s_addGradientFill(SWFFill *fill, SWFGradient gradient, int flags, int p4)
{
	strcpy(funcname, "s_addGradientFill");
	*fill = SWFShape_addGradientFill(mhsp_shape, gradient, (byte)flags);
	if (!mhsp_fill) mhsp_fill = *fill;
	return 0;
}

EXPORT BOOL WINAPI s_setLeftFill(SWFFill fill, int p2, int p3, int p4)
{
	strcpy(funcname, "s_setLeftFill");
	SWFShape_setLeftFill(mhsp_shape, fill);
	return 0;
}

EXPORT BOOL WINAPI s_setRightFill(SWFFill fill, int p2, int p3, int p4)
{
	strcpy(funcname, "s_setLeftFill");
	SWFShape_setRightFill(mhsp_shape, fill);
	return 0;
}

EXPORT BOOL WINAPI s_movePenTo(float x, float y, int p3, int p4)
{
	strcpy(funcname, "s_movePenTo");
	SWFShape_movePenTo(mhsp_shape, x, y);
	return 0;
}

EXPORT BOOL WINAPI s_movePen(float dx, float dy, int p3, int p4)
{
	strcpy(funcname, "s_movePen");
	SWFShape_movePen(mhsp_shape, dx, dy);
	return 0;
}

EXPORT BOOL WINAPI s_drawLineTo(float x, float y, int p3, int p4)
{
	strcpy(funcname, "s_drawLineTo");
	SWFShape_drawLineTo(mhsp_shape, x, y);
	return 0;
}

EXPORT BOOL WINAPI s_drawLine(float dx, float dy, int p3, int p4)
{
	strcpy(funcname, "s_drawLine");
	SWFShape_drawLine(mhsp_shape, dx, dy);
	return 0;
}

EXPORT BOOL WINAPI s_drawCurveTo(float controlx, float controly,
			  float anchorx, float anchory)
{
	strcpy(funcname, "s_drawCurveTo");
	SWFShape_drawCurveTo(mhsp_shape, controlx, controly, anchorx, anchory);
	return 0;
}

EXPORT BOOL WINAPI s_drawCurve(float controldx, float controldy,
			  float anchordx, float anchordy)
{
	strcpy(funcname, "s_drawCurve");
	SWFShape_drawCurve(mhsp_shape, controldx, controldy, anchordx, anchordy);
	return 0;
}

EXPORT BOOL WINAPI s_drawGlyph(SWFFont font, unsigned char c, int p3, int p4)
{
	strcpy(funcname, "s_drawGlyph");
	SWFShape_drawGlyph(mhsp_shape, font, c);
	return 0;
}

EXPORT BOOL WINAPI s_drawCircle(int r, int p2, int p3, int p4)
{
	strcpy(funcname, "s_drawCircle");
	SWFShape_drawCircle(mhsp_shape, r);
	return 0;
}

EXPORT BOOL WINAPI s_drawArc(int r, float startAngle, float endAngle, int p4)
{
	strcpy(funcname, "s_drawArc");
	SWFShape_drawArc(mhsp_shape, r, startAngle, endAngle);
	return 0;
}

EXPORT BOOL WINAPI s_drawCubicTo(float bx, float by, float cx, float cy)
{
	if (drawCubicTo_flag)
	{
		strcpy(funcname, "s_drawCubicTo");
		SWFShape_drawCubicTo(mhsp_shape, bx, by, cx, cy, ct1, ct2);
		drawCubicTo_flag = 0;
	}else{
		ct1 = bx;
		ct2 = by;
		drawCubicTo_flag = 1;
	}
	return 0;
}

EXPORT BOOL WINAPI s_drawCubic(float bx, float by, float cx, float cy)
{
	if (drawCubic_flag)
	{
		strcpy(funcname, "s_drawCubic");
		SWFShape_drawCubic(mhsp_shape, bx, by, cx, cy, c1, c2);
		drawCubic_flag = 0;
	}else{
		c1 = bx;
		c2 = by;
		drawCubic_flag = 1;
	}
	return 0;
}

/* SWFDisplayItem */

EXPORT BOOL WINAPI i_moveTo(float x, float y, int p3, int p4)
{
	strcpy(funcname, "i_moveTo");
	SWFDisplayItem_moveTo(mhsp_item, x, y);
	return 0;
}

EXPORT BOOL WINAPI i_move(float x, float y, int p3, int p4)
{
	strcpy(funcname, "i_move");
	SWFDisplayItem_move(mhsp_item, x, y);
	return 0;
}

EXPORT BOOL WINAPI i_scaleTo(float xScale, float yScale, int p3, int p4)
{
	strcpy(funcname, "i_scaleTo");
	SWFDisplayItem_scaleTo(mhsp_item, xScale, yScale);
	return 0;
}

EXPORT BOOL WINAPI i_scale(float xScale, float yScale, int p3, int p4)
{
	strcpy(funcname, "i_scale");
	SWFDisplayItem_scale(mhsp_item, xScale, yScale);
	return 0;
}

EXPORT BOOL WINAPI i_rotateTo(float degrees, int p2, int p3, int p4)
{
	strcpy(funcname, "i_rotateTo");
	SWFDisplayItem_rotateTo(mhsp_item, degrees);
	return 0;
}

EXPORT BOOL WINAPI i_rotate(float degrees, int p2, int p3, int p4)
{
	strcpy(funcname, "i_rotate");
	SWFDisplayItem_rotate(mhsp_item, degrees);
	return 0;
}

EXPORT BOOL WINAPI i_skewXTo(float x, int p2, int p3, int p4)
{
	strcpy(funcname, "i_skewXTo");
	SWFDisplayItem_skewXTo(mhsp_item, x);
	return 0;
}

EXPORT BOOL WINAPI i_skewX(float x, int p2, int p3, int p4)
{
	strcpy(funcname, "i_skewX");
	SWFDisplayItem_skewX(mhsp_item, x);
	return 0;
}

EXPORT BOOL WINAPI i_skewYTo(float y, int p2, int p3, int p4)
{
	strcpy(funcname, "i_skewYTo");
	SWFDisplayItem_skewYTo(mhsp_item, y);
	return 0;
}

EXPORT BOOL WINAPI i_skewY(float y, int p2, int p3, int p4)
{
	strcpy(funcname, "i_skewY");
	SWFDisplayItem_skewY(mhsp_item, y);
	return 0;
}

EXPORT BOOL WINAPI i_setDepth(int depth, int p2, int p3, int p4)
{
	strcpy(funcname, "i_setDepth");
	SWFDisplayItem_setDepth(mhsp_item, depth);
	return 0;
}

EXPORT BOOL WINAPI i_remove(int p1, int p2, int p3, int p4)
{
	strcpy(funcname, "i_remove");
	SWFDisplayItem_remove(mhsp_item);
	return 0;
}

EXPORT BOOL WINAPI i_setName(void *p1, const char *name, int p3, int p4)
{
	strcpy(funcname, "i_setName");
	SWFDisplayItem_setName(mhsp_item, name);
	return 0;
}

EXPORT BOOL WINAPI i_setRatio(float ratio, int p2, int p3, int p4)
{
	strcpy(funcname, "i_setRatio");
	SWFDisplayItem_setRatio(mhsp_item, ratio);
	return 0;
}

EXPORT BOOL WINAPI i_addColor(int r, int g, int b, int a)
{
	strcpy(funcname, "i_addColor");
	SWFDisplayItem_setColorAdd(mhsp_item, r, g, b, a);
	return 0;
}

EXPORT BOOL WINAPI i_multColor(float r, float g, float b, float a)
{
	strcpy(funcname, "i_multColor");
//	if (a == 0) a = 1.0;
	SWFDisplayItem_setColorMult(mhsp_item, r, g, b, a);
	return 0;
}


EXPORT BOOL WINAPI i_setMatrix(float a, float b, float c, float d)
{
	if (setMatrix_flag)
	{
		strcpy(funcname, "i_setMatrix");
		SWFDisplayItem_setMatrix(mhsp_item, a, b, c, d, m1, m2);
		setMatrix_flag = 0;
		return 0;
	}else{
		m1 = a;
		m2 = b;
		setMatrix_flag = 1;
		return 0;
	}
}

EXPORT BOOL WINAPI i_addAction(SWFAction action, int flags, int p3, int p4)
{
	strcpy(funcname, "i_addAction");
	SWFDisplayItem_addAction(mhsp_item, action, flags);
	return 0;
}


/* SWFGradient */

EXPORT BOOL WINAPI mhsp_SWFGradient(SWFGradient *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "SWFGradient");
	*p1 = newSWFGradient();
	if (mhsp_gradient == NULL) mhsp_gradient = *p1;
	return 0;
}

EXPORT BOOL WINAPI g_addEntry(float ratio, int p2, int p3, int p4)
{
	if (addEntry_flag)
	{
		strcpy(funcname, "g_addEntry");
		SWFGradient_addEntry(mhsp_gradient, ratio, p2 & 0xff, p3 & 0xff, p4 & 0xff, e & 0xff);
		addEntry_flag = 0;
	}else{
		e = p2;
		addEntry_flag = 1;
	}
	return 0;
}

/* SWFBitmap */

EXPORT BOOL WINAPI mhsp_SWFBitmap(SWFBitmap *p1, char *filename, int p3, int p4)
{
	FILE *fp1, *fp2;
	int fnlen;

	strcpy(funcname, "SWFBitmap");

	fp1 = fopen(filename, "rb");
	if (!fp1)
		return -2;	/* 第２引数に指定したファイルを開けない */

	if (bmptmp)
	{
		fp2 = fopen(bmptmp, "rb");
		if (!fp2)
			return -3;	/* 第３引数に指定したファイルを開けない */
		*p1 = newSWFJpegWithAlpha(fp2, fp1);
		if (!mhsp_bitmap) mhsp_bitmap = *p1;
		free(bmptmp);
		bmptmp = NULL;
	}

	if (isDBL(fp1))
	{
		fseek(fp1, 0, SEEK_SET);
		*p1 = newSWFDBLBitmap(fp1);
		if (!mhsp_bitmap) mhsp_bitmap = *p1;
		return 0;
	}
	else if (isJPEG(fp1))
	{
		if (p3)
		{
			fnlen = strlen(filename);
			bmptmp = malloc(fnlen + 1);
			strcpy(bmptmp, filename);
			return 0;
		}
		else
		{
			fseek(fp1, 0, SEEK_SET);
			*p1 = newSWFJpegBitmap(fp1);
		}
		if (!mhsp_bitmap) mhsp_bitmap = *p1;
		return 0;
	}

	return -1;	/* 非対応フォーマット */
}

EXPORT BOOL WINAPI mhsp_SWFBitmap_buf(PVAL2 *p1, int p2, int p3, int p4)
{
	char *buf;

	buf = malloc(p2);
	memcpy(buf, p1->pt, p2);

	strcpy(funcname, "SWFBitmap_buf");

	if (input_bmptmp)
	{
		p1->flag = 4;
		*(SWFBitmap *)p1->pt = newSWFJpegWithAlpha_fromInput(input_bmptmp, newSWFInput_allocedBuffer(buf, p2));
		input_bmptmp = NULL;
		if (!mhsp_bitmap) mhsp_bitmap = *(SWFBitmap *)p1->pt;
		return 0;
	}

	if (p3)
	{
		input_bmptmp = newSWFInput_allocedBuffer(buf, p2);
	}else{
		p1->flag = 4;
		*(SWFBitmap *)p1->pt = newSWFBitmap_fromInput(newSWFInput_allocedBuffer(buf, p2));
		if (!mhsp_bitmap) mhsp_bitmap = *(SWFBitmap *)p1->pt;
	}

	return 0;
}

EXPORT BOOL WINAPI bmp_getWidth(int *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "bmp_getWidth");
	*p1 = SWFBitmap_getWidth(mhsp_bitmap);
	return 0;
}

EXPORT BOOL WINAPI bmp_getHeight(int *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "bmp_getHeight");
	*p1 = SWFBitmap_getHeight(mhsp_bitmap);
	return 0;
}

/* SWFFill */

EXPORT BOOL WINAPI f_moveTo(float x, float y, int p3, int p4)
{
	strcpy(funcname, "f_moveTo");
	SWFFill_moveTo(mhsp_fill, x, y);
	return 0;
}

EXPORT BOOL WINAPI f_scaleTo(float x, float y, int p3, int p4)
{
	strcpy(funcname, "f_scaleTo");
	SWFFill_scaleTo(mhsp_fill, x, y);
	return 0;
}

EXPORT BOOL WINAPI f_rotateTo(float degrees, int p2, int p3, int p4)
{
	strcpy(funcname, "f_rotateTo");
	SWFFill_rotateTo(mhsp_fill, degrees);
	return 0;
}

EXPORT BOOL WINAPI f_skewXTo(float x, int p2, int p3, int p4)
{
	strcpy(funcname, "f_skewXTo");
	SWFFill_skewXTo(mhsp_fill, x);
	return 0;
}

EXPORT BOOL WINAPI f_skewYTo(float y, int p2, int p3, int p4)
{
	strcpy(funcname, "f_skewYTo");
	SWFFill_skewYTo(mhsp_fill, y);
	return 0;
}

/* SWFMorph */

EXPORT BOOL WINAPI mhsp_SWFMorph(SWFMorph *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "SWFMorph");
	*p1 = newSWFMorphShape();
	if (!mhsp_morph) mhsp_morph = *p1;
	return 0;
}

EXPORT BOOL WINAPI p_getShape1(SWFShape *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "p_getShape1");
	*p1 = SWFMorph_getShape1(mhsp_morph);
	if (!mhsp_shape) mhsp_shape = *p1;
	return 0;
}

EXPORT BOOL WINAPI p_getShape2(SWFShape *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "p_getShape2");
	*p1 = SWFMorph_getShape2(mhsp_morph);
	if (!mhsp_shape) mhsp_shape = *p1;
	return 0;
}

/* SWFText */

EXPORT BOOL WINAPI mhsp_SWFText(SWFText *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "SWFText");
	*p1 = newSWFText();
	if (!mhsp_text) mhsp_text = *p1;
	return 0;
}

EXPORT BOOL WINAPI t_setFont(SWFBlock font, int p2, int p3, int p4)
{
	strcpy(funcname, "t_setFont");
	SWFText_setFont(mhsp_text, font);
	return 0;
}

EXPORT BOOL WINAPI t_setHeight(float height, int p2, int p3, int p4)
{
	strcpy(funcname, "t_setHeight");
	SWFText_setHeight(mhsp_text, height);
	return 0;
}

EXPORT BOOL WINAPI t_setSpacing(float spacing, int p2, int p3, int p4)
{
	strcpy(funcname, "t_setSpacing");
	SWFText_setSpacing(mhsp_text, spacing);
	return 0;
}

EXPORT BOOL WINAPI t_setColor(int r, int g, int b, int a)
{
	strcpy(funcname, "t_setColor");
//	if (a == 0) a = 0xff;
	SWFText_setColor(mhsp_text, r, g, b, a);
	return 0;
}

EXPORT BOOL WINAPI t_moveTo(float x, float y, int p3, int p4)
{
	strcpy(funcname, "t_moveTo");
	SWFText_moveTo(mhsp_text, x, y);
	return 0;
}

EXPORT BOOL WINAPI t_addString(void *p1, char *string, int p3, int p4)
{
	char *out;
	strcpy(funcname, "t_addString");
	utf8_encode(string, &out);
	SWFText_addString(mhsp_text, out, NULL);
	free(out);
	return 0;
}

EXPORT BOOL WINAPI t_getWidth(float *p1, unsigned char *string, int p3, int p4)
{
	char *out;
	strcpy(funcname, "t_getWidth");
	utf8_encode(string, &out);
	*p1 = SWFText_getStringWidth(mhsp_text, out);
	free(out);
	return 0;
}

EXPORT BOOL WINAPI t_getAscent(float *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "t_getAscent");
	*p1 = SWFFont_getAscent(mhsp_text);
	return 0;
}

EXPORT BOOL WINAPI t_getDescent(float *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "t_getDescent");
	*p1 = SWFFont_getDescent(mhsp_text);
	return 0;
}
EXPORT BOOL WINAPI t_getLeading(float *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "t_getLeading");
	*p1 = SWFFont_getLeading(mhsp_text);
	return 0;
}

/* SWFFont */

EXPORT BOOL WINAPI mhsp_SWFFont(SWFFont *p1, char *name, int p3, int p4)
{
	int fnlen;
	fnlen = strlen(name);
	strcpy(funcname, "SWFFont");
	if (fnlen > 4 && strcmp(CharLower(name + fnlen - 4), ".fdb") == 0)
	{
		FILE *font = fopen(name, "rb");
		if (!font) return -1;
		*p1 = loadSWFFontFromFile(font);
		fclose(font);
	}
	else
		*p1 = newSWFBrowserFont(name);
	if (!mhsp_font) mhsp_font = *p1;
	return 0;
}

EXPORT BOOL WINAPI fnt_getWidth(float *p1, unsigned char *string, int p3, int p4)
{
	strcpy(funcname, "fnt_getWidth");
	*p1 = SWFFont_getStringWidth(mhsp_font, string);
	return 0;
}

#ifdef JAMING
EXPORT BOOL WINAPI fnt_setFlags(int flags, int p2, int p3, int p4)
{
	strcpy(funcname, "fnt_setFlags");
	SWFBrowserFont_setFlags((SWFBrowserFont)mhsp_font, (byte)flags);
	return 0;
}
#endif

EXPORT BOOL WINAPI fnt_getAscent(float *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "fnt_getAscent");
	*p1 = SWFFont_getAscent(mhsp_font);
	return 0;
}

EXPORT BOOL WINAPI fnt_getDescent(float *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "fnt_getDescent");
	*p1 = SWFFont_getDescent(mhsp_font);
	return 0;
}

EXPORT BOOL WINAPI fnt_getLeading(float *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "fnt_getLeading");
	*p1 = SWFFont_getLeading(mhsp_font);
	return 0;
}

/* SWFTextField */

EXPORT BOOL WINAPI mhsp_SWFTextField(SWFTextField *p1, int flags, int p3, int p4)
{
	strcpy(funcname, "SWFTextField");
	*p1 = newSWFTextField();
	SWFTextField_setFlags(*p1, flags);
	if (!mhsp_field) mhsp_field = *p1;
	return 0;
}

EXPORT BOOL WINAPI tf_setFont(SWFBlock font, int p2, int p3, int p4)
{
	strcpy(funcname, "tf_setFont");
	SWFTextField_setFont(mhsp_field, font);
	return 0;
}

EXPORT BOOL WINAPI tf_setBounds(float width, float height, int p3, int p4)
{
	strcpy(funcname, "tf_setBounds");
	SWFTextField_setBounds(mhsp_field, width, height);
	return 0;
}

EXPORT BOOL WINAPI tf_align(int alignment, int p2, int p3, int p4)
{
	strcpy(funcname, "tf_align");
	SWFTextField_setAlignment(mhsp_field, alignment);
	return 0;
}

EXPORT BOOL WINAPI tf_setHeight(float height, int p2, int p3, int p4)
{
	strcpy(funcname, "tf_setHeight");
	SWFTextField_setHeight(mhsp_field, height);
	return 0;
}

EXPORT BOOL WINAPI tf_setLeftMargin(float leftMargin, int p2, int p3, int p4)
{
	strcpy(funcname, "tf_setLeftMargin");
	SWFTextField_setLeftMargin(mhsp_field, leftMargin);
	return 0;
}

EXPORT BOOL WINAPI tf_setRightMargin(float rightMargin, int p2, int p3, int p4)
{
	strcpy(funcname, "tf_setRightMargin");
	SWFTextField_setRightMargin(mhsp_field, rightMargin);
	return 0;
}

EXPORT BOOL WINAPI tf_setMargins(float left, float right, int p3, int p4)
{
	strcpy(funcname, "tf_setMargins");
	SWFTextField_setLeftMargin(mhsp_field, left);
	SWFTextField_setRightMargin(mhsp_field, right);
	return 0;
}

EXPORT BOOL WINAPI tf_setIndentation(float indentation, int p2, int p3, int p4)
{
	strcpy(funcname, "tf_setIndentation");
	SWFTextField_setIndentation(mhsp_field, indentation);
	return 0;
}

EXPORT BOOL WINAPI tf_setLineSpacing(float lineSpacing, int p2, int p3, int p4)
{
	strcpy(funcname, "tf_setLineSpacing");
	SWFTextField_setLineSpacing(mhsp_field, lineSpacing);
	return 0;
}

EXPORT BOOL WINAPI tf_setColor(int r, int g, int b, int a)
{
	strcpy(funcname, "tf_setColor");
//	if (a == 0) a = 0xff;
	SWFTextField_setColor(mhsp_field, r, g, b, a);
	return 0;
}

EXPORT BOOL WINAPI tf_setName(void *p1, char *name, int p3, int p4)
{
	strcpy(funcname, "tf_setName");
	SWFTextField_setVariableName(mhsp_field, name);
	return 0;
}

EXPORT BOOL WINAPI tf_addString(void *p1, char *string, int p3, int p4)
{
	char *out;
	strcpy(funcname, "tf_addString");
	utf8_encode(string, &out);
	SWFTextField_addString(mhsp_field, out);
	free(out);
	return 0;
}

/* SWFMovieClip */

EXPORT BOOL WINAPI mhsp_SWFMovieClip(SWFMovieClip *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "SWFMovieClip");
	*p1 = newSWFMovieClip();
	if (!mhsp_movieclip) mhsp_movieclip = *p1;
	return 0;
}

EXPORT BOOL WINAPI mc_add(SWFDisplayItem *p1, SWFBlock block, int p3, int p4)
{
	strcpy(funcname, "mc_add");
	*p1 = SWFMovieClip_add(mhsp_movieclip, block);
	if (!mhsp_item) mhsp_item = *p1;
	return 0;
}

EXPORT BOOL WINAPI mc_remove(SWFDisplayItem item, int p2, int p3, int p4)
{
	strcpy(funcname, "mc_remove");
	SWFMovieClip_remove(mhsp_movieclip, item);
	return 0;
}

EXPORT BOOL WINAPI mc_nextFrame(int p1, int p2, int p3, int p4)
{
	strcpy(funcname, "mc_nextFrame");
	SWFMovieClip_nextFrame(mhsp_movieclip);
	return 0;
}

EXPORT BOOL WINAPI mc_labelFrame(void *p1, char *label, int p3, int p4)
{
	strcpy(funcname, "mc_labelFrame");
	SWFMovieClip_labelFrame(mhsp_movieclip, label);
	return 0;
}

EXPORT BOOL WINAPI mc_setFrames(int totalFrames, int p2, int p3, int p4)
{
	strcpy(funcname, "mc_setFrames");
	SWFMovieClip_setNumberOfFrames(mhsp_movieclip, totalFrames);
	return 0;
}

#ifdef JAMING
EXPORT BOOL WINAPI mc_addSound(SWFDisplayItem *item, char *file, int flags, int p4)
{
	FILE *fp = fopen(file, "rb");
	if (!fp) return -1;
	strcpy(funcname, "mc_addSound");
	*item = SWFMovieClip_add(mhsp_movieclip, newSWFSound(fp, (byte)flags));
	if (!mhsp_item) mhsp_item = *item;
	return 0;
}

EXPORT BOOL WINAPI mc_addSound_buf(PVAL2 *p1, int p2, int flags, int p4)
{
	char *buf;
	strcpy(funcname, "mc_addSound_buf");
	buf = malloc(p2);
	memcpy(buf, p1->pt, p2);
	p1->flag = 4;
	*(SWFDisplayItem *)p1->pt = SWFMovieClip_add(mhsp_movieclip, newSWFSound_fromInput(newSWFInput_allocedBuffer(buf, p2), (byte)flags));
	if (!mhsp_item) mhsp_item = *(SWFDisplayItem *)p1->pt;
	return 0;
}

EXPORT BOOL WINAPI mc_startSound(SWFDisplayItem item, int p2, int p3, int p4)
{
	strcpy(funcname, "mc_startSound");
	SWFSprite_startSound(mhsp_movieclip, item);
	return 0;
}

EXPORT BOOL WINAPI mc_stopSound(SWFDisplayItem item, int p2, int p3, int p4)
{
	strcpy(funcname, "mc_stopSound");
	SWFSprite_stopSound(mhsp_movieclip, item);
	return 0;
}
#endif

/* SWFButton */

EXPORT BOOL WINAPI mhsp_SWFButton(SWFButton *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "SWFButton");
	*p1 = newSWFButton();
	if (!mhsp_button) mhsp_button = *p1;
	return 0;
}

EXPORT BOOL WINAPI b_addShape(SWFCharacter character, byte flags, int p3, int p4)
{
	strcpy(funcname, "b_addShape");
	SWFButton_addShape(mhsp_button, character, flags);
	return 0;
}

EXPORT BOOL WINAPI b_setUp(SWFCharacter character, int p2, int p3, int p4)
{
	strcpy(funcname, "b_setUp");
	SWFButton_addShape(mhsp_button, character, SWFBUTTON_UP);
	return 0;
}

EXPORT BOOL WINAPI b_setOver(SWFCharacter character, int p2, int p3, int p4)
{
	strcpy(funcname, "b_setOver");
	SWFButton_addShape(mhsp_button, character, SWFBUTTON_OVER);
	return 0;
}

EXPORT BOOL WINAPI b_setDown(SWFCharacter character, int p2, int p3, int p4)
{
	strcpy(funcname, "b_setDown");
	SWFButton_addShape(mhsp_button, character, SWFBUTTON_DOWN);
	return 0;
}

EXPORT BOOL WINAPI b_setHit(SWFCharacter character, int p2, int p3, int p4)
{
	strcpy(funcname, "b_setHit");
	SWFButton_addShape(mhsp_button, character, SWFBUTTON_HIT);
	return 0;
}

EXPORT BOOL WINAPI b_addAction(SWFAction action, int flags, int p3, int p4)
{
	strcpy(funcname, "b_addAction");
	SWFButton_addAction(mhsp_button, action, flags);
	return 0;
}

EXPORT BOOL WINAPI b_setAction(SWFAction action, int p2, int p3, int p4)
{
	strcpy(funcname, "b_setAction");
	SWFButton_addAction(mhsp_button, action, SWFBUTTON_MOUSEUP);
	return 0;
}

/* SWFAction */

EXPORT BOOL WINAPI mhsp_SWFAction(SWFAction *p1, char *script, int p3, int p4)
{
	strcpy(funcname, "SWFAction");
	*p1 = compileSWFActionCode(script);
	return 0;
}

#ifndef JAMING
/* SWFVideoStream */

EXPORT BOOL WINAPI mhsp_SWFVideoStream(SWFVideoStream *p1, char *filename, int p3, int p4)
{
	strcpy(funcname, "SWFVideoStream");
	if (!filename)
	{
		*p1 = newSWFVideoStream();
	}else{
		FILE *fp;
		fp = fopen(filename, "rb");
		if (!fp) return -1;
		*p1 = newSWFVideoStream_fromFile(fp);
		fclose(fp);
	}
	if (!mhsp_stream) mhsp_stream = *p1;
	return 0;
}

EXPORT BOOL WINAPI v_setDimension(int width, int height, int p3, int p4)
{
	strcpy(funcname, "v_setDimension");
	SWFVideoStream_setDimension(mhsp_stream, width, height);
	return 0;
}

EXPORT BOOL WINAPI v_getNumFrames(int *p1, int p2, int p3, int p4)
{
	strcpy(funcname, "v_getNumFrames");
	*p1 = SWFVideoStream_getNumFrames(mhsp_stream);
	return 0;
}
#endif

EXPORT BOOL WINAPI sel_s(SWFShape shape, int p2, int p3, int p4)
{
	mhsp_shape = shape;
	return 0;
}

EXPORT BOOL WINAPI sel_m(SWFMovie movie, int p2, int p3, int p4)
{
	mhsp_movie = movie;
	return 0;
}

EXPORT BOOL WINAPI sel_i(SWFDisplayItem item, int p2, int p3, int p4)
{
	mhsp_item = item;
	return 0;
}

EXPORT BOOL WINAPI sel_g(SWFGradient gradient, int p2, int p3, int p4)
{
	mhsp_gradient = gradient;
	return 0;
}

EXPORT BOOL WINAPI sel_bmp(SWFBitmap bitmap, int p2, int p3, int p4)
{
	mhsp_bitmap = bitmap;
	return 0;
}

EXPORT BOOL WINAPI sel_f(SWFFill fill, int p2, int p3, int p4)
{
	mhsp_fill = fill;
	return 0;
}

EXPORT BOOL WINAPI sel_p(SWFMorph morph, int p2, int p3, int p4)
{
	mhsp_morph = morph;
	return 0;
}

EXPORT BOOL WINAPI sel_t(SWFText text, int p2, int p3, int p4)
{
	mhsp_text = text;
	return 0;
}

EXPORT BOOL WINAPI sel_fnt(SWFFont font, int p2, int p3, int p4)
{
	mhsp_font = font;
	return 0;
}

EXPORT BOOL WINAPI sel_tf(SWFTextField field, int p2, int p3, int p4)
{
	mhsp_field = field;
	return 0;
}

EXPORT BOOL WINAPI sel_mc(SWFMovieClip movieclip, int p2, int p3, int p4)
{
	mhsp_movieclip = movieclip;
	return 0;
}

EXPORT BOOL WINAPI sel_b(SWFButton button, int p2, int p3, int p4)
{
	mhsp_button = button;
	return 0;
}

#ifndef JAMING
EXPORT BOOL WINAPI sel_v(SWFVideoStream stream, int p2, int p3, int p4)
{
	mhsp_stream = stream;
	return 0;
}
#endif

EXPORT BOOL WINAPI destroy_s(SWFShape shape, int p2, int p3, int p4)
{
	destroySWFShape(shape);
	return 0;
}

EXPORT BOOL WINAPI destroy_m(SWFMovie movie, int p2, int p3, int p4)
{
	destroySWFMovie(movie);
	return 0;
}

EXPORT BOOL WINAPI destroy_bmp(SWFBitmap bitmap, int p2, int p3, int p4)
{
	destroySWFBitmap(bitmap);
	return 0;
}

EXPORT BOOL WINAPI destroy_g(SWFGradient gradient, int p2, int p3, int p4)
{
	destroySWFGradient(gradient);
	return 0;
}

EXPORT BOOL WINAPI destroy_p(SWFMorph morph, int p2, int p3, int p4)
{
	destroySWFMorph(morph);
	return 0;
}

EXPORT BOOL WINAPI destroy_fnt(SWFFont font, int p2, int p3, int p4)
{
	destroySWFFont(font);
	return 0;
}

EXPORT BOOL WINAPI destroy_t(SWFText text, int p2, int p3, int p4)
{
	destroySWFText(text);
	return 0;
}

EXPORT BOOL WINAPI destroy_bfnt(SWFBrowserFont font, int p2, int p3, int p4)
{
	destroySWFBrowserFont(font);
	return 0;
}

EXPORT BOOL WINAPI destroy_tf(SWFTextField field, int p2, int p3, int p4)
{
	destroySWFTextField(field);
	return 0;
}

EXPORT BOOL WINAPI destroy_f(SWFFill fill, int p2, int p3, int p4)
{
	destroySWFFill(fill);
	return 0;
}

EXPORT BOOL WINAPI destroy_mc(SWFMovieClip movieclip, int p2, int p3, int p4)
{
	destroySWFMovieClip(movieclip);
	return 0;
}

EXPORT BOOL WINAPI destroy_a(SWFAction action, int p2, int p3, int p4)
{
	destroySWFAction(action);
	return 0;
}

EXPORT BOOL WINAPI destroy_b(SWFButton button, int p2, int p3, int p4)
{
	destroySWFButton(button);
	return 0;
}

/*
#ifndef JAMING
EXPORT BOOL WINAPI destroy_v(SWFVideoStream stream, int p2, int p3, int p4)
{
	destroySWFVideoStream(stream);
	return 0;
}
#endif
*/

/* float calculation */

EXPORT BOOL WINAPI mhsp_add(float *p1, float p2, float p3, int p4)
{
	*p1 = p2 + p3;
	return 0;
}

EXPORT BOOL WINAPI mhsp_sub(float *p1, float p2, float p3, int p4)
{
	*p1 = p2 - p3;
	return 0;
}

EXPORT BOOL WINAPI mhsp_mul(float *p1, float p2, float p3, int p4)
{
	*p1 = p2 * p3;
	return 0;
}

EXPORT BOOL WINAPI mhsp_div(float *p1, float p2, float p3, int p4)
{
	if (!p3) return -1;
	*p1 = p2 / p3;
	return 0;
}

EXPORT BOOL WINAPI mhsp_float(float *p1, int p2, int p3, int p4)
{
	*p1 = p2;
	return 0;
}

EXPORT BOOL WINAPI mhsp_int(int *p1, float p2, int p3, int p4)
{
	*p1 = p2;
	return 0;
}

/* utility */

EXPORT BOOL WINAPI mhsp_fcloseall(int p1, int p2, int p3, int p4)
{
	_fcloseall();
	return 0;
}

EXPORT BOOL WINAPI mhsp_setwarnbuf(void *p1, int p2, int p3, int p4)
{
	warnbuf = p1;
	warnlimit = p2;
	return 0;
}
