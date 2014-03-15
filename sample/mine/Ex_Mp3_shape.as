fname="test.swf"
;-------------------------------------------------------------------------------
;sample 03 Mp3 Streaming再生
;-------------------------------------------------------------------------------
#include "minghsp.as"

frame_rate=12	;フレームレート

SWFShape s					;sという図形の設定開始

 			;eにカラーを設定   (R,G,B,アルファ値(0xFF=100%))
s_setline 50,0x0,0x0,0xff,0xff				;ラインの形式を設定(線幅(TWIPS),カラー=e)
s_addSolidFill f,0xff,0x0,0x0,0xff	;fにSolidFillを定義(R,G,B,アルファ値(0xFF=100%))
s_setRightFill f				;RightFillをfに設定

s_movePenTo   10f,10f				;= pos   10,10
s_drawLineTo 310f,10f				;= line 310,10
s_drawLineTo 310f,230f				;= line 310,10
s_drawCurveTo 10f,230f,10f,10f			;カレント-(10,230)-(10,10)の2次曲線を引く

;ここまで"s"という図形の定義

SWFMovie m					;ムービーを設定
mhsp_float fr,frame_rate
m_setRate fr					;FrameRate
m_setBackground 0xff,0xff,0xff			;背景色(R,G,B)
m_setDimension 320f,230f			;ムービーの幅と高さを設定

fn_mp3="sample.mp3"
mci "open \""+fn_mp3+"\" alias mysound"
mci "set mysound time format milliseconds"
mci "status mysound length"
fr_length=0+stat*frame_rate/1000

repeat 24
	m_nextFrame					;次のフレームへ
loop

m_streamMp3  "sample.mp3"
m_setFrames fr_length+12

	m_add i,s					;先に定義した図形"s"をムービーに"i"として追加	
							;i_...の対象を"i"とする
repeat 230
	mhsp_float ii,cnt
	i_moveTo ii,ii					;
	m_nextFrame					;次のフレームへ
loop

repeat 230
	mhsp_float ii,230-cnt
	i_moveTo ii,ii					;
	m_nextFrame					;次のフレームへ
loop


;;;repeat 1410:m_nextFrame:loop

;以下swfのセーブ----------------------------
skiperr 1
del fname
skiperr
m_save fname
mes "Output file.."+fname+"("+stat+"bytes)"
exec fname,16
mes "end"
;-------------------------------------------
stop
