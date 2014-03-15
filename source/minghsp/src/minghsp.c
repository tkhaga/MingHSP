/*
    MingHSP - Ming wrapper for HSP
    Copyright (C) 2003-2007 THAGA http://www.h5.dion.ne.jp/~markent/
*/

/* JaMing用コードは古いので有効にしないで下さい(-DJAMINGを指定しないでください) */

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#ifdef JAMING
#include "../ming.h"
#else
#include "ming.h"
#endif

#include "pdr.h"
#include "formats.h"
#include "toutf8.h"
#include "mhsputil.h"
#include "mhspobj.h"
#include "mhsperror.h"
#include "hspdll.h"

#undef EXPORT
#define EXPORT __declspec(dllexport)

/* mhsperror.cでも同名の定義を使用(将来は統一) */
#define MHSP_STRMAX 0x1000

#ifndef JAMING
void Ming_useConstants(int flag);
void SWFDisplayItem_endMask(SWFDisplayItem item);
float SWFDisplayItem_get_x(SWFDisplayItem item);
float SWFDisplayItem_get_y(SWFDisplayItem item);
float SWFDisplayItem_get_xScale(SWFDisplayItem item);
float SWFDisplayItem_get_yScale(SWFDisplayItem item);
float SWFDisplayItem_get_xSkew(SWFDisplayItem item);
float SWFDisplayItem_get_ySkew(SWFDisplayItem item);
float SWFDisplayItem_get_rot(SWFDisplayItem item);
void SWFMovie_protect(SWFMovie movie);
void SWFMovie_writeExports(SWFMovie movie);
/* void SWFTextField_addUTF8Chars(SWFTextField field, const char *string); */

void destroySWFVideoStream(SWFVideoStream stream);
void destroySWFPrebuiltClip(SWFPrebuiltClip clip);
#endif

void SWFShape_newStyles(SWFShape shape);
/* void SWFShape_setWhole(SWFShape shape); */

SWFShape mhsp_shape;
SWFMovie mhsp_movie;
SWFDisplayItem mhsp_item;
SWFMovieClip mhsp_movieclip;
SWFGradient mhsp_gradient;
SWFBitmap mhsp_bitmap;
SWFFill mhsp_fill;
SWFMorph mhsp_morph;
SWFText mhsp_text;
SWFFont mhsp_font;
SWFTextField mhsp_field;
SWFButton mhsp_button;
#ifndef JAMING
SWFVideoStream mhsp_stream;
SWFFontCharacter mhsp_fontchar;
SWFSoundInstance mhsp_instance;
#endif

extern mhspList mhspObjects;

static int drawCubicTo_flag = 0;
static float ct1, ct2;

static int drawCubic_flag = 0;
static float c1, c2;

static int setMatrix_flag = 0;
static float m1, m2;

static int addEntry_flag = 0;
static unsigned int e;

int method_index, method_size;

void mhsp_method(byte b, byte *data)
{
	if (method_index < method_size) {
		data[method_index++] = b;
	}
}

EXPORT BOOL WINAPI mhsp_initselection(int p1, int p2, int p3, int p4)
{
#ifndef JAMING
	mhsp_stream    = NULL;
	mhsp_fontchar = NULL;
	mhsp_instance  = NULL;
#endif
	mhsp_shape     = NULL;
	mhsp_movie     = NULL;
	mhsp_item      = NULL;
	mhsp_movieclip = NULL;
	mhsp_gradient  = NULL;
	mhsp_bitmap    = NULL;
	mhsp_fill      = NULL;
	mhsp_morph     = NULL;
	mhsp_text      = NULL;
	mhsp_font      = NULL;
	mhsp_field     = NULL;
	mhsp_button    = NULL;

	return 0;
}

EXPORT BOOL WINAPI mhsp_init(int p1, int p2, int p3, int p4)
{
	Ming_init();
#ifdef JAMING
	Ming_setErrorFunction((void (*)(char *msg, ...))mhsp_error);
	Ming_setWarnFunction((void (*)(char *msg, ...))mhsp_warn);
#else
	Ming_setErrorFunction(mhsp_error);
	Ming_setWarnFunction(mhsp_warn);
#endif

	mhsp_initselection(0, 0, 0, 0);

	mhsp_list_init(&mhspObjects, (int (*)(void*))destroyMhspObject);

	return 0;
}

EXPORT BOOL WINAPI ming_setCubicThreshold(int num, int p2, int p3, int p4)
{
	setFuncName("Ming_setCubicThreshold");
	Ming_setCubicThreshold(num);
	return 0;
}

EXPORT BOOL WINAPI ming_setScale(float scale, int p2, int p3, int p4)
{
	setFuncName("Ming_setScale");
	Ming_setScale(scale);
	return 0;
}

EXPORT BOOL WINAPI ming_useSWFVersion(int version, int p2, int p3, int p4)
{
	setFuncName("Ming_useSWFVersion");
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
	setFuncName("Ming_useConstants");
	Ming_useConstants(flag);
	return 0;
}

EXPORT BOOL WINAPI ming_setSWFCompression(int level, int p2, int p3, int p4)
{
	setFuncName("Ming_setSWFCompression");
	return -Ming_setSWFCompression(level);
}
/*
EXPORT BOOL WINAPI ming_collectGarbage(int p1, int p2, int p3, int p4)
{
	setFuncName("Ming_collectGarbage");
	Ming_collectGarbage();
	return 0;
}
*/
#endif

/* SWFMovie */

EXPORT BOOL WINAPI mhsp_SWFMovie(PMhspObject *p1, int version, int p3, int p4)
{
	setFuncName("SWFMovie");
	if (version) {
		*p1 = addMhspObject(newSWFMovieWithVersion(version), T_SWFMovie);
	}
	else {
		*p1 = addMhspObject(newSWFMovie(version), T_SWFMovie);
	}
	if (!*p1) {
		return -1;
	}
	if (!mhsp_movie) {
		mhsp_movie = MINGOBJ(*p1, SWFMovie);
	}
	return 0;
}

#ifdef JAMING
EXPORT BOOL WINAPI m_save(void *p1, char *filename, int p3, int p4)
{
	setFuncName("m_save");
	return -SWFMovie_save(mhsp_movie, filename);
}
#else
EXPORT BOOL WINAPI m_save(HSPEXINFO *hei, int p2, int p3, int p4)
{
	char *filename;
	int level, oldval, size;

	setFuncName("m_save");
	filename = hei->HspFunc_prm_gets();
	level = hei->HspFunc_prm_getdi(-1);
	if (level < -1 || level > 9) {
		return -1;
	}
	oldval = Ming_setSWFCompression(level);
	size = -SWFMovie_save(mhsp_movie, filename);
	Ming_setSWFCompression(oldval);

	return size;
}
#endif

EXPORT BOOL WINAPI m_add(PMhspObject *p1, PMhspObject block, int p3, int p4)
{
	SWFDisplayItem item;

	setFuncName("m_add");
	switch (OBJTYPE(block)) {
		case T_SWFAction:
			SETNODESTFLAG(block);
			break;
		default:
			break;
	}
	item = SWFMovie_add(mhsp_movie, MINGOBJ(block, SWFBlock));
	if ( !(*p1 = addMhspObject(item, T_SWFDisplayItem)) ) {
		return -1;
	}
	SETNODESTFLAG(*p1);
	if (!mhsp_item) {
		mhsp_item = item;
	}
	return 0;
}

#ifdef JAMING
EXPORT BOOL WINAPI m_output(void *data, int p2, int p3, int p4)
{
	setFuncName("m_output");
	method_size = p2;
	method_index = 0;
	return -SWFMovie_output(mhsp_movie, (SWFByteOutputMethod)mhsp_method, data);
}
#else
EXPORT BOOL WINAPI m_output(HSPEXINFO *hei, int p2, int p3, int p4)
{
	char *data;
	int level, oldval, size;

	setFuncName("m_output");
	data = (char *)hei->HspFunc_prm_getv();
	method_size = hei->HspFunc_prm_geti();
	level = hei->HspFunc_prm_getdi(-1);
	if (level < -1 || level > 9) {
		return -1;
	}
	oldval = Ming_setSWFCompression(level);
	method_index = 0;
	size = -SWFMovie_output(mhsp_movie, (SWFByteOutputMethod)mhsp_method, data);
	Ming_setSWFCompression(oldval);
	return size;
}
#endif

EXPORT BOOL WINAPI m_remove(MhspObject *object, int p2, int p3, int p4)
{
	setFuncName("m_remove");
	SWFMovie_remove(mhsp_movie, (SWFDisplayItem)object->realObject);
	return 0;
}

EXPORT BOOL WINAPI m_nextFrame(int p1, int p2, int p3, int p4)
{
	setFuncName("m_nextFrame");
	SWFMovie_nextFrame(mhsp_movie);
	return 0;
}

EXPORT BOOL WINAPI m_labelFrame(void *p1, char *label, int p3, int p4)
{
	setFuncName("m_labelFrame");
	SWFMovie_labelFrame(mhsp_movie, label);
	return 0;
}

