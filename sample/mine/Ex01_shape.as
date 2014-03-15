fname="test.swf"
;-------------------------------------------------------------------------------
;sample 01 図形の描画
;-------------------------------------------------------------------------------
#include "minghsp.as"

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
m_setBackground 0xff,0xff,0xff			;背景色(R,G,B)
m_setDimension 320f,240f			;ムービーの幅と高さを設定

m_add i,s					;先に定義した図形"s"をムービーに"i"として追加
						;i_...の対象を"i"とする
i_moveTo 0f,0f					;
m_nextFrame					;次のフレームへ

;以下swfのセーブ----------------------------
skiperr 1
del fname
skiperr
m_save fname
mes "Output file.."+fname+"("+stat+"bytes)"
mes "end"
;-------------------------------------------
stop
