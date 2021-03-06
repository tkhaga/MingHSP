	#include "minghsp.as"		;ヘッダファイルの結合
	;test(from "blink.php")

	ming_useSWFVersion 4		;フラッシュのバージョンを4に設定(携帯で再生するため)

	SWFShape s			;SWFShapeオブジェクトの作成(sにオブジェクトの参照を代入)
	;sel_s s			;s_で始まる命令で操作するSWFShapeオブジェクトの指定(現バージョンでは不要)
	SWFMovieClip c			;SWFMovieClipオブジェクトの作成(cにオブジェクトの参照を代入)
	s_addSolidFill f,0,0xff,0x80	;(R,G,B,A)=(0,0xff,0x80,0xff)の塗りつぶしオブジェクトを作成(fにオブジェクトの参照を代入)
	s_setRightFill f		;右フィルにfを設定
	s_movePenTo -48f,-48f		;描画基準点を(-48,-48)に移動
	s_drawLine 96f			;直線を引く
	s_drawLine 0,96f
	s_drawLine -96f
	s_drawLine 0,-96f
	mc_add i,s

	repeat 24
		mhsp_float a,cnt
		mhsp_div a,a,24f
		i_multColor 1f,1f,1f,a
		mhsp_float a,cnt*-90
		mhsp_div a,a,24f
		i_rotateTo a
		mc_nextFrame
	loop

	repeat 24
		mhsp_float a,24-cnt
		mhsp_div a,a,24f
		i_multColor 1f,1f,1f,a
		mhsp_float a,cnt*-90
		mhsp_div a,a,24f
		i_rotateTo a
		mc_nextFrame
	loop

	SWFMovie m
	m_add i,c
	sel_i i
	i_moveTo 72f,72f

	m_setBackground 0xff,0xff,0xff	;背景色を(0xff,0xff,0xff)に設定
	m_setDimension 144f,144f	;表示領域を(144,144)に設定

	fname="test.swf"
	m_save fname
	mes stat			;ファイルサイズを表示
	stop
