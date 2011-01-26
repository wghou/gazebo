/*
 * Copyright 2011 Nate Koenig & Andrew Howard
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
/* Desc: Audio Decoder
 * Author: Nathan Koenig
 * Date: 20 Jan 2008
 * SVN: $Id:$
 */

#include "gazebo_config.h"
#ifdef HAVE_FFMPEG

#include <stdlib.h>
#include <string.h>

#include "AudioDecoder.hh"


bool AudioDecoder::initialized = false;

////////////////////////////////////////////////////////////////////////////////
/// Constructor
AudioDecoder::AudioDecoder()
{

  this->formatCtx = NULL;
  this->codecCtx = NULL;
  this->codec = NULL;
  this->audioStream = 0;

  // Initialize the ffmpeg library only once
  if (!initialized)
  {
    initialized = true;
    avcodec_init();
    avcodec_register_all();

    // Register all formats and codecs
    av_register_all();
  }
}

////////////////////////////////////////////////////////////////////////////////
/// Destructor
AudioDecoder::~AudioDecoder()
{
}

////////////////////////////////////////////////////////////////////////////////
/// Decode and audio file
int AudioDecoder::Decode(uint8_t **outBuffer, unsigned int *outBufferSize)
{
  AVPacket packet;
  char tmpBuf[AVCODEC_MAX_AUDIO_FRAME_SIZE];
  int tmpBufsize = 0;
  int bytesDecoded = 0;
  unsigned int maxBufferSize = 0;

  if (this->codec == NULL)
  {
    printf("Set an audio file before decoding.\n");
    return -1;
  }

  if (outBufferSize == NULL)
  {
    printf("outBufferSize is NULL!!\n");
    return -1;
  }

  *outBufferSize = 0;

  if (*outBuffer)
  {
    delete [] *outBuffer;
    *outBuffer = NULL;
  }

  // Read the next frame of a stream
  while (av_read_frame(this->formatCtx, &packet) >=0)
  {
    if (packet.stream_index == this->audioStream)
    {
      tmpBufsize = sizeof(tmpBuf);

      // Decode the frame
      bytesDecoded = avcodec_decode_audio2( this->codecCtx, (int16_t*)tmpBuf, 
          &tmpBufsize, packet.data, packet.size );

      if (bytesDecoded < 0)
      {
        printf("Error decoding audio\n");
        return -1;
      }

      if (tmpBufsize <= 0)
      {
        printf("No data yet\n");
        return -1;
      }

      // Resize the audio buffer as necessary
      if (*outBufferSize + tmpBufsize > maxBufferSize)
      {
        maxBufferSize += tmpBufsize * 10;
        *outBuffer = (uint8_t*)realloc(*outBuffer, 
                                       maxBufferSize * sizeof(*outBuffer[0]) );
      }

      memcpy(*outBuffer + *outBufferSize, tmpBuf, tmpBufsize); 
      *outBufferSize += tmpBufsize;
    }
  }

  av_free_packet(&packet);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// Get the sample rate from the latest decoded file
int AudioDecoder::GetSampleRate()
{
  return this->codecCtx->sample_rate;
}

////////////////////////////////////////////////////////////////////////////////
/// Helper function to read in an audio file
int AudioDecoder::SetFile(const std::string &filename)
{
  unsigned int i;

  // Open file
  if (av_open_input_file(&this->formatCtx, filename.c_str(), NULL, 0, NULL) != 0)
  {
    printf("Unable to open input file\n");
    return -1;
  }

  // Retrieve some information
  if (av_find_stream_info(this->formatCtx) < 0)
  {
    printf("Unable to find stream info\n");
    return -1;
  }

  // Dump information about file onto standard error
  //dump_format(formatCtx, 0, "dump.txt", false);

  // Find audio stream;
  this->audioStream = -1;
  for (i=0; i < this->formatCtx->nb_streams; i++)
  {
    if (this->formatCtx->streams[i]->codec->codec_type == CODEC_TYPE_AUDIO)
    {
      this->audioStream = i;
      break;
    }
  }

  if (this->audioStream == -1)
  {
    printf("Couldn't find audio stream\n");
    return -1;
  }

  // Get the audio stream codec
  this->codecCtx = this->formatCtx->streams[audioStream]->codec;

  // Find a decoder
  this->codec = avcodec_find_decoder(codecCtx->codec_id);

  if (this->codec == NULL)
  {
    perror("couldn't find codec");
    return -1;
  }

  if (this->codec->capabilities & CODEC_CAP_TRUNCATED)
    this->codecCtx->flags |= CODEC_FLAG_TRUNCATED;

  // Open codec
  if (avcodec_open(this->codecCtx, this->codec) < 0)
    perror("couldn't open codec");

  return 0;
}

#endif