EXPORT BOOL WINAPI m_setBackground(int r, int g, int b, int p4)
{
	setFuncName("m_setBackground");
	SWFMovie_setBackground(mhsp_movie, r, g, b);
	return 0;
}

EXPORT BOOL WINAPI m_setRate(float rate, int p2, int p3, int p4)
{
	setFuncName("m_setRate");
	SWFMovie_setRate(mhsp_movie, rate);
	return 0;
}

EXPORT BOOL WINAPI m_setDimension(float width, float height, int p3, int p4)
{
	setFuncName("m_setDimention");
	SWFMovie_setDimension(mhsp_movie, width, height);
	return 0;
}

EXPORT BOOL WINAPI m_setFrames(int totalFrames, int p2, int p3, int p4)
{
	setFuncName("m_setFrames");
	SWFMovie_setNumberOfFrames(mhsp_movie, totalFrames);
	return 0;
}

#ifdef JAMING
EXPORT BOOL WINAPI m_streamMp3(void *p1, char *mp3FileName, int p3, int p4)
{
	SWFSoundStream stream;
	FILE *fp = fopen(mp3FileName, "rb");
	if (!fp) {
		return 12;	/* File Not Found */
	}
	setFuncName("m_streamMp3");
	stream = newSWFSoundStream(fp);
	addMhspObject(stream, T_SWFSoundStream);
	SWFMovie_setSoundStream(mhsp_movie, stream);
	return 0;
}

EXPORT BOOL WINAPI m_streamMp3_buf(void *p1, int p2, int p3, int p4)
{
	setFuncName("m_streamMp3_buf");
	SWFMovie_setSoundStream(mhsp_movie, newSWFSoundStream_fromInput(newSWFInput_allocedBuffer(p1, p2)));
	return 0;
}

EXPORT BOOL WINAPI m_addSound(MhspObject *p1, char *file, int flags, int p4)
{
	SWFSound sound;
	FILE *fp = fopen(file, "rb");

	if (!fp) {
		return 12;	/* File Not Found */
	}
	setFuncName("m_addSound");
	sound = newSWFSound(fp, (byte)flags);
	if (!mhsp_list_append(&mhspObjects, sound, T_SWFSound)) {
		return -1;
	}
	if ( !(*p1 = mhsp_list_append(&mhspObjects, SWFMovie_add(mhsp_movie, sound), T_SWFDisplayItem)) ) {
		return -1;
	}
	if (!mhsp_item) {
		mhsp_item = (SWFDisplayItem)*p1->realObject;
	}
	return 0;
}

EXPORT BOOL WINAPI m_addSound_buf(HSPEXINFO *hei, int p2, int p3, int p4)
{
	SWFInput input;
	SWFSound sound;
	PMhspObject *p1;
	MhspObject *rel, *object;
	SWFDisplayItem item;

	setFuncName("m_addSound_buf");
	p1 = ( *)hei->HspFunc_prm_getv();
	input = newSWFInput_allocedBuffer((unsigned char *)hei->HspFunc_prm_getv(), hei->HspFunc_prm_geti()),
										 (byte)hei->HspFunc_prm_getdi(0));
	if (!mhsp_list_append(&mhspObjects, input, T_SWFInput)) {
		return -1;
	}
	sound = newSWFSound_fromInput(input);
	if (!mhsp_list_append(&mhspObjects, sound, T_SWFSound)) {
		return -1;
	}
	item = SWFMovie_add(mhsp_movie, sound);
	if ( !(*p1 = mhsp_list_append(&mhspObjects, item, T_SWFMovie)) ) {
		return -1;
	}
	if (!mhsp_item) {
		mhsp_item = item;
	}
	return 0;
}

EXPORT BOOL WINAPI m_startSound(SWFDisplayItem item, int p2, int p3, int p4)
{
	setFuncName("m_startSound");
	SWFMovie_startSound(mhsp_movie, item);
	return 0;
}

EXPORT BOOL WINAPI m_stopSound(SWFDisplayItem item, int p2, int p3, int p4)
{
	setFuncName("m_stopSound");
	SWFMovie_stopSound(mhsp_movie, item);
	return 0;
}

EXPORT BOOL WINAPI m_setButtonSound(PMhspObject *p1, SWFDisplayItem ibutton, int p3, int p4)
{
	SWFDisplayItem item[4];
	int i;

	setFuncName("m_setButtonSound");
	for (i = 0; i < 4; i++) {
		item[i] = (SWFDisplayItem)(p1[i]->realObject);
	}
	SWFMovie_setButtonSound(mhsp_movie, ibutton, item);
	return 0;
}

#else
EXPORT BOOL WINAPI m_streamMp3(PMhspObject *p1, char *mp3FileName, float skip, int p4)
{
	FILE *fp = fopen(mp3FileName, "rb");
	SWFSoundStream sound;
	MhspObject *object, *rel;

	if (!fp) {
		return 12;
	}
	setFuncName("m_streamMp3");
	if ( !(rel = allocMhspObject(fp, T_FILE)) ) {
		return -1;
	}
	sound = newSWFSoundStream(fp);
	if ( !(object = addMhspObject(sound, T_SWFSoundStream)) ) {
		destroyMhspObject(rel);
		return -1;
	}
	appendRelative(object, rel);
	SWFMovie_setSoundStreamAt(mhsp_movie, sound, skip);
	return -SWFSoundStream_getFrames(sound);
}

EXPORT BOOL WINAPI m_streamMp3_buf(void *p1, int p2, float skip, int p4)
{
	SWFInput input;
	SWFSoundStream sound;
	MhspObject *object, *rel;

	setFuncName("m_streamMp3_buf");
	input = newSWFInput_allocedBuffer(p1, p2);
	if ( !(rel = allocMhspObject(input, T_SWFInput)) ) {
		return -1;
	}
	sound = newSWFSoundStream_fromInput(input);
	if ( !(object = addMhspObject(sound, T_SWFSoundStream)) ) {
		destroyMhspObject(rel);
		return -1;
	}
	appendRelative(object, rel);
	SWFMovie_setSoundStreamAt(mhsp_movie, sound, skip);
	return -SWFSoundStream_getFrames(sound);
}

/* PHPではSWFMovieClipを返すことになっているが、実際はSWFCharacterを返す */
EXPORT BOOL WINAPI m_importChar(HSPEXINFO *hei, int p2, int p3, int p4)
{
	char filename[MHSP_STRMAX], name[MHSP_STRMAX];
	PMhspObject *p1;
	SWFCharacter clip;

	setFuncName("m_importChar");
	p1 = (PMhspObject *)hei->HspFunc_prm_getv();
	lstrcpyn(filename, hei->HspFunc_prm_gets(), MHSP_STRMAX);
	lstrcpyn(name, hei->HspFunc_prm_gets(), MHSP_STRMAX);
	clip = SWFMovie_importCharacter(mhsp_movie, filename, name);
	if ( !(*p1 = addMhspObject(clip, T_SWFCharacter)) ) {
		return -1;
	}
	if (!mhsp_movieclip) {
		mhsp_movieclip = (SWFMovieClip)clip;
	}
	return 0;
}

EXPORT BOOL WINAPI m_addFont(PMhspObject *p1, PMhspObject font, int p3, int p4)
{
	SWFFontCharacter fontchar;

	setFuncName("m_addFont");
	fontchar = SWFMovie_addFont(mhsp_movie, MINGOBJ(font, SWFFont));
	if ( !(*p1 = addMhspObject(fontchar, T_SWFFontCharacter)) ) {
		return -1;
	}
	if (!mhsp_fontchar) {
		mhsp_fontchar = fontchar;
	}
	return 0;
}

EXPORT BOOL WINAPI m_importFont(HSPEXINFO *hei, int p2, int p3, int p4)
{
	char filename[MHSP_STRMAX], name[MHSP_STRMAX];
	PMhspObject *p1;
	SWFFontCharacter fontchar;

	setFuncName("m_importFont");
	p1 = (PMhspObject *)hei->HspFunc_prm_getv();
	lstrcpyn(filename, hei->HspFunc_prm_gets(), MHSP_STRMAX);
	lstrcpyn(name, hei->HspFunc_prm_gets(), MHSP_STRMAX);
	fontchar = SWFMovie_importFont(mhsp_movie, filename, name);
	if ( !(*p1 = addMhspObject(fontchar, T_SWFFontCharacter)) ) {
		return -1;
	}
	if (!mhsp_fontchar) {
		mhsp_fontchar = fontchar;
	}
	return 0;
}

EXPORT BOOL WINAPI m_addExport(PMhspObject block, char *name, int p3, int p4)
{
	setFuncName("m_addExport");
	SWFMovie_addExport(mhsp_movie, MINGOBJ(block, SWFBlock), name);
	return 0;
}

EXPORT BOOL WINAPI m_writeExports(int p1, int p2, int p3, int p4)
{
	setFuncName("m_writeExports");
	SWFMovie_writeExports(mhsp_movie);
	return 0;
}

