%index
SWFShape
SWFShape�I�u�W�F�N�g�̍쐬

%prm
v1
v1 : �쐬���ꂽSWFShape�I�u�W�F�N�g���������鐔�l�^�ϐ�

%group
SWFShape�I�u�W�F�N�g���얽��

%inst
�V����SWFShape�I�u�W�F�N�g���쐬���܂��B
SWFShape�I�u�W�F�N�g�̓x�N�^�摜�������I�u�W�F�N�g�ł���A
s_�Ŏn�܂閽�ߌQ�ő��삵�܂��B

�ʏ�A�����̖��߂̑���Ώۂ̃I�u�W�F�N�g��sel_s���߂őI�����܂����A
����Ώۂ̃I�u�W�F�N�g���܂��I������Ă��Ȃ���0���I������Ă���ꍇ�͎����I��
���̖��߂ō쐬���ꂽ�I�u�W�F�N�g���I������܂��B

%href
sel_s
destroy_s
s_setLine
s_addSolidFill
s_addBitmapFill
s_addGradientFill
s_setLeftFill
s_setRightFill
s_movePenTo
s_movePen
s_drawLineTo
s_drawLine
s_drawCurveTo
s_drawCurve
s_drawGlyph
s_drawCircle
s_drawArc
s_drawCubicTo
s_drawCubic
s_getPenX
s_getPenY
s_getPen
s_loadPdr
s_newStyles


%index
s_setLine
���̑����ƐF��ݒ�

%prm
n1, n2, n3, n4, n5
n1=0�`         : ���̑���(�s�N�Z���P��)
n2=0�`255      : R�l
n3=0�`255      : G�l
n4=0�`255      : B�l
n5=0�`255(255) : 0�Ŋ��S�ɓ����A255�Ŋ��S�ɕs����

%group
SWFShape�I�u�W�F�N�g���얽��

%inst
���ݑI������Ă���SWFShape�I�u�W�F�N�g�ɑ΂����ꂩ��������̑����ƐF��ݒ肵�܂��B
���łɈ�����Ă�����ɂ͉e������܂���B

%href
sel_s
SWFShape
s_drawLineTo
s_drawLine
s_drawCurveTo
s_drawCurve
s_drawGlyph
s_drawCircle
s_drawArc
s_drawCubicTo
s_drawCubic
s_newStyles


%index
s_addSolidFill
�x�^�h���SWFFill�I�u�W�F�N�g���쐬

%prm
v1, n2, n3, n4, n5
v1             : �쐬����SWFFill�I�u�W�F�N�g���������鐔�l�^�ϐ�
n2=0�`255      : R�l
n3=0�`255      : G�l
n4=0�`255      : B�l
n5=0�`255(255) : 0�Ŋ��S�ɓ����A255�Ŋ��S�ɕs����

%group
SWFShape�I�u�W�F�N�g���얽��

%inst
�I�����ꂽSWFShape�I�u�W�F�N�g�ɑ΂���x�^�h���SWFFill�I�u�W�F�N�g���쐬���܂��B
�쐬���ꂽSWFFill�I�u�W�F�N�g��f_�Ŏn�܂閽�ߌQ�ő��삵�܂��B

�ʏ�A�����̖��߂̑���Ώۂ̃I�u�W�F�N�g��sel_f���߂őI�����܂����A
����Ώۂ̃I�u�W�F�N�g���܂��I������Ă��Ȃ���0���I������Ă���ꍇ�͎����I��
���̖��߂ō쐬���ꂽ�I�u�W�F�N�g���I������܂��B

%href
sel_s
sel_f
destroy_f
SWFShape
s_setLeftFill
s_setRightFill
f_moveTo
f_scaleTo
f_rotateTo
f_skewXTo
f_skewYTo


%index
s_addBitmapFill
�摜�ɂ��h���SWFFill�I�u�W�F�N�g���쐬

%prm
v1, n2, n3
v1 : �쐬����SWFFill�I�u�W�F�N�g���������鐔�l�^�ϐ�
n2 : SWFBitmap�I�u�W�F�N�g
n3 : �t���O

%group
SWFShape�I�u�W�F�N�g���얽��

%inst
�I�����ꂽSWFShape�I�u�W�F�N�g�ɑ΂���摜�ɂ��h���SWFFill�I�u�W�F�N�g���쐬���܂��B
n2�ɂ�SWFBitmap, SWFBitmap_buf���߂ō쐬�����SWFBitmap�I�u�W�F�N�g���w�肵�܂��B
n3�ɂ͎��̒萔�̂ǂ��炩���w�肵�܂�
-----------------------
 SWFFILL_TILED_BITMAP   �^�C����ɉ摜���~���l�߂���
 SWFFILL_CLIPPED_BITMAP 
-----------------------
�ȗ�����SWFFILL_TILED_BITMAP���w�肳�ꂽ�̂Ɠ����ɂȂ�܂��B

%href
sel_s
sel_f
destroy_f
SWFShape
s_setLeftFill
s_setRightFill
SWFBitmap
SWFBitmap_buf
bmp_getWidth
bmp_getHeight
f_moveTo
f_scaleTo
f_rotateTo
f_skewXTo
f_skewYTo

%index
s_addGradientFill
�O���f�[�V�����h���SWFFill�I�u�W�F�N�g���쐬

%prm
v1, n2, n3
v1 : �쐬����SWFFill�I�u�W�F�N�g���������鐔�l�^�ϐ�
n2 : SWFGradient�I�u�W�F�N�g
n3 : �t���O

%group
SWFShape�I�u�W�F�N�g���얽��

%inst
�I�����ꂽSWFShape�I�u�W�F�N�g�ɑ΂���O���f�[�V�����h���SWFFill�I�u�W�F�N�g���쐬���܂��B
n2�ɂ�SWFGradient���߂ō쐬�����SWFGradient�I�u�W�F�N�g���w�肵�܂��B
n3�ɂ͎��̒萔�̂ǂ��炩���w�肵�܂�

----------------------------------------------------------------
 SWFFILL_LINEAR_GRADIENT ����O���f�[�V����
 SWFFILL_RADIAL_GRADIENT ���S���瓯�S�~��ɍL����O���f�[�V����
----------------------------------------------------------------

�ȗ�����SWFFILL_LINEAR_GRADIENT���w�肳�ꂽ�̂Ɠ����ɂȂ�܂��B

%href
sel_s
sel_f
destroy_f
SWFShape
s_setLeftFill
s_setRightFill
SWFGradient
g_addEntry
f_moveTo
f_scaleTo
f_rotateTo
f_skewXTo
f_skewYTo


%index
s_setLeftFill
�h���ݒ�(�ʏ�)

%prm
n1
n1 : SWFFill�I�u�W�F�N�g

%group
SWFShape�I�u�W�F�N�g���얽��

%inst
�I�����ꂽSWFShape�I�u�W�F�N�g�Ɏw�肵��SWFFill�I�u�W�F�N�g�ɂ��h���ݒ肵�܂��B
n1��0���w�肷��Ɠh��͐ݒ肳��܂���B
���̐ݒ�͂��̖��߈ȍ~�Ɉ�������(���m�ɂ͐V���������G�b�W)�ɑ΂��ēK�p����܂��B
���l�̖��߂Ƃ���s_setRightFill������܂����A�ʏ��s_setLeftFill���g���悤�ɂ��ĉ������B

s_setRightFill�̗p�r��s_setRightFill�̍��Ő������܂��B

%href
sel_s
SWFShape
s_addSolidFill
s_addBitmapFill
s_addGradientFill
s_setRightFill


%index
s_setRightFill
�h���ݒ�(����)

%prm
n1
n1 : SWFFill�I�u�W�F�N�g

%group
SWFShape�I�u�W�F�N�g���얽��

%inst
�I�����ꂽSWFShape�I�u�W�F�N�g�Ɏw�肵��SWFFill�I�u�W�F�N�g�ɂ��h���ݒ肵�܂��B
n1��0���w�肷��Ɠh��͐ݒ肳��܂���B
���̐ݒ�͂��̖��߈ȍ~�Ɉ�������(���m�ɂ͐V���������G�b�W)�ɑ΂��ēK�p����܂��B
���l�̖��߂Ƃ���s_setLeftFill������܂����A�ʏ��s_setLeftFill���g���悤�ɂ��ĉ������B

s_setRightFill�Őݒ肵���h��́A�h�蓯�m���d�Ȃ��Ă��鎞�Ɏg���܂��B

%href
sel_s
SWFShape
s_addSolidFill
s_addBitmapFill
s_addGradientFill
s_setLeftFill


%index
s_movePenTo
�`��J�n�_���ړ�(��΍��W�w��)

