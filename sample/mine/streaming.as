
fname="test.swf"
;-------------------------------------------------------------------------------
;sample 03 MP3Streaming
;-------------------------------------------------------------------------------
#include "minghsp.as"

SWFMovie m   ;���[�r�[��ݒ�
m_setBackground 0xff,0xff,0xff   ;�w�i�F(R,G,B)
m_setDimension 320f,230f   ;���[�r�[�̕��ƍ�����ݒ�

m_setRate 12f     ;FrameRate
m_streamMp3 "sample.mp3"

m_setFrames 984

;�ȉ�swf�̃Z�[�u----------------------------
skiperr 1
del fname
skiperr
m_save fname
mes "Output file.."+fname+"("+stat+"bytes)"
mes "end"
;-------------------------------------------
stop