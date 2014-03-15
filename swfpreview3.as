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

