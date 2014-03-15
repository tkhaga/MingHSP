/*
    MingHSP - Ming wrapper for HSP
    THAGA http://www.h5.dion.ne.jp/~markent/
*/

#include <stdio.h>

#include "blocks/input.h"

/* JPEG stream markers: */
#define JPEG_MARKER 0xFF

/* Start of Image, End of Image */
#define JPEG_SOI  0xD8
#define JPEG_EOI  0xD9

#define JPEG_JFIF 0xE0

/* encoding markers, quantization tables and Huffman tables */
#define JPEG_QUANT 0xDB
#define JPEG_HUFF  0xC4

/* image markers, start of frame and start of scan */
#define JPEG_SOF0 0xC0
#define JPEG_SOF1 0xC1
#define JPEG_SOF2 0xC2
#define JPEG_SOS  0xDA

#define JPEG_ED   0xED /* app13 */
#define JPEG_EE   0xEE /* app14 */
#define JPEG_DD   0xDD /* ??? */

void skipJpegBlock2(SWFInput input)
{
  int length = (SWFInput_getChar(input)<<8) + SWFInput_getChar(input);

  SWFInput_seek(input, length-2, SEEK_CUR);

  return;
}

int isJPEG(SWFInput input)
{
	int c;

	SWFInput_seek(input, 0, SEEK_SET);

	if (SWFInput_getChar(input) != JPEG_MARKER)
		return 0;
	if (SWFInput_getChar(input) != JPEG_SOI)
		return 0;

	for (;;)
	{
		if (SWFInput_getChar(input) != JPEG_MARKER)
			return 0;
		switch (c = SWFInput_getChar(input))
		{
			case JPEG_EOI:
				return 0;

	/*      case JPEG_JFIF: */
			case JPEG_QUANT:
			case JPEG_HUFF:
			case JPEG_DD:

	/*	if(!finishedEncoding) */
	skipJpegBlock2(input);
	/* else
	   SWF_error("Encoding tables found in Jpeg image section!"); */

	break;

      case JPEG_SOF1:
      case JPEG_SOF2:
      	return 0;

      case JPEG_SOF0:

	skipJpegBlock2(input);
	break;

      case JPEG_SOS:
	/*
	if(!finishedEncoding)
	  SWF_error("Found SOS before SOF in Jpeg file!");
	*/
	break;

      default:
	/* dumpJpegBlock(c, fp, method, data); */
	skipJpegBlock2(input);
    }

    if(c == JPEG_SOS)
      break;

    if(SWFInput_eof(input))
      return 0;

	}

	return 1;
}

int isDBL(SWFInput input)
{
	int version;

	SWFInput_seek(input, 0, SEEK_SET);

	if (SWFInput_getChar(input) != 'D' || SWFInput_getChar(input) != 'B')
		return 0;

	version = SWFInput_getChar(input);

	if(version != 'L' && version != 'l')
		return 0;

	version = SWFInput_getChar(input);

	if (version != 1 && version != 2)
		return 0;

	return 1;
}

int isPNG(SWFInput input)
{
	SWFInput_seek(input, 1, SEEK_SET);

	if (SWFInput_getChar(input) != 'P' || SWFInput_getChar(input) != 'N')
		return 0;

	return 1;
}

int isGIF(SWFInput input)
{
	SWFInput_seek(input, 0, SEEK_SET);

	if (SWFInput_getChar(input) != 'G' || SWFInput_getChar(input) != 'I')
		return 0;

	return 1;
}
