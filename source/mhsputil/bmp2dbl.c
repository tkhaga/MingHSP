/*
  bmp2dbl.c - Convert Windows, OS/2 BMP to DefineBitsLossless/DefineBitsLossless2

    supported BMP format
    bitdepth         :1 ,4 ,8, 24bpp
    compression type :BI_RGB(not compressed)
    top-down         :OK(probabry)
    OS/2 BMP         :OK(probabry)
*/

#include <stdio.h>
#include <stdlib.h>

#include <zlib.h>
#ifdef DEBUG
#include <windows.h>
#include <mmsystem.h>
#endif
#include "mhsputil.h"

#define BMP_HEADERSIZE 54	/* sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) */

typedef struct dbl_data
{	int length;
	char hasalpha, format, format2;
	unsigned short width, height;
	unsigned char *data;
} dbl_data;
typedef struct dbl_data *dblData;

int readBMP(FILE *fp, dblData result, int alpha, int level);
void usage();

int main(int argc, char *argv[])
{
	FILE *fp, *dbl;
	dbl_data bmpdata;
	char *inname = NULL, *outname = NULL;
	int index, alpha = 0xff, level = 9, lhsize;

	#ifdef DEBUG
	FILE *f = fopen("debug.txt", "a");
	unsigned long before, after;
	before =timeGetTime();
	#endif

	if (argc < 2)
	{
		usage();
		return 0;
	}

	for (index = 1; index < argc; index++)
	{
		if (*argv[index] == '-' && *(argv[index] + 2) == '\0')
		{
			switch (*(argv[index] + 1))
			{
				case '1':
					level = 1;
					continue;
				case '2':
					level = 2;
					continue;
				case '3':
					level = 3;
					continue;
				case '4':
					level = 4;
					continue;
				case '5':
					level = 5;
					continue;
				case '6':
					level = 6;
					continue;
				case '7':
					level = 7;
					continue;
				case '8':
					level = 8;
					continue;
				case '9':
					level = 9;
					continue;
				default:
					break;
			}
			if (index < argc - 1)
			{
				switch (*(argv[index] + 1))
				{
					case 'a':
						alpha = GetNum(argv[++index]);
						if (alpha == -1)
						{
							alpha = 255;
							break;
						}
						continue;
					default:
						break;
				}
			}
		}
		if (!inname)
		{
			inname = argv[index];
		}
		else if (!outname)
		{
			outname = argv[index];
		}
	}

	if (!inname)
	{
		usage();
		return 0;
	}

	if (level < 1 || level > 9 || alpha < 0 || alpha > 0xff)
	{
		printf("Illegal parameter");
		return 0;
	}

	fp = fopen(inname, "rb");
	if (!fp)
	{
		printf("Failed to open input file");
		return 0;
	}

	if (!readBMP(fp, &bmpdata, alpha, level))
		return 0;

	if (bmpdata.format == 3)
	{
	  lhsize = 6;
	}else{
	  lhsize = 5;
	}

	if (!outname)
	{
		outname = malloc(strlen(inname) + 5);
		strcpy(outname, inname);
		ChangeExt(outname, "dbl");
	}

	dbl = fopen(outname, "wb");
	free(outname);
	if (!dbl)
	{
		printf("Failed to open output file");
		return 0;
	}

	fputs("DBl", dbl);
	fputc(bmpdata.hasalpha + 1, dbl);
	fputc(((bmpdata.length + lhsize) >> 24) & 0xff, dbl);
	fputc(((bmpdata.length + lhsize) >> 16) & 0xff, dbl);
	fputc(((bmpdata.length + lhsize) >>  8) & 0xff, dbl);
	fputc(((bmpdata.length + lhsize)      ) & 0xff, dbl);

	fputc(bmpdata.format, dbl);

	fputc(bmpdata.width & 0xff, dbl);
	fputc((bmpdata.width >> 8) & 0xff, dbl);
	fputc(bmpdata.height & 0xff, dbl);
	fputc((bmpdata.height >> 8) & 0xff, dbl);

	if (bmpdata.format == 3)
	{
	  fputc(bmpdata.format2, dbl);	/* パレットモード時の色数 */
	}

	if(fwrite(bmpdata.data, sizeof(char), bmpdata.length, dbl) != bmpdata.length)
	{
		printf("Could not write all of the file");
		return 0;
	}

	fclose(dbl);
	free(bmpdata.data);

	#ifdef DEBUG
	after =timeGetTime();
	fprintf(f, "%d\r\n", after-before);
	fclose(f);
	#endif
	return 0;
}

