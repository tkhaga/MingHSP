#include "minghsp.as"
;test(from "blink.php")

ming_useSWFVersion 4

SWFShape s
SWFMovieClip c
s_addSolidFill f,0,0xff,0x80,0xff
s_setRightFill f
s_movePenTo -48f,-48f
s_drawLine 96f
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

m_setBackground 0xff,0xff,0xff
m_setDimension 144f,144f

fname="test.swf"
m_save fname
mes stat
stop