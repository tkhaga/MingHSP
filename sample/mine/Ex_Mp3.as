fname="test.swf"
;-------------------------------------------------------------------------------
;sample 03 Mp3 Streaming�Đ�
;-------------------------------------------------------------------------------
#include "minghsp.as"

frame_rate=12	;�t���[�����[�g

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

m_streamMp3  "sample.mp3"
m_setFrames fr_length+12

;;;repeat 1410:m_nextFrame:loop

;�ȉ�swf�̃Z�[�u----------------------------
skiperr 1
del fname
skiperr
m_save fname
mes "Output file.."+fname+"("+stat+"bytes)"
mes "end"
;-------------------------------------------
stop
