/* ifdbl.spi DefineBitsLossless/DefineBitsLossless2 to DIB filter Ver 1.0 */

#include <windows.h>
#include <zlib.h>
#ifdef DEBUG
#include <stdio.h>
#include <mmsystem.h>
#endif

#include "mhsputil.h"

#undef EXPORT
#define EXPORT __declspec(dllexport)

#pragma pack(push, 1)
typedef struct PictureInfo {
	long left,top;
	long width;
	long height;
	WORD x_density;
	WORD y_density;
	short colorDepth;
	HLOCAL hInfo;
} PictureInfo;
#pragma pack(pop)

#define DBLHEADERSIZE 14

static const char *pluginfo[] = {
	"00IN",
	"DefineBitsLossless/DefineBitsLossless2 to DIB filter Ver 1.0 (C) THAGA 2005",
	"*.dbl",
	"DBL ファイル"
};

typedef int (CALLBACK *SPI_PROGRESS)(int, int, long);

EXPORT int WINAPI GetPluginInfo(int infono, LPSTR buf, int buflen)
{
	if (infono < 0 || infono >= 4) 
		return 0;

	lstrcpyn(buf, pluginfo[infono], buflen);

	return lstrlen(buf);
}

EXPORT int WINAPI IsSupported(LPSTR filename, DWORD dw)
{
	char *data;
	char header[DBLHEADERSIZE];
	int bit;
	DWORD BytesRead;

	if ((dw & 0xffff0000) == 0)
	{
		if (!ReadFile((HANDLE)dw, header, DBLHEADERSIZE, &BytesRead, NULL)) {
			return 0;
		}
		data = header;
	}
	else
	{
		data = (char *)dw;
	}

	if (data[0] != 'D' || data[1] != 'B')
		return 0;

	if (data[2] == 'l')
	{
		bit = data[8];
	}
	else if (data[2] == 'L')
	{
		bit = data[6];
	}
	else
	{
		return 0;
	}

	if (data[3] != 1 && data[3] != 2)
		return 0;

	if (bit != 3 && bit != 5)
		return 0;

	return 1;
}

EXPORT int WINAPI GetPictureInfo(
		LPSTR buf, long len, unsigned int flag, struct PictureInfo *lpInfo)
{
	char header[DBLHEADERSIZE];
	char *data;
	HANDLE hfile;
	DWORD BytesRead;

	if (!(flag & 7))
	{
		hfile = CreateFile(buf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hfile == INVALID_HANDLE_VALUE)
			return 6;
		if (SetFilePointer(hfile, len, NULL, FILE_BEGIN) != (DWORD)len) {
			CloseHandle(hfile);
			return 6;
		}
		if (!ReadFile(hfile, header, 14, &BytesRead, NULL)) {
			CloseHandle(hfile);
			return 6;
		}
		CloseHandle(hfile);
		if (BytesRead != DBLHEADERSIZE)
			return 2;
		data = header;
	}
	else
	{
		if (len < DBLHEADERSIZE)
			return 2;
		data = (char *)buf;
	}

	InitIO(data, 2);
	switch(getChar())
	{
		case 'l':
			IOseek(5);
			break;
		case 'L':
			IOseek(3);
			break;
		default:
			return 2;
	}

	switch(getChar())
	{
		case 3:
			lpInfo->colorDepth = 8;
			break;
		case 5:
			lpInfo->colorDepth = 24;
			break;
		default:
			return 2;
	}

	lpInfo->width      = getShortBE();
	lpInfo->height     = getShortBE();

	lpInfo->left		= 0;
	lpInfo->top			= 0;
	lpInfo->x_density	= 0;
	lpInfo->y_density	= 0;
	lpInfo->hInfo		= NULL;

	return 0;
}

