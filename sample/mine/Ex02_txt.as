fname="test.swf"
;-------------------------------------------------------------------------------
;sample 02 �e�L�X�g�̕`��
;-------------------------------------------------------------------------------
#include "minghsp.as"

SWFFont fnt,"ipag.fdb"			;�t�H���g�̓o�^
SWFText txt					;txt�Ƃ����e�L�X�g�̐ݒ�J�n

t_setFont fnt					;txt�̃t�H���g��fnt�ɃZ�b�g
;;;;colconv e,0xff,0xff,0x00,0xff		;�~ e�ɃJ���[��ݒ�(R,G,B,�A���t�@�l)
;;;;t_setColor e				;�~ �J���[��(0xff,0xff,0x00,)=(R,G,B,��)�ɃZ�b�g
t_setColor 0x00, 0x00, 0xff			;�J���[��(0xff,0xff,0x00)=(R,G,B)�ɃZ�b�g
mhsp_float pt,12				;����12pt.��float�ɕϊ�
t_setHeight pt					;�������Z�b�g(12pt.)

mhsp_float x,0					;�e�L�X�g��0,0�̃|�W�V�����́A
mhsp_float y,12					;x���̓e�L�X�g�̒����Ay���͉��[�Ȃ̂�
t_moveTo x,y					;�����n�߂�y�l�����������炷�B

t_addString "MING�œ��{��"

;�����܂�"txt"�Ƃ����e�L�X�g�̒�`

SWFMovie m					;���[�r�[��ݒ�
m_setBackground 0xff,0xff,0xff			;�w�i�F(R,G,B)
m_setDimension 320f,230f			;���[�r�[�̕��ƍ�����ݒ�

m_add i,txt					;��ɒ�`�����}�`"s"�����[�r�[��"i"�Ƃ��Ēǉ�
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
