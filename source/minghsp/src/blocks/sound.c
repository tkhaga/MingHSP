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

/* $Id: sound.c,v 1.14 2004/03/30 15:07:28 strk Exp $ */

#include <stdio.h>
#include <stdlib.h>

#include "outputblock.h"
#include "sound.h"
#include "character.h"
#include "method.h"
#include "input.h"
#include "libming.h"

struct SWFSound_s
{
	struct SWFCharacter_s character;

	byte flags;
	byte isFinished;
	int numSamples;
	int delay;
	int samplesPerFrame;

	SWFInput input;
	byte *data;

	int realsize;	/* To deal with tagged files */
};


int
getMP3Size(SWFInput input);


static int
soundDataSize(SWFSound sound)
{
	if ((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_NOT_COMPRESSED ||
		 (sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_NOT_COMPRESSED_LE)
	{
		int sampleCount;
		if (sound->realsize)
		{
			sampleCount = sound->realsize;
		}
		else {
			sampleCount = SWFInput_length(sound->input);
		}

		if ((sound->flags & SWF_SOUND_BITS) == SWF_SOUND_16BITS)
			sampleCount /= 2;

		if ((sound->flags & SWF_SOUND_CHANNELS) == SWF_SOUND_STEREO)
			sampleCount /= 2;

		return sampleCount;
	}
	else if ((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_ADPCM_COMPRESSED)
	{
		int filesize, channels, nbits;
		int bitsize, blocksize, n, res, m;

		SWF_assert((sound->flags & SWF_SOUND_BITS) == SWF_SOUND_16BITS);

		filesize = SWFInput_length(sound->input);

		if ((sound->flags&SWF_SOUND_CHANNELS) == SWF_SOUND_MONO)
			channels = 1;
		else if ((sound->flags & SWF_SOUND_CHANNELS) == SWF_SOUND_STEREO)
			channels = 2;
		else
			channels = 1;	 /* ? */

		nbits = 4;	/* XXX - testing.. */

		/*
		 * Estimation of the sample count in ADPCM data from file size of the data.
		 * This is an approximate calculation.
		 */
		bitsize = 8 * filesize - (2 + (8 - 1));
		/* 2: header, (8 - 1): possible padding */
		blocksize = ((16 + 6) + nbits * 4095) * channels;
		n = bitsize / blocksize;
		res = bitsize % blocksize;
		m = (res - (16 + 6) * channels) / (nbits * channels);
		return 4096 * n + m;
	}
	else if ((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_MP3_COMPRESSED)
	{
		int pos = SWFInput_tell(sound->input);
		int samples = getMP3Size(sound->input);
		SWFInput_seek(sound->input, pos, SEEK_SET);
		return samples;
	}
	else /* ??? */
	{
		return 0;
	}
}


void
writeSWFSoundToStream(SWFBlock block, SWFByteOutputMethod method, void *data)
{
	int l, i;
	SWFSound sound = (SWFSound)block;

	methodWriteUInt16(CHARACTERID(sound), method, data);
	method(sound->flags, data);

	if (sound->realsize > 0)
	{
		l = sound->realsize;
	}
	else {
		l = SWFInput_length(sound->input);
	}

	methodWriteUInt32(soundDataSize(sound), method, data);

	if ( (sound->flags & SWF_SOUND_COMPRESSION) == SWF_SOUND_MP3_COMPRESSED )
		//methodWriteUInt16(SWFSOUND_INITIAL_DELAY, method, data);	// XXX - delay?
		methodWriteUInt16(0, method, data);	/* above is not good */

	/* write samples */
	for ( i=0; i<l; ++i )
		method((unsigned char)SWFInput_getChar(sound->input), data);
}


int
completeDefineSWFSoundBlock(SWFBlock block)
{
	int l;
	SWFSound sound = (SWFSound)block;

	if (sound->realsize > 0)
	{
		l = sound->realsize;
	}
	else {
		l = SWFInput_length(sound->input);
	}

	if ((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_MP3_COMPRESSED)
		return 7 + 2 + l;
	else
		return 7 + l;
}


void
destroySWFSound(SWFSound sound)
{
	destroySWFCharacter((SWFCharacter) sound);
}


SWFSound
newSWFSound(FILE *f, byte flags)
{
	return newSWFSound_fromInput(newSWFInput_file(f), flags);
}

/* added by David McNab <david@rebirthing.co.nz> */
/* required so that python can pass in file descriptors instead of FILE* streams */

SWFSound
newSWFSoundFromFileno(int fd, byte flags)
{
  FILE *fp = fdopen(fd, "r");
  return newSWFSound(fp, flags);
}


SWFSound
newSWFSound_fromInput(SWFInput input, byte flags)
{
	unsigned long valid;
	SWFSound sound = (SWFSound)malloc(sizeof(struct SWFSound_s));
	SWFBlock block = (SWFBlock)sound;

	SWFCharacterInit((SWFCharacter)sound);

	CHARACTERID(sound) = ++SWF_gNumCharacters;

	block->type = SWF_DEFINESOUND;

	block->writeBlock = writeSWFSoundToStream;
	block->complete = completeDefineSWFSoundBlock;
	block->dtor = (destroySWFBlockMethod) destroySWFSound;

	sound->input = input;
	sound->flags = flags;

	valid = (SWFInput_getUInt32(input) != 0x46464952);	/* "RIFF" */
	SWFInput_seek(input, 4, SEEK_CUR);
	valid += (SWFInput_getUInt32(input) != 0x45564157);	/* "WAVE" */
	if (valid == 0) {
		byte flags2 = 0;
		if (SWFInput_getUInt32(input) != 0x20746d66) {	/* "fmt " */
			goto notwave;
		}
		valid = SWFInput_getUInt32(input);
		if (SWFInput_getUInt16(input) != 1) {
			goto notwave;
		}
		if (SWFInput_getUInt16(input) == 2) {
			flags2 |= SWF_SOUND_STEREO;
		}
		switch (SWFInput_getUInt32(input))
		{
		case 11025:
			flags2 |= SWF_SOUND_11KHZ;
			break;
		case 22050:
			flags2 |= SWF_SOUND_22KHZ;
			break;
		case 44100:
			flags2 |= SWF_SOUND_44KHZ;
			break;
		default:
			break;
		}
		SWFInput_seek(input, 6, SEEK_CUR);
		if (SWFInput_getUInt16(input) == 16) {
			flags2 |= SWF_SOUND_16BITS;
		}
		SWFInput_seek(input, valid - 16, SEEK_CUR);
		if (SWFInput_getUInt32(input) == 0x74636166) {	/* "fact" */
			SWFInput_seek(input, 8, SEEK_CUR);
		}
		else {
			SWFInput_seek(input, -4, SEEK_CUR);
		}
		if (SWFInput_getUInt32(input) != 0x61746164) {	/* "data" */
			goto notwave;
		}
		sound->realsize = SWFInput_getUInt32(input);
		sound->flags = flags2;
	}
	else {
notwave:
		SWFInput_seek(input, 0, SEEK_SET);
		sound->realsize = 0;
	}

	return sound;
}


void
SWFSound_setData(SWFSound sound, byte flags, int numSamples, byte *data)
{
	sound->flags = flags;
	sound->numSamples = numSamples;
	sound->data = data;
}


/*
 * Local variables:
 * tab-width: 2
 * c-basic-offset: 2
 * End:
 */
