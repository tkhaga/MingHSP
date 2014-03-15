
fname="test.swf"
;-------------------------------------------------------------------------------
;sample 03 MP3Streaming
;-------------------------------------------------------------------------------
#include "minghsp.as"

SWFMovie m   ;ムービーを設定
m_setBackground 0xff,0xff,0xff   ;背景色(R,G,B)
m_setDimension 320f,230f   ;ムービーの幅と高さを設定

m_setRate 12f     ;FrameRate
m_streamMp3 "sample.mp3"

m_setFrames 984

;以下swfのセーブ----------------------------
skiperr 1
del fname
skiperr
m_save fname
mes "Output file.."+fname+"("+stat+"bytes)"
mes "end"
;-------------------------------------------
stop