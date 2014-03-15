;MingHSP header file
	#uselib "minghsp.hpi"

;constant values
	#define SWFFILL_SOLID		0x00
	#define SWFFILL_GRADIENT	0x10
	#define SWFFILL_LINEAR_GRADIENT 0x10
	#define SWFFILL_RADIAL_GRADIENT 0x12
	#define SWFFILL_BITMAP		0x40
	#define SWFFILL_TILED_BITMAP	0x40
	#define SWFFILL_CLIPPED_BITMAP	0x41

	#define SWFTEXTFIELD_HASLENGTH 0x02
	#define SWFTEXTFIELD_NOEDIT    0x08
	#define SWFTEXTFIELD_PASSWORD  0x10
	#define SWFTEXTFIELD_MULTILINE 0x20
	#define SWFTEXTFIELD_WORDWRAP  0x40
	#define SWFTEXTFIELD_DRAWBOX   0x800
	#define SWFTEXTFIELD_NOSELECT  0x1000
	#define SWFTEXTFIELD_HTML      0x200
	#define SWFTEXTFIELD_USEFONT   0x2000
	#define SWFTEXTFIELD_AUTOSIZE  0x8000

	#define SWFTEXTFIELD_ALIGN_LEFT    0
	#define SWFTEXTFIELD_ALIGN_RIGHT   1
	#define SWFTEXTFIELD_ALIGN_CENTER  2
	#define SWFTEXTFIELD_ALIGN_JUSTIFY 3

	#define SWFBUTTON_HIT  0x08
	#define SWFBUTTON_DOWN 0x04
	#define SWFBUTTON_OVER 0x02
	#define SWFBUTTON_UP   0x01

	#define SWFBUTTON_MOUSEUPOUTSIDE  0x100
	#define SWFBUTTON_DRAGOVER        0xa0
	#define SWFBUTTON_DRAGOUT         0x110
	#define SWFBUTTON_MOUSEUP         0x08
	#define SWFBUTTON_MOUSEDOWN       0x04
	#define SWFBUTTON_MOUSEOUT        0x02
	#define SWFBUTTON_MOUSEOVER       0x01

	#define SWFACTION_ONLOAD      0x01
	#define SWFACTION_ENTERFRAME  0x02
	#define SWFACTION_UNLOAD      0x04
	#define SWFACTION_MOUSEMOVE   0x08
	#define SWFACTION_MOUSEDOWN   0x10
	#define SWFACTION_MOUSEUP     0x20
	#define SWFACTION_KEYDOWN     0x40
	#define SWFACTION_KEYUP       0x80
	#define SWFACTION_DATA        0x100

	#define SWF_SOUND_NOT_COMPRESSED    0
	#define SWF_SOUND_ADPCM_COMPRESSED  16
	#define SWF_SOUND_MP3_COMPRESSED    32
	#define SWF_SOUND_NOT_COMPRESSED_LE 48
	#define SWF_SOUND_NELLY_COMPRESSED  96

	#define SWF_SOUND_5KHZ             0
	#define SWF_SOUND_11KHZ            4
	#define SWF_SOUND_22KHZ            8
	#define SWF_SOUND_44KHZ            12

	#define SWF_SOUND_8BITS            0
	#define SWF_SOUND_16BITS           2

	#define SWF_SOUND_MONO             0
	#define SWF_SOUND_STEREO           1

;original functions
	#func mhsp_init mhsp_init 0

	#func sel_s sel_s 0
	#func sel_m sel_m 0
	#func sel_i sel_i 0
	#func sel_g sel_g 0
	#func sel_bmp sel_bmp 0
	#func sel_f sel_f 0
	#func sel_p sel_p 0
	#func sel_t sel_t 0
	#func sel_fnt sel_fnt 0
	#func sel_tf sel_tf 0
	#func sel_mc sel_mc 0
	#func sel_b sel_b 0
	#func sel_v sel_v 0
	#func sel_c sel_c 0
	#func sel_si sel_si 0

	#func destroy_s destroy_s 0
	#func destroy_m destroy_m 0
	#func destroy_g destroy_g 0
	#func destroy_bmp destroy_bmp 0
	#func destroy_f destroy_f 0
	#func destroy_p destroy_p 0
	#func destroy_t destroy_t 0
	#func destroy_fnt destroy_fnt 0
	#func destroy_bfnt destroy_bfnt 0
	#func destroy_tf destroy_tf 0
	#func destroy_mc destroy_mc 0
	#func destroy_b destroy_b 0
	#func destroy_a destroy_a 0
	#func destroy_snd destroy_snd 0

	#func mhsp_float mhsp_float 1
	#func mhsp_add mhsp_add 1
	#func mhsp_sub mhsp_sub 1
	#func mhsp_mul mhsp_mul 1
	#func mhsp_div mhsp_div 1
	#func mhsp_int mhsp_int 1

	#func mhsp_fcloseall mhsp_fcloseall 0
	#func mhsp_setwarnbuf mhsp_setwarnbuf 1
	#func mhsp_getaswarn mhsp_getaswarn 1
	#func mhsp_getaserror mhsp_getaserror 1

