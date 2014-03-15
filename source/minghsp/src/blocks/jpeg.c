/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

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

/* $Id: jpeg.c,v 1.15 2004/06/27 07:18:56 whamann Exp $ */

#include <stdlib.h>

#include "jpeg.h"
#include "character.h"
#include "input.h"
#include "error.h"
#include "method.h"
#include "libming.h"


struct SWFJpegBitmap_s
{
	struct SWFCharacter_s character;

	SWFInput input;
	int length;

#if TRACK_ALLOCS
	/* memory node for garbage collection */
	mem_node *gcnode;
#endif
};

struct SWFJpegWithAlpha_s
{
	struct SWFCharacter_s character;

	SWFInput input;	 /* leave these here so that we */
	int length;			 /* can cast this to swfJpegBitmap */

	SWFInput alpha;
	int jpegLength;
};

/* JPEG stream markers: */
#define JPEG_MARKER 0xFF

/* Start of Image, End of Image */
#define JPEG_SOI	0xD8
#define JPEG_EOI	0xD9

#define JPEG_JFIF 0xE0

/* encoding markers, quantization tables and Huffman tables */
#define JPEG_QUANT 0xDB
#define JPEG_HUFF	 0xC4

/* image markers, start of frame and start of scan */
#define JPEG_SOF0 0xC0
#define JPEG_SOF1 0xC1
#define JPEG_SOF2 0xC2
#define JPEG_SOS	0xDA

#define JPEG_ED		0xED /* app13 */
#define JPEG_EE		0xEE /* app14 */
#define JPEG_DD		0xDD /* ??? */


static int
completeSWFJpegBitmap(SWFBlock block)
{
	return ((SWFJpegBitmap)block)->length;
}



/* clumsy utility function.. */
void
dumpJpegBlock(byte type,
							SWFInput input, SWFByteOutputMethod method, void *data)
{
	int i, l0, l1, length;

	method(JPEG_MARKER, data);
	method(type, data);

	method((unsigned char)(l0 = SWFInput_getChar(input)), data);
	method((unsigned char)(l1 = SWFInput_getChar(input)), data);

	length = (l0<<8) + l1 - 2;

	for ( i=0; i<length; ++i )
		method((unsigned char)SWFInput_getChar(input), data);
}


int
skipJpegBlock(SWFInput input)
{
	int length = (SWFInput_getChar(input)<<8) + SWFInput_getChar(input);

	SWFInput_seek(input, length-2, SEEK_CUR);

	return length;
}


void
methodWriteJpegFile(SWFInput input, SWFByteOutputMethod method, void *data)
{
	int c;

	SWFInput_rewind(input);

	if ( (c = SWFInput_getChar(input)) != JPEG_MARKER )
		SWF_error("Initial Jpeg marker not found!");

	if ( (c = SWFInput_getChar(input)) != JPEG_SOI )
		SWF_error("Jpeg SOI not found!");

	method(JPEG_MARKER, data);
	method(JPEG_SOI, data);

	/* write compression table at first */

	for ( ;; )
	{
		if ( SWFInput_getChar(input) != JPEG_MARKER )
			SWF_error("Jpeg marker not found where expected!");

		switch ( c = SWFInput_getChar(input) )
		{
			case JPEG_SOS:
			case JPEG_EOI:
				break;

			case JPEG_QUANT:
			case JPEG_HUFF:
			case JPEG_DD:
				dumpJpegBlock((unsigned char)c, input, method, data);
				break;

			default:
				skipJpegBlock(input);
		}

		if ( c == JPEG_SOS || c == JPEG_EOI)
			break;

		if ( SWFInput_eof(input) )
			SWF_error("Unexpected end of Jpeg file (EOF found)!");
	}

	method(JPEG_MARKER, data);
	method(JPEG_EOI, data);

	if (c == JPEG_EOI) {
		/* do nothing */
	} else {
		SWFInput_rewind(input);
	}

	/* write image data at next stage */

	if ( (c = SWFInput_getChar(input)) != JPEG_MARKER )
		SWF_error("Initial Jpeg marker not found!");

	if ( (c = SWFInput_getChar(input)) != JPEG_SOI )
		SWF_error("Jpeg SOI not found!");

	method(JPEG_MARKER, data);
	method(JPEG_SOI, data);

	for ( ;; )
	{
		if ( SWFInput_getChar(input) != JPEG_MARKER )
			SWF_error("Jpeg marker not found where expected!");

		switch ( c = SWFInput_getChar(input) )
		{
			case JPEG_EOI:
				SWF_error("Unexpected end of Jpeg file (EOI found)!");

			case JPEG_QUANT:
			case JPEG_HUFF:
			case JPEG_DD:
				skipJpegBlock(input);
				break;

			case JPEG_JFIF:
			case JPEG_SOF0:
			case JPEG_SOF1:
			case JPEG_SOF2:
				dumpJpegBlock((unsigned char)c, input, method, data);
				break;

			case JPEG_SOS:
				break;

			default:
				/* dumpJpegBlock(c, input, method, data); */
				skipJpegBlock(input);
		}

		if ( c == JPEG_SOS )
			break;

		if ( SWFInput_eof(input) )
			SWF_error("Unexpected end of Jpeg file (EOF found)!");
	}

	if ( c != JPEG_SOS )
		SWF_error("SOS block not found in Jpeg file!");

	/* rest is SOS, dump to end of file */
	method(JPEG_MARKER, data);
	method((unsigned char)c, data);

	while ( (c = SWFInput_getChar(input)) != EOF )
		method((unsigned char)c, data);
}


