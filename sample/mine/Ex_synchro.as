fname="test.swf"
;-------------------------------------------------------------------------------
;sample 02 �e�L�X�g�̕`��
;-------------------------------------------------------------------------------
#include "minghsp.as"
frame_rate=12
mes "Font"
SWFFont fnt,"ipag.fdb"			;�t�H���g�̓o�^

SWFMovie m					;���[�r�[��ݒ�
m_setBackground 0xff,0xff,0xff			;�w�i�F(R,G,B)
m_setDimension 320f,230f			;���[�r�[�̕��ƍ�����ݒ�

mes "Sound"
fn_mp3="sample.mp3"				;streaming �\��t��
mci "open \""+fn_mp3+"\" alias mysound"
mci "set mysound time format milliseconds"
mci "status mysound length"
fr_length=0+stat*frame_rate/1000
m_streamMp3  "sample.mp3"
m_setFrames fr_length+12

mes "Shape"
;�A�j���[�V������`�J�n

sdim wrd,21,4
wrd.0="���������������̂��"
wrd.1="���ԂȂ肵���̂���"
wrd.2="��߂͂��܂��߂����"
wrd.3="�킷�ꂪ�����ӂ邳��"

repeat fr_length+12
	frn=cnt
	gosub *word_rt
	m_nextFrame					;���̃t���[����
loop

mes "Save"
;�ȉ�swf�̃Z�[�u----------------------------
skiperr 1
del fname
skiperr
m_save fname
mes "Output file.."+fname+"("+stat+"bytes)"
mes "End"
;-------------------------------------------
stop

*txt_rt
;����	sub_txt
;	sub_x
;	sub_y

	SWFText txt					;txt�Ƃ����e�L�X�g�̐ݒ�J�n
	t_setFont fnt					;txt�̃t�H���g��fnt�ɃZ�b�g
	t_setColor 0x00, 0x00, 0xff			;�J���[��(0xff,0xff,0x00)=(R,G,B)�ɃZ�b�g
	mhsp_float pt,12				;����12pt.��float�ɕϊ�
	t_setHeight pt					;�������Z�b�g(12pt.)
	mhsp_float x,0					;�e�L�X�g��0,0�̃|�W�V�����́A
	mhsp_float y,12					;x���̓e�L�X�g�̒����Ay���͉��[�Ȃ̂�
	t_moveTo x,y					;�����n�߂�y�l�����������炷�B
	t_addString sub_txt
	m_add i,txt					;��ɒ�`�����}�`"s"�����[�r�[��"i"�Ƃ��Ēǉ�
	mhsp_seldisplayitem i				;i_...�̑Ώۂ�"i"�Ƃ���
	mhsp_float x,sub_x				;�e�L�X�g��0,0�̃|�W�V�����́A
	mhsp_float y,sub_y				;x���̓e�L�X�g�̒����Ay���͉��[�Ȃ̂�
	i_moveTo x,y					;
	return



*word_rt

	if frn=110:sub_x=12*0:sub_y=12:strmid sub_txt,wrd.0, 0,2:gosub *txt_rt	;��
	if frn=116:sub_x=12*1:sub_y=12:strmid sub_txt,wrd.0, 2,2:gosub *txt_rt	;��
	if frn=123:sub_x=12*2:sub_y=12:strmid sub_txt,wrd.0, 4,2:gosub *txt_rt	;��
	if frn=131:sub_x=12*3:sub_y=12:strmid sub_txt,wrd.0, 6,2:gosub *txt_rt	;��
	if frn=142:sub_x=12*4:sub_y=12:strmid sub_txt,wrd.0, 8,2:gosub *txt_rt	;��
	if frn=145:sub_x=12*5:sub_y=12:strmid sub_txt,wrd.0,10,2:gosub *txt_rt	;��
	if frn=152:sub_x=12*6:sub_y=12:strmid sub_txt,wrd.0,12,2:gosub *txt_rt	;��
	if frn=159:sub_x=12*7:sub_y=12:strmid sub_txt,wrd.0,14,2:gosub *txt_rt	;��
	if frn=166:sub_x=12*8:sub_y=12:strmid sub_txt,wrd.0,16,2:gosub *txt_rt	;��
	if frn=173:sub_x=12*9:sub_y=12:strmid sub_txt,wrd.0,18,2:gosub *txt_rt	;��

	if frn=195:sub_x=12*0:sub_y=24:strmid sub_txt,wrd.1, 0,2:gosub *txt_rt	;��
	if frn=203:sub_x=12*1:sub_y=24:strmid sub_txt,wrd.1, 2,2:gosub *txt_rt	;��
	if frn=210:sub_x=12*2:sub_y=24:strmid sub_txt,wrd.1, 4,2:gosub *txt_rt	;��
	if frn=217:sub_x=12*3:sub_y=24:strmid sub_txt,wrd.1, 6,2:gosub *txt_rt	;��
	if frn=227:sub_x=12*4:sub_y=24:strmid sub_txt,wrd.1, 8,2:gosub *txt_rt	;��
	if frn=231:sub_x=12*5:sub_y=24:strmid sub_txt,wrd.1,10,2:gosub *txt_rt	;��
	if frn=238:sub_x=12*6:sub_y=24:strmid sub_txt,wrd.1,12,2:gosub *txt_rt	;��
	if frn=245:sub_x=12*7:sub_y=24:strmid sub_txt,wrd.1,14,2:gosub *txt_rt	;��
	if frn=252:sub_x=12*8:sub_y=24:strmid sub_txt,wrd.1,16,2:gosub *txt_rt	;��
	if frn=260:sub_x=12*9:sub_y=24:strmid sub_txt,wrd.1,18,2:gosub *txt_rt	;��

	if frn=282:sub_x=12*0:sub_y=36:strmid sub_txt,wrd.2, 0,2:gosub *txt_rt	;��
	if frn=289:sub_x=12*1:sub_y=36:strmid sub_txt,wrd.2, 2,2:gosub *txt_rt	;��
	if frn=296:sub_x=12*2:sub_y=36:strmid sub_txt,wrd.2, 4,2:gosub *txt_rt	;��
	if frn=303:sub_x=12*3:sub_y=36:strmid sub_txt,wrd.2, 6,2:gosub *txt_rt	;��
	if frn=310:sub_x=12*4:sub_y=36:strmid sub_txt,wrd.2, 8,2:gosub *txt_rt	;��
	if frn=317:sub_x=12*5:sub_y=36:strmid sub_txt,wrd.2,10,2:gosub *txt_rt	;��
	if frn=325:sub_x=12*6:sub_y=36:strmid sub_txt,wrd.2,12,2:gosub *txt_rt	;��
	if frn=332:sub_x=12*7:sub_y=36:strmid sub_txt,wrd.2,14,2:gosub *txt_rt	;��
	if frn=346:sub_x=12*8:sub_y=36:strmid sub_txt,wrd.2,16,2:gosub *txt_rt	;��
	if frn=353:sub_x=12*9:sub_y=36:strmid sub_txt,wrd.2,18,2:gosub *txt_rt	;��

	if frn=371:sub_x=12*0:sub_y=48:strmid sub_txt,wrd.3, 0,2:gosub *txt_rt	;��
	if frn=376:sub_x=12*1:sub_y=48:strmid sub_txt,wrd.3, 2,2:gosub *txt_rt	;��
	if frn=383:sub_x=12*2:sub_y=48:strmid sub_txt,wrd.3, 4,2:gosub *txt_rt	;��
	if frn=390:sub_x=12*3:sub_y=48:strmid sub_txt,wrd.3, 6,2:gosub *txt_rt	;��
	if frn=400:sub_x=12*4:sub_y=48:strmid sub_txt,wrd.3, 8,2:gosub *txt_rt	;��
	if frn=404:sub_x=12*5:sub_y=48:strmid sub_txt,wrd.3,10,2:gosub *txt_rt	;��
	if frn=411:sub_x=12*6:sub_y=48:strmid sub_txt,wrd.3,12,2:gosub *txt_rt	;��
	if frn=419:sub_x=12*7:sub_y=48:strmid sub_txt,wrd.3,14,2:gosub *txt_rt	;��
	if frn=425:sub_x=12*8:sub_y=48:strmid sub_txt,wrd.3,16,2:gosub *txt_rt	;��
	if frn=433:sub_x=12*9:sub_y=48:strmid sub_txt,wrd.3,18,2:gosub *txt_rt	;��

	return
