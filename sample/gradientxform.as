#include "minghsp.as"

#include "rrmod/com/activex.as"
#include "swfpreview.as"

SWFMorph p	;�V�F�C�v�g�D�C�[���i���[�t�B���O�j�I�u�W�F�N�g�̍쐬
SWFGradient g	;�O���f�[�V�����I�u�W�F�N�g�̍쐬

g_addEntry 0,0,0,0,0xff	;�O���f�[�V�����̊e�n�_�ɐF��ݒ�
g_addEntry 0.16f,0xff,0xff,0xff,0xff
g_addEntry 0.32f,0,0,0,0xff
g_addEntry 0.48f,0xff,0xff,0xff,0xff
g_addEntry 0.64f,0,0,0,0xff
g_addEntry 0.80f,0xff,0xff,0xff,0xff
g_addEntry 1.00f,0,0,0,0xff

p_getShape1 s	;���[�t�B���O���̃V�F�C�v�I�u�W�F�N�g���擾
s_addGradientFill f,g,SWFFILL_RADIAL_GRADIENT
f_scaleTo 0,1f
s_setLeftFill f
s_movePenTo -160f,-160f
s_drawLine 320f
s_drawLine 0,240f
s_drawLine -320f
s_drawLine 0,-240f

SWFGradient g
sel_g g
g_addEntry 0,0,0,0,0xff
g_addEntry 0.16f,0xff,0,0,0xff
g_addEntry 0.32f,0,0,0,0xff
g_addEntry 0.48f,0,0xff,0,0xff
g_addEntry 0.64f,0,0,0,0xff
g_addEntry 0.80f,0,0,0xff,0xff
g_addEntry 1.00f,0,0,0,0xff

p_getShape2 s
sel_s s
s_addGradientFill f,g,SWFFILL_RADIAL_GRADIENT
sel_f f
f_scaleTo 0.16f
f_skewXTo -0.5f
s_setLeftFill f
s_movePenTo -160f,-120f
s_drawLine 320f
s_drawLine 0,240f
s_drawLine -320f
s_drawLine 0,-240f

SWFMovie m
m_setDimension 320f,240f
m_add i,p
i_moveTo 160f,120f

repeat 100
mhsp_float fv,cnt
mhsp_mul fv,fv,0.01f
i_setRatio fv
m_nextFrame
loop

m_save "gradientxform.swf"
mes stat	;stat�ɂ̓t�@�C���T�C�Y����������

swfpreview "gradientxform.swf"
stop