EXPORT BOOL WINAPI m_startSound(PMhspObject *p1, PMhspObject sound, int p3, int p4)
{
	SWFSoundInstance inst;
	
	setFuncName("m_startSound");
	inst = SWFMovie_startSound(mhsp_movie, MINGOBJ(sound, SWFSound));
	if ( !(*p1 = addMhspObject(inst, T_SWFSoundInstance)) ) {
		return -1;
	}
	SETNODESTFLAG(*p1);
	if (!mhsp_instance) {
		mhsp_instance = inst;
	}
	return 0;
}

EXPORT BOOL WINAPI m_stopSound(PMhspObject sound, int p2, int p3, int p4)
{
	setFuncName("m_stopSound");
	SWFMovie_stopSound(mhsp_movie, MINGOBJ(sound, SWFSound));
	return 0;
}

/* ming.cには実装されていなかったけれども一応 */
EXPORT BOOL WINAPI m_protect(int p1, int p2, int p3, int p4)
{
	setFuncName("m_protect");
	SWFMovie_protect(mhsp_movie);
	return 0;
}

EXPORT BOOL WINAPI m_namedAnchor(int p1, const char *label, int p3, int p4)
{
	setFuncName("m_namedAnchor");
	SWFMovie_namedAnchor(mhsp_movie, label);
	return 0;
}
#endif

/* SWFShape */

EXPORT BOOL WINAPI mhsp_SWFShape(PMhspObject *p1, int p2, int p3, int p4)
{
	SWFShape shape;
	
	setFuncName("SWFShape");
	shape = newSWFShape();
	if ( !(*p1 = addMhspObject(shape, T_SWFShape)) ) {
		return -1;
	}
	if (!mhsp_shape) {
		mhsp_shape = shape;
	}
	return 0;
}

EXPORT BOOL WINAPI s_setLine(HSPEXINFO *hei, int p2, int p3, int p4)
{
	int width;
	byte r, g, b, a;

	setFuncName("s_setLine");
	width = hei->HspFunc_prm_getdi(0);
	r = (byte)hei->HspFunc_prm_getdi(0);
	g = (byte)hei->HspFunc_prm_getdi(0);
	b = (byte)hei->HspFunc_prm_getdi(0);
	a = (byte)hei->HspFunc_prm_getdi(0xff);
	SWFShape_setLine(mhsp_shape, width, r, g, b, a);
	return 0;
}

EXPORT BOOL WINAPI s_addSolidFill(HSPEXINFO *hei, int p2, int p3, int p4)
{
	byte r, g, b, a;
	PMhspObject *p1;
	SWFFill fill;

	setFuncName("s_addSolidFill");
	p1 = (PMhspObject *)hei->HspFunc_prm_getv();
	r = (byte)hei->HspFunc_prm_getdi(0);
	g = (byte)hei->HspFunc_prm_getdi(0);
	b = (byte)hei->HspFunc_prm_getdi(0);
	a = (byte)hei->HspFunc_prm_getdi(0xff);
	fill = SWFShape_addSolidFill(mhsp_shape, r, g, b, a);
	if ( !(*p1 = addMhspObject(fill, T_SWFFill)) ) {
		return -1;
	}
	if (!mhsp_fill) {
		mhsp_fill = fill;
	}
	return 0;
}

EXPORT BOOL WINAPI s_addBitmapFill(HSPEXINFO *hei, int p2, int p3, int p4)
{
	byte flags;
	PMhspObject *p1;
	PMhspObject bitmap;
	SWFFill fill;

	setFuncName("s_addBitmapFill");
	p1 = (PMhspObject *)hei->HspFunc_prm_getv();
	bitmap = (PMhspObject)hei->HspFunc_prm_geti();
	flags = (byte)hei->HspFunc_prm_getdi(SWFFILL_TILED_BITMAP);
	fill = SWFShape_addBitmapFill(mhsp_shape, MINGOBJ(bitmap, SWFBitmap), flags);
	if ( !(*p1 = addMhspObject(fill, T_SWFFill)) ) {
		return -1;
	}
	if (!mhsp_fill) {
		mhsp_fill = fill;
	}
	return 0;
}

EXPORT BOOL WINAPI s_addGradientFill(HSPEXINFO *hei, int p2, int p3, int p4)
{
	byte flags;
	PMhspObject *p1;
	PMhspObject gradient;
	SWFFill fill;

	setFuncName("s_addGradientFill");
	p1 = (PMhspObject *)hei->HspFunc_prm_getv();
	gradient = (PMhspObject)hei->HspFunc_prm_geti();
	flags = (byte)hei->HspFunc_prm_getdi(SWFFILL_LINEAR_GRADIENT);
	fill = SWFShape_addGradientFill(mhsp_shape, MINGOBJ(gradient, SWFGradient), flags);
	if ( !(*p1 = addMhspObject(fill, T_SWFFill)) ) {
		return -1;
	}
	if (!mhsp_fill) {
		mhsp_fill = fill;
	}
	return 0;
}

EXPORT BOOL WINAPI s_setLeftFill(PMhspObject fill, int p2, int p3, int p4)
{
	setFuncName("s_setLeftFill");
	SWFShape_setLeftFill(mhsp_shape, MINGOBJ(fill, SWFFill));
	return 0;
}

EXPORT BOOL WINAPI s_setRightFill(PMhspObject fill, int p2, int p3, int p4)
{
	setFuncName("s_setLeftFill");
	SWFShape_setRightFill(mhsp_shape, MINGOBJ(fill, SWFFill));
	return 0;
}

EXPORT BOOL WINAPI s_movePenTo(float x, float y, int p3, int p4)
{
	setFuncName("s_movePenTo");
	SWFShape_movePenTo(mhsp_shape, x, y);
	return 0;
}

EXPORT BOOL WINAPI s_movePen(float dx, float dy, int p3, int p4)
{
	setFuncName("s_movePen");
	SWFShape_movePen(mhsp_shape, dx, dy);
	return 0;
}

EXPORT BOOL WINAPI s_drawLineTo(float x, float y, int p3, int p4)
{
	setFuncName("s_drawLineTo");
	SWFShape_drawLineTo(mhsp_shape, x, y);
	return 0;
}

EXPORT BOOL WINAPI s_drawLine(float dx, float dy, int p3, int p4)
{
	setFuncName("s_drawLine");
	SWFShape_drawLine(mhsp_shape, dx, dy);
	return 0;
}

EXPORT BOOL WINAPI s_drawCurveTo(float controlx, float controly,
			  float anchorx, float anchory)
{
	setFuncName("s_drawCurveTo");
	SWFShape_drawCurveTo(mhsp_shape, controlx, controly, anchorx, anchory);
	return 0;
}

EXPORT BOOL WINAPI s_drawCurve(float controldx, float controldy,
			  float anchordx, float anchordy)
{
	setFuncName("s_drawCurve");
	SWFShape_drawCurve(mhsp_shape, controldx, controldy, anchordx, anchordy);
	return 0;
}

EXPORT BOOL WINAPI s_drawGlyph(PMhspObject font, unsigned char c, int p3, int p4)
{
	setFuncName("s_drawGlyph");
	SWFShape_drawGlyph(mhsp_shape, MINGOBJ(font, SWFFont), c);
	return 0;
}

EXPORT BOOL WINAPI s_drawCircle(int r, int p2, int p3, int p4)
{
	setFuncName("s_drawCircle");
	SWFShape_drawCircle(mhsp_shape, r);
	return 0;
}

EXPORT BOOL WINAPI s_drawArc(int r, float startAngle, float endAngle, int p4)
{
	setFuncName("s_drawArc");
	SWFShape_drawArc(mhsp_shape, r, startAngle, endAngle);
	return 0;
}

EXPORT BOOL WINAPI s_drawCubicTo(float bx, float by, float cx, float cy)
{
	if (drawCubicTo_flag) {
		setFuncName("s_drawCubicTo");
		SWFShape_drawCubicTo(mhsp_shape, bx, by, cx, cy, ct1, ct2);
		drawCubicTo_flag = 0;
	}
	else {
		ct1 = bx;
		ct2 = by;
		drawCubicTo_flag = 1;
	}
	return 0;
}

EXPORT BOOL WINAPI s_drawCubic(float bx, float by, float cx, float cy)
{
	if (drawCubic_flag) {
		setFuncName("s_drawCubic");
		SWFShape_drawCubic(mhsp_shape, bx, by, cx, cy, c1, c2);
		drawCubic_flag = 0;
	}
	else {
		c1 = bx;
		c2 = by;
		drawCubic_flag = 1;
	}
	return 0;
}

#ifndef JAMING
/* ming.cには実装されていなかったけれども一応 */
EXPORT BOOL WINAPI s_getPenX(float *p1, int p2, int p3, int p4)
{
	setFuncName("s_getPenX");
	*p1 = SWFShape_getPenX(mhsp_shape);
	return 0;
}

EXPORT BOOL WINAPI s_getPenY(float *p1, int p2, int p3, int p4)
{
	setFuncName("s_getPenY");
	*p1 = SWFShape_getPenY(mhsp_shape);
	return 0;
}

EXPORT BOOL WINAPI s_getPen(HSPEXINFO *hei, int p2, int p3, int p4)
{
	float *penx, *peny;

	setFuncName("s_getPen");
	penx = (float *)hei->HspFunc_prm_getv();
	peny = (float *)hei->HspFunc_prm_getv();
	SWFShape_getPen(mhsp_shape, penx, peny);
	return 0;
}
#endif