void
writeSWFJpegBitmapToMethod(SWFBlock block, 
													 SWFByteOutputMethod method, void *data)
{
	SWFJpegBitmap jpeg = (SWFJpegBitmap)block;

	methodWriteUInt16(CHARACTERID(jpeg), method, data);
	methodWriteJpegFile(jpeg->input, method, data);
}


void
writeSWFJpegWithAlphaToMethod(SWFBlock block,
															SWFByteOutputMethod method, void *data)
{
	SWFJpegWithAlpha jpeg = (SWFJpegWithAlpha)block;
	int c;

	methodWriteUInt16(CHARACTERID(jpeg), method, data);
	methodWriteUInt32(jpeg->jpegLength, method, data);
	methodWriteJpegFile(jpeg->input, method, data);

	/* now write alpha file.. */

	SWFInput_rewind(jpeg->alpha);

	while ( (c = SWFInput_getChar(jpeg->alpha)) != EOF )
		method((unsigned char)c, data);
}


void
destroySWFJpegBitmap(SWFJpegBitmap jpegBitmap)
{
	free(CHARACTER(jpegBitmap)->bounds);
#if TRACK_ALLOCS
	ming_gc_remove_node(jpegBitmap->gcnode);
#endif
	free(jpegBitmap);
}


struct jpegInfo
{
	int width;
	int height;
	int length;
};

static struct jpegInfo*
scanJpegFile(SWFInput input)
{
	int length = 0, l, c;
	long pos, end;

	struct jpegInfo* info = (struct jpegInfo*)malloc(sizeof(struct jpegInfo));

	/* scan file, get height and width, make sure it looks valid,
		 also figure length of block.. */

	if ( SWFInput_getChar(input) != JPEG_MARKER )
		SWF_error("Initial Jpeg marker not found!");

	if ( SWFInput_getChar(input) != JPEG_SOI )
		SWF_error("Jpeg SOI not found!");

	for ( ;; )
	{
		if ( SWFInput_getChar(input) != JPEG_MARKER )
			SWF_error("Jpeg marker not found where expected!");

		switch ( c = SWFInput_getChar(input) )
		{
			case JPEG_EOI:
				SWF_error("Unexpected end of Jpeg file (EOI found)!");

			case JPEG_JFIF:
			case JPEG_QUANT:
			case JPEG_HUFF:
			case JPEG_DD:
				/*
				if(finishedEncoding)
					SWF_error("Encoding tables found in Jpeg image section!");
				*/
				length += skipJpegBlock(input) + 2;
				break;

			case JPEG_SOF2:
				SWF_error("Only baseline (frame 0) jpegs are supported!");

			case JPEG_SOF0:
			case JPEG_SOF1:
				/*
				if ( finishedEncoding )
					SWF_error("Found second SOF in Jpeg file!");
				else
				{
					finishedEncoding = TRUE;
					length += 4; // end image, start image
				}
				*/

				l = SWFInput_getUInt16_BE(input);
				SWFInput_getChar(input); /* precision */
				info->height = SWFInput_getUInt16_BE(input);
				info->width = SWFInput_getUInt16_BE(input);

				length += l + 2;
				l -= 7;
				
				SWFInput_seek(input, l, SEEK_CUR);

				break;

			case JPEG_SOS:
				/*
					if(!finishedEncoding)
					SWF_error("Found SOS before SOF in Jpeg file!");
				*/
				break;

			default:
				/* length += */
				skipJpegBlock(input); /* + 2 */
		}

		if ( c == JPEG_SOS )
			break;

		if ( SWFInput_eof(input) )
			SWF_error("Unexpected end of Jpeg file (EOF found)!");
	}

	if ( c != JPEG_SOS )
		SWF_error("SOS block not found in Jpeg file!");

	/* rest is SOS, dump to end of file */

	length += 2; /* SOS tag */

	/* figure out how long the rest of the file is */
	pos = SWFInput_tell(input);
	SWFInput_seek(input, 0, SEEK_END);
	end = SWFInput_tell(input);

	length += end - pos;

	length += 4; /* additional EOI and SOI */

	info->length = length;

	return info;
}


