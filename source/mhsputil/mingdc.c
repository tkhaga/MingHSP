#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <zlib.h>
#include "hspdll.h"
#include "../src/blocks/input.h"
#include "../src/blocks/output.h"

#include "mhsputil.h"

#undef EXPORT
#define EXPORT __declspec(dllexport)

void fileOutputMethod(byte b, void *data);

EXPORT BOOL WINAPI vram2dbl(HSPEXINFO *hei, int p1, int p2, int p3)
{
	BMSCR *bm;
	unsigned char *tmpbuf, *outdata;
	char *outname;
	int alpha, level, hasalpha, lhsize, pals = 0;
	unsigned long alwidth, tmpwidth, tmpsize = 0, x, y, outsize;
	FILE *dbl;

	outname = hei->HspFunc_prm_gets();

	bm = (BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);
	alpha = hei->HspFunc_prm_getdi(0xff);
	level = hei->HspFunc_prm_getdi(9);

	if (level < 1 || level > 9 || alpha < 0 || alpha > 0xff)
		return -1;

	if (alpha == 0xff) {
		hasalpha = 1;
	}
	else {
		hasalpha = 2;
	}

	if (bm->palmode)
		pals = bm->pals;

	if (pals) {
		tmpwidth = alwidth = (bm->sx + 3) & ~3;
		tmpsize += pals * (2 + hasalpha);
	}
	else {
		alwidth = (bm->sx * 3 + 3) & ~3;
		tmpwidth = (alwidth + bm->sx) & ~3;
	}

	tmpsize += tmpwidth * bm->sy;
	tmpbuf = malloc(tmpsize);
	if (!tmpbuf)
		return -2;

	if (pals) {
		unsigned char *p, *pp, *pt, *ppt;

		p = tmpbuf;
		pp = (unsigned char *)bm->pal;
		if (hasalpha == 2) {	/* 8bitパレットモード  透明度指定有り */
			for (x = 0; x < pals; x++) {
				*p++ = pp[2] * alpha >> 8;
				*p++ = pp[1] * alpha >> 8;
				*p++ = pp[0] * alpha >> 8;
				*p++ = alpha;
				pp += 4;
			}
		}
		else {	/* 8bitパレットモード  透明度指定無し */
			for (x = 0; x < pals; x++) {
				*p++ = pp[2];
				*p++ = pp[1];
				*p++ = pp[0];
				pp += 4;
			}
		}
		pp = bm->pBit + ((bm->sy - 1) * alwidth);
		for (y = 0; y < bm->sy; y++) {
			memcpy(p, pp, bm->sx);
			p += tmpwidth;
			pp -= alwidth;
		}
	}
	else {
		unsigned char *p, *pp, *pt, *ppt;

		p = tmpbuf;
		pp = bm->pBit + ((bm->sy - 1) * alwidth);

		if (hasalpha == 2) {	/* 24bitフルカラー  透明度指定有り */
			for (y = 0; y < bm->sy; y++) {
				pt = p;
				ppt = pp;
				for (x = 0; x < bm->sx; x++) {
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
		else {	/* 24bitフルカラー  透明度指定無し */
			for (y = 0; y < bm->sy; y++) {
				pt = p;
				ppt = pp;
				for (x = 0; x < bm->sx; x++) {
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

	outdata = malloc(tmpsize);
	if (!outdata)
		return -2;

	if (compress2(outdata, &outsize, tmpbuf, tmpsize, level) != Z_OK)
		return -3;

	if (pals) {
		lhsize = 6;
	}
	else {
		lhsize = 5;
	}

	free(tmpbuf);

	dbl = fopen(outname, "wb");
	if (!dbl)
		return -4;

	fputs("DBl", dbl);
	fputc(hasalpha, dbl);
	fputc(((outsize + lhsize) >> 24) & 0xff, dbl);
	fputc(((outsize + lhsize) >> 16) & 0xff, dbl);
	fputc(((outsize + lhsize) >>  8) & 0xff, dbl);
	fputc(((outsize + lhsize)      ) & 0xff, dbl);

	if (pals) {
		fputc(3, dbl);	/* ピクセル当たり8bit */
	}
	else {
		fputc(5, dbl);	/* ピクセル当たり32bit */
	}

	fputc(bm->sx & 0xff, dbl);
	fputc((bm->sx >> 8) & 0xff, dbl);
	fputc(bm->sy & 0xff, dbl);
	fputc((bm->sy >> 8) & 0xff, dbl);

	if (pals)
		fputc(pals - 1, dbl);	/* パレットモード時の色数 */

	if(fwrite(outdata, sizeof(char), outsize, dbl) != outsize)
		return -5;

	fclose(dbl);
	free(outdata);

	return -(outsize + lhsize + 8);
}

#define NUM_SAMPLE 1024*512

typedef struct{
  short channels;
  short format;
  unsigned int srate;
  short bits;
  unsigned int datasize;
} WAV_INFO;

/*
 * ADPCM tables
 */

static const int piIndexTable2[2] =
{
    -1, 2,
};

static const int piIndexTable3[4] =
{
    -1, -1, 2, 4,
};

static const int piIndexTable4[8] =
{
    -1, -1, -1, -1, 2, 4, 6, 8,
};

static const int piIndexTable5[16] =
{
 -1, -1, -1, -1, -1, -1, -1, -1, 1, 2, 4, 6, 8, 10, 13, 16,
};

static const int* ppiIndexTables[] =
{
    piIndexTable2,
    piIndexTable3,
    piIndexTable4,
    piIndexTable5
};

static const int piStepSizeTable[89] =
{
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

void writeADPCMData(SWFInput input, int bits16, int stereo, int sample_count, SWFOutput output) {
  int nBits = 4;  /* number of bits per ADPCM sample */
  int iSignBit = 1 << (nBits-1);  /* Sign bit mask */
  const int*  piIndexTable = ppiIndexTables[nBits-2];  /* Select index table to use */

  int iValPred[2] = {0, 0};   /* Predicted output value(s) */
  int iIndex[2]   = {0, 0};   /* Indeces int StepSizeTable */
  int iStep[2]    = {0, 0};   /* Step size */

  int iSampleCount = sample_count;  /* Number of samples. */
  int iChannelCount = 1 + stereo;  /* Number of channels (mono, stereo) */

  short*  psSample = malloc(sizeof(short) * NUM_SAMPLE * iChannelCount);  /* Pointer to start of 16-bit/sample data */

  int i, j;
  int sindex = NUM_SAMPLE * iChannelCount;

  /* Write number of bits per ADPCM sample */
  SWFOutput_writeBits(output, nBits-2, 2);

  for (i=0; i < iSampleCount; i++) {
	if (sindex == NUM_SAMPLE * iChannelCount) {
		if (bits16) {
			for (j = 0; j < min(NUM_SAMPLE, iSampleCount - i) * iChannelCount; j++)
				psSample[j] = (short)SWFInput_getSInt16(input);
		}
		else {
			for (j = 0; j < min(NUM_SAMPLE, iSampleCount - i) * iChannelCount; j++)
				psSample[j] = (short)SWFInput_getChar(input);
		}
		sindex = 0;
	}
    if ((i & 0xfff) == 0) {
      int c;
      for (c=0; c<iChannelCount; c++) {
	/* First sample in the block, so no delta */
	short sSample = psSample[sindex++];
	int iDiff;

	/* Write full 16-bit sample */
	SWFOutput_writeSBits(output, (int)sSample, 16);  /* XXX - should be swapped? */
	iValPred[c] = sSample;

	/* Calculate initial index & step */
	if (iSampleCount == 1)  /* XXX - stereo OK? */
	  iDiff = abs(psSample[sindex] - sSample);
	else
	  iDiff = abs(psSample[sindex+1] - sSample);
	for (iIndex[c]=0; iIndex[c] < 88; iIndex[c]++)
	  if (iDiff <= piStepSizeTable[iIndex[c]])
	    break;

	if (iIndex[c] > 63)
	  iIndex[c] = 63;

	iStep[c] = piStepSizeTable[iIndex[c]];

	/* Write initial index into StepSizeTable */
	SWFOutput_writeBits(output, (int)iIndex[c], 6);
      }
    } else {
      int c;
      for (c=0; c<iChannelCount; c++) {
	short sSample = psSample[sindex++];
	int iDiff, iSign, iDelta, iVPDiff;
	int k;

	/* Step 1 - compute difference with previous value */
	iDiff = sSample - iValPred[c];

	/* Make iDiff absolute value */
	iSign = (iDiff < 0) ? iSignBit : 0;
	if (iSign)
	  iDiff = (-iDiff);

	/*
	 * Step 2 - Divide and clamp
	 * Note:
	 * This code *approximately* computes:
         *    iDelta = iDiff*4/iStep;
         *    iVPDiff = (iDelta+0.5)*iStep/4;
         * but in shift step bits are dropped. The net result of this is
         * that even if you have fast mul/div hardware you cannot put it to
         * good use since the fixup would be too expensive.
	 */
	iDelta  = 0;
	iVPDiff = (iStep[c] >> (nBits-1));

	for (k = nBits-2; k >= 0; k--, iStep[c] >>= 1) {
	  if (iDiff >= iStep[c]) {
	    iDelta |= (1<<k);
	    iVPDiff += iStep[c];
	    if (k > 0)
	      iDiff -= iStep[c];
	  }
	}

	/* Step 3 - Update previous value */
	if (iSign)
	  iValPred[c] -= iVPDiff;
	else
	  iValPred[c] += iVPDiff;

	/* Step 4 - Clamp previous value to 16 bits */
	if (iValPred[c] > 32767)
	  iValPred[c] = 32767;
	else if (iValPred[c] < -32768)
	  iValPred[c] = -32768;

	/* Step 5 - Assemble value, update index and step values */
	iIndex[c] += piIndexTable[iDelta];
	iDelta |= iSign; /* Or with iSign *after* indexing */

	/* Clamp StepSizeTable index */
	if (iIndex[c] < 0) iIndex[c] = 0;
	if (iIndex[c] > 88) iIndex[c] = 88;
	iStep[c] = piStepSizeTable[iIndex[c]];

	/* Step 6 - Output value */
	SWFOutput_writeBits(output, iDelta, nBits);
      }
    }
  }
  free(psSample);
}

WAV_INFO readWAVheader(SWFInput input)
{
  WAV_INFO info;
  unsigned long i;

  i = (SWFInput_getUInt32(input) != 0x46464952);  /* "RIFF" */
  SWFInput_seek(input, 4, SEEK_CUR);
  i = (SWFInput_getUInt32(input) != 0x45564157);  /* "WAVE" */
  if (!i)
  {
    if (SWFInput_getUInt32(input) != 0x20746d66)  /* "fmt " */
      goto notwave;
    i = SWFInput_getUInt32(input);
    info.format = SWFInput_getUInt16(input);
    info.channels = SWFInput_getUInt16(input);
    info.srate = SWFInput_getUInt32(input);
    SWFInput_seek(input, 6, SEEK_CUR);
    info.bits = SWFInput_getUInt16(input);
    SWFInput_seek(input, i - 16, SEEK_CUR);
    if (SWFInput_getUInt32(input) == 0x74636166)
    {
      SWFInput_seek(input, 8, SEEK_CUR);
    }else{
      SWFInput_seek(input, -4, SEEK_CUR);
    }
    if (SWFInput_getUInt32(input) != 0x61746164)  /* "data" */
      goto notwave;
    info.datasize = SWFInput_getUInt32(input);
  }else{
notwave:
    info.datasize = 0;
  }

  return info;
}

EXPORT BOOL WINAPI wav2adpcm(HSPEXINFO *hei, int p1, int p2, int p3)
{
  FILE *fp_in, *fp_out;
  SWFInput input;
  SWFOutput output;
  short *samples, *i_sample, bits16, stereo;
  unsigned int i, sample_count, fsize;
  char *fname_in, *fname_out, *fname_tmp, *p;
  WAV_INFO wav;

  fname_in = hei->HspFunc_prm_gets();
  fname_tmp = malloc(strlen(fname_in) + 7);
  strcpy(fname_tmp, fname_in);
  ChangeExt(fname_tmp, "adpcm");
  fname_out = hei->HspFunc_prm_getds(fname_tmp);
  bits16 = hei->HspFunc_prm_getdi(1);
  stereo = hei->HspFunc_prm_getdi(1);

  fp_in = fopen(fname_in, "rb");
  if (!fp_in) {
    free(fname_tmp);
    return -1;
  }

  fp_out = fopen(fname_out, "wb");
  free(fname_tmp);
  if (!fp_out) {
    return -2;
  }

  input = newSWFInput_file(fp_in);

  wav = readWAVheader(input);

  if (wav.format != 1)
    return -3;
  stereo = wav.channels - 1;
  if (wav.bits == 8)
    bits16 = 0;

  /* read RAW sound data */
  if (wav.datasize)
  {
    sample_count = wav.datasize / (1+bits16) / (1+stereo);
  }else{
    sample_count = SWFInput_length(input) / (1+bits16) / (1+stereo);
  }

  /* create a SWFOutput */
  output = newSWFOutput();

  writeADPCMData(input, bits16, stereo, sample_count, output);

  destroySWFInput(input);
  fclose(fp_in);

  fsize = SWFOutput_getLength(output);

  /* write to file */
  SWFOutput_writeToMethod(output, fileOutputMethod, fp_out);

  destroySWFOutput(output);
  fclose(fp_out);

  return -fsize;
}

EXPORT BOOL WINAPI getwavinfo(HSPEXINFO *hei, int p1, int p2, int p3)
{
  char flag = 0;
  unsigned int *p;
  double length;
  FILE *fp;
  SWFInput input;
  WAV_INFO wav;
  PVAL2 *pv;

  p = (unsigned int*)hei->HspFunc_prm_getv();
  pv = *hei->pval;
  fp = fopen(hei->HspFunc_prm_gets(), "rb");

  if (!fp)
    return -1;

  input = newSWFInput_file(fp);

  wav = readWAVheader(input);

  fclose(fp);

  if (pv->flag != 4 || pv->len[1] < 6)
    return -2;

  p[0] = wav.datasize / (wav.channels * wav.srate * (wav.bits / 8));
  p[1] = wav.channels;
  p[2] = wav.format;
  p[3] = wav.srate;
  p[4] = wav.bits;
  p[5] = wav.datasize;

  if (wav.channels == 2)
    flag |= 1;

  switch (wav.srate)
  {
    case 11025:
      flag |= 4;
      break;
    case 22050:
      flag |= 8;
      break;
    case 44100:
      flag |= 12;
      break;
    default:
      break;
  }

  if (wav.bits == 16)
    flag |= 2;

  return -flag;
}
