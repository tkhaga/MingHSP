#ifndef _MINGHSP_PDR_H_
#define _MINGHSP_PDR_H_


#define PDR_MORPH       0x01	/* StateNewStylesを使用しない、エッジの省略を行わない */
#define PDR_GETRECTONLY 0x02	/* サイズの取得のみ */
#define PDR_ASCLIP      0x04	/* PDCLIP形式をクリップとして扱う */

int SWFShape_loadPdr(SWFShape shape, int rect[], char *filename, int flag, int size, float orgX, float orgY);

#endif