SWFJpegBitmap
newSWFJpegBitmap_fromInput(SWFInput input)
{
	SWFJpegBitmap jpeg;
	struct jpegInfo *info;

	jpeg = (SWFJpegBitmap) malloc(sizeof(struct SWFJpegBitmap_s));

	SWFCharacterInit((SWFCharacter)jpeg);

	CHARACTERID(jpeg) = ++SWF_gNumCharacters;

	BLOCK(jpeg)->writeBlock = writeSWFJpegBitmapToMethod;
	BLOCK(jpeg)->complete = completeSWFJpegBitmap;
	BLOCK(jpeg)->dtor = (destroySWFBlockMethod) destroySWFJpegBitmap;
	BLOCK(jpeg)->type = SWF_DEFINEBITSJPEG2;

	jpeg->input = input;

	info = scanJpegFile(input);

	CHARACTER(jpeg)->bounds = newSWFRect(0, info->width, 0, info->height);

	jpeg->length = info->length + 4;

	free(info);

#if TRACK_ALLOCS
	jpeg->gcnode = ming_gc_add_node(jpeg, (dtorfunctype) destroySWFBitmap);
#endif

	return jpeg;
}


static void
destroySWFJpegBitmap_andInputs(SWFJpegBitmap jpegBitmap)
{
	destroySWFInput(jpegBitmap->input);
	destroySWFJpegBitmap(jpegBitmap);
}


SWFJpegBitmap
newSWFJpegBitmap(FILE *f)
{
	SWFJpegBitmap jpeg = newSWFJpegBitmap_fromInput(newSWFInput_file(f));
	BLOCK(jpeg)->dtor = (destroySWFBlockMethod) destroySWFJpegBitmap_andInputs;
	return jpeg;
}


/* f is a jpeg file, alpha is zlib-compressed data */

SWFJpegWithAlpha
newSWFJpegWithAlpha_fromInput(SWFInput input, SWFInput alpha)
{
	SWFJpegWithAlpha jpeg;
	struct jpegInfo *info;
	int alen;

	jpeg = (SWFJpegWithAlpha) malloc(sizeof(struct SWFJpegWithAlpha_s));

	SWFCharacterInit((SWFCharacter)jpeg);

	CHARACTERID(jpeg) = ++SWF_gNumCharacters;

	BLOCK(jpeg)->writeBlock = writeSWFJpegWithAlphaToMethod;
	BLOCK(jpeg)->complete = completeSWFJpegBitmap; // can use same complete
	BLOCK(jpeg)->dtor = (destroySWFBlockMethod) destroySWFJpegBitmap;			 // ditto here
	BLOCK(jpeg)->type = SWF_DEFINEBITSJPEG3;

	jpeg->input = input;
	jpeg->alpha = alpha;

	info = scanJpegFile(input);

	CHARACTER(jpeg)->bounds = newSWFRect(0, info->width, 0, info->height);

	jpeg->jpegLength = info->length + 2; /* ?? */

	free(info);

	if ( (alen = SWFInput_length(alpha)) == -1 )
		SWF_error("couldn't get alpha file length!");

	jpeg->length = jpeg->jpegLength + alen + 6;

	return jpeg;
}


void
destroySWFJpegAlpha_andInputs(SWFJpegWithAlpha jpegWithAlpha)
{
	destroySWFInput(jpegWithAlpha->input);
	destroySWFInput(jpegWithAlpha->alpha);
	destroySWFJpegBitmap((SWFJpegBitmap) jpegWithAlpha);
}


SWFJpegWithAlpha
newSWFJpegWithAlpha(FILE *f, FILE *alpha)
{
	SWFJpegWithAlpha jpeg =
		newSWFJpegWithAlpha_fromInput(newSWFInput_file(f),
																	newSWFInput_file(alpha));

	BLOCK(jpeg)->dtor = (destroySWFBlockMethod) destroySWFJpegAlpha_andInputs;
	return jpeg;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