EXPORT BOOL WINAPI s_loadPdr(HSPEXINFO *hei, int p2, int p3, int p4)
{
	int rect[2];
	int flag, size, orgX, orgY;
	int ret;
	char *filename, *p;
#ifdef PDR_USE_SWFINPUT
	FILE *fp = NULL;
	SWFInput input;
#endif

	setFuncName("s_loadPdr");

	p = hei->HspFunc_prm_gets();
	filename = (char *)malloc(strlen(p) + 1);
	filename[0] = '\0';
	strcpy(filename, p);

	flag = hei->HspFunc_prm_getdi(0);
	size = hei->HspFunc_prm_getdi(0);
	orgX = hei->HspFunc_prm_getdi(0);
	orgY = hei->HspFunc_prm_getdi(0);
#ifdef PDR_USE_SWFINPUT
	if (size > 0) {
		input = newSWFInput_allocedBuffer((unsigned char *)filename, size);
	}
	else {
		fp = fopen(filename, "rb");
		if (!fp) {
			free(filename);
			return 0;
		}
		input = newSWFInput_file(fp);
	}
#endif
#ifdef PDR_USE_SWFINPUT
	if (SWFShape_loadPdr(mhsp_shape, rect, input, flag, orgX, orgY))
#else
	if (SWFShape_loadPdr(mhsp_shape, rect, filename, flag, size, orgX, orgY))
#endif
	{
		ret = 0;
	}
	else {
		ret = (rect[0] & 0xffff) | ((rect[1] & 0xeffff) << 16);
	}
#ifdef PDR_USE_SWFINPUT
	if (fp) {
		fclose(fp);
	}
	destroySWFInput(input);
#endif
	free(filename);

	return -ret;
}

EXPORT BOOL WINAPI s_newStyles(int p1, int p2, int p3, int p4)
{
	setFuncName("s_newStyles");
	SWFShape_newStyles(mhsp_shape);
	return 0;
}
/*
EXPORT BOOL WINAPI s_setWhole(int p1, int p2, int p3, int p4)
{
	setFuncName("s_setWhole");
	SWFShape_setWhole(mhsp_shape);
	return 0;
}
*/
/* SWFDisplayItem */

EXPORT BOOL WINAPI i_moveTo(float x, float y, int p3, int p4)
{
	setFuncName("i_moveTo");
	SWFDisplayItem_moveTo(mhsp_item, x, y);
	return 0;
}

EXPORT BOOL WINAPI i_move(float x, float y, int p3, int p4)
{
	setFuncName("i_move");
	SWFDisplayItem_move(mhsp_item, x, y);
	return 0;
}

EXPORT BOOL WINAPI i_scaleTo(float xScale, float yScale, int p3, int p4)
{
	setFuncName("i_scaleTo");
	SWFDisplayItem_scaleTo(mhsp_item, xScale, yScale);
	return 0;
}

EXPORT BOOL WINAPI i_scale(float xScale, float yScale, int p3, int p4)
{
	setFuncName("i_scale");
	SWFDisplayItem_scale(mhsp_item, xScale, yScale);
	return 0;
}

EXPORT BOOL WINAPI i_rotateTo(float degrees, int p2, int p3, int p4)
{
	setFuncName("i_rotateTo");
	SWFDisplayItem_rotateTo(mhsp_item, degrees);
	return 0;
}

EXPORT BOOL WINAPI i_rotate(float degrees, int p2, int p3, int p4)
{
	setFuncName("i_rotate");
	SWFDisplayItem_rotate(mhsp_item, degrees);
	return 0;
}

EXPORT BOOL WINAPI i_skewXTo(float x, int p2, int p3, int p4)
{
	setFuncName("i_skewXTo");
	SWFDisplayItem_skewXTo(mhsp_item, x);
	return 0;
}

EXPORT BOOL WINAPI i_skewX(float x, int p2, int p3, int p4)
{
	setFuncName("i_skewX");
	SWFDisplayItem_skewX(mhsp_item, x);
	return 0;
}

EXPORT BOOL WINAPI i_skewYTo(float y, int p2, int p3, int p4)
{
	setFuncName("i_skewYTo");
	SWFDisplayItem_skewYTo(mhsp_item, y);
	return 0;
}

EXPORT BOOL WINAPI i_skewY(float y, int p2, int p3, int p4)
{
	setFuncName("i_skewY");
	SWFDisplayItem_skewY(mhsp_item, y);
	return 0;
}

EXPORT BOOL WINAPI i_setDepth(int depth, int p2, int p3, int p4)
{
	setFuncName("i_setDepth");
	SWFDisplayItem_setDepth(mhsp_item, depth);
	return 0;
}

EXPORT BOOL WINAPI i_remove(int p1, int p2, int p3, int p4)
{
	setFuncName("i_remove");
	SWFDisplayItem_remove(mhsp_item);
	return 0;
}

EXPORT BOOL WINAPI i_setName(void *p1, char *name, int p3, int p4)
{
	setFuncName("i_setName");
	SWFDisplayItem_setName(mhsp_item, name);
	return 0;
}

EXPORT BOOL WINAPI i_setRatio(float ratio, int p2, int p3, int p4)
{
	setFuncName("i_setRatio");
	SWFDisplayItem_setRatio(mhsp_item, ratio);
	return 0;
}

EXPORT BOOL WINAPI i_addColor(int r, int g, int b, int a)
{
	setFuncName("i_addColor");
	SWFDisplayItem_setColorAdd(mhsp_item, r, g, b, a);
	return 0;
}

EXPORT BOOL WINAPI i_multColor(float r, float g, float b, float a)
{
	setFuncName("i_multColor");
//	if (a == 0) a = 1.0;
	SWFDisplayItem_setColorMult(mhsp_item, r, g, b, a);
	return 0;
}


EXPORT BOOL WINAPI i_setMatrix(float a, float b, float c, float d)
{
	if (setMatrix_flag)
	{
		setFuncName("i_setMatrix");
		SWFDisplayItem_setMatrix(mhsp_item, a, b, c, d, m1, m2);
		setMatrix_flag = 0;
	}
	else {
		m1 = a;
		m2 = b;
		setMatrix_flag = 1;
	}
	return 0;
}

EXPORT BOOL WINAPI i_addAction(PMhspObject action, int flags, int p3, int p4)
{
	setFuncName("i_addAction");
	SWFDisplayItem_addAction(mhsp_item, MINGOBJ(action, SWFAction), flags);
	return 0;
}

#ifndef JAMING
EXPORT BOOL WINAPI i_setMaskLevel(int masklevel, int p2, int p3, int p4)
{
	setFuncName("i_setMaskLevel");
	SWFDisplayItem_setMaskLevel(mhsp_item, masklevel);
	return 0;
}

EXPORT BOOL WINAPI i_endMask(int p1, int p2, int p3, int p4)
{
	setFuncName("i_endMask");
	SWFDisplayItem_endMask(mhsp_item);
	return 0;
}

EXPORT BOOL WINAPI i_getX(float *p1, int p2, int p3, int p4)
{
	setFuncName("i_getX");
	*p1 = SWFDisplayItem_get_x(mhsp_item);
	return 0;
}

EXPORT BOOL WINAPI i_getY(float *p1, int p2, int p3, int p4)
{
	setFuncName("i_getY");
	*p1 = SWFDisplayItem_get_y(mhsp_item);
	return 0;
}

EXPORT BOOL WINAPI i_getXScale(float *p1, int p2, int p3, int p4)
{
	setFuncName("i_getXScale");
	*p1 = SWFDisplayItem_get_xScale(mhsp_item);
	return 0;
}

EXPORT BOOL WINAPI i_getYScale(float *p1, int p2, int p3, int p4)
{
	setFuncName("i_getYScale");
	*p1 = SWFDisplayItem_get_yScale(mhsp_item);
	return 0;
}

EXPORT BOOL WINAPI i_getXSkew(float *p1, int p2, int p3, int p4)
{
	setFuncName("i_getXSkew");
	*p1 = SWFDisplayItem_get_xSkew(mhsp_item);
	return 0;
}

EXPORT BOOL WINAPI i_getYSkew(float *p1, int p2, int p3, int p4)
{
	setFuncName("i_getYSkew");
	*p1 = SWFDisplayItem_get_ySkew(mhsp_item);
	return 0;
}

EXPORT BOOL WINAPI i_getRot(float *p1, int p2, int p3, int p4)
{
	setFuncName("i_getRot");
	*p1 = SWFDisplayItem_get_rot(mhsp_item);
	return 0;
}

/* ming.cming.cには実装されていなかったけれども一応 */
EXPORT BOOL WINAPI i_getDepth(int *p1, int p2, int p3, int p4)
{
	setFuncName("i_getDepth");
	*p1 = SWFDisplayItem_getDepth(mhsp_item);
	return 0;
}
#endif

/* SWFGradient */

