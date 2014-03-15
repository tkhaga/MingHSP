#ifndef _MINGHSP_PDR_H_
#define _MINGHSP_PDR_H_


#define PDR_MORPH       0x01	/* StateNewStyles���g�p���Ȃ��A�G�b�W�̏ȗ����s��Ȃ� */
#define PDR_GETSIZEONLY 0x02	/* �T�C�Y�̎擾�̂� */
#define PDR_ASCLIP      0x04	/* PDCLIP�`�����N���b�v�Ƃ��Ĉ��� */

#ifdef PDR_USE_SWFINPUT
int SWFShape_loadPdr(SWFShape shape, int size[], SWFInput input, int flag, float orgX, float orgY);
#else
int SWFShape_loadPdr(SWFShape shape, int size[], char *filename, int flag, int pdrsize, float orgX, float orgY);
#endif

#endif
