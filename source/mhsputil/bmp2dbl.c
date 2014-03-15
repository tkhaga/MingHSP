#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef DEBUG
#include <windows.h>
#include <mmsystem.h>
#endif

#include "bmp2dbl.h"
#include "mhsputil.h"

void usage();

int main(int argc, char *argv[])
{
	FILE *fp, *dbl;
	dbl_data bmpdata;
	char *inname = NULL, *outname = NULL;
	int c[3];
	int format;
	int index, alpha = 0xff, level = 9, lhsize;

	#ifdef DEBUG
	FILE *f = fopen("debug.txt", "a");
	unsigned long before, after;
	before =timeGetTime();
	#endif

	for (index = 1; index < argc; index++) {
		if (*argv[index] == '-' && *(argv[index] + 2) == '\0') {
			switch (*(argv[index] + 1)) {
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
			if (index < argc - 1) {
				switch (*(argv[index] + 1)) {
					case 'a':
						alpha = GetNum(argv[++index]);
						if (alpha == -1) {
							alpha = 0xff;
							break;
						}
						continue;
					default:
						break;
				}
			}
		}
		if (!inname) {
			inname = argv[index];
		}
		else if (!outname) {
			outname = argv[index];
		}
	}

	if (!inname) {
		usage();
		return 1;
	}

	if (alpha < 0 || alpha > 0xff) {
		printf("Illegal parameter");
		return 1;
	}

	fp = fopen(inname, "rb");
	if (!fp) {
		printf("Failed to open input file");
		return 1;
	}

	c[0] = fgetc(fp);
	c[1] = fgetc(fp);
	c[2] = fgetc(fp);
	format = BMP2DBL_UNKNOWN;

	if (c[0] == 'B' && c[1] == 'M')
		format = BMP2DBL_BMP;

	if (c[0] == 'G' && c[1] == 'I')
		format = BMP2DBL_GIF;

	if (c[1] == 'P' && c[2] == 'N')
		format = BMP2DBL_PNG;

	fseek(fp, 0, SEEK_SET);

	switch (format) {
		case BMP2DBL_BMP:
			if (!readBMP(fp, &bmpdata, alpha, level))
				goto READ_ERROR;
			break;
		case BMP2DBL_GIF:
			if (!readGif(inname, &bmpdata, alpha, level))
				goto READ_ERROR;
			break;
		case BMP2DBL_PNG:
			if (!readPNG(fp, &bmpdata, alpha, level))
				goto READ_ERROR;
			break;
		case BMP2DBL_UNKNOWN:
			printf("Unknown format");
			fclose(fp);
			return 1;
		default:
READ_ERROR:
			printf("Read error");
			fclose(fp);
			return 1;
	}

	fclose(fp);

	if (bmpdata.format == 3) {
	  lhsize = 6;
	}
	else {
	  lhsize = 5;
	}

	if (!outname) {
		outname = malloc(strlen(inname) + 5);
		strcpy(outname, inname);
		ChangeExt(outname, "dbl");
	}

	dbl = fopen(outname, "wb");
	free(outname);
	if (!dbl) {
		printf("Failed to open output file");
		return 1;
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
		fputc(bmpdata.format2, dbl);	/* パレットモード時の色数 */

	if(fwrite(bmpdata.data, sizeof(char), bmpdata.length, dbl) != bmpdata.length) {
		printf("Could not write all of the file");
		return 1;
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

void usage()
{
	printf(
		"usage:\n\n\tbmp2dbl in.bmp [out] [-a <alpha>] [-#]\n\n"
		"  in.bmp\tInput file name\n"
		"  out\t\tOutput file name. Default is in.dbl\n"
		"  -a <alpha>\tSet the alpha value (ranges from 0-255)\n"
		"  -#\t\tCompression level. 1 (fast) <-> 9 (best). Default is 9\n"
	);
}
