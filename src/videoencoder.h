/*
 * videoencoder.h -- encode the video frame based on the selected encoder
 * Copyright © 2015  e-con Systems India Pvt. Limited
 *
 * This file is part of Qtcam.
 *
 * Qtcam is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Qtcam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qtcam. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __VideoEncoder_H
#define __VideoEncoder_H

#include <QIODevice>
#include <QFile>
#include <QImage>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/rational.h"
#include "libavutil/avstring.h"
#include "libswscale/swscale.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/dict.h"
#include "libavutil/channel_layout.h"
}
#include <iostream>
#include <cmath>
#include <cassert>
#include <cstddef>
#include <stdlib.h>
//#include "portaudiocpp/PortAudioCpp.hxx"





class VideoEncoder
{

public:

   VideoEncoder();
   virtual ~VideoEncoder();

   bool createFile(QString filename, AVCodecID encodeType, unsigned width,unsigned height,unsigned fpsDenominator, unsigned fpsNumerator, unsigned bitRate);
   bool closeFile();

   int encodeImage(const QImage &);
   bool isOk();
   int fill_audio_buffer(u_int64_t ts);
   int record_sound ( const void *inputBuffer, unsigned long numSamples,
    void *userData );

   void set_sound (unsigned long fpsNumerator, unsigned long fpsDenominator);
   int port_init_audio();
   int init_sound();
   int get_audio_flag(struct paRecordData *pdata);
   bool is_audio_processing(struct paRecordData *pdata, bool set_processing);
//   void* AudioLoop(void *arg);
   int encodeAudio();


protected:
      unsigned Width,Height;
      unsigned Bitrate;
      unsigned Gop;
      bool ok;
      int i;

      // FFmpeg stuff
      AVFormatContext *pFormatCtx;
      AVOutputFormat *pOutputFormat;
      AVCodecContext *pCodecCtx;
      AVStream *pVideoStream;
      AVCodec *pCodec;

      // Frame data
      AVFrame *ppicture;
      uint8_t *picture_buf;

      // Compressed data
      int outbuf_size;
      uint8_t* outbuf;
      uint8_t* finalBuf;

      // Conversion
      SwsContext *img_convert_ctx;

      // Packet
      AVPacket pkt;

      AVStream *pAudioStream;
      AVCodec *paudioCodec;
      AVFrame *pAudioFrame;
      uint8_t *paudioOutbuf;
      AVCodecContext *paudioCodecCtx;
      int audio_outbuf_size;

//      PaStreamParameters  inputParameters;
//      PaStream*           stream;
//      const PaDeviceInfo *deviceInfo;
//      PaError             err;
//      PaDeviceIndex       deviceIndex;


      QString fileName;
      QString tempExtensionCheck;

      unsigned getWidth();
      unsigned getHeight();
      bool isSizeValid();

      void initVars();
      bool initCodec();

      // Alloc/free the output buffer
      bool initOutputBuf();
      void freeOutputBuf();

      // Alloc/free a frame
      bool initFrame();
      void freeFrame();

      // Frame conversion
      bool convertImage(const QImage &img);
      bool convertImage_sws(const QImage &img);

      AVStream* add_audio_stream(AVFormatContext *oc, AVCodecID codec_id, unsigned fpsDenominator, unsigned fpsNumerator);
      void open_audio(AVStream *st);


      int check_sample_fmt(AVCodec *codec, enum AVSampleFormat sample_fmt);
      int select_sample_rate(AVCodec *codec);
      int select_channel_layout(AVCodec *codec);

//      int recordCallback( const void *pInputBuffer, void *outputBuffer,
//                                        unsigned long framesPerBuffer,
//                                        const PaStreamCallbackTimeInfo* timeInfo,
//                                        PaStreamCallbackFlags statusFlags,
//                                        void *userData );




};




#endif // VideoEncoder_H

