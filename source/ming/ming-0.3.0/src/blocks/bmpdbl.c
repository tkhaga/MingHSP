/*
  readbmp.c - Convert Windows, OS/2 BMP to DefineBitsLossless/DefineBitsLossless2

    supported BMP format
    bitdepth         :1 ,4 ,8, 24bpp
    compression type :BI_RGB(not compressed)
    top-down         :OK(probabry)
    OS/2 BMP         :OK(probabry)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <zlib.h>

#include "ming_config.h"
#include "libming.h"

#include "bitmap.h"
#include "dbl.h"
#include "input.h"

#define BMP_HEADERSIZE 54	/* sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) */

#if 1
int readBMP(SWFInput input, dblData result)
{
	unsigned char *imgdata, *tmpbuf;
	int index, pals = 0, palinc, palsize;
	unsigned int isos2, topdown = 0, bit, compression, bihsize, offbits;
	unsigned long width, tmpwidth, tmpsize, y, x, outsize, imgsize, bitmask[1];
	long height, alwidth;

	if (SWFInput_getUInt16(input) != 0x4D42) {			/* bfType (BFH + 0) */
		/* printf("Invalid input file"); */
		return 0;
	}

/*
	if (alpha == 0xff) {
		result->hasalpha = 0;
	}
	else {
		result->hasalpha = 1;
	}
*/
	result->hasalpha = 0;

	SWFInput_seek(input, 14, SEEK_SET);
	bihsize = SWFInput_getUInt32(input);			/* biSize or bcSize (BIH + 0) */
	if (bihsize < 40) {
		isos2 = 1;	/* OS2�r�b�g�}�b�v */
		width = SWFInput_getSInt16(input);		/* bcWidth (BIH + 4) */
		height = SWFInput_getSInt16(input);		/* bcHeight (BIH + 6) */
		if (height < 0) {
			/* printf("Broken input file"); */
			return 0;
		}
		SWFInput_seek(input, 2, SEEK_CUR);
		bit = SWFInput_getUInt16(input);			/* bcBitCount (BIH + 10) */
		compression = 0;
	}
	else {
		isos2 = 0;
		width = (int)SWFInput_getUInt32(input);			/* biWidth (BIH + 4) */
		height = (int)SWFInput_getUInt32(input);		/* biHeight (BIH + 8) */
		if (height < 0) {
			topdown = 1;
			height = -height;
		}
		SWFInput_seek(input, 2, SEEK_CUR);
		bit = SWFInput_getUInt16(input);			/* biBitCount (BIH + 14) */
		compression = SWFInput_getUInt32(input);	/* biCompression (BIH + 16) */
	}

	/* ��Ή���bpp�͂͂��� */
	if (bit != 1 && bit != 4 && bit != 8 && bit != 24) {
		/* printf("Unsupported bit depth :%d", bit); */
		return 0;
	}

	/* ����ł͔񈳏k�ȊO�͂͂��� */
	if (compression != 0) {
		/* printf("Unsupported compression type"); */
		return 0;
	}

	if (!isos2) {
		SWFInput_seek(input, 46, SEEK_SET);
		pals = SWFInput_getUInt32(input);			/* biClrUsed (BIH + 32) */
	}

	if (pals == 0) {
		switch(bit) {
			case 1:
				pals = 2;
				break;
			case 4:
				pals = 16;
				break;
			case 8:
				pals = 256;
				break;
			default:
				break;
		}
	}

	alwidth = (width * bit + 31) / 32 * 4;

	if (bit <= 8) {
		tmpwidth = (width + 3) & ~3;
	}
	else {
		tmpwidth = width * 4;
	}

	tmpsize = tmpwidth * height;
	tmpsize += pals * (3 + result->hasalpha);
	tmpbuf = malloc(tmpsize);
	if (!tmpbuf) {
		/* printf("Failed to allocate memory"); */
		return 0;
	}

	imgsize = alwidth * height;

	SWFInput_seek(input, 10, SEEK_SET);
	offbits = SWFInput_getUInt32(input);			/* bfOffBits (BFH + 10) */

	if (isos2) {
		palinc = 3;
	}
	else {
		palinc = 4;
	}
	palsize = pals * palinc;

	if (bit <= 8) {
		unsigned char *paldata;
		unsigned char *p, *pp, *pt, *ppt;

		paldata = malloc(palsize);
		SWFInput_seek(input, 14 + bihsize, SEEK_SET);
		SWFInput_read(input, (char *)paldata, palsize);

		p = tmpbuf;
		pp = paldata;
		/* 8bit�p���b�g���[�h  �����x�w�薳�� */
		for (x = 0; x < pals; x++) {
			*p++ = pp[2];
			*p++ = pp[1];
			*p++ = pp[0];
			pp += palinc;
		}

		free(paldata);
		imgdata = malloc(imgsize);
		if (offbits > 0)
			SWFInput_seek(input, offbits, SEEK_SET);
		if (SWFInput_read(input, (char *)imgdata, imgsize) != imgsize) {
			/* printf("Read error"); */
			return 0;
		}

		/* �g�b�v�_�E���`���ɂ��Ή� */
		if (topdown) {
			pp = imgdata;
			alwidth = -alwidth;
		}
		else {
			pp = imgdata + ((height - 1) * alwidth);
		}

		if (bit == 8) {
			for (y = 0; y < height; y++) {
				memcpy(p, pp, width);
				p += tmpwidth;
				pp -= alwidth;
			}
		}
		else {
			bitmask[0] = 0xffffffff >> (32 - bit);
			for (y = 0; y < height; y++) {
				/* ��ʂ̃r�b�g�����̃s�N�Z�� */
				index = 8;
				pt = p;
				ppt = pp;
				for (x = 0; x < width; x++) {
					index -= bit;
					*pt++ = (*ppt >> index) & bitmask[0];
					if (index == 0) {
						ppt++;
						index = 8;
					}
				}
				p += tmpwidth;
				pp -= alwidth;
			}
		}

	}
	else {
		unsigned char *p, *pp, *pt, *ppt;

		imgdata = malloc(imgsize);
		if (offbits > 0) {
			SWFInput_seek(input, offbits, SEEK_SET);
		}
		else {
			/* �K�i��̓p���b�g�����݂��邱�Ƃ����邪�g��Ȃ��̂œǂݔ�΂� */
			SWFInput_seek(input, 14 + bihsize + palsize, SEEK_SET);
		}
		if (SWFInput_read(input, (char *)imgdata, imgsize) != imgsize) {
			/* printf("Read error"); */
			return 0;
		}

		p = tmpbuf;

		if (topdown) {
			pp = imgdata;
			alwidth = -alwidth;
		}
		else {
			pp = imgdata + ((height - 1) * alwidth);
		}

		/* 24bit�t���J���[  �����x�w�薳�� */
		for (y = 0; y < height; y++) {
			pt = p;
			ppt = pp;
			for (x = 0; x < width; x++) {
				*pt++;
				*pt++ = ppt[2];
				*pt++ = ppt[1];
				*pt++ = ppt[0];
				ppt += 3;
			}
			p += tmpwidth;
			pp -= alwidth;
		}
	}

	result->width = width;
	result->height = height;

	if (pals) {
		result->format = 3;	/* �s�N�Z��������8bit */
		result->format2 = pals - 1;
	}
	else {
		result->format = 5;	/* �s�N�Z��������32bit */
	}

	result->data = malloc(outsize = (int)floor(tmpsize*1.01+12));
	if (!result->data) {
		/* printf("Failed to allocate memory"); */
		return 0;
	}

	if (compress2(result->data, &outsize, tmpbuf, tmpsize, 9) != Z_OK) {
		/* printf("Compression failed"); */
		free(tmpbuf);
		free(result->data);
		return 0;
	}
	free(tmpbuf);

	result->length = outsize;
	return 1;
}

SWFDBLBitmapData newSWFDBLBitmapData_fromBmpFile(const char *fileName)
{
	SWFDBLBitmapData ret;
	struct dbl_data bmpdata;
	SWFInput input;
	FILE *fp;

	fp = fopen(fileName, "rb");
	input = newSWFInput_file(fp);

	if(!readBMP(input, &bmpdata))
		return NULL;
	ret = newSWFDBLBitmapData_fromData(&bmpdata);
	// ret->input = NULL;
	destroySWFInput(input);
	fclose(fp);
	return ret;
}

SWFDBLBitmapData newSWFDBLBitmapData_fromBmpInput(SWFInput input)
{
	SWFDBLBitmapData ret;
	struct dbl_data bmpdata;

	if(!readBMP(input, &bmpdata))
		return NULL;
	ret = newSWFDBLBitmapData_fromData(&bmpdata);
	// ret->input = NULL;
	return ret;
}
#endif
