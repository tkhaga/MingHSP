#ifndef _MINGHSP_PDR_H_
#define _MINGHSP_PDR_H_


#define PDR_MORPH       0x01	/* StateNewStyles���g�p���Ȃ��A�G�b�W�̏ȗ����s��Ȃ� */
#define PDR_GETRECTONLY 0x02	/* �T�C�Y�̎擾�̂� */
#define PDR_ASCLIP      0x04	/* PDCLIP�`�����N���b�v�Ƃ��Ĉ��� */

int SWFShape_loadPdr(SWFShape shape, int rect[], char *filename, int flag, int size, float orgX, float orgY);

#endif
