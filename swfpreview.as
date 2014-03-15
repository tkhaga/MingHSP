#ifdef __hsp26__
;swfpreview (05/09/07)

	#module "swfpreview"

	#define IID_IShockwaveFlash "{D27CDB6C-AE6D-11CF-96B8-444553540000}"

	#define IShockwaveFlash_get_ReadyState        7
	#define IShockwaveFlash_get_TotalFrames       8
	#define IShockwaveFlash_get_Playing           9
	#define IShockwaveFlash_put_Playing           10
	#define IShockwaveFlash_get_Quality           11
	#define IShockwaveFlash_put_Quality           12
	#define IShockwaveFlash_get_ScaleMode         13
	#define IShockwaveFlash_put_ScaleMode         14
	#define IShockwaveFlash_get_AlignMode         15
	#define IShockwaveFlash_put_AlignMode         16
	#define IShockwaveFlash_get_BackgroundColor   17
	#define IShockwaveFlash_put_BackgroundColor   18
	#define IShockwaveFlash_get_Loop              19
	#define IShockwaveFlash_put_Loop              20
	#define IShockwaveFlash_get_Movie             21
	#define IShockwaveFlash_put_Movie             22
	#define IShockwaveFlash_get_FrameNum          23
	#define IShockwaveFlash_put_FrameNum          24
	#define IShockwaveFlash_SetZoomRect           25
	#define IShockwaveFlash_Zoom                  26
	#define IShockwaveFlash_Pan                   27
	#define IShockwaveFlash_Play                  28
	#define IShockwaveFlash_Stop                  29
	#define IShockwaveFlash_Back                  30
	#define IShockwaveFlash_Forward               31
	#define IShockwaveFlash_Rewind                32
	#define IShockwaveFlash_StopPlay              33
	#define IShockwaveFlash_GotoFrame             34
	#define IShockwaveFlash_CurrentFrame          35
	#define IShockwaveFlash_IsPlaying             36
	#define IShockwaveFlash_PercentLoaded         37
	#define IShockwaveFlash_FrameLoaded           38
	#define IShockwaveFlash_FlashVersion          39
	#define IShockwaveFlash_get_WMode             40
	#define IShockwaveFlash_put_WMode             41
	#define IShockwaveFlash_get_SAlign            42
	#define IShockwaveFlash_put_SAlign            43
	#define IShockwaveFlash_get_Menu              44
	#define IShockwaveFlash_put_Menu              45
	#define IShockwaveFlash_get_Base              46
	#define IShockwaveFlash_put_Base              47
	#define IShockwaveFlash_get_Scale             48
	#define IShockwaveFlash_put_Scale             49
	#define IShockwaveFlash_get_DeviceFont        50
	#define IShockwaveFlash_put_DeviceFont        51
	#define IShockwaveFlash_get_EmbedMovie        52
	#define IShockwaveFlash_put_EmbedMovie        53
	#define IShockwaveFlash_get_BGColor           54
	#define IShockwaveFlash_put_BGColor           55
	#define IShockwaveFlash_get_Quality2          56
	#define IShockwaveFlash_put_Quality2          57
	#define IShockwaveFlash_LoadMovie             58
	#define IShockwaveFlash_TGotoFrame            59
	#define IShockwaveFlash_TGotoLabel            60
	#define IShockwaveFlash_TCurrentFrame         61
	#define IShockwaveFlash_TCurrentLabel         62
	#define IShockwaveFlash_TPlay                 63
	#define IShockwaveFlash_TStopPlay             64
	#define IShockwaveFlash_SetVariable           65
	#define IShockwaveFlash_GetVariable           66
	#define IShockwaveFlash_TSetProperty          67
	#define IShockwaveFlash_TGetProperty          68
	#define IShockwaveFlash_TCallFrame            69
	#define IShockwaveFlash_TCallLabel            70
	#define IShockwaveFlash_TSetPropertyNum       71
	#define IShockwaveFlash_TGetPropertyNum       72
	#define IShockwaveFlash_TGetPropertyAsNumber  73
	#define IShockwaveFlash_get_SWRemote          74
	#define IShockwaveFlash_put_SWRemote          75
	#define IShockwaveFlash_get_FlashVars         76
	#define IShockwaveFlash_put_FlashVars         77
	#define IShockwaveFlash_get_AllowScriptAccess 78
	#define IShockwaveFlash_put_AllowScriptAccess 79
	#define IShockwaveFlash_get_MovieData         80
	#define IShockwaveFlash_put_MovieData         81
	#define IShockwaveFlash_get_InlineData        82
	#define IShockwaveFlash_put_InlineData        83
	#define IShockwaveFlash_get_SeamlessTabbing   84
	#define IShockwaveFlash_put_SeamlessTabbing   85

	#deffunc swfpreview str,int,int

	mref fname,32
	mref x,1
	mref y,2
	mref stt,64

	if hwnd :stt=1 :return

	axinit
	if stat :stt=2 :return

	exist fname
	if strsize<16 :stt=3 :return
	bload fname,swf,16

	compressed=0
	peek data,swf
	if data='C' :compressed=1
	peek data,swf,1
	if data!'W'{
		peek data,swf,2
		if data!'S'{
			stt=3
			return
		}
	}

	index=8
	peek data,swf,index
	bits=data&0xff>>3
	bindex=5
	data=data<<5
	rindex=0 :tmp=0 :i=0
	repeat
		if rindex=bits{
			rect.i=tmp
			if i=3 :break
			i+
			rindex=0
			tmp=0
		}
		if bindex=8{
			bindex=0
			index+
			peek data,swf,index
		}
		tmp=data&0x80>>7|(tmp<<1)
		data=data<<1&0xff
		bindex+
		rindex+
	loop
	swfx=rect.1-rect/20 :swfy=rect.3-rect.2/20
	if compressed :swfx=winx-csrx :swfy=winy-csry
	if x :swfx=x
	if y :swfy=y

	axcreate  hwnd,"ShockwaveFlash.ShockwaveFlash.1",csrx,csry,swfx,swfy
	pos csrx,csry+swfy
	axgetcontrol pSWF,hwnd,IID_IShockwaveFlash

	peek data,fname,1
	if data!':' :fname=curdir+"\\"+fname

	bstrset bstrvar,fname
	param=0,bstrvar
	icall pSWF,IShockwaveFlash_LoadMovie,param,2
	bstrfree bstrvar

	if compressed :stt=0 :else :stt=swfx|swfy<<16

	return

	#deffunc swf_totalframes val

	mref retval,16

	dispget retval,pSWF,"TotalFrames"

	return

	#deffunc swf_get_quality val

	mref retval,16

	dispget retval,pSWF,"Quality"

	return

	#deffunc swf_set_quality int

	mref quality

	dispput pSWF,"Quality",quality

	return

	#deffunc swf_get_loop val

	mref isloop,16

	dispget isloop,pSWF,"Loop"

	return

	#deffunc swf_set_loop int

	mref isloop

	dispput pSWF,"Loop",isloop

	return

	#deffunc swf_play

	icall pSWF,IShockwaveFlash_Play

	return

	#deffunc swf_back

	icall pSWF,IShockwaveFlash_Back

	return

	#deffunc swf_forword

	icall pSWF,IShockwaveFlash_Forword

	return

	#deffunc swf_rewind

	icall pSWF,IShockwaveFlash_Rewind

	return

	#deffunc swf_gotoframe int

	mref frame

	icall pSWF,IShockwaveFlash_GotoFrame,frame,1

	return

	#deffunc swf_stopplay

	icall pSWF,IShockwaveFlash_StopPlay

	return

	#deffunc swf_currentframe val

	mref retval,16

	dispget retval,pSWF,"CurrentFrame"

	return

	#deffunc swf_isplaying

	mref stt,64

	str retstr
	dispget retstr,pSWF,"IsPlaying"

	if retstr="True" :stt=1 :else :stt=0

	return

	#deffunc swf_cleanup onexit

	axdestroy hwnd
	hwnd=0
	if pSWF :release pSWF :pSWF=0

	return

	#global
