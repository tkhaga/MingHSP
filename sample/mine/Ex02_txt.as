fname="test.swf"
;-------------------------------------------------------------------------------
;sample 02 テキストの描画
;-------------------------------------------------------------------------------
#include "minghsp.as"

SWFFont fnt,"ipag.fdb"			;フォントの登録
SWFText txt					;txtというテキストの設定開始

t_setFont fnt					;txtのフォントをfntにセット
;;;;colconv e,0xff,0xff,0x00,0xff		;× eにカラーを設定(R,G,B,アルファ値)
;;;;t_setColor e				;× カラーを(0xff,0xff,0x00,)=(R,G,B,α)にセット
t_setColor 0x00, 0x00, 0xff			;カラーを(0xff,0xff,0x00)=(R,G,B)にセット
mhsp_float pt,12				;高さ12pt.をfloatに変換
t_setHeight pt					;高さをセット(12pt.)

mhsp_float x,0					;テキストの0,0のポジションは、
mhsp_float y,12					;x軸はテキストの中央、y軸は下端なので
t_moveTo x,y					;書き始めのy値を高さ分ずらす。

t_addString "MINGで日本語"

;ここまで"txt"というテキストの定義

SWFMovie m					;ムービーを設定
m_setBackground 0xff,0xff,0xff			;背景色(R,G,B)
m_setDimension 320f,230f			;ムービーの幅と高さを設定

m_add i,txt					;先に定義した図形"s"をムービーに"i"として追加
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
