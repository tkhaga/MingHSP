fname="test.swf"
;-------------------------------------------------------------------------------
;sample 01 �}�`�̕`��
;-------------------------------------------------------------------------------
#include "minghsp.as"

SWFShape s					;s�Ƃ����}�`�̐ݒ�J�n

 			;e�ɃJ���[��ݒ�   (R,G,B,�A���t�@�l(0xFF=100%))
s_setline 50,0x0,0x0,0xff,0xff				;���C���̌`����ݒ�(����(TWIPS),�J���[=e)
s_addSolidFill f,0xff,0x0,0x0,0xff	;f��SolidFill���`(R,G,B,�A���t�@�l(0xFF=100%))
s_setRightFill f				;RightFill��f�ɐݒ�

s_movePenTo   10f,10f				;= pos   10,10
s_drawLineTo 310f,10f				;= line 310,10
s_drawLineTo 310f,230f				;= line 310,10
s_drawCurveTo 10f,230f,10f,10f			;�J�����g-(10,230)-(10,10)��2���Ȑ�������

;�����܂�"s"�Ƃ����}�`�̒�`

SWFMovie m					;���[�r�[��ݒ�
m_setBackground 0xff,0xff,0xff			;�w�i�F(R,G,B)
m_setDimension 320f,240f			;���[�r�[�̕��ƍ�����ݒ�

m_add i,s					;��ɒ�`�����}�`"s"�����[�r�[��"i"�Ƃ��Ēǉ�
						;i_...�̑Ώۂ�"i"�Ƃ���
i_moveTo 0f,0f					;
m_nextFrame					;���̃t���[����

;�ȉ�swf�̃Z�[�u----------------------------
skiperr 1
del fname
skiperr
m_save fname
mes "Output file.."+fname+"("+stat+"bytes)"
mes "end"
;-------------------------------------------
stop