EXPORT BOOL WINAPI mhsp_SWFGradient(PMhspObject *p1, int p2, int p3, int p4)
{
	SWFGradient gradient;

	setFuncName("SWFGradient");
	gradient = newSWFGradient();
	if ( !(*p1 = addMhspObject(gradient, T_SWFGradient)) ) {
		return -1;
	}
	if (!mhsp_gradient) {
		mhsp_gradient = gradient;
	}
	return 0;
}

EXPORT BOOL WINAPI g_addEntry(float ratio, int p2, int p3, int p4)
{
	if (addEntry_flag) {
		setFuncName("g_addEntry");
		SWFGradient_addEntry(mhsp_gradient, ratio, p2 & 0xff, p3 & 0xff, p4 & 0xff, e & 0xff);
		addEntry_flag = 0;
	}
	else {
		e = p2;
		addEntry_flag = 1;
	}
	return 0;
}

/* SWFBitmap */

EXPORT BOOL WINAPI mhsp_SWFBitmap(HSPEXINFO *hei, int p2, int p3, int p4)
{
	FILE *fp1, *fp2;
	char filename[MHSP_STRMAX], alpha[MHSP_STRMAX];
	PMhspObject *p1, file1, rel1;
	SWFInput input1;
	SWFBitmap bitmap;

	setFuncName("SWFBitmap");
	p1 = (PMhspObject *)hei->HspFunc_prm_getv();
	lstrcpyn(filename, hei->HspFunc_prm_gets(), MHSP_STRMAX);
	lstrcpyn(alpha, hei->HspFunc_prm_getds(""), MHSP_STRMAX);
	fp1 = fopen(filename, "rb");
	if (!fp1) {
		return 12;
	}
	input1 = newSWFInput_file(fp1);
	if ( !(rel1 = addMhspObject(input1, T_SWFInput)) ) {
		return -1;
	}
	if ( !(file1 = addMhspObject(fp1, T_FILE)) ) {
		destroyMhspObject(rel1);
		return -1;
	}
	appendRelative(rel1, file1);
	fp2 = fopen(alpha, "rb");
	if (fp2 && isJPEG(input1)) {
		PMhspObject file2, rel2;
		SWFInput input2;

		input2 = newSWFInput_file(fp2);
		if ( !(rel2 = addMhspObject(input2, T_SWFInput)) ) {
			destroyMhspObject(rel1);
			return -1;
		}
		if ( !(file2 = addMhspObject(fp2, T_FILE)) ) {
			destroyMhspObject(rel1);
			destroyMhspObject(rel2);
			return -1;
		}
		SWFInput_seek(input1, 0, SEEK_SET);
		bitmap = (SWFBitmap)newSWFJpegWithAlpha_fromInput(input1, input2);
		appendRelative(rel2, file2);
		appendRelative(file1, rel2);
	}
#ifdef JAMING
	else if (isDBL(input1) || isJPEG(input1))
#else
	else if (isDBL(input1) || isJPEG(input1) || isGIF(input1) || isPNG(input1) || isBMP(input1))
#endif
	{
		SWFInput_seek(input1, 0, SEEK_SET);
		bitmap = newSWFBitmap_fromInput(input1);
	}
	else {
		destroyMhspObject(rel1);
		return -1;	/* 非対応フォーマット */
	}
	if ( !(*p1 = addMhspObject(bitmap, T_SWFBitmap)) ) {
		destroyMhspObject(rel1);
		return -1;
	}
	appendRelative(*p1, rel1);
	if (!mhsp_bitmap) {
		mhsp_bitmap = bitmap;
	}
	return 0;
}

EXPORT BOOL WINAPI mhsp_SWFBitmap_buf(HSPEXINFO *hei, int p2, int p3, int p4)
{
	unsigned char *buf, *buf2;
	int size, size2;
	PMhspObject *p1, rel1;
	SWFInput input1;
	SWFBitmap bitmap;

	setFuncName("SWFBitmap_buf");
	p1 = (PMhspObject *)hei->HspFunc_prm_getv();
	buf = (unsigned char *)hei->HspFunc_prm_getv();
	size = hei->HspFunc_prm_geti();
	buf2 = (unsigned char *)hei->HspFunc_prm_getv();
	size2 = hei->HspFunc_prm_getdi(0);
	input1 = newSWFInput_allocedBuffer(buf, size);
	if ( !(rel1 = addMhspObject(input1, T_SWFInput)) ) {
		return -1;
	}
	if (size2 && isJPEG(input1)) {
		PMhspObject rel2;
		SWFInput input2;

		input2 = newSWFInput_allocedBuffer(buf2, size2);
		if ( !(rel2 = addMhspObject(input2, T_SWFInput)) ) {
			destroyMhspObject(rel1);
			return -1;
		}
		SWFInput_seek(input1, 0, SEEK_SET);
		bitmap = (SWFBitmap)newSWFJpegWithAlpha_fromInput(input1, input2);
		appendRelative(rel1, rel2);
	}
#ifdef JAMING
	else if (isDBL(input1) || isJPEG(input1))
#else
	else if (isDBL(input1) || isJPEG(input1) || isGIF(input1) || isPNG(input1) || isBMP(input1))
#endif
	{
		SWFInput_seek(input1, 0, SEEK_SET);
		bitmap = newSWFBitmap_fromInput(input1);
	}
	else {
		destroyMhspObject(rel1);
		return -1;
	}
	if ( !(*p1 = addMhspObject(bitmap, T_SWFBitmap)) ) {
		destroyMhspObject(rel1);
		return -1;
	}
	appendRelative(*p1, rel1);
	if (!mhsp_bitmap) {
		mhsp_bitmap = bitmap;
	}

	return 0;
}

EXPORT BOOL WINAPI bmp_getWidth(int *p1, int p2, int p3, int p4)
{
	setFuncName("bmp_getWidth");
	*p1 = SWFBitmap_getWidth(mhsp_bitmap);

	return 0;
}

EXPORT BOOL WINAPI bmp_getHeight(int *p1, int p2, int p3, int p4)
{
	setFuncName("bmp_getHeight");
	*p1 = SWFBitmap_getHeight(mhsp_bitmap);

	return 0;
}

/* SWFFill */

EXPORT BOOL WINAPI f_moveTo(float x, float y, int p3, int p4)
{
	setFuncName("f_moveTo");
	SWFFill_moveTo(mhsp_fill, x, y);

	return 0;
}

EXPORT BOOL WINAPI f_scaleTo(float x, float y, int p3, int p4)
{
	setFuncName("f_scaleTo");
	SWFFill_scaleTo(mhsp_fill, x, y);

	return 0;
}

EXPORT BOOL WINAPI f_rotateTo(float degrees, int p2, int p3, int p4)
{
	setFuncName("f_rotateTo");
	SWFFill_rotateTo(mhsp_fill, degrees);

	return 0;
}

EXPORT BOOL WINAPI f_skewXTo(float x, int p2, int p3, int p4)
{
	setFuncName("f_skewXTo");
	SWFFill_skewXTo(mhsp_fill, x);

	return 0;
}

EXPORT BOOL WINAPI f_skewYTo(float y, int p2, int p3, int p4)
{
	setFuncName("f_skewYTo");
	SWFFill_skewYTo(mhsp_fill, y);

	return 0;
}

/* SWFMorph */

EXPORT BOOL WINAPI mhsp_SWFMorph(PMhspObject *p1, int p2, int p3, int p4)
{
	SWFMorph morph;

	setFuncName("SWFMorph");
	morph = newSWFMorphShape();
	if ( !(*p1 = addMhspObject(morph, T_SWFMorph)) ) {
		return -1;
	}
	if (!mhsp_morph) {
		mhsp_morph = morph;
	}

	return 0;
}

EXPORT BOOL WINAPI p_getShape1(PMhspObject *p1, int p2, int p3, int p4)
{
	SWFShape shape;

	setFuncName("p_getShape1");
	shape = SWFMorph_getShape1(mhsp_morph);
	if ( !(*p1 = addMhspObject(shape, T_SWFShape)) ) {
		return -1;
	}
	if (!mhsp_shape) {
		mhsp_shape = shape;
	}

	return 0;
}

EXPORT BOOL WINAPI p_getShape2(PMhspObject *p1, int p2, int p3, int p4)
{
	SWFShape shape;

	setFuncName("p_getShape2");
	shape = SWFMorph_getShape2(mhsp_morph);
	if ( !(*p1 = addMhspObject(shape, T_SWFShape)) ) {
		return -1;
	}
	if (!mhsp_shape) {
		mhsp_shape = shape;
	}

	return 0;
}

/* SWFText */

EXPORT BOOL WINAPI mhsp_SWFText(PMhspObject *p1, int p2, int p3, int p4)
{
	SWFText text;

	setFuncName("SWFText");
	text = newSWFText();
	if ( !(*p1 = addMhspObject(text, T_SWFText)) ) {
		return -1;
	}
	if (!mhsp_text) {
		mhsp_text = text;
	}

	return 0;
}

EXPORT BOOL WINAPI t_setFont(PMhspObject font, int p2, int p3, int p4)
{
	setFuncName("t_setFont");
	SWFText_setFont(mhsp_text, MINGOBJ(font, SWFBlock));

	return 0;
}

EXPORT BOOL WINAPI t_setHeight(float height, int p2, int p3, int p4)
{
	setFuncName("t_setHeight");
	SWFText_setHeight(mhsp_text, height);

	return 0;
}

