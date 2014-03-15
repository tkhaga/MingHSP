#ifndef _MINGHSP_PDR_H_
#define _MINGHSP_PDR_H_


#define PDR_MORPH       0x01	/* StateNewStylesを使用しない、エッジの省略を行わない */
#define PDR_GETSIZEONLY 0x02	/* サイズの取得のみ */
#define PDR_ASCLIP      0x04	/* PDCLIP形式をクリップとして扱う */

#ifdef PDR_USE_SWFINPUT
int SWFShape_loadPdr(SWFShape shape, int size[], SWFInput input, int flag, float orgX, float orgY);
#else
int SWFShape_loadPdr(SWFShape shape, int size[], char *filename, int flag, int pdrsize, float orgX, float orgY);
#endif

#endif
