fname="test.swf"
;-------------------------------------------------------------------------------
;sample 03 Mp3 Streaming�Đ�
;-------------------------------------------------------------------------------
#include "minghsp.as"

frame_rate=12	;�t���[�����[�g

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
mhsp_float fr,frame_rate
m_setRate fr					;FrameRate
m_setBackground 0xff,0xff,0xff			;�w�i�F(R,G,B)
m_setDimension 320f,230f			;���[�r�[�̕��ƍ�����ݒ�

fn_mp3="sample.mp3"
mci "open \""+fn_mp3+"\" alias mysound"
mci "set mysound time format milliseconds"
mci "status mysound length"
fr_length=0+stat*frame_rate/1000

repeat 24
	m_nextFrame					;���̃t���[����
loop

m_streamMp3  "sample.mp3"
m_setFrames fr_length+12

	m_add i,s					;��ɒ�`�����}�`"s"�����[�r�[��"i"�Ƃ��Ēǉ�	
							;i_...�̑Ώۂ�"i"�Ƃ���
repeat 230
	mhsp_float ii,cnt
	i_moveTo ii,ii					;
	m_nextFrame					;���̃t���[����
loop

repeat 230
	mhsp_float ii,230-cnt
	i_moveTo ii,ii					;
	m_nextFrame					;���̃t���[����
loop


;;;repeat 1410:m_nextFrame:loop

;�ȉ�swf�̃Z�[�u----------------------------
skiperr 1
del fname
skiperr
m_save fname
mes "Output file.."+fname+"("+stat+"bytes)"
exec fname,16
mes "end"
;-------------------------------------------
stop
