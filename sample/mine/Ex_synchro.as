fname="test.swf"
;-------------------------------------------------------------------------------
;sample 02 テキストの描画
;-------------------------------------------------------------------------------
#include "minghsp.as"
frame_rate=12
mes "Font"
SWFFont fnt,"ipag.fdb"			;フォントの登録

SWFMovie m					;ムービーを設定
m_setBackground 0xff,0xff,0xff			;背景色(R,G,B)
m_setDimension 320f,230f			;ムービーの幅と高さを設定

mes "Sound"
fn_mp3="sample.mp3"				;streaming 貼り付け
mci "open \""+fn_mp3+"\" alias mysound"
mci "set mysound time format milliseconds"
mci "status mysound length"
fr_length=0+stat*frame_rate/1000
m_streamMp3  "sample.mp3"
m_setFrames fr_length+12

mes "Shape"
;アニメーション定義開始

sdim wrd,21,4
wrd.0="うさぎおいしかのやま"
wrd.1="こぶなつりしかのかわ"
wrd.2="ゆめはいまもめぐりて"
wrd.3="わすれがたきふるさと"

repeat fr_length+12
	frn=cnt
	gosub *word_rt
	m_nextFrame					;次のフレームへ
loop

mes "Save"
;以下swfのセーブ----------------------------
skiperr 1
del fname
skiperr
m_save fname
mes "Output file.."+fname+"("+stat+"bytes)"
mes "End"
;-------------------------------------------
stop

*txt_rt
;引数	sub_txt
;	sub_x
;	sub_y

	SWFText txt					;txtというテキストの設定開始
	t_setFont fnt					;txtのフォントをfntにセット
	t_setColor 0x00, 0x00, 0xff			;カラーを(0xff,0xff,0x00)=(R,G,B)にセット
	mhsp_float pt,12				;高さ12pt.をfloatに変換
	t_setHeight pt					;高さをセット(12pt.)
	mhsp_float x,0					;テキストの0,0のポジションは、
	mhsp_float y,12					;x軸はテキストの中央、y軸は下端なので
	t_moveTo x,y					;書き始めのy値を高さ分ずらす。
	t_addString sub_txt
	m_add i,txt					;先に定義した図形"s"をムービーに"i"として追加
	mhsp_seldisplayitem i				;i_...の対象を"i"とする
	mhsp_float x,sub_x				;テキストの0,0のポジションは、
	mhsp_float y,sub_y				;x軸はテキストの中央、y軸は下端なので
	i_moveTo x,y					;
	return



