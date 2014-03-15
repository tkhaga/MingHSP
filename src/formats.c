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

#include <stdio.h>

#include "formats.h"

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

void skipJpegBlock2(FILE *fp)
{
  int length = (fgetc(fp)<<8) + fgetc(fp);

  fseek(fp, length-2, SEEK_CUR);

  return;
}

int isJPEG(FILE *fp)
{
	int c;

	fseek(fp, 0, SEEK_SET);

	if (fgetc(fp) != JPEG_MARKER)
		return 0;
	if (fgetc(fp) != JPEG_SOI)
		return 0;

	for (;;)
	{
		if (fgetc(fp) != JPEG_MARKER)
			return 0;
		switch (c = fgetc(fp))
		{
			case JPEG_EOI:
				return 0;

	/*      case JPEG_JFIF: */
			case JPEG_QUANT:
			case JPEG_HUFF:
			case JPEG_DD:

	/*	if(!finishedEncoding) */
	skipJpegBlock2(fp);
	/* else
	   SWF_error("Encoding tables found in Jpeg image section!"); */

	break;

      case JPEG_SOF1:
      case JPEG_SOF2:
      	return 0;

      case JPEG_SOF0:

	skipJpegBlock2(fp);
	break;

      case JPEG_SOS:
	/*
	if(!finishedEncoding)
	  SWF_error("Found SOS before SOF in Jpeg file!");
	*/
	break;

      default:
	/* dumpJpegBlock(c, fp, method, data); */
	skipJpegBlock2(fp);
    }

    if(c == JPEG_SOS)
      break;

    if(feof(fp))
      return 0;

	}

	return 1;
}

int isDBL(FILE *fp)
{
	int version;

	fseek(fp, 0, SEEK_SET);

	if (fgetc(fp) != 'D' || fgetc(fp) != 'B')
		return 0;

	version = fgetc(fp);

	if(version != 'L' && version != 'l')
		return 0;

	version = fgetc(fp);

	if (version != 1 && version != 2)
		return 0;

	return 1;
}
/*
int isPNG(FILE *fp)
{
	return 1;
}

int isGIF(FILE *fp)
{
	return 1;
}
*/

/*
int main(int argc, char *argv[])
{
	FILE *fp = fopen(argv[1], "rb");
	printf("%d\n", isJPEG(fp));
	printf("%d\n", isDBL(fp));
	return 0;
}
*/