#endif
#ifdef __hsp30__
;swfpreview3 (05/09/07)

#module swfpreview3

	#ifndef __KERNEL32__
	#uselib "kernel32.dll"
	#define GetFullPathName GetFullPathNameA
	#func GetFullPathNameA "GetFullPathNameA" sptr,sptr,sptr,sptr
	#endif

*atlerror
	if wparam=21 :stt=2	;サポートされない機能を実行しました
	return

	#deffunc swfpreview str flash,int x,int y

	if objid!-1 :stt=1 :return

	mref stt,64

	fname=flash

	exist fname
	if strsize<16 :stt=3 :return
	sdim buf,16
	bload fname,buf

	compressed=0
	if peek(buf, 0)='C' :compressed=1
	if peek(buf, 1)!'W'|peek(buf, 2)!'S' :stt=3 :return

	index=8
	data=peek(buf, index)
	bits=(data&0xff)>>3
	bindex=5
	data=data<<5
	rindex=0 :tmp=0 :i=0
	repeat
		if rindex=bits{
			rect.i=tmp
			if i=3 :break
			i+
			rindex=0
			tmp=0
		}
		if bindex=8{
			bindex=0
			index+
			data=peek(buf,index)
		}
		tmp=(data&0x80)>>7|(tmp<<1)
		data=data<<1&0xff
		bindex+
		rindex+
	loop
	swfx=(rect.1-rect)/20 :swfy=(rect.3-rect.2)/20
	if compressed :swfx=ginfo(12)-ginfo(22) :swfy=ginfo(13)-ginfo(23)
	if x :swfx=x
	if y :swfy=y

	onerror gosub *atlerror
	axobj swf,"ShockwaveFlash.ShockwaveFlash.1",swfx,swfy
	objid=stat
	onerror 0
	if stt=1 :return

	if peek(fname, 1)!':' :GetFullPathName fname,1024,varptr(fname),0

	swf->"LoadMovie" 0,fname

	if compressed :stt=0 :else :stt=swfx|swfy<<16

	return

	#deffunc swf_totalframes var retval

	retval=swf("TotalFrames")

	return

	#deffunc swf_get_quality var quality

	quality=swf("Quality")

	return

	#deffunc swf_set_quality int quality

	swf("Quality")=quality

	return

	#deffunc swf_get_loop var isloop

	isloop=swf("Loop")

	return

	#deffunc swf_set_loop int isloop

	swf("Loop")=isloop

	return

	#deffunc swf_play

	swf->"Play"

	return

	#deffunc swf_back

	swf->"Back"

	return

	#deffunc swf_forword

	swf->"Forword"

	return

	#deffunc swf_rewind

	swf->"Rewind"

	return

	#deffunc swf_gotoframe int frame

	swf->"GotoFrame" frame

	return

	#deffunc swf_stopplay

	swf->"StopPlay"

	return

	#deffunc swf_currentframe var retval

	comres retval
	swf->"CurrentFrame"

	return

	#deffunc swf_isplaying

	mref stt,64

	comres retval
	swf->"IsPlaying"

	if retval=0 :stt=0 :else :stt=1

	return

	#deffunc swf_cleanup onexit

	if objid!-1{
		delcom swf
		clrobj objid,objid
		objid=-1
	}
	else :return

	return

#global

	sdim fname@swfpreview3,1024
	objid@swfpreview3=-1


#endif