*word_rt

	if frn=110:sub_x=12*0:sub_y=12:strmid sub_txt,wrd.0, 0,2:gosub *txt_rt	;う
	if frn=116:sub_x=12*1:sub_y=12:strmid sub_txt,wrd.0, 2,2:gosub *txt_rt	;さ
	if frn=123:sub_x=12*2:sub_y=12:strmid sub_txt,wrd.0, 4,2:gosub *txt_rt	;ぎ
	if frn=131:sub_x=12*3:sub_y=12:strmid sub_txt,wrd.0, 6,2:gosub *txt_rt	;お
	if frn=142:sub_x=12*4:sub_y=12:strmid sub_txt,wrd.0, 8,2:gosub *txt_rt	;い
	if frn=145:sub_x=12*5:sub_y=12:strmid sub_txt,wrd.0,10,2:gosub *txt_rt	;し
	if frn=152:sub_x=12*6:sub_y=12:strmid sub_txt,wrd.0,12,2:gosub *txt_rt	;か
	if frn=159:sub_x=12*7:sub_y=12:strmid sub_txt,wrd.0,14,2:gosub *txt_rt	;の
	if frn=166:sub_x=12*8:sub_y=12:strmid sub_txt,wrd.0,16,2:gosub *txt_rt	;や
	if frn=173:sub_x=12*9:sub_y=12:strmid sub_txt,wrd.0,18,2:gosub *txt_rt	;ま

	if frn=195:sub_x=12*0:sub_y=24:strmid sub_txt,wrd.1, 0,2:gosub *txt_rt	;こ
	if frn=203:sub_x=12*1:sub_y=24:strmid sub_txt,wrd.1, 2,2:gosub *txt_rt	;ぶ
	if frn=210:sub_x=12*2:sub_y=24:strmid sub_txt,wrd.1, 4,2:gosub *txt_rt	;な
	if frn=217:sub_x=12*3:sub_y=24:strmid sub_txt,wrd.1, 6,2:gosub *txt_rt	;つ
	if frn=227:sub_x=12*4:sub_y=24:strmid sub_txt,wrd.1, 8,2:gosub *txt_rt	;り
	if frn=231:sub_x=12*5:sub_y=24:strmid sub_txt,wrd.1,10,2:gosub *txt_rt	;し
	if frn=238:sub_x=12*6:sub_y=24:strmid sub_txt,wrd.1,12,2:gosub *txt_rt	;か
	if frn=245:sub_x=12*7:sub_y=24:strmid sub_txt,wrd.1,14,2:gosub *txt_rt	;の
	if frn=252:sub_x=12*8:sub_y=24:strmid sub_txt,wrd.1,16,2:gosub *txt_rt	;か
	if frn=260:sub_x=12*9:sub_y=24:strmid sub_txt,wrd.1,18,2:gosub *txt_rt	;わ

	if frn=282:sub_x=12*0:sub_y=36:strmid sub_txt,wrd.2, 0,2:gosub *txt_rt	;ゆ
	if frn=289:sub_x=12*1:sub_y=36:strmid sub_txt,wrd.2, 2,2:gosub *txt_rt	;め
	if frn=296:sub_x=12*2:sub_y=36:strmid sub_txt,wrd.2, 4,2:gosub *txt_rt	;は
	if frn=303:sub_x=12*3:sub_y=36:strmid sub_txt,wrd.2, 6,2:gosub *txt_rt	;い
	if frn=310:sub_x=12*4:sub_y=36:strmid sub_txt,wrd.2, 8,2:gosub *txt_rt	;ま
	if frn=317:sub_x=12*5:sub_y=36:strmid sub_txt,wrd.2,10,2:gosub *txt_rt	;も
	if frn=325:sub_x=12*6:sub_y=36:strmid sub_txt,wrd.2,12,2:gosub *txt_rt	;め
	if frn=332:sub_x=12*7:sub_y=36:strmid sub_txt,wrd.2,14,2:gosub *txt_rt	;ぐ
	if frn=346:sub_x=12*8:sub_y=36:strmid sub_txt,wrd.2,16,2:gosub *txt_rt	;り
	if frn=353:sub_x=12*9:sub_y=36:strmid sub_txt,wrd.2,18,2:gosub *txt_rt	;て

	if frn=371:sub_x=12*0:sub_y=48:strmid sub_txt,wrd.3, 0,2:gosub *txt_rt	;わ
	if frn=376:sub_x=12*1:sub_y=48:strmid sub_txt,wrd.3, 2,2:gosub *txt_rt	;す
	if frn=383:sub_x=12*2:sub_y=48:strmid sub_txt,wrd.3, 4,2:gosub *txt_rt	;れ
	if frn=390:sub_x=12*3:sub_y=48:strmid sub_txt,wrd.3, 6,2:gosub *txt_rt	;が
	if frn=400:sub_x=12*4:sub_y=48:strmid sub_txt,wrd.3, 8,2:gosub *txt_rt	;た
	if frn=404:sub_x=12*5:sub_y=48:strmid sub_txt,wrd.3,10,2:gosub *txt_rt	;き
	if frn=411:sub_x=12*6:sub_y=48:strmid sub_txt,wrd.3,12,2:gosub *txt_rt	;ふ
	if frn=419:sub_x=12*7:sub_y=48:strmid sub_txt,wrd.3,14,2:gosub *txt_rt	;る
	if frn=425:sub_x=12*8:sub_y=48:strmid sub_txt,wrd.3,16,2:gosub *txt_rt	;さ
	if frn=433:sub_x=12*9:sub_y=48:strmid sub_txt,wrd.3,18,2:gosub *txt_rt	;と

	return
