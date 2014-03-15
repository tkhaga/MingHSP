#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
#include "hspdll.h"
#include "../src/blocks/input.h"
#include "../src/blocks/output.h"

#undef EXPORT
#define EXPORT __declspec(dllexport)

EXPORT BOOL WINAPI vram2dbl(HSPEXINFO *hei, int p1, int p2, int p3)
{
	BMSCR *bm;
	unsigned char *buf, *tmpbuf, *outdata, *p, *pp;
	short alpha, level, hasalpha, channels, lhsize;
	unsigned int width, tmpwidth, tmpsize, y, x, k;
	unsigned long outsize;
	PVAL2 *pvbuf;

	hei->HspFunc_prm_getv();
	pvbuf = *hei->pval;
	bm = (BMSCR *)hei->HspFunc_getbmscr(*hei->actscr);
	alpha = hei->HspFunc_prm_getdi(0xff);
	level = hei->HspFunc_prm_getdi(9);

	if (level < 1 || level > 9 || alpha < 0 || alpha > 0xff)
		return -1;

	if (alpha == 0xff)
	{
		hasalpha = 1;
	}else{
		hasalpha = 2;
	}

	if (bm->palmode)
	{
		channels = 1;
		width = (bm->sx + 3) & ~3;
	}else{
		channels = 4;
		width = (bm->sx * 3 + 3) & ~3;
	}

	tmpwidth = (bm->sx * channels + 3) & ~3;
  tmpsize = tmpwidth * bm->sy;

    tmpsize += bm->pals * (2 + hasalpha);
	tmpbuf = malloc(tmpsize);

  if (bm->palmode)
  {
    p = tmpbuf;
    pp = (char *)bm->pal;
    if (hasalpha == 2)	/* 8bitパレットモード  透明度指定有り */
    {
      for (x = 0; x < bm->pals; x++)
      {
        *p++ = pp[2] * alpha >> 8;
        *p++ = pp[1] * alpha >> 8;
        *p++ = pp[0] * alpha >> 8;
        *p++ = alpha;
        pp += 4;
      }
    }else{	/* 8bitパレットモード  透明度指定無し */
      for (x = 0; x < bm->pals; x++)
      {
        *p++ = pp[2];
        *p++ = pp[1];
        *p++ = pp[0];
        pp += 4;
      }
    }
    pp = bm->pBit + ((bm->sy - 1) * width);
    for (y = 0; y < bm->sy; y++)
    {
      memcpy(p, pp, bm->sx);
      p += tmpwidth;
      pp -= width;
    }
  }else{
    if (hasalpha == 2)	/* 24bitフルカラー  透明度指定有り */
    {
	  for (y = 0; y < bm->sy; y++)
	  {
		p = tmpbuf + (y * tmpwidth);
		pp = bm->pBit + ((bm->sy - 1 - y) * width);
	    for (x = 0; x < bm->sx; x++)
	    {
          *p++ = alpha;
          *p++ = pp[2] * alpha >> 8;
          *p++ = pp[1] * alpha >> 8;
          *p++ = pp[0] * alpha >> 8;
          pp += 3;
	    }
	  }
    }else{	/* 24bitフルカラー  透明度指定無し */
	  for (y = 0; y < bm->sy; y++)
	  {
		p = tmpbuf + (y * tmpwidth);
		pp = bm->pBit + ((bm->sy - 1 - y) * width);
	    for (x = 0; x < bm->sx; x++)
	    {
          *p++;
          *p++ = pp[2];
          *p++ = pp[1];
          *p++ = pp[0];
          pp += 3;
	    }
	  }
    }
  }
	outdata = malloc(tmpsize);

	compress2(outdata, &outsize, tmpbuf, tmpsize, level);

	if (bm->palmode)
	{
	  lhsize = 6;
	}else{
	  lhsize = 5;
	}

	free(tmpbuf);

    hei->HspFunc_val_realloc(pvbuf, outsize + lhsize + 8, 0);
    buf = pvbuf->pt;

	*buf++ = 'D';
	*buf++ = 'B';
	*buf++ = 'l';
	*buf++ = hasalpha;
	*buf++ = ((outsize + lhsize) >> 24) & 0xff;
	*buf++ = ((outsize + lhsize) >> 16) & 0xff;
	*buf++ = ((outsize + lhsize) >> 8) & 0xff;
	*buf++ = (outsize + lhsize) & 0xff;

	if (bm->palmode)
	{
	  *buf++ = 3;	/* ピクセル当たり8bit */
	}else{
	  *buf++ = 5;	/* ピクセル当たり32bit */
	}

	*buf++ = bm->sx & 0xff;
	*buf++ = (bm->sx >> 8) & 0xff;
	*buf++ = bm->sy & 0xff;
	*buf++ = (bm->sy >> 8) & 0xff;

	if (bm->palmode)
	{
	  *buf++ = bm->pals - 1;	/* パレットモード時の色数 */
	}

	memcpy(buf, outdata, outsize);

	free(outdata);

	return -(outsize + lhsize + 8);
}

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