EXPORT BOOL WINAPI t_setSpacing(float spacing, int p2, int p3, int p4)
{
	setFuncName("t_setSpacing");
	SWFText_setSpacing(mhsp_text, spacing);

	return 0;
}

EXPORT BOOL WINAPI t_setColor(HSPEXINFO *hei, int p2, int p3, int p4)
{
	int r, g, b, a;

	setFuncName("t_setColor");
	r = hei->HspFunc_prm_getdi(0);
	g = hei->HspFunc_prm_getdi(0);
	b = hei->HspFunc_prm_getdi(0);
	a = hei->HspFunc_prm_getdi(0xff);
	SWFText_setColor(mhsp_text, r, g, b, a);

	return 0;
}

EXPORT BOOL WINAPI t_moveTo(float x, float y, int p3, int p4)
{
	setFuncName("t_moveTo");
	SWFText_moveTo(mhsp_text, x, y);

	return 0;
}

EXPORT BOOL WINAPI t_addString(void *p1, char *string, int p3, int p4)
{
#ifdef JAMING
	char *out;

	setFuncName("t_addString");
	out = toutf8(string);
	SWFText_addString(mhsp_text, out, NULL);
	free(out);
#else
	setFuncName("t_addString");
	SWFText_addString(mhsp_text, string, NULL);
#endif

	return 0;
}

EXPORT BOOL WINAPI t_getWidth(float *p1, char *string, int p3, int p4)
{
#ifdef JAMING
	char *out;

	setFuncName("t_getWidth");
	out = toutf8(string);
	*p1 = SWFText_getStringWidth(mhsp_text, out);
	free(out);
#else
	setFuncName("t_getWidth");
	*p1 = SWFText_getStringWidth(mhsp_text, (unsigned char *)string);
#endif

	return 0;
}

EXPORT BOOL WINAPI t_getAscent(float *p1, int p2, int p3, int p4)
{
	setFuncName("t_getAscent");
	*p1 = SWFText_getAscent(mhsp_text);

	return 0;
}

EXPORT BOOL WINAPI t_getDescent(float *p1, int p2, int p3, int p4)
{
	setFuncName("t_getDescent");
	*p1 = SWFText_getDescent(mhsp_text);

	return 0;
}
EXPORT BOOL WINAPI t_getLeading(float *p1, int p2, int p3, int p4)
{
	setFuncName("t_getLeading");
	*p1 = SWFText_getLeading(mhsp_text);

	return 0;
}

#ifndef JAMING
EXPORT BOOL WINAPI t_addUTF8String(void *p1, char *string, int p3, int p4)
{
	char *out;

	setFuncName("t_addUTF8String");
	out = toutf8(string);
	SWFText_addUTF8String(mhsp_text, out, NULL);
	free(out);

	return 0;
}

EXPORT BOOL WINAPI t_getUTF8Width(float *p1, char *string, int p3, int p4)
{
	char *out;

	setFuncName("t_getUTF8Width");
	out = toutf8(string);
	*p1 = SWFText_getUTF8StringWidth(mhsp_text, (unsigned char *)out);
	free(out);

	return 0;
}
#endif

/* SWFFont */

EXPORT BOOL WINAPI mhsp_SWFFont(PMhspObject *p1, char *name, int p3, int p4)
{
	FILE *fdbfont = fopen(name, "rb");
	SWFFont font;

	setFuncName("SWFFont");
	if (fdbfont) {
		if (fgetc(fdbfont) != 'f' || fgetc(fdbfont) != 'd' ||
			 fgetc(fdbfont) != 'b' || fgetc(fdbfont) != '0')
		{
			return -1;
		}
		fseek(fdbfont, 0, SEEK_SET);
		font = loadSWFFontFromFile(fdbfont);
		fclose(fdbfont);
		if ( !(*p1 = addMhspObject(font, T_SWFFont)) ) {
			return -1;
		}
	}
	else {
		char *out;
		out = toutf8(name);
		font = (SWFFont)newSWFBrowserFont(out);	/* 本当は違うけど */
		free(out);
		if ( !(*p1 = addMhspObject(font, T_SWFBrowserFont)) ) {
			return -1;
		}
	}
	if (!mhsp_font) {
		mhsp_font = font;
	}

	return 0;
}

EXPORT BOOL WINAPI fnt_getWidth(float *p1, char *string, int p3, int p4)
{
#ifdef JAMING
	char *out;

	setFuncName("fnt_getWidth");
	out = toutf8(string);
	*p1 = SWFFont_getStringWidth(mhsp_font, out);
	free(out);
#else
	setFuncName("fnt_getWidth");
	*p1 = SWFFont_getStringWidth(mhsp_font, (unsigned char *)string);
#endif

	return 0;
}

#ifdef JAMING
EXPORT BOOL WINAPI fnt_setFlags(int flags, int p2, int p3, int p4)
{
	setFuncName("fnt_setFlags");
	SWFBrowserFont_setFlags((SWFBrowserFont)mhsp_font, (byte)flags);

	return 0;
}
#endif

EXPORT BOOL WINAPI fnt_getAscent(float *p1, int p2, int p3, int p4)
{
	setFuncName("fnt_getAscent");
	*p1 = SWFFont_getAscent(mhsp_font);

	return 0;
}

EXPORT BOOL WINAPI fnt_getDescent(float *p1, int p2, int p3, int p4)
{
	setFuncName("fnt_getDescent");
	*p1 = SWFFont_getDescent(mhsp_font);

	return 0;
}

EXPORT BOOL WINAPI fnt_getLeading(float *p1, int p2, int p3, int p4)
{
	setFuncName("fnt_getLeading");
	*p1 = SWFFont_getLeading(mhsp_font);

	return 0;
}

#ifndef JAMING
EXPORT BOOL WINAPI fnt_getUTF8Width(float *p1, char *string, int p3, int p4)
{
	char *out;

	setFuncName("fnt_getUTF8Width");
	out = toutf8(string);
	*p1 = SWFFont_getUTF8StringWidth(mhsp_font, (unsigned char *)out);
	free(out);

	return 0;
}

EXPORT BOOL WINAPI fnt_getShape(char **p1, int c, int p3, int p4)
{
	setFuncName("fnt_getShape");
	*p1 = SWFFont_getShape(mhsp_font, c);	/* ポインタなのでヘッダでの対策が必要 */
	addMhspObject(*p1, T_DATA);

	return 0;
}
#endif

/* SWFTextField */

EXPORT BOOL WINAPI mhsp_SWFTextField(PMhspObject *p1, int flags, int p3, int p4)
{
	SWFTextField field;

	setFuncName("SWFTextField");
	field = newSWFTextField();
	if ( !(*p1 = addMhspObject(field, T_SWFTextField)) ) {
		return -1;
	}
	SWFTextField_setFlags(field, flags);
	if (!mhsp_field) {
		mhsp_field = field;
	}

	return 0;
}

EXPORT BOOL WINAPI tf_setFont(PMhspObject font, int p2, int p3, int p4)
{
	setFuncName("tf_setFont");
	SWFTextField_setFont(mhsp_field, MINGOBJ(font, SWFBlock));
	return 0;
}

EXPORT BOOL WINAPI tf_setBounds(float width, float height, int p3, int p4)
{
	setFuncName("tf_setBounds");
	SWFTextField_setBounds(mhsp_field, width, height);
	return 0;
}

EXPORT BOOL WINAPI tf_align(int alignment, int p2, int p3, int p4)
{
	setFuncName("tf_align");
	SWFTextField_setAlignment(mhsp_field, alignment);
	return 0;
}

EXPORT BOOL WINAPI tf_setHeight(float height, int p2, int p3, int p4)
{
	setFuncName("tf_setHeight");
	SWFTextField_setHeight(mhsp_field, height);
	return 0;
}

EXPORT BOOL WINAPI tf_setLeftMargin(float leftMargin, int p2, int p3, int p4)
{
	setFuncName("tf_setLeftMargin");
	SWFTextField_setLeftMargin(mhsp_field, leftMargin);

	return 0;
}

EXPORT BOOL WINAPI tf_setRightMargin(float rightMargin, int p2, int p3, int p4)
{
	setFuncName("tf_setRightMargin");
	SWFTextField_setRightMargin(mhsp_field, rightMargin);

	return 0;
}

EXPORT BOOL WINAPI tf_setMargins(float left, float right, int p3, int p4)
{
	setFuncName("tf_setMargins");
	SWFTextField_setLeftMargin(mhsp_field, left);
	SWFTextField_setRightMargin(mhsp_field, right);

	return 0;
}

EXPORT BOOL WINAPI tf_setIndentation(float indentation, int p2, int p3, int p4)
{
	setFuncName("tf_setIndentation");
	SWFTextField_setIndentation(mhsp_field, indentation);

	return 0;
}

EXPORT BOOL WINAPI tf_setLineSpacing(float lineSpacing, int p2, int p3, int p4)
{
	setFuncName("tf_setLineSpacing");
	SWFTextField_setLineSpacing(mhsp_field, lineSpacing);

	return 0;
}

