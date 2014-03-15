#include <stdio.h>
#include <string.h>

#include "mhsputil.h"

/* Dealing with SJIS problem */
#define isDirDelimiter(p) ((*p == '\\' && !((0x81 <= *(p - 1) && *(p - 1) <= 0x9f)\
 || (0xe0 <= *(p - 1) && *(p - 1) <= 0xf5))) || *p == '/')

/*
  Šg’£Žq‚Ì•ÏX
*/
void ChangeExt(char *fname, const char *ext)
{
	unsigned char *p, *pp;

	pp = (unsigned char *)fname + strlen(fname);
	p = pp - 1;

	for(;;) {
		if (*p == '.')
			pp = p;
		if (p == (unsigned char *)fname)
			break;
		if (isDirDelimiter(p))
			break;
		p--;
	}

	*pp = '\0';

	if (ext) {
		strcat(fname, ".");
		strcat(fname, ext);
	}
}

/*
  ”Žš‚ð”’l‚É
*/
int GetNum(const char *p)
{
	int num = 0;

	while(*p != '\0') {
		if (!('0' <= *p && *p <= '9')) {
			num = -1;
			break;
		}
		num *= 10;
		num += *p - '0';
		p++;
	}
	return num;
}

unsigned char *data;

void InitIO(unsigned char *p, unsigned long offset)
{
	data = p + offset;
}

void IOseek(long dest)
{
	data += dest;
}

void *IOgetPtr()
{
	return data;
}

unsigned long getLongBE()
{
	unsigned long ret;
	ret =  ((unsigned long)data[3] << 24) + ((unsigned long)data[2] << 16) +
	       ((unsigned long)data[1] <<  8) + ((unsigned long)data[0]);
	data += 4;
	return ret;
}

unsigned short getShortBE()
{
	unsigned short ret;
	ret =  ((unsigned short)data[1] << 8) + ((unsigned short)data[0]);
	data += 2;
	return ret;
}

unsigned char getChar()
{
	unsigned char ret;
	ret = (unsigned char)data[0];
	data += 1;
	return ret;
}

unsigned long getLongLE(unsigned char *p)
{
	return ((unsigned long)p[0]      ) + ((unsigned long)p[1] <<  8) +
	       ((unsigned long)p[2] << 16) + ((unsigned long)p[3] << 24);
}

unsigned short getShortLE(unsigned char *p)
{
	return ((unsigned short)p[0]) + ((unsigned short)p[1] << 8);
}
