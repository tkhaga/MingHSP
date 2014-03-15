#ifndef _MINGHSP_BMP2DBL_H_
#define _MINGHSP_BMP2DBL_H_

typedef struct dbl_data
{	int length;
	char hasalpha, format, format2;
	unsigned short width, height;
	unsigned char *data;
} dbl_data;
typedef struct dbl_data *dblData;

enum BMP2DBL_FORMATS
{
	BMP2DBL_BMP, 
	BMP2DBL_GIF, 
	BMP2DBL_PNG, 
	BMP2DBL_UNKNOWN
};

int readBMP(FILE *fp, dblData result, int alpha, int level);
int readGif(char *fileName, dblData result, int alpha2, int level);
int readPNG(FILE *fp, dblData result, int alpha2, int level);

#endif