EXPORT BOOL WINAPI tf_setColor(HSPEXINFO *hei, int p2, int p3, int p4)
{
	int r, g, b, a;

	setFuncName("tf_setColor");
	r = hei->HspFunc_prm_getdi(0);
	g = hei->HspFunc_prm_getdi(0);
	b = hei->HspFunc_prm_getdi(0);
	a = hei->HspFunc_prm_getdi(0xff);
	SWFTextField_setColor(mhsp_field, r, g, b, a);

	return 0;
}

EXPORT BOOL WINAPI tf_setName(void *p1, char *name, int p3, int p4)
{
	setFuncName("tf_setName");
	SWFTextField_setVariableName(mhsp_field, name);

	return 0;
}

EXPORT BOOL WINAPI tf_addString(void *p1, char *string, int p3, int p4)
{
#ifdef JAMING
	char *out;

	setFuncName("tf_addString");
	out = toutf8(string);
	SWFTextField_addString(mhsp_field, out);
	free(out);
#else
	setFuncName("tf_addString");
	SWFTextField_addString(mhsp_field, string);
#endif

	return 0;
}

EXPORT BOOL WINAPI tf_setLength(int length, int p2, int p3, int p4)
{
	setFuncName("tf_setLength");
	SWFTextField_setLength(mhsp_field, length);

	return 0;
}

#ifndef JAMING
EXPORT BOOL WINAPI tf_setPadding(float padding, int p2, int p3, int p4)
{
	setFuncName("tf_setPadding");
	SWFTextField_setPadding(mhsp_field, padding);

	return 0;
}

EXPORT BOOL WINAPI tf_addUTF8String(void *p1, char *string, int p3, int p4)
{
	char *out;

	setFuncName("tf_addUTF8String");
	out = toutf8(string);
	SWFTextField_addUTF8String(mhsp_field, out);
	free(out);

	return 0;
}

EXPORT BOOL WINAPI tf_addChars(void *p1, char *string, int p3, int p4)
{
	setFuncName("tf_addChars");
	SWFTextField_addChars(mhsp_field, string);

	return 0;
}
/*
EXPORT BOOL WINAPI tf_addUTF8Chars(void *p1, char *string, int p3, int p4)
{
	char *out;
	setFuncName("tf_addUTF8Chars");
	out = toutf8(string);
	SWFTextField_addUTF8Chars(mhsp_field, out);
	free(out);
	return 0;
}
*/
#endif

/* SWFMovieClip */

EXPORT BOOL WINAPI mhsp_SWFMovieClip(PMhspObject *p1, int p2, int p3, int p4)
{
	SWFMovieClip clip;

	setFuncName("SWFMovieClip");
	clip = newSWFMovieClip();
	if ( !(*p1 = addMhspObject(clip, T_SWFMovieClip)) ) {
		return -1;
	}
	if (!mhsp_movieclip) {
		mhsp_movieclip = clip;
	}

	return 0;
}

EXPORT BOOL WINAPI mc_add(PMhspObject *p1, PMhspObject block, int p3, int p4)
{
	SWFDisplayItem item;

	setFuncName("mc_add");
	item = SWFMovieClip_add(mhsp_movieclip, MINGOBJ(block, SWFBlock));
	if ( !(*p1 = addMhspObject(item, T_SWFDisplayItem)) ) {
		return -1;
	}
	SETNODESTFLAG(*p1);
	if (!mhsp_item) {
		mhsp_item = item;
	}
	return 0;
}

EXPORT BOOL WINAPI mc_remove(PMhspObject item, int p2, int p3, int p4)
{
	setFuncName("mc_remove");
	SWFMovieClip_remove(mhsp_movieclip, MINGOBJ(item, SWFDisplayItem));
	return 0;
}

EXPORT BOOL WINAPI mc_nextFrame(int p1, int p2, int p3, int p4)
{
	setFuncName("mc_nextFrame");
	SWFMovieClip_nextFrame(mhsp_movieclip);
	return 0;
}

EXPORT BOOL WINAPI mc_labelFrame(void *p1, char *label, int p3, int p4)
{
	setFuncName("mc_labelFrame");
	SWFMovieClip_labelFrame(mhsp_movieclip, label);
	return 0;
}

EXPORT BOOL WINAPI mc_setFrames(int totalFrames, int p2, int p3, int p4)
{
	setFuncName("mc_setFrames");
	SWFMovieClip_setNumberOfFrames(mhsp_movieclip, totalFrames);
	return 0;
}

#ifdef JAMING
EXPORT BOOL WINAPI mc_addSound(SWFDisplayItem *p1, char *file, int flags, int p4)
{
	FILE *fp = fopen(file, "rb");
	if (!fp) {
		return 12;
	}
	setFuncName("mc_addSound");
	*p1 = SWFMovieClip_add(mhsp_movieclip, newSWFSound(fp, (byte)flags));
	if (!mhsp_item)
		mhsp_item = *p1;
	return 0;
}

EXPORT BOOL WINAPI mc_addSound_buf(HSPEXINFO *hei, int p2, int p3, int p4)
{
	unsigned char *buf;
	int size;
	byte flags;
	SWFDisplayItem *p1;
	setFuncName("mc_addSound_buf");
	p1 = (SWFDisplayItem *)hei->HspFunc_prm_getv();
	buf = (unsigned char *)hei->HspFunc_prm_getv();
	size = hei->HspFunc_prm_geti();
	flags = hei->HspFunc_prm_getdi(0);
	*p1 = SWFMovieClip_add(mhsp_movieclip, newSWFSound_fromInput(newSWFInput_allocedBuffer(buf, size), flags));
	if (!mhsp_item) {
		mhsp_item = *p1;
	}
	return 0;
}

EXPORT BOOL WINAPI mc_startSound(SWFDisplayItem item, int p2, int p3, int p4)
{
	setFuncName("mc_startSound");
	SWFSprite_startSound(mhsp_movieclip, item);
	return 0;
}

EXPORT BOOL WINAPI mc_stopSound(SWFDisplayItem item, int p2, int p3, int p4)
{
	setFuncName("mc_stopSound");
	SWFSprite_stopSound(mhsp_movieclip, item);
	return 0;
}
#else
EXPORT BOOL WINAPI mc_startSound(PMhspObject *p1, PMhspObject sound, int p3, int p4)
{
	SWFSoundInstance inst;

	setFuncName("mc_startSound");
	inst = SWFMovieClip_startSound(mhsp_movieclip, MINGOBJ(sound, SWFSound));
	if ( !(*p1 = addMhspObject(inst, T_SWFSoundInstance)) ) {
		return -1;
	}
	SETNODESTFLAG(*p1);
	if (!mhsp_instance) {
		mhsp_instance = inst;
	}
	return 0;
}

EXPORT BOOL WINAPI mc_stopSound(PMhspObject sound, int p2, int p3, int p4)
{
	setFuncName("mc_stopSound");
	SWFMovieClip_stopSound(mhsp_movieclip, MINGOBJ(sound, SWFSound));
	return 0;
}
#endif

/* SWFButton */

EXPORT BOOL WINAPI mhsp_SWFButton(PMhspObject *p1, int p2, int p3, int p4)
{
	SWFButton button;

	setFuncName("SWFButton");
	button = newSWFButton();
	if ( !(*p1 = addMhspObject(button, T_SWFButton)) ) {
		return -1;
	}
	if (!mhsp_button) {
		mhsp_button = button;
	}

	return 0;
}

EXPORT BOOL WINAPI b_addShape(PMhspObject character, byte flags, int p3, int p4)
{
	setFuncName("b_addShape");
	SWFButton_addShape(mhsp_button, MINGOBJ(character, SWFCharacter), flags);

	return 0;
}

EXPORT BOOL WINAPI b_setUp(PMhspObject character, int p2, int p3, int p4)
{
	setFuncName("b_setUp");
	SWFButton_addShape(mhsp_button, MINGOBJ(character, SWFCharacter), SWFBUTTON_UP);

	return 0;
}

EXPORT BOOL WINAPI b_setOver(PMhspObject character, int p2, int p3, int p4)
{
	setFuncName("b_setOver");
	SWFButton_addShape(mhsp_button, MINGOBJ(character, SWFCharacter), SWFBUTTON_OVER);

	return 0;
}

EXPORT BOOL WINAPI b_setDown(PMhspObject character, int p2, int p3, int p4)
{
	setFuncName("b_setDown");
	SWFButton_addShape(mhsp_button, MINGOBJ(character, SWFCharacter), SWFBUTTON_DOWN);

	return 0;
}

EXPORT BOOL WINAPI b_setHit(PMhspObject character, int p2, int p3, int p4)
{
	setFuncName("b_setHit");
	SWFButton_addShape(mhsp_button, MINGOBJ(character, SWFCharacter), SWFBUTTON_HIT);

	return 0;
}

/* 追加したあとにactionを削除できるか？ */
EXPORT BOOL WINAPI b_addAction(PMhspObject action, int flags, int p3, int p4)
{
	setFuncName("b_addAction");
	SWFButton_addAction(mhsp_button, MINGOBJ(action, SWFAction), flags);

	return 0;
}