;global functions(?)
	#func Ming_setCubicThreshold ming_setCubicThreshold 0
	#func Ming_setScale ming_setScale 0
	#func Ming_useSWFVersion ming_useSWFVersion 0
	#func SWFBUTTON_KEYPRESS swfbutton_keypress 5
	#func Ming_useConstants ming_useConstants 0

;SWFMovie
	#func SWFMovie mhsp_SWFMovie 1
	#func m_output m_output $202
	#func m_save m_save $202
	#func m_add m_add 1
	#func m_remove m_remove 0
	#func m_nextFrame m_nextFrame 0
	#func m_labelFrame m_labelFrame 6
	#func m_setBackground m_setBackground 0
	#func m_setRate m_setRate 0
	#func m_setDimension m_setDimension 0
	#func m_setFrames m_setFrames 0
	#func m_streamMp3 m_streamMp3 6
	#func m_streamMp3_buf m_streamMp3_buf 1
	#func m_startSound m_startSound 1
	#func m_stopSound m_stopSound 0
	#func m_importChar m_importChar $202
	#func m_addFont m_addFont 1
	#func m_importFont m_importFont $202
	#func m_addExport m_addExport 4
	#func m_writeExports m_writeExports 0
	#func m_Protect m_Protect 0

;SWFShape
	#func SWFShape mhsp_SWFShape 1
	#func s_setLine s_setLine $202
	#func s_addSolidFill s_addSolidFill $202
	#func s_addBitmapFill s_addBitmapFill $202
	#func s_addGradientFill s_addGradientFill $202
	#func s_setLeftFill s_setLeftFill 0
	#func s_setRightFill s_setRightFill 0
	#func s_movePenTo s_movePenTo 0
	#func s_movePen s_movePen 0
	#func s_drawLineTo s_drawLineTo 0
	#func s_drawLine s_drawLine 0
	#func s_drawCurve s_drawCurve 0
	#func s_drawCurveTo s_drawCurveTo 0
	#func s_drawGlyph s_drawGlyph 0
	#func s_drawCircle s_drawCircle 0
	#func s_drawArc s_drawArc 0
	#func _s_drawCubicTo s_drawCubicTo 0
	#func _s_drawCubic s_drawCubic 0
	#func s_getPenX s_getPenX 1
	#func s_getPenY s_getPenY 1
	#func s_getPen s_getPen $202

;SWFDisplayitem
	#func i_moveTo i_moveTo 0
	#func i_move i_move 0
	#func i_scaleTo i_scaleTo 0
	#func i_scale i_scale 0
	#func i_rotateTo i_rotateTo 0
	#func i_rotate i_rotate 0
	#func i_skewXTo i_skewXTo 0
	#func i_skewX i_skewX 0
	#func i_skewYTo i_skewYTo 0
	#func i_skewY i_skewY 0
	#func i_setDepth i_setDepth 0
	#func i_remove i_remove 0
	#func i_setName i_setName 6
	#func i_setRatio i_setRatio 0
	#func i_addColor i_addColor 0
	#func i_multColor i_multColor 0
	#func _i_setMatrix i_setMatrix 0
	#func i_addAction i_addAction 0
	#func i_setMaskLevel i_setMaskLevel 0
	#func i_endMask i_endMask 0
	#func i_getX i_getX 1
	#func i_getY i_getY 1
	#func i_getXScale i_getXScale 1
	#func i_getYScale i_getYScale 1
	#func i_getXSkew i_getXSkew 1
	#func i_getYSkew i_getYSkew 1
	#func i_getRot i_getRot 1
	#func i_getDepth i_getDepth 1

;SWFGradient
	#func SWFGradient mhsp_SWFGradient 1
	#func _g_addEntry g_addEntry 0

;SWFBitmap
	#func SWFBitmap mhsp_SWFBitmap $202
	#func SWFBitmap_buf mhsp_SWFBitmap_buf $202
	#func bmp_getWidth bmp_getWidth 1
	#func bmp_getHeight bmp_getHeight 1

;SWFFill
	#func f_moveTo f_moveTo 0
	#func f_scaleTo f_scaleTo 0
	#func f_rotateTo f_rotateTo 0
	#func f_skewXTo f_skewXTo 0
	#func f_skewYTo f_skewYTo 0

;SWFMorph
	#func SWFMorph mhsp_SWFMorph 1
	#func p_getShape1 p_getShape1 1
	#func p_getShape2 p_getShape2 1

