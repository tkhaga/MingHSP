#include "minghsp.as"

#include "rrmod/com/activex.as"
#include "swfpreview.as"

SWFShape s
s_addSolidFill f,0xff,0,0,0xff
s_setRightFill f

s_movePenTo -25f,-25f
s_drawLineTo 25f,-25f
s_drawLineTo 25f,25f
s_drawLineTo -25f,25f
s_drawLineTo -25f,-25f

SWFMovieClip mc
mc_add i,s
i_setDepth 1
mc_nextFrame

repeat 5
i_rotate -15f
mc_nextFrame
loop

SWFMovie m
m_setBackground 0xff,0xff,0xff,0xff
m_setDimension 320f,240f
m_add i2,mc
sel_i i2
i_setDepth 1
i_moveTo -50f,120f
i_setName "box"

SWFAction a,"box._x += 3;"
m_add tmp,a
m_nextFrame
SWFAction a,"prevFrame(); play();"
m_add tmp,a
m_nextFrame
m_save "action.swf"
mes stat

swfpreview "action.swf"
stop