EXPORT BOOL WINAPI b_setAction(PMhspObject action, int p2, int p3, int p4)
{
	setFuncName("b_setAction");
	SWFButton_addAction(mhsp_button, MINGOBJ(action, SWFAction), SWFBUTTON_OVERDOWNTOOVERUP);

	return 0;
}

#ifndef JAMING
EXPORT BOOL WINAPI b_setMenu(int flag, int p2, int p3, int p4)
{
	setFuncName("b_setMenu");
	SWFButton_setMenu(mhsp_button, flag);

	return 0;
}

EXPORT BOOL WINAPI b_addSound(PMhspObject sound, int flag, int p3, int p4)
{
	setFuncName("b_addSound");
	SWFButton_addSound(mhsp_button, MINGOBJ(sound, SWFSound), flag);

	return 0;
}

/* SWFVideoStream */

EXPORT BOOL WINAPI mhsp_SWFVideoStream(PMhspObject *p1, char *filename, int p3, int p4)
{
	MhspObject *rel = NULL;
	SWFVideoStream stream;

	setFuncName("SWFVideoStream");
	if (*filename == '\0') {
		stream = newSWFVideoStream();
	}
	else {
		FILE *fp;

		fp = fopen(filename, "rb");
		if (!fp) {
			return 12;
		}
		if ( !(rel = allocMhspObject(fp, T_FILE)) ) {
			return -1;
		}
		stream = newSWFVideoStream_fromFile(fp);
	}
	if ( !(*p1 = addMhspObject(stream, T_SWFVideoStream)) ) {
		destroyMhspObject(rel);
		return -1;
	}
	appendRelative(*p1, rel);
	if (!mhsp_stream) {
		mhsp_stream = stream;
	}

	return 0;
}

EXPORT BOOL WINAPI v_setDimension(int width, int height, int p3, int p4)
{
	setFuncName("v_setDimension");
	SWFVideoStream_setDimension(mhsp_stream, width, height);
	return 0;
}

EXPORT BOOL WINAPI v_getNumFrames(int *p1, int p2, int p3, int p4)
{
	setFuncName("v_getNumFrames");
	*p1 = SWFVideoStream_getNumFrames(mhsp_stream);

	return 0;
}

/* SWFFontCharacter */

EXPORT BOOL WINAPI c_addChars(void *p1, char *string, int p3, int p4)
{
	setFuncName("c_addChars");
	SWFFontCharacter_addChars(mhsp_fontchar, string);

	return 0;
}

EXPORT BOOL WINAPI c_addUTF8Chars(void *p1, char *string, int p3, int p4)
{
	char *out;

	setFuncName("c_addUTF8Chars");
	out = toutf8(string);
	SWFFontCharacter_addUTF8Chars(mhsp_fontchar, out);
	free(out);

	return 0;
}

/* SWFSound */

EXPORT BOOL WINAPI mhsp_SWFSound(PMhspObject *p1, char *file, byte flags, int p4)
{
	PMhspObject rel;
	SWFSound sound;
	FILE *fp = fopen(file, "rb");

	if (!fp) {
		return 12;
	}
	if ( !(rel = allocMhspObject(fp, T_FILE)) ) {
		return -1;
	}
	setFuncName("SWFSound");
	sound = newSWFSound(fp, flags);
	if ( !(*p1 = addMhspObject(sound, T_SWFSound)) ) {
		destroyMhspObject(rel);
		return -1;
	}
	appendRelative(*p1, rel);

	return 0;
}

EXPORT BOOL WINAPI mhsp_SWFSound_buf(HSPEXINFO *hei, int p2, int p3, int p4)
{
	unsigned char *buf;
	int size;
	byte flags;
	PMhspObject rel, *p1;
	SWFInput input;
	SWFSound sound;

	setFuncName("SWFSound_buf");
	p1 = (PMhspObject *)hei->HspFunc_prm_getv();
	buf = (unsigned char *)hei->HspFunc_prm_getv();
	size = hei->HspFunc_prm_geti();
	flags = (byte)hei->HspFunc_prm_getdi(0);
	input = newSWFInput_allocedBuffer(buf, size);
	if ( !(rel = allocMhspObject(input, T_SWFInput)) ) {
		return -1;
	}
	sound = (SWFSound)newSWFSound_fromInput(input, flags);
	if ( !(*p1 = addMhspObject(sound, T_SWFSound)) ) {
		destroyMhspObject(rel);
		return -1;
	}
	appendRelative(*p1, rel);

	return 0;
}

/* SWFSoundInstance */

EXPORT BOOL WINAPI si_noMultiple(int p1, int p2, int p3, int p4)
{
	setFuncName("si_noMultiple");
	SWFSoundInstance_setNoMultiple(mhsp_instance);
	return 0;
}

EXPORT BOOL WINAPI si_loopInPoint(int point, int p2, int p3, int p4)
{
	setFuncName("si_loopInPoint");
	SWFSoundInstance_setLoopInPoint(mhsp_instance, point);
	return 0;
}

EXPORT BOOL WINAPI si_loopOutPoint(int point, int p2, int p3, int p4)
{
	setFuncName("si_loopOutPoint");
	SWFSoundInstance_setLoopOutPoint(mhsp_instance, point);
	return 0;
}

EXPORT BOOL WINAPI si_loopCount(int count, int p2, int p3, int p4)
{
	setFuncName("si_loopCount");
	SWFSoundInstance_setLoopCount(mhsp_instance, count);
	return 0;
}

/* SWFPrebuiltClip */

EXPORT BOOL WINAPI mhsp_SWFPrebuiltClip(PMhspObject *p1, const char *filename, int p3, int p4)
{
	SWFPrebuiltClip clip;

	setFuncName("SWFPrebuiltClip");
	clip = newSWFPrebuiltClip_fromFile(filename);
	if ( !(*p1 = addMhspObject(clip, T_SWFPrebuiltClip)) ) {
	}
	return 0;
}
#endif

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
	if (p3 == 0) {
		return -1;
	}
	*p1 = p2 / p3;
	return 0;
}

/* ダサい方法 */
EXPORT BOOL WINAPI mhsp_float(float *p1, int p2, int p3, int p4)
{
	*p1 = (float)p2;
	return 0;
}

/* ダサい方法 */
EXPORT BOOL WINAPI mhsp_int(int *p1, float p2, int p3, int p4)
{
	*p1 = (int)p2;
	return 0;
}

/* ダサい方法 */
EXPORT BOOL WINAPI mhsp_f2d(double *p1, float p2, int p3, int p4)
{
	*p1 = (double)p2;
	return 0;
}

/* utility */

EXPORT BOOL WINAPI mhsp_cleanup(int p1, int p2, int p3, int p4)
{
	mhsp_list_clear(&mhspObjects);
	mhsp_initselection(0, 0, 0, 0);

	return 0;
}

EXPORT BOOL WINAPI mhsp_setwarnbuf(void *p1, int p2, int p3, int p4)
{
	setWarnBuf(p1, p2);
	return 0;
}

EXPORT BOOL WINAPI mhsp_sel(MhspObject *object, int p2, int p3, int p4)
{
	switch (object->type) {
		case T_SWFShape:
			mhsp_shape = (SWFShape)object->realObject;
			break;
		case T_SWFMovie:
			mhsp_movie = (SWFMovie)object->realObject;
			break;
		case T_SWFDisplayItem:
			mhsp_item = (SWFDisplayItem)object->realObject;
			break;
		case T_SWFMovieClip:
		case T_SWFCharacter:
			mhsp_movieclip = (SWFMovieClip)object->realObject;
			break;
		case T_SWFGradient:
			mhsp_gradient = (SWFGradient)object->realObject;
			break;
		case T_SWFBitmap:
			mhsp_bitmap = (SWFBitmap)object->realObject;
			break;
		case T_SWFFill:
			mhsp_fill = (SWFFill)object->realObject;
			break;
		case T_SWFMorph:
			mhsp_morph = (SWFMorph)object->realObject;
			break;
		case T_SWFText:
			mhsp_text = (SWFText)object->realObject;
			break;
		case T_SWFFont:
		case T_SWFBrowserFont:
			mhsp_font = (SWFFont)object->realObject;
			break;
		case T_SWFTextField:
			mhsp_field = (SWFTextField)object->realObject;
			break;
		case T_SWFButton:
			mhsp_button = (SWFButton)object->realObject;
			break;
		case T_SWFVideoStream:
			mhsp_stream = (SWFVideoStream)object->realObject;
			break;
		case T_SWFFontCharacter:
			mhsp_fontchar = (SWFFontCharacter)object->realObject;
			break;
		case T_SWFSoundInstance:
			mhsp_instance = (SWFSoundInstance)object->realObject;
			break;
		default:
			return -1;
	}
	return 0;
}

EXPORT BOOL WINAPI mhsp_destroy(MhspObject *object, int p2, int p3, int p4)
{
	mhsp_list_remove(&mhspObjects, object);
	destroyMhspObject(object);

	return 0;
}

EXPORT BOOL WINAPI mhsp_isalive(MhspObject *object, int p2, int p3, int p4)
{
	return mhsp_list_member(&mhspObjects, object);
}
