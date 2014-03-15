#include "minghsp.as"

#include "rrmod/com/activex.as"
#include "swfpreview.as"

ming_setScale 20f
SWFShape s
s_addSolidFill f1,0xff,0,0,255
s_addSolidFill f2,0xff,0x7f,0,255
s_addSolidFill f3,0xff,0xff,0,255
s_addSolidFill f4,0,0xff,0,255
s_addSolidFill f5,0,0,0xff,255

SWFFont fnt,"../ipag.fdb"

s_setRightFill f1
s_setLine 0,0x7f,0x3f,0,0xff
s_movePenTo 50f,50f
s_drawGlyph fnt,'*'
SWFMovie m
m_add i,s
m_save "glyph.swf"
mes stat

swfpreview "glyph.swf"
stop