int WINAPI GetPicture(
		LPSTR buf, long len, unsigned int flag, 
		HANDLE *pHBInfo, HANDLE *pHBm,
		SPI_PROGRESS lpPrgressCallback, long lData)
{
	unsigned char *data, *pbmdat, *imgdata;
	unsigned long datasize;
	HANDLE hfile, hdbl;
	DWORD BytesRead;
	BITMAPINFO *pinfo;

	unsigned char *p, *pp, *pt, *ppt;
	long height;
	unsigned long width, imgwidth, dibwidth, srcsize, imgsize = 0, dibsize, infosize, palsize, outsize;
	unsigned long x, y;
	unsigned int hasalpha, pals, bit, r, g, b;

	if ((flag & 7) == 0) {
		hfile = CreateFile(buf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hfile == INVALID_HANDLE_VALUE)
			return 6;
		datasize = GetFileSize(hfile, NULL) - len;
		if (datasize < DBLHEADERSIZE) {
			CloseHandle(hfile);
			return 2;
		}
		if (SetFilePointer(hfile, len, NULL, FILE_BEGIN) != (DWORD)len) {
			CloseHandle(hfile);
			return 6;
		}
		data = (char *)LocalAlloc(LMEM_FIXED, datasize);
		if (data == NULL) {
			CloseHandle(hfile);
			return 5;
		}
		if (!ReadFile(hfile, data, datasize, &BytesRead, NULL)) {
			CloseHandle(hfile);
			LocalFree(data);
			return 6;
		}
		CloseHandle(hfile);
		if (BytesRead != (DWORD)datasize) {
			LocalFree(data);
			return 6;
		}
	}
	else
	{
		data = buf;
		datasize = len;
	}

	if (lpPrgressCallback != NULL)
		if (lpPrgressCallback(0, 1, lData)) /* 0% */
			return 1;

	infosize = sizeof(BITMAPINFOHEADER);

	InitIO(data, 2);

	switch(getChar())
	{
		case 'l':
			hasalpha = getChar();
			srcsize = getLongBE();
			break;
		case 'L':
			hasalpha = getChar();
			srcsize = getShortBE();
			break;
		default:
			return 2;
	}

	switch(getChar())
	{
		case 3:
			pals = 1;
			bit = 8;
			break;
		case 5:
			pals = 0;
			bit = 24;
			break;
		default:
			return 2;
	}

	width      = getShortBE();
	height     = getShortBE();

	if (pals)
	{
		pals = getChar() + 1;
		imgwidth = dibwidth = (width + 3) & ~3;
		palsize = pals * (2 + hasalpha);
		infosize += sizeof(RGBQUAD) * 256;
	}
	else
	{
		dibwidth = (width * 3 + 3) & ~3;
		imgwidth = width * 4;
		palsize = 0;
	}

	imgsize = imgwidth * height + palsize;
	dibsize = dibwidth * height;

	hdbl = HeapCreate(0, imgsize, 0);
	if (!hdbl)
		return 4;

	imgdata = HeapAlloc(hdbl, HEAP_ZERO_MEMORY, imgsize);
	if (!imgdata)
		return 4;

	outsize = imgsize;
	if (uncompress(imgdata, &outsize, (unsigned char *)IOgetPtr(), srcsize) != Z_OK)
	{
		HeapDestroy(hdbl);
		return 8;
	}

	if (outsize != imgsize)
	{
		HeapDestroy(hdbl);
		return 3;
	}

	*pHBInfo = LocalAlloc(LMEM_MOVEABLE, infosize);
	*pHBm    = LocalAlloc(LMEM_MOVEABLE, dibsize);

	if (*pHBInfo == NULL || *pHBm == NULL) {
		if (*pHBInfo != NULL) LocalFree(*pHBInfo);
		if (*pHBm != NULL) LocalFree(*pHBm);
		return 4;
	}

	pinfo  = (BITMAPINFO *)LocalLock(*pHBInfo);
	pbmdat = (unsigned char *)LocalLock(*pHBm);
	if (pinfo == NULL || pbmdat == NULL) {
		LocalFree(*pHBInfo);
		LocalFree(*pHBm);
		return 5;
	}

	pinfo->bmiHeader.biSize				= sizeof(BITMAPINFOHEADER);
	pinfo->bmiHeader.biWidth			= width;
	pinfo->bmiHeader.biHeight			= height;
	pinfo->bmiHeader.biPlanes			= 1;
	pinfo->bmiHeader.biBitCount			= bit;
	pinfo->bmiHeader.biCompression		= BI_RGB;
	pinfo->bmiHeader.biSizeImage		= 0;
	pinfo->bmiHeader.biXPelsPerMeter	= 0;
	pinfo->bmiHeader.biYPelsPerMeter	= 0;
	pinfo->bmiHeader.biClrUsed			= 0;
	pinfo->bmiHeader.biClrImportant		= 0;

	if (pals)
	{
		p = (unsigned char *)pinfo->bmiColors;
		pp = imgdata;
		ZeroMemory(p, sizeof(RGBQUAD) * 256);
		if (hasalpha == 2)	/* 8bitパレットモード  透明度指定有り */
		{
			for (x = 0; x < pals; x++)
			{
				r = pp[0] + 255 - pp[3];
				g = pp[1] + 255 - pp[3];
				b = pp[2] + 255 - pp[3];
				if (r > 255) r = 255;
				if (g > 255) g = 255;
				if (b > 255) b = 255;
				*p++ = b;
				*p++ = g;
				*p++ = r;
				*p++;
				pp += 4;
			}
		}
		else
		{	/* 8bitパレットモード  透明度指定無し */
			for (x = 0; x < pals; x++)
			{
				*p++ = pp[2];
				*p++ = pp[1];
				*p++ = pp[0];
				*p++;
				pp += 3;
			}
		}
		p = pbmdat;
		pp = imgdata + palsize + ((height - 1) * imgwidth);
		for (y = 0; y < height; y++)
		{
			memcpy(p, pp, width);
			p += dibwidth;
			pp -= imgwidth;
		}
	}
	else
	{
		p = pbmdat;
		pp = imgdata + ((height - 1) * imgwidth);

		if (hasalpha == 2)	/* 24bitフルカラー  透明度指定有り */
		{
			#ifdef DEBUG
			FILE *f = fopen("debug.txt", "a");
			unsigned long before, after;
			before =timeGetTime();
			#endif
			for (y = 0; y < height; y++)
			{
				pt = p;
				ppt = pp;
				for (x = 0; x < width; x++)
				{
					r = ppt[1] + 255 - ppt[0];
					g = ppt[2] + 255 - ppt[0];
					b = ppt[3] + 255 - ppt[0];
					if (r > 255) r = 255;
					if (g > 255) g = 255;
					if (b > 255) b = 255;
					*pt++ = b;
					*pt++ = g;
					*pt++ = r;
					ppt += 4;
				}
				p += dibwidth;
				pp -= imgwidth;
			}
			#ifdef DEBUG
			after =timeGetTime();
			fprintf(f, "%d\r\n", after-before);
			fclose(f);
			#endif
		}
		else
		{	/* 24bitフルカラー  透明度指定無し */
			for (y = 0; y < height; y++)
			{
				pt = p;
				ppt = pp;
				for (x = 0; x < width; x++)
				{
					*pt++ = ppt[3];
					*pt++ = ppt[2];
					*pt++ = ppt[1];
					ppt += 4;
				}
				p += dibwidth;
				pp -= imgwidth;
			}
		}
	}

	HeapDestroy(hdbl);

	LocalUnlock(*pHBInfo);
	LocalUnlock(*pHBm);

	if ((flag & 7) == 0) LocalFree(data);

	if (lpPrgressCallback != NULL)
		if (lpPrgressCallback(1, 1, lData)) /* 100% */
			return 1;

	return 0;
}

EXPORT int WINAPI GetPreview(
	LPSTR buf, long len, unsigned int flag,
	HANDLE *pHBInfo, HANDLE *pHBm,
	SPI_PROGRESS lpPrgressCallback, long lData)
{
	return -1;
}
