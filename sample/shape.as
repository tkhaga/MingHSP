#include "minghsp.as"

#include "rrmod/com/activex.as"
#include "swfpreview.as"

SWFShape s
s_setLine 4,0x7f,0,0,0xff
s_addSolidFill f,0xff,0,0,255
s_setRightFill f
s_movePenTo 10f,10f
s_drawLineTo 310f,10f
s_drawLineTo 310f,230f
s_drawCurveTo 10f,230f,10f,10f
SWFMovie m
m_setDimension 320f,240f
m_setRate 12f
m_add i,s
m_nextFrame
m_save "shape.swf"
mes stat

swfpreview "shape.swf"
stop