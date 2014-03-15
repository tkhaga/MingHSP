/*
    Ming, an SWF output library
    Copyright (C) 2002  Opaque Industries - http://www.opaque.net/

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* $Id: morph.c,v 1.13 2004/03/30 15:07:28 strk Exp $ */

#include <stdlib.h>

#include "morph.h"
#include "character.h"
#include "shape.h"
#include "fillstyle.h"
#include "linestyle.h"

#include "libming.h"

struct SWFMorph_s
{
	struct SWFCharacter_s character;

	SWFOutput out;
	SWFShape shape1;
	SWFShape shape2;
};


		char aaa[10000];
/* morph stuff */
static int
completeSWFMorphBlock(SWFBlock block)
{
	SWFMorph morph = (SWFMorph)block;

	SWFOutput out = morph->out;
	SWFShape shape1 = morph->shape1;
	SWFShape shape2 = morph->shape2;

	int nFills1;
	int nFills2;
	SWFFillStyle* fills1;
	SWFFillStyle* fills2;

	int nLines1;
	int nLines2;
	SWFLineStyle* lines1;
	SWFLineStyle* lines2;

	byte *patch;
	unsigned int patch2;
	unsigned int offset;

	SWFShape_end(shape1);
	SWFShape_end(shape2);

	/* build header */
	SWFOutput_writeUInt16(out, CHARACTERID(morph));
	SWFOutput_writeRect(out, CHARACTER(shape1)->bounds);
	SWFOutput_byteAlign(out);
	SWFOutput_writeRect(out, CHARACTER(shape2)->bounds);

	SWFOutput_byteAlign(out);
	patch2 = SWFOutput_getCurPos(out);
	SWFOutput_writeUInt32(out, 0); /* shape2 offset, patched in below.. */

	SWFShape_getFills(shape1, &fills1, &nFills1);
	SWFShape_getFills(shape2, &fills2, &nFills2);

	SWFOutput_writeMorphFillStyles(out, fills1, nFills1, fills2, nFills2);

	SWFShape_getLines(shape1, &lines1, &nLines1);
	SWFShape_getLines(shape2, &lines2, &nLines2);

	SWFOutput_writeMorphLineStyles(out, lines1, nLines1, lines2, nLines2);

	SWFOutput_byteAlign(out);

	offset =
		SWFOutput_getCurPos(out) - patch2 +
		SWFOutput_getLength(SWFShape_getOutput(shape1)) - 4;

	patch = SWFOutput_getBuffer(out) + patch2;
	*(patch++) = offset & 0xff;
	offset >>= 8;
	*(patch++) = offset & 0xff;
	offset >>= 8;
	*(patch++) = offset & 0xff;
	offset >>= 8;
	*patch = offset & 0xff;

	return
		SWFOutput_getLength(out) +
		SWFOutput_getLength(SWFShape_getOutput(shape1)) +
		SWFOutput_getLength(SWFShape_getOutput(shape2));
}


void
writeSWFMorphBlockToStream(SWFBlock block,
													 SWFByteOutputMethod method, void *data)
{
	SWFMorph morph = (SWFMorph)block;

	SWFOutput_writeToMethod(morph->out, method, data);
	SWFOutput_writeToMethod(SWFShape_getOutput(morph->shape1), method, data);
	SWFOutput_writeToMethod(SWFShape_getOutput(morph->shape2), method, data);
}


void
destroySWFMorph(SWFMorph morph)
{
	destroySWFOutput(morph->out);

	/* these must be destroyed by hand, since script wrappers can't
		 tell the difference between these and newSWFShape()'s..

	destroySWFShape(morph->shape1);
	destroySWFShape(morph->shape2);
	*/

	free(morph);
}


SWFShape
SWFMorph_getShape1(SWFMorph morph)
{
	return morph->shape1;
}


SWFShape
SWFMorph_getShape2(SWFMorph morph)
{
	return morph->shape2;
}


SWFMorph
newSWFMorphShape()
{
	SWFMorph morph = (SWFMorph) malloc(sizeof(struct SWFMorph_s));

	SWFCharacterInit((SWFCharacter)morph);

	BLOCK(morph)->type = SWF_DEFINEMORPHSHAPE;
	BLOCK(morph)->writeBlock = writeSWFMorphBlockToStream;
	BLOCK(morph)->complete = completeSWFMorphBlock;
	BLOCK(morph)->dtor = (destroySWFBlockMethod) destroySWFMorph;

	CHARACTERID(morph) = ++SWF_gNumCharacters;

	morph->out = newSWFOutput();

	morph->shape1 = newSWFShape();
	BLOCK(morph->shape1)->type = SWF_END;

	morph->shape2 = newSWFShape();
	BLOCK(morph->shape2)->type = SWF_END;
	SWFShape_setMorphFlag(morph->shape2);

	return morph;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