void writeADPCMData(short *samples, int stereo, int sample_count, void* output) {
  int nBits = 4;  /* number of bits per ADPCM sample */
  int iSignBit = 1 << (nBits-1);  /* Sign bit mask */
  const int*  piIndexTable = ppiIndexTables[nBits-2];  /* Select index table to use */

  int iValPred[2] = {0, 0};   /* Predicted output value(s) */
  int iIndex[2]   = {0, 0};   /* Indeces int StepSizeTable */
  int iStep[2]    = {0, 0};   /* Step size */

  int iSampleCount = sample_count;  /* Number of samples. */
  int iChannelCount = 1 + stereo;  /* Number of channels (mono, stereo) */

  short*  psSample = samples;  /* Pointer to start of 16-bit/sample data */

  int i;

  /* Write number of bits per ADPCM sample */
  SWFOutput_writeBits(output, nBits-2, 2);

  for (i=0; i < iSampleCount; i++) {
    if ((i & 0xfff) == 0) {
      int c;
      for (c=0; c<iChannelCount; c++) {
	/* First sample in the block, so no delta */
	short sSample = *psSample++;
	int iDiff;

	/* Write full 16-bit sample */
	SWFOutput_writeSBits(output, (int)sSample, 16);  /* XXX - should be swapped? */
	iValPred[c] = sSample;

	/* Calculate initial index & step */
	if (iSampleCount == 1)  /* XXX - stereo OK? */
	  iDiff = abs(*psSample - sSample);
	else
	  iDiff = abs(*(psSample+1) - sSample);
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
	short sSample = *psSample++;
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
}

unsigned long idx = 0;

void _method(byte b, byte *data)
{
  data[idx++] = b;
}

typedef struct{
  short channels;
  short format;
  unsigned int srate;
  short bits;
  unsigned int datasize;
} WAV_INFO;

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
  FILE *fp_in;
  SWFInput input;
  SWFOutput output;
  short *samples, *i_sample, bits16, stereo;
  unsigned int i, fsize, sample_count;
  char *fname, *p;
  PVAL2 *pvout;
  WAV_INFO wav;

  hei->HspFunc_prm_getv();
  pvout = *hei->pval;
  fname = hei->HspFunc_prm_gets();
  bits16 = hei->HspFunc_prm_getdi(1);
  stereo = hei->HspFunc_prm_getdi(1);

  fp_in = fopen(fname, "rb");

  if (!fp_in)
    return -1;

  input = newSWFInput_file(fp_in);

  wav = readWAVheader(input);

  if (wav.format != 1)
    return -2;
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
  samples = malloc(sizeof(short) * sample_count * (1+stereo));
  i_sample = samples;
  if (bits16)
  {
    for (i=0; i<sample_count*(1+stereo); i++) {
      *i_sample = (short)SWFInput_getSInt16(input);
      i_sample++;
    }
  }else{
    for (i=0; i<sample_count*(1+stereo); i++) {
      *i_sample = (short)SWFInput_getChar(input);
      i_sample++;
    }
  }
  destroySWFInput(input);
  fclose(fp_in);

  /* create a SWFOutput */
  output = newSWFOutput();

  writeADPCMData(samples, stereo, sample_count, output);

  fsize = SWFOutput_length(output);

  hei->HspFunc_val_realloc(pvout, fsize, 0);
  //SWFOutput_writeBuffer(output, pvout->pt, fsize);
  SWFOutput_writeToMethod(output, (SWFByteOutputMethod)_method, pvout->pt);
  idx = 0;

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
