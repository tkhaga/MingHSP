%index
SWFShape
SWFShapeオブジェクトの作成

%prm
v1
v1 : 作成されたSWFShapeオブジェクトが代入される数値型変数

%group
SWFShapeオブジェクト操作命令

%inst
新しいSWFShapeオブジェクトを作成します。
SWFShapeオブジェクトはベクタ画像を扱うオブジェクトであり、
s_で始まる命令群で操作します。

通常、これらの命令の操作対象のオブジェクトはsel_s命令で選択しますが、
操作対象のオブジェクトがまだ選択されていないか0が選択されている場合は自動的に
この命令で作成されたオブジェクトが選択されます。

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
線の太さと色を設定

%prm
n1, n2, n3, n4, n5
n1=0～         : 線の太さ(ピクセル単位)
n2=0～255      : R値
n3=0～255      : G値
n4=0～255      : B値
n5=0～255(255) : 0で完全に透明、255で完全に不透明

%group
SWFShapeオブジェクト操作命令

%inst
現在選択されているSWFShapeオブジェクトに対しこれから引く線の太さと色を設定します。
すでに引かれている線には影響ありません。

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
ベタ塗りのSWFFillオブジェクトを作成

%prm
v1, n2, n3, n4, n5
v1             : 作成したSWFFillオブジェクトが代入される数値型変数
n2=0～255      : R値
n3=0～255      : G値
n4=0～255      : B値
n5=0～255(255) : 0で完全に透明、255で完全に不透明

%group
SWFShapeオブジェクト操作命令

%inst
選択されたSWFShapeオブジェクトに対するベタ塗りのSWFFillオブジェクトを作成します。
作成されたSWFFillオブジェクトはf_で始まる命令群で操作します。

通常、これらの命令の操作対象のオブジェクトはsel_f命令で選択しますが、
操作対象のオブジェクトがまだ選択されていないか0が選択されている場合は自動的に
この命令で作成されたオブジェクトが選択されます。

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
画像による塗りのSWFFillオブジェクトを作成

%prm
v1, n2, n3
v1 : 作成したSWFFillオブジェクトが代入される数値型変数
n2 : SWFBitmapオブジェクト
n3 : フラグ

%group
SWFShapeオブジェクト操作命令

%inst
選択されたSWFShapeオブジェクトに対する画像による塗りのSWFFillオブジェクトを作成します。
n2にはSWFBitmap, SWFBitmap_buf命令で作成されるSWFBitmapオブジェクトを指定します。
n3には次の定数のどちらかを指定します
-----------------------
 SWFFILL_TILED_BITMAP   タイル状に画像が敷き詰められる
 SWFFILL_CLIPPED_BITMAP 
-----------------------
省略時はSWFFILL_TILED_BITMAPが指定されたのと同じになります。

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
グラデーション塗りのSWFFillオブジェクトを作成

%prm
v1, n2, n3
v1 : 作成したSWFFillオブジェクトが代入される数値型変数
n2 : SWFGradientオブジェクト
n3 : フラグ

%group
SWFShapeオブジェクト操作命令

%inst
選択されたSWFShapeオブジェクトに対するグラデーション塗りのSWFFillオブジェクトを作成します。
n2にはSWFGradient命令で作成されるSWFGradientオブジェクトを指定します。
n3には次の定数のどちらかを指定します

----------------------------------------------------------------
 SWFFILL_LINEAR_GRADIENT 線状グラデーション
 SWFFILL_RADIAL_GRADIENT 中心から同心円状に広がるグラデーション
----------------------------------------------------------------

省略時はSWFFILL_LINEAR_GRADIENTが指定されたのと同じになります。

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
塗りを設定(通常)

%prm
n1
n1 : SWFFillオブジェクト

%group
SWFShapeオブジェクト操作命令

%inst
選択されたSWFShapeオブジェクトに指定したSWFFillオブジェクトによる塗りを設定します。
n1に0を指定すると塗りは設定されません。
この設定はこの命令以降に引いた線(正確には新しく作られるエッジ)に対して適用されます。
同様の命令としてs_setRightFillがありますが、通常はs_setLeftFillを使うようにして下さい。

s_setRightFillの用途はs_setRightFillの項で説明します。

%href
sel_s
SWFShape
s_addSolidFill
s_addBitmapFill
s_addGradientFill
s_setRightFill


%index
s_setRightFill
塗りを設定(特殊)

%prm
n1
n1 : SWFFillオブジェクト

%group
SWFShapeオブジェクト操作命令

%inst
選択されたSWFShapeオブジェクトに指定したSWFFillオブジェクトによる塗りを設定します。
n1に0を指定すると塗りは設定されません。
この設定はこの命令以降に引いた線(正確には新しく作られるエッジ)に対して適用されます。
同様の命令としてs_setLeftFillがありますが、通常はs_setLeftFillを使うようにして下さい。

s_setRightFillで設定した塗りは、塗り同士が重なっている時に使われます。

%href
sel_s
SWFShape
s_addSolidFill
s_addBitmapFill
s_addGradientFill
s_setLeftFill


%index
s_movePenTo
描画開始点を移動(絶対座標指定)

%prm
f1, f2
f1 : 移動先のX座標(ピクセル)
f2 :    〃   Y      〃

%group
SWFShapeオブジェクト操作命令

%inst
選択されたSWFShapeオブジェクトに対する描画命令群の描画開始点を指定された座標に移動します。
HSP2では、パラメータにはfを付けるかmhsp_float命令で変換した値を指定する必要があります。

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
描画開始点を移動(相対座標指定)

%prm
f1, f2
f1 : X方向への移動量(ピクセル)
f2 : Y          〃

%group
SWFShapeオブジェクト操作命令

%inst
選択されたSWFShapeオブジェクトに対する描画命令群の描画開始点を指定された量だけ移動します。
HSP2では、パラメータにはfを付けるかmhsp_float命令で変換した値を指定する必要があります。

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
線を引く(絶対座標指定)

%prm
f1, f2
f1 : 描画終了点のX座標(ピクセル)
f2 :      〃     Y 〃
