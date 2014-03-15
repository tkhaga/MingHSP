/*
    Ming, an SWF output library
    Copyright (C) 2001  Opaque Industries - http://www.opaque.net/

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

#if 1

#include <stdio.h>

#include "outputblock.h"
#include "sound.h"
#include "mpeg.h"

int soundDataSize(SWFSound sound)
{
  if((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_NOT_COMPRESSED)
  {
    int sampleCount = SWFInput_length(sound->input);
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
      channels = 1;  /* ? */

    nbits = 4;  /* XXX - testing.. */

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
    int samples = MPEG_getSamples(sound->input->data);
    SWFInput_seek(sound->input, pos, SEEK_SET);
    return samples;
  }
  else
  {
    return 0;
  }
}

void writeSWFSoundToStream(SWFBlock block,
			   SWFByteOutputMethod method, void *data)
{
  int l, i;
  SWFSound sound = (SWFSound)block;

  methodWriteUInt16(CHARACTERID(sound), method, data);
  method(sound->flags, data);

  l = SWFInput_length(sound->input);

  methodWriteUInt32(soundDataSize(sound), method, data);

  if ((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_MP3_COMPRESSED) {
    methodWriteUInt16(1663, method, data);  /* XXX - delay? */
  }

  /* write samples */
  for(i=0; i<l; ++i)
    method(SWFInput_getChar(sound->input), data);
}
int completeDefineSWFSoundBlock(SWFBlock block)
{
  SWFSound sound = (SWFSound)block;
  if ((sound->flags&SWF_SOUND_COMPRESSION) == SWF_SOUND_MP3_COMPRESSED) {
    return 7 + 2 + SWFInput_length(sound->input);
  } else {
    return 7 + SWFInput_length(sound->input);
  }
}

SWFOutputBlock newDefineSWFSoundStreamBlock(SWFOutput data)
{
  return newSWFOutputBlock(data, SWF_SOUNDSTREAMBLOCK);
}

void writeSWFSoundStreamHeadToMethod(SWFBlock block,
				     SWFByteOutputMethod method, void *data)
{
  SWFSound sound = (SWFSound)block;
  int numSamples = sound->numSamples;

  method(sound->flags, data);
  method(sound->flags, data);
  methodWriteUInt16(numSamples, method, data);
}
int completeSWFSoundStreamHead(SWFBlock block)
{
  return 4;
}
SWFBlock newSWFSoundStreamHead(SWFSound sound)
{
  return NULL;  /* functions of SoundStream is implemented in soundstream.c */
/*  
  return newSWFBlock(SWF_SOUNDSTREAMHEAD, sound,
		     &writeSWFSoundStreamHeadToMethod,
		     &completeSWFSoundStreamHead);
*/
}
SWFBlock newSWFSoundStreamHead2(SWFSound sound)
{
  return NULL;  /* functions of SoundStream is implemented in soundstream.c */
/*
  return newSWFBlock(SWF_SOUNDSTREAMHEAD2, sound,
		     &writeSWFSoundStreamHeadBlockToMethod,
		     &completeSWFSoundStreamHeadBlock);
*/
}

void writeSWFStartSoundToMethod(SWFBlock block,
				SWFByteOutputMethod method, void *data)
{
  SWFStartSound startSound = (SWFStartSound)block;
  SWFSoundInfo soundInfo = startSound->soundinfo;
  byte flags = soundInfo->flags;
  int i;

  if (startSound->sound)
    methodWriteUInt16(CHARACTERID(startSound->sound), method, data);
  else
    methodWriteUInt16(0, method, data);  /* 0 means NULL character */
  method(flags, data);

  if(flags & SWF_SOUNDINFO_HASINPOINT)
    methodWriteUInt32(soundInfo->inPoint, method, data);
  if(flags & SWF_SOUNDINFO_HASOUTPOINT)
    methodWriteUInt32(soundInfo->outPoint, method, data);
  if(flags & SWF_SOUNDINFO_HASLOOPS)
    methodWriteUInt16(soundInfo->numLoops, method, data);
  if(flags & SWF_SOUNDINFO_HASENVELOPE)
  {
    method(soundInfo->numEnvPoints, data);
    for(i=0; i<soundInfo->numEnvPoints; ++i)
    {
      methodWriteUInt32((soundInfo->envPoints[i]).mark44, method, data);
      methodWriteUInt16((soundInfo->envPoints[i]).level0, method, data);
      methodWriteUInt16((soundInfo->envPoints[i]).level1, method, data);
    }
  }
}
int completeSWFStartSound(SWFBlock block)
{
  SWFStartSound startSound = (SWFStartSound)block;
  SWFSoundInfo soundInfo = startSound->soundinfo;
  byte flags = soundInfo->flags;

  return 3 + /* sound id + flags */
    ((flags&SWF_SOUNDINFO_HASINPOINT) ? 4 : 0) +
    ((flags&SWF_SOUNDINFO_HASOUTPOINT) ? 4 : 0) +
    ((flags&SWF_SOUNDINFO_HASLOOPS) ? 2 : 0) +
    ((flags&SWF_SOUNDINFO_HASENVELOPE) ? (1+8*soundInfo->numEnvPoints) : 0);
}

SWFBlock newSWFStartSound(SWFSound sound, SWFSoundInfo soundInfo)
{
  SWFStartSound startSound = calloc(1, SWF_STARTSOUND_SIZE);
  SWFBlock block = (SWFBlock)startSound;

  block->type = SWF_STARTSOUND;

  block->writeBlock = &writeSWFStartSoundToMethod;
  block->complete = &completeSWFStartSound;
  /* block->dtor = &destroySWFStartSoundBlock; */

  startSound->sound = sound;
  startSound->soundinfo = soundInfo;

  return block;
}

SWFSoundInfo newSWFSoundInfo()
{
  SWFSoundInfo soundInfo = calloc(1, SWF_SOUNDINFO_SIZE);
  soundInfo->envPoints = NULL;
  return soundInfo;
}

/* XXX - temporary standin, ignoring all the soundinfo options */
SWFOutputBlock newStartSWFSoundBlock(SWFSound sound, int loops)
{
  SWFOutput out = newSizedSWFOutput(5);

  SWFOutput_writeUInt16(out, CHARACTERID(sound));

  if(loops>1)
  {
    SWFOutput_writeUInt8(out, 0x04);
    SWFOutput_writeUInt16(out, loops);
  }
  else
    SWFOutput_writeUInt8(out, 0);

  return newSWFOutputBlock(out, SWF_STARTSOUND);
}
SWFOutputBlock newStopSWFSoundBlock(SWFSound sound)
{
  SWFOutput out = newSizedSWFOutput(3);

  SWFOutput_writeUInt16(out, CHARACTERID(sound));
  SWFOutput_writeUInt8(out, 0x20);

  return newSWFOutputBlock(out, SWF_STARTSOUND);
}

SWFSound newSWFSound(FILE *f, byte flags)
{
  return newSWFSound_fromInput(newSWFInput_file(f), flags);
}
SWFSound newSWFSound_fromInput(SWFInput input, byte flags)
{
  unsigned long valid;
  SWFSound sound = calloc(1, SWFSOUND_SIZE);
  SWFBlock block = (SWFBlock)sound;

  CHARACTERID(sound) = ++SWF_gNumCharacters;

  block->type = SWF_DEFINESOUND;

  block->writeBlock = &writeSWFSoundToStream;
  block->complete = &completeDefineSWFSoundBlock;
  block->dtor = &destroySWFSound;

  sound->input = input;
  sound->flags = flags;

/* read RIFF WAVE header - THAGA */
  /* "RIFF" */
  valid = (SWFInput_getUInt32(input) != 0x46464952);
  SWFInput_seek(input, 4, SEEK_CUR);
  /* "WAVE" */
  valid += (SWFInput_getUInt32(input) != 0x45564157);
  if (valid == 0)
  {
    byte flags2 = 0;
    /* "fmt " */
    if (SWFInput_getUInt32(input) != 0x20746d66)
      goto notwave;
    valid = SWFInput_getUInt32(input);
    if (SWFInput_getUInt16(input) != 1)
      goto notwave;
    if (SWFInput_getUInt16(input) == 2)
      flags2 |= SWF_SOUND_STEREO;
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
    if (SWFInput_getUInt16(input) == 16)
      flags2 |= SWF_SOUND_16BITS;
    SWFInput_seek(input, valid - 16, SEEK_CUR);
    /* "fact" */
    if (SWFInput_getUInt32(input) == 0x74636166)
    {
      SWFInput_seek(input, 8, SEEK_CUR);
    }else{
      SWFInput_seek(input, -4, SEEK_CUR);
    }
    /* "data" */
    if (SWFInput_getUInt32(input) != 0x61746164)
      goto notwave;
    input->length = SWFInput_getUInt32(input);
    sound->flags = flags2;
  } else {
notwave:
    SWFInput_seek(input, 0, SEEK_SET);
  }

  return sound;
}
void destroySWFSound(SWFBlock sound)
{
  free(sound);
}

void SWFSound_setData(SWFSound sound, byte flags, int numSamples, byte *data)
{
  sound->flags = flags;
  sound->numSamples = numSamples;
  sound->data = data;
}

SWFBlock newSWFStartSound_start(SWFCharacter character)
{
  SWFSoundInfo soundInfo = newSWFSoundInfo();
  soundInfo->flags = 0;
  return newSWFStartSound((SWFSound)character, soundInfo);
}
SWFBlock newSWFStartSound_stop(SWFCharacter character)
{
  SWFSoundInfo soundInfo = newSWFSoundInfo();
  soundInfo->flags = SWF_SOUNDINFO_SYNCSTOPSOUND;
  return newSWFStartSound((SWFSound)character, soundInfo);
}
SWFBlock newSWFStartSound_startNoMultiple(SWFCharacter character)
{
  SWFSoundInfo soundInfo = newSWFSoundInfo();
  soundInfo->flags = SWF_SOUNDINFO_SYNCNOMULTIPLE;
  return newSWFStartSound((SWFSound)character, soundInfo);
}

#endif