int readBMP(FILE *fp, dblData result, int alpha, int level)
{
	char *paldata, *imgdata, *tmpbuf, *outdata, *p, *pp, *pt, *ppt;
	char *inname = NULL, *outname = NULL;
	int index, pals = 0, palinc, palsize;
	unsigned int isos2, topdown = 0, bit, compression, bihsize, offbits;
	unsigned long width, tmpwidth, tmpsize, y, x, outsize, imgsize, bitmask[1];
	long height, alwidth;
	char header[BMP_HEADERSIZE];

	fread(header, 1, BMP_HEADERSIZE, fp);
	if (getShortLE(header) != 0x4D42)			/* bfType (BFH + 0) */
	{
		printf("Invalid input file");
		return 0;
	}

	if (alpha == 0xff)
	{
		result->hasalpha = 0;
	}else{
		result->hasalpha = 1;
	}

	bihsize = getLongLE(header + 14);			/* biSize or bcSize (BIH + 0) */
	if (bihsize < 40)
	{
		isos2 = 1;	/* OS2ビットマップ */
		width = getShortLE(header + 18);		/* bcWidth (BIH + 4) */
		height = getShortLE(header + 20);		/* bcHeight (BIH + 6) */
		if (height < 0)
		{
			printf("Broken input file");
			return 0;
		}
		bit = getShortLE(header + 24);			/* bcBitCount (BIH + 10) */
		compression = 0;
	}
	else
	{
		isos2 = 0;
		width = getLongLE(header + 18);			/* biWidth (BIH + 4) */
		height = getLongLE(header + 22);		/* biHeight (BIH + 8) */
		if (height < 0)
		{
			topdown = 1;
			height = -height;
		}
		bit = getShortLE(header + 28);			/* biBitCount (BIH + 14) */
		compression = getLongLE(header + 30)	/* biCompression (BIH + 16) */;
	}

	/* 非対応なbppははじく */
	if (bit != 1 && bit != 4 && bit != 8 && bit != 24)
	{
		printf("Unsupported bit depth :%d", bit);
		return 0;
	}

	/* 現状では非圧縮以外ははじく */
	if (compression != 0)
	{
		printf("Unsupported compression type");
		return 0;
	}

	if (!isos2)
		pals = getLongLE(header + 46);			/* biClrUsed (BIH + 32) */

	if (pals == 0)
	{
		switch(bit)
		{
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

	if (bit <= 8)
	{
		tmpwidth = (width + 3) & ~3;
	}else{
		tmpwidth = width * 4;
	}

	tmpsize = tmpwidth * height;
	tmpsize += pals * (3 + result->hasalpha);
	tmpbuf = malloc(tmpsize);
	if (!tmpbuf)
	{
		printf("Failed to allocate memory");
		return 0;
	}

	imgsize = alwidth * height;

	offbits = getLongLE(header + 10);			/* bfOffBits (BFH + 10) */

	if (isos2)
	{
		palinc = 3;
	}
	else
	{
		palinc = 4;
	}
	palsize = pals * palinc;

	if (bit <= 8)
	{
		paldata = malloc(palsize);
		fseek(fp ,14 + bihsize ,SEEK_SET);
		fread(paldata, sizeof(char), palsize, fp);

		p = tmpbuf;
		pp = paldata;

		if (result->hasalpha)	/* 8bitパレットモード  透明度指定有り */
		{
			for (x = 0; x < pals; x++)
			{
				*p++ = pp[2] * alpha >> 8;
				*p++ = pp[1] * alpha >> 8;
				*p++ = pp[0] * alpha >> 8;
				*p++ = alpha;
				pp += palinc;
			}
		}
		else
		{	/* 8bitパレットモード  透明度指定無し */
			for (x = 0; x < pals; x++)
			{
				*p++ = pp[2];
				*p++ = pp[1];
				*p++ = pp[0];
				pp += palinc;
			}
		}

		free(paldata);
		imgdata = malloc(imgsize);
		if (offbits > 0)
			fseek(fp, offbits, SEEK_SET);
		if (fread(imgdata, sizeof(char), imgsize, fp) != imgsize)
		{
			printf("Read error");
			return 0;
		}

		/* トップダウン形式にも対応 */
		if (topdown)
		{
			pp = imgdata;
			alwidth = -alwidth;
		}
		else
		{
			pp = imgdata + ((height - 1) * alwidth);
		}

		if (bit == 8)
		{
			for (y = 0; y < height; y++)
			{
				memcpy(p, pp, width);
				p += tmpwidth;
				pp -= alwidth;
			}
		}
		else
		{
			bitmask[0] = 0xffffffff >> (32 - bit);
			for (y = 0; y < height; y++)
			{
				/* 上位のビットが左のピクセル */
				index = 8;
				pt = p;
				ppt = pp;
				for (x = 0; x < width; x++)
				{
					index -= bit;
					*pt++ = (*(unsigned long *)ppt >> index) & bitmask[0];
					if (index == 0)
					{
						ppt++;
						index = 8;
					}
				}
				p += tmpwidth;
				pp -= alwidth;
			}
		}

	}
	else
	{
		imgdata = malloc(imgsize);
		if (offbits > 0)
		{
			fseek(fp, offbits, SEEK_SET);
		}
		else
		{
			/* 規格上はパレットが存在することがあるが使わないので読み飛ばす */
			fseek(fp, 14 + bihsize + palsize, SEEK_SET);
		}
		if (fread(imgdata, sizeof(char), imgsize, fp) != imgsize)
		{
			printf("Read error");
			return 0;
		}

		p = tmpbuf;

		if (topdown)
		{
			pp = imgdata;
			alwidth = -alwidth;
		}
		else
		{
			pp = imgdata + ((height - 1) * alwidth);
		}

		if (result->hasalpha)	/* 24bitフルカラー  透明度指定有り */
		{
			for (y = 0; y < height; y++)
			{
				pt = p;
				ppt = pp;
				for (x = 0; x < width; x++)
				{
					*pt++ = alpha;
					*pt++ = ppt[2] * alpha >> 8;
					*pt++ = ppt[1] * alpha >> 8;
					*pt++ = ppt[0] * alpha >> 8;
					ppt += 3;
				}
				p += tmpwidth;
				pp -= alwidth;
			}
		}
		else
		{	/* 24bitフルカラー  透明度指定無し */
			for (y = 0; y < height; y++)
			{
				pt = p;
				ppt = pp;
				for (x = 0; x < width; x++)
				{
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
	}
	fclose(fp);

	result->width = width;
	result->height = height;

	if (pals)
	{
		result->format = 3;	/* ピクセル当たり8bit */
		result->format2 = pals - 1;
	}else{
		result->format = 5;	/* ピクセル当たり32bit */
	}

	result->data = malloc(outsize = (int)floor(tmpsize*1.01+12));
	if (!outdata)
	{
		printf("Failed to allocate memory");
		return 0;
	}

	if (compress2(result->data, &outsize, tmpbuf, tmpsize, level) != Z_OK)
	{
		printf("Compression failed");
		free(tmpbuf);
		free(outdata);
		return 0;
	}
/*
	index = compress2(outdata, &outsize, tmpbuf, tmpsize, level);
	if (index != Z_OK)
	{
		#define ZERRBUFSIZE 16
		char z_err[ZERRBUFSIZE];
		switch(index)
		{
			case Z_MEM_ERROR:
				strncpy(z_err, "Z_MEM_ERROR", ZERRBUFSIZE);
				break;
			case Z_BUF_ERROR:
				strncpy(z_err, "Z_BUF_ERROR", ZERRBUFSIZE);
				break;
			case Z_STREAM_ERROR:
				strncpy(z_err, "Z_STREAM_ERROR", ZERRBUFSIZE);
				break;
			default:
				strncpy(z_err, "Unknown error", ZERRBUFSIZE);
				break;
		}
		printf("Compression failed (zlib error = %s)", z_err);
		free(tmpbuf);
		free(outdata);
		return 0;
	}
*/
	free(tmpbuf);

	result->length = outsize;
	return 1;
}

void usage()
{
	printf("usage:\n\n\tbmp2dbl in.bmp [out] [-a <alpha>] [-#]\n\n\
  in.bmp\tInput file name\n\
  out\t\tOutput file name. Default is in.dbl\n\
  -a <alpha>\tSet the alpha value (ranges from 0-255)\n\
  -#\t\tCompression level. 1 (fast) <-> 9 (best). Default is 9\n\
");
}