;SWFText
	#func SWFText mhsp_SWFText 1
	#func t_setFont t_setFont 0
	#func t_setHeight t_setHeight 0
	#func t_setSpacing t_setSpacing 0
	#func t_setColor t_setColor $202
	#func t_moveTo t_moveTo 0
	#func t_addString t_addString 6
	#func t_getWidth t_getWidth 5
	#func t_getAscent t_getAscent 1
	#func t_getDescent t_getDescent 1
	#func t_getLeading t_getLeading 1
	#func t_addUTF8String t_addUTF8String 6
	#func t_getUTF8Width t_getUTF8Width 5

;SWFFont
	#func SWFFont mhsp_SWFFont 5
	#func fnt_getWidth fnt_getWidth 5
	#func fnt_getAscent fnt_getAscent 1
	#func fnt_getDescent fnt_getDescent 1
	#func fnt_getLeading fnt_getLeading 1
	#func fnt_getUTF8Width fnt_getUTF8Width 5
	#func fnt_getShape fnt_getShape 1	;returns pointer

;SWFTextField
	#func SWFTextField mhsp_SWFTextField 1
	#func tf_setFont tf_setFont 0
	#func tf_setBounds tf_setBounds 0
	#func tf_align tf_align 0
	#func tf_setHeight tf_setHeight 0
	#func tf_setLeftMargin tf_setLeftMargin 0
	#func tf_setRightMargin tf_setRightMargin 0
	#func tf_setMargins tf_setMargins 0
	#func tf_setIndentation tf_setIndentation 0
	#func tf_setLineSpacing tf_setLineSpacing 0
	#func tf_setColor tf_setColor $202
	#func tf_setName tf_setName 6
	#func tf_addString tf_addString 6
	#func tf_setLength tf_setLength 0
	#func tf_setPadding tf_setPadding 0
	#func tf_addUTF8String tf_addUTF8String 6
	#func tf_addChars tf_addChars 6

;SWFMovieClip
	#func SWFMovieClip mhsp_SWFMovieClip 1
	#func mc_add mc_add 1
	#func mc_remove mc_remove 0
	#func mc_nextFrame mc_nextFrame 0
	#func mc_labelFrame mc_labelFrame 6
	#func mc_setFrames mc_setFrames 0
	#func mc_startSound mc_startSound 1
	#func mc_stopSound mc_stopSound 0

;SWFButton
	#func SWFButton mhsp_SWFButton 1
	#func b_addShape b_addShape 0
	#func b_setUp b_setUp 0
	#func b_setOver b_setOver 0
	#func b_setDown b_setDown 0
	#func b_setHit b_setHit 0
	#func b_addAction b_addAction 0
	#func b_setAction b_setAction 0
	#func b_setMenu b_setMenu 0
	#func b_addSound b_addSound 0

;SWFAction
	#func SWFAction mhsp_SWFAction 5
	#func SWFAction_save SWFAction_save 1
	#func SWFAction_load SWFAction_load $202

;SWFVideoStream
	#func SWFVideoStream mhsp_SWFVideoStream 5
	#func v_setDimension v_setDimension 0
	#func v_getNumFrames v_getNumFrames 1

;SWFFontCharacter
	#func c_addChars c_addChars 6
	#func c_addUTF8Chars c_addUTF8Chars 6

;SWFSound
	#func SWFSound mhsp_SWFSound 5
	#func SWFSound_buf mhsp_SWFSound_buf $202

;SWFSoundInstance
	#func si_noMultiple si_noMultiple 0
	#func si_loopInPoint si_loopInPoint 0
	#func si_loopOutPoint si_loopOutPoint 0
	#func si_loopCount si_loopCount 0

	#module

	#deffunc s_drawCubicTo int,int,int,int,int,int
	mref bx
	mref by,1
	mref cx,2
	mref cy,3
	mref dx,4
	mref dy,5
	_s_drawCubicTo@ dx,dy
	_s_drawCubicTo@ bx,by,cx,cy
	return

	#deffunc s_drawCubic int,int,int,int,int,int

	mref bx
	mref by,1
	mref cx,2
	mref cy,3
	mref dx,4
	mref dy,5

	_s_drawCubic@ dx,dy
	_s_drawCubic@ bx,by,cx,cy

	return

	#deffunc i_setMatrix int,int,int,int,int,int

	mref a
	mref b,1
	mref c,2
	mref d,3
	mref x,4
	mref y,5

	_i_setMatrix@ x,y
	_i_setMatrix@ a,b,c,d

	return

	#deffunc g_addEntry int,int,int,int,int

	mref ratio
	mref r,1
	mref g,2
	mref b,3
	mref a,4

	_g_addEntry@ ratio,a
	_g_addEntry@ ratio,r,g,b

	return

	#global

	mhsp_init