%prm
f1, f2
f1 : �ړ����X���W(�s�N�Z��)
f2 :    �V   Y      �V

%group
SWFShape�I�u�W�F�N�g���얽��

%inst
�I�����ꂽSWFShape�I�u�W�F�N�g�ɑ΂���`�施�ߌQ�̕`��J�n�_���w�肳�ꂽ���W�Ɉړ����܂��B
HSP2�ł́A�p�����[�^�ɂ�f��t���邩mhsp_float���߂ŕϊ������l���w�肷��K�v������܂��B

%href
sel_s
mhsp_float
ming_setScale
SWFShape
s_movePen
s_drawLineTo
s_drawLine
s_drawCurveTo
s_drawCurve
s_drawGlyph
s_drawCircle
s_drawArc
s_drawCubicTo
s_drawCubic
s_getPenX
s_getPenY
s_getPen


%index
s_movePen
�`��J�n�_���ړ�(���΍��W�w��)

%prm
f1, f2
f1 : X�����ւ̈ړ���(�s�N�Z��)
f2 : Y          �V

%group
SWFShape�I�u�W�F�N�g���얽��

%inst
�I�����ꂽSWFShape�I�u�W�F�N�g�ɑ΂���`�施�ߌQ�̕`��J�n�_���w�肳�ꂽ�ʂ����ړ����܂��B
HSP2�ł́A�p�����[�^�ɂ�f��t���邩mhsp_float���߂ŕϊ������l���w�肷��K�v������܂��B

%href
sel_s
mhsp_float
ming_setScale
SWFShape
s_movePenTo
s_drawLineTo
s_drawLine
s_drawCurveTo
s_drawCurve
s_drawGlyph
s_drawCircle
s_drawArc
s_drawCubicTo
s_drawCubic
s_getPenX
s_getPenY
s_getPen


%index
s_drawLineTo
��������(��΍��W�w��)

%prm
f1, f2
f1 : �`��I���_��X���W(�s�N�Z��)
f2 :      �V     Y �V
