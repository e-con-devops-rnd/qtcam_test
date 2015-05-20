///*
// * videoencoder.cpp -- encode the video frame based on the selected encoder
// * Copyright © 2015  e-con Systems India Pvt. Limited
// *
// * This file is part of Qtcam.
// *
// * Qtcam is free software; you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation; either version 3, or (at your option)
// * any later version.
// *
// * Qtcam is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with Qtcam. If not, see <http://www.gnu.org/licenses/>.
// */


#include <QPainter>
#include "videoencoder.h"
#include "defs.h"
#include "time.h"
#include "sound.h"
#include "lavc_common.h"

#ifndef G_NSEC_PER_SEC
#define G_NSEC_PER_SEC 1000000000LL
#endif

using namespace std;

typedef float SAMPLE;
struct paRecordData *pdata;

int finished;
int capVid = 0;


/**
  gop: maximal interval in frames between keyframes
**/
VideoEncoder::VideoEncoder()
{
    initVars();
    initCodec();
}

VideoEncoder::~VideoEncoder()
{
    closeFile();
}


/* check that a given sample format is supported by the encoder */
int VideoEncoder::check_sample_fmt(AVCodec *codec, enum AVSampleFormat sample_fmt)
{
   const enum AVSampleFormat *p = codec->sample_fmts;

   while (*p != AV_SAMPLE_FMT_NONE) {
       if (*p == sample_fmt)
           return 1;
       p++;
   }
   return 0;
}

/* just pick the highest supported samplerate */
int VideoEncoder::select_sample_rate(AVCodec *codec)
{
   const int *p;
   int best_samplerate = 0;

   if (!codec->supported_samplerates)
       return 44100;

   p = codec->supported_samplerates;
   while (*p) {
       best_samplerate = FFMAX(*p, best_samplerate);
       p++;
   }
   return best_samplerate;
}

/* select layout with the highest channel count */
int VideoEncoder::select_channel_layout(AVCodec *codec)
{
   const uint64_t *p;
   uint64_t best_ch_layout = 0;
   int best_nb_channells   = 0;

   if (!codec->channel_layouts)
       return AV_CH_LAYOUT_STEREO;

   p = codec->channel_layouts;
   while (*p) {
       int nb_channels = av_get_channel_layout_nb_channels(*p);

       if (nb_channels > best_nb_channells) {
           best_ch_layout    = *p;
           best_nb_channells = nb_channels;
       }
       p++;
   }
   return best_ch_layout;
}

AVStream* VideoEncoder::add_audio_stream(AVFormatContext *oc, AVCodecID codec_id, unsigned fpsDenominator, unsigned fpsNumerator)
{
    AVStream *st;

    paudioCodec = avcodec_find_encoder(AV_CODEC_ID_MP2);
    if (!paudioCodec)
    {
        printf("codec not found\n");
        return false;
    }
    st = avformat_new_stream(oc, paudioCodec);
    if (!st) {
        fprintf(stderr, "Could not alloc stream\n");
        exit(1);
    }

    paudioCodecCtx = st->codec;
    paudioCodecCtx->time_base.num = fpsNumerator;
    paudioCodecCtx->time_base.den = fpsDenominator;
    st->time_base.num = fpsNumerator;
    st->time_base.den = fpsDenominator;
    paudioCodecCtx->codec_id = codec_id;
    paudioCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;

    /* put sample parameters */
    paudioCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    paudioCodecCtx->bit_rate = 64000;
    paudioCodecCtx->sample_rate = select_sample_rate(paudioCodec);
    paudioCodecCtx->channel_layout = select_channel_layout(paudioCodec);
    paudioCodecCtx->channels    = av_get_channel_layout_nb_channels(paudioCodecCtx->channel_layout);


    // some formats want stream headers to be separate
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
        paudioCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;

    return st;
}

int VideoEncoder::fill_audio_buffer(u_int64_t ts)
{
    int ret=0;
    UINT64 buffer_length;

    if(pdata->sampleIndex >= pdata->aud_numSamples)
    {
        buffer_length = (G_NSEC_PER_SEC * pdata->aud_numSamples)/(pdata->samprate * pdata->channels);

        /*first frame time stamp*/
        if(pdata->a_ts < 0)
        {
            /* if sound begin time > first video frame ts then sync audio to video
             * else set audio ts to aprox. the video ts */
            if((pdata->ts_ref > 0) && (pdata->ts_ref < pdata->snd_begintime))
                pdata->a_ts = pdata->snd_begintime - pdata->ts_ref;
            else
                pdata->a_ts = 0;
        }
        else /*increment time stamp for audio frame*/
            pdata->a_ts += buffer_length;

        /* check audio drift through timestamps */
        if (ts > pdata->snd_begintime)
            ts -= pdata->snd_begintime;
        else
            ts = 0;
        if (ts > buffer_length)
            ts -= buffer_length;
        else
            ts = 0;
        pdata->ts_drift = ts - pdata->a_ts;

        pdata->sampleIndex = 0; /*reset*/

//        __LOCK_MUTEX( __AMUTEX );
        int flag = pdata->audio_buff_flag[pdata->bw_ind];
//        __UNLOCK_MUTEX( __AMUTEX );
        printf("\nfill audio buffer:%d\n", flag);
        if(flag == AUD_READY || flag == AUD_IN_USE)
        {
            if(flag == AUD_READY)
            {
                /*flag as IN_USE*/
                //__LOCK_MUTEX( __AMUTEX );
                    pdata->audio_buff_flag[pdata->bw_ind] = AUD_IN_USE;
                    printf("\nfill audio buffer pdata->audio_buff_flag[pdata->bw_ind if ready :%d\n", flag);
                //__UNLOCK_MUTEX( __AMUTEX );
            }



            printf("\nfill audio buffer");
            /*copy data to audio buffer*/
            memcpy(pdata->audio_buff[pdata->bw_ind][pdata->w_ind].frame, pdata->recordedSamples, pdata->aud_numBytes);
            printf("\n~~~~~~~~~~~~~%x\n",pdata->audio_buff[pdata->bw_ind][pdata->w_ind].frame);
            pdata->audio_buff[pdata->bw_ind][pdata->w_ind].time_stamp = pdata->a_ts + pdata->delay;
            pdata->audio_buff[pdata->bw_ind][pdata->w_ind].used = true;

            pdata->blast_ind = pdata->bw_ind;
            pdata->last_ind  = pdata->w_ind;

            /*doesn't need locking as it's only used in the callback*/
            NEXT_IND(pdata->w_ind, AUDBUFF_SIZE);

            if(pdata->w_ind == 0)
            {
                    /* reached end of current ring buffer
                     * flag it as AUD_PROCESS
                     * move to next one and flag it as AUD_IN_USE (if READY)
                     */
                pdata->audio_buff_flag[pdata->bw_ind] = AUD_PROCESS;

                printf("\nfill audio buffer pdata->audio_buff_flag[pdata->bw_ind if  pdata->w_ind == 0 :%d\n", pdata->audio_buff_flag[pdata->bw_ind] );
                NEXT_IND(pdata->bw_ind, AUDBUFF_NUM);

                if(pdata->audio_buff_flag[pdata->bw_ind] != AUD_READY)
                {
                    printf("AUDIO: next buffer is not yet ready\n");
                }
                else
                {
                    pdata->audio_buff_flag[pdata->bw_ind] = AUD_IN_USE;
                }

            }
        }
        else
        {
            ret = -1;
        }

    }

    return ret;

}

UINT64 ns_time_monotonic()
{
    static struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ((UINT64) ts.tv_sec * G_NSEC_PER_SEC + (ULLONG) ts.tv_nsec);
}

int VideoEncoder::record_sound ( const void *inputBuffer, unsigned long numSamples, void *userData )
{
    int j;
    pdata = (struct paRecordData*)userData;
    const SAMPLE *rptr = (const SAMPLE*) inputBuffer;

    UINT64 numFrames = numSamples / 2;
        /* buffer ends at timestamp "now", calculate beginning timestamp */
    UINT64 nsec_per_frame = G_NSEC_PER_SEC / 48000;

    UINT64 ts = ns_time_monotonic() - numFrames * nsec_per_frame;

    for(j=0; j<numSamples; j++)
    {
        pdata->recordedSamples[pdata->sampleIndex] = inputBuffer ? *rptr++ : 0;
        pdata->sampleIndex++;

        fill_audio_buffer(ts);

        /* increment timestamp accordingly while copying */
        if (j % 2 == 0)
            ts += nsec_per_frame;
    }

    if (capVid) return (0);
    else
    {
        pdata->audio_buff_flag[pdata->bw_ind] = AUD_PROCESS;
    }
    return -1;
}



int recordCallback (const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
{
    VideoEncoder *obj = reinterpret_cast<VideoEncoder *>(userData);
    unsigned long numSamples = framesPerBuffer * 2;
    printf("\nrecord callback");
    int res = obj->record_sound (inputBuffer, numSamples, userData );

    if(res < 0 )
        return (paComplete); /*capture stopped*/
    else
        return (paContinue); /*still capturing*/

}



void VideoEncoder::set_sound (unsigned long fpsNumerator, unsigned long fpsDenominator)
{
    printf("here .............");

    pdata->audio_buff[0] = NULL;
    pdata->recordedSamples = NULL;

    pdata->samprate = 48000;
    pdata->channels = 2;


    //outbuffer size in bytes (max value is for pcm 2 bytes per sample)
    pdata->outbuf_size = MPG_NUM_SAMP * pdata->channels * 2; //a good value is 240000;

//    /*initialize lavc data*/
//    if(!(pdata->lavc_data))
//    {
//        pdata->lavc_data = init_lavc_audio(fpsNumerator, fpsDenominator);
//    }
//    /*use lavc audio codec frame size to determine samples*/
//    pdata->aud_numSamples = (pdata->lavc_data)->codec_context->frame_size * pdata->channels;

//    if(pdata->aud_numSamples <= 0)
//    {
        pdata->aud_numSamples = MPG_NUM_SAMP * pdata->channels;
//    }


    pdata->outbuf = (BYTE *)malloc(pdata->outbuf_size * sizeof(BYTE *));
    pdata->aud_numBytes = pdata->aud_numSamples * sizeof(SAMPLE);
    pdata->input_type = PA_SAMPLE_TYPE;
    pdata->mp2Buff = NULL;

    pdata->sampleIndex = 0;

    printf("AUDIO: samples(%d)\n", pdata->aud_numSamples);
    pdata->flush = 0;
    pdata->a_ts= -1;
    pdata->ts_ref = 0;

    pdata->stream = NULL;


    pdata->delay += 0; /*add predefined delay - def = 0*/

    /*reset the indexes*/
    pdata->r_ind     = 0;
    pdata->w_ind     = 0;
    pdata->bw_ind    = 0;
    pdata->br_ind    = 0;
    pdata->blast_ind = 0;
    pdata->last_ind  = 0;
    /*buffer for video PCM 16 bits*/
    pdata->pcm_sndBuff=NULL;

}


int VideoEncoder::port_init_audio()
{
    int ret, numDevices, i;
    PaStreamParameters  inputParameters;

    const PaDeviceInfo *deviceInfo;
    PaError             err;
    PaDeviceIndex       deviceIndex;

    Pa_Initialize();
    numDevices = Pa_GetDeviceCount();
    printf("%d", numDevices);
    for(i=0; i<numDevices; i++ )
    {
      deviceInfo = Pa_GetDeviceInfo (i);
      if (strcmp(deviceInfo->name, "Microsoft® LifeCam Studio(TM): USB Audio (hw:1,0)") == 0 )
      {
        printf("deviceInfo->name: %s\n", deviceInfo->name);
        printf("deviceInfo->inp chan: %d\n", deviceInfo->maxInputChannels);
        printf("deviceInfo->max output channels: %d\n", deviceInfo->maxOutputChannels);
        printf("deviceInfo->sample rate: %f\n", deviceInfo->defaultSampleRate);
        deviceIndex = i;
        break;
      }

    }

    inputParameters.device = deviceIndex;
    inputParameters.channelCount = deviceInfo->maxInputChannels;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = deviceInfo->defaultHighInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;



    /* Record some audio. -------------------------------------------- */
    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* &outputParameters, */
              paudioCodecCtx->sample_rate,
              1152,
              paNoFlag,      /* we won't output out of range samples so don't bother clipping them */
              recordCallback,
              pdata );
    if(err != paNoError) {
        printf("here 22222222222%s", Pa_GetErrorText(err));
    }
   pdata->stream = (void *) stream;

    err = Pa_StartStream(stream);
    if( err != paNoError ) {
        printf("here 333333333333%s", Pa_GetErrorText(err));
    }

}


int VideoEncoder::init_sound()
{
    int err = paNoError;
    int i = 0;
    int j = 0;


    /*alloc audio ring buffers*/

        for(j=0; j< AUDBUFF_NUM; j++)
        {
            pdata->audio_buff[j] = (AudBuff *) malloc (sizeof(AudBuff) * AUDBUFF_SIZE);
            for(i=0; i<AUDBUFF_SIZE; i++)
            {
                pdata->audio_buff[j][i].frame = (SAMPLE *) malloc(sizeof(SAMPLE) * pdata->aud_numSamples);
                pdata->audio_buff[j][i].used = false;
                pdata->audio_buff[j][i].time_stamp = 0;
            }
            pdata->audio_buff_flag[j] = AUD_READY;
        }


    /*alloc the callback buffer*/
    pdata->recordedSamples = (SAMPLE *) malloc(sizeof(SAMPLE) * pdata->aud_numSamples);

    err = port_init_audio();
    if(err)
        goto error;


    /*sound start time - used to sync with video*/
    pdata->snd_begintime = ns_time_monotonic();

    return (0);

error:
    pdata->streaming=false;
    pdata->flush=0;
    pdata->delay=0;

    if(pdata->recordedSamples) free (pdata->recordedSamples);
    pdata->recordedSamples=NULL;
    if(pdata->audio_buff)
    {

        for(j=0; j< AUDBUFF_NUM; j++)
        {
            for(i=0; i<AUDBUFF_SIZE; i++)
            {
                free(pdata->audio_buff[j][i].frame);
            }
            free(pdata->audio_buff[j]);
            pdata->audio_buff[j] = NULL;
        }
    }
    /*lavc is allways checked and cleaned when finishing worker thread*/
    return(-1);
}



void VideoEncoder::open_audio(AVStream *st)
{

    AVCodec *codec;
    int ret, numDevices;
    int outbuf_size;

    paudioCodecCtx = st->codec;

    /* open it */
    if (avcodec_open2(paudioCodecCtx, paudioCodec, NULL) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }


    /* frame containing input raw audio */
    pAudioFrame = av_frame_alloc();
    if (!pAudioFrame) {
        fprintf(stderr, "Could not allocate audio frame\n");
        exit(1);
    }

    pAudioFrame->nb_samples     = paudioCodecCtx->frame_size;
    pAudioFrame->format         = paudioCodecCtx->sample_fmt;
    pAudioFrame->channel_layout = paudioCodecCtx->channel_layout;

}






int VideoEncoder::encodeAudio()
{
   int got_packet, ret;

   printf("encodeAudio here..........\n");

   if(!isOk())
      return -1;

   AVPacket apkt;

   av_init_packet(&apkt);

   apkt.data = NULL;
   apkt.size = 0;
   apkt.pts = apkt.dts = pAudioFrame->pts;
   apkt.stream_index = pAudioStream->index;

   int samples_size = av_samples_get_buffer_size(NULL, paudioCodecCtx->channels,
                                                 pAudioFrame->nb_samples,
                                                 paudioCodecCtx->sample_fmt, 1);
   printf("channels ==============%d\n",  paudioCodecCtx->channels);
   printf("samples_size ==============%d\n", samples_size);
   printf("ret :avfill :%d\n", avcodec_fill_audio_frame(pAudioFrame, paudioCodecCtx->channels,
                                  paudioCodecCtx->sample_fmt,
                                  (const uint8_t *) pdata->pcm_sndBuff, samples_size, 1));
   printf("getpocket:%d\n", got_packet);

   //pAudioFrame->pts += (paudioCodecCtx->time_base.num/paudioCodecCtx->time_base.den);

   ret = avcodec_encode_audio2(paudioCodecCtx, &apkt,pAudioFrame, &got_packet);
   printf("ret :encode audio :%d\n", ret);
   if (!ret && got_packet && paudioCodecCtx->coded_frame)
   {
       paudioCodecCtx->coded_frame->pts       = apkt.pts;
       paudioCodecCtx->coded_frame->key_frame = !!(apkt.flags & AV_PKT_FLAG_KEY);
   }


    /* write the compressed frame in the media file */
    if (av_write_frame(pFormatCtx, &apkt) != 0) {
        fprintf(stderr, "Error while writing audio frame\n");

    }
    av_free_packet(&apkt);
   return 0;
}

/* saturate float samples to int16 limits*/
static int16_t clip_int16 (float in)
{
    in = (in < -32768) ? -32768 : (in > 32767) ? 32767 : in;

    return ((int16_t) in);
}


int VideoEncoder::get_audio_flag(struct paRecordData *pdata)
{
    int flag = 0;
    //__LOCK_MUTEX(__AMUTEX);
        flag = pdata->audio_buff_flag[pdata->br_ind];
    //__UNLOCK_MUTEX(__AMUTEX);
    return flag;
}

bool VideoEncoder::is_audio_processing(struct paRecordData *pdata, bool set_processing)
{
    int flag = get_audio_flag(pdata);

    printf("\nflag: get audio flag: %d", flag);
    if((set_processing) && (flag == AUD_PROCESS))
    {
       // __LOCK_MUTEX(__AMUTEX);
            pdata->audio_buff_flag[pdata->br_ind] = AUD_PROCESSING;
       // __UNLOCK_MUTEX(__AMUTEX);

        flag = AUD_PROCESSING;
    }
    printf("\nflag: is_audio_processing: %d", flag);
    if(flag == AUD_PROCESSING)
        return true;
    else
        return false;
}

void *AudioLoop(void *a)
{
    VideoEncoder *obj = reinterpret_cast<VideoEncoder *>(a);
    printf("finished flag:%d", finished);

    while(!finished)
    {
        printf("is_audio_processing :%d\n", obj->is_audio_processing(pdata, true));
        printf("pdata->audio_buff[pdata->br_ind][pdata->r_ind].used: %d", pdata->audio_buff[pdata->br_ind][pdata->r_ind].used);
        if(obj->is_audio_processing(pdata, true) &&
            pdata->audio_buff[pdata->br_ind][pdata->r_ind].used)
        {
            printf("is_audio_processing :%d\n", obj->is_audio_processing(pdata, true));

            //if (pdata->pcm_sndBuff == NULL)
            pdata->pcm_sndBuff = (int16_t *) malloc(pdata->aud_numSamples);

            int samp = 0;

            for(samp=0; samp < pdata->aud_numSamples; samp++)
            {
                pdata->pcm_sndBuff[samp] = clip_int16(pdata->audio_buff[pdata->br_ind][pdata->r_ind].frame[samp] * 32767.0); //* 32768 + 385;
                printf("audio loop: samples");
            }

            printf("\nbefore encode audio");
            obj->encodeAudio();
            printf("\n After encode audio");

            pdata->audio_buff[pdata->br_ind][pdata->r_ind].used = false;
            NEXT_IND(pdata->r_ind, AUDBUFF_SIZE);
            /*start of new buffer block*/
             if(pdata->r_ind == 0)
             {
                pdata->audio_buff_flag[pdata->br_ind] = AUD_READY;
                NEXT_IND(pdata->br_ind, AUDBUFF_NUM);
             }
        }
        else
        {
            if (capVid)
            {
                /*video buffer underrun            */
                /*wait for next frame (sleep 10 ms)*/
                sleep(10);
            }
            else
            {
                finished = 1; /*all frames processed and no longer capturing so finish*/
            }
        }

        // free(pdata->pcm_sndBuff);
        //finished = 1;
    }
    delete obj;
    return ((void *) 0);
}

//void * VideoEncoder::AudioLoop(void *arg)
//{

//    while(!finished)
//    {
//        if(is_audio_processing(pdata, true) &&
//            pdata->audio_buff[pdata->br_ind][pdata->r_ind].used)
//        {

//            //if (pdata->pcm_sndBuff == NULL)
//            pdata->pcm_sndBuff = (int16_t *) malloc(pdata->aud_numSamples);

//            int samp = 0;

//            for(samp=0; samp < pdata->aud_numSamples; samp++)
//            {
//                pdata->pcm_sndBuff[samp] = clip_int16(pdata->audio_buff[pdata->br_ind][pdata->r_ind].frame[samp] * 32767.0); //* 32768 + 385;
//            }

//            encodeAudio();

//            pdata->audio_buff[pdata->br_ind][pdata->r_ind].used = false;
//            NEXT_IND(pdata->r_ind, AUDBUFF_SIZE);
//            /*start of new buffer block*/
//             if(pdata->r_ind == 0)
//             {
//                pdata->audio_buff_flag[pdata->br_ind] = AUD_READY;
//                NEXT_IND(pdata->br_ind, AUDBUFF_NUM);
//             }
//        }
//        else
//        {
//            if (capVid)
//            {
//                /*video buffer underrun            */
//                /*wait for next frame (sleep 10 ms)*/
//                sleep(10);
//            }
//            else
//            {
//                finished = 1; /*all frames processed and no longer capturing so finish*/
//            }
//        }

//        // free(pdata->pcm_sndBuff);
//        //finished = 1;
//    }
//    return ((void *) 0);

//}

bool VideoEncoder::createFile(QString fileName,AVCodecID encodeType, unsigned width,unsigned height,unsigned fpsDenominator, unsigned fpsNumerator, unsigned bitrate)
{
    pthread_t audThread;

    finished = 0;

    // If we had an open video, close it.
    closeFile();

    Width=width;
    Height=height;
    Bitrate=bitrate;

    if(!isSizeValid())
    {
        printf("Invalid size\n");
        return false;
    }

    pOutputFormat = av_guess_format(NULL, fileName.toStdString().c_str(), NULL);
    if (!pOutputFormat) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        pOutputFormat = av_guess_format("mpeg", NULL, NULL);
    }

    pOutputFormat->video_codec = (AVCodecID)encodeType;
    pFormatCtx= avformat_alloc_context();
    if(!pFormatCtx)
    {
        printf("Error allocating format context\n");
        return false;
    }
    pFormatCtx->oformat = pOutputFormat;

    snprintf(pFormatCtx->filename, sizeof(pFormatCtx->filename), "%s", fileName.toStdString().c_str());

    // find the video encoder

    if(pOutputFormat->video_codec != AV_CODEC_ID_NONE) {
        pCodec = avcodec_find_encoder(pOutputFormat->video_codec);
        if (!pCodec)
        {
            printf("codec not found\n");
            return false;
        }
        // Add the video stream

        pVideoStream = avformat_new_stream(pFormatCtx,pCodec);
        if(!pVideoStream )
        {
            printf("Could not allocate stream\n");
            return false;
        }

        pCodecCtx=pVideoStream->codec;
        // some formats want stream headers to be separate
        if(pFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
            pCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;


        pCodecCtx->codec_id = pOutputFormat->video_codec;

        if(encodeType == AV_CODEC_ID_RAWVIDEO)
            pCodecCtx->pix_fmt =  PIX_FMT_YUYV422;//AV_PIX_FMT_YUV444P;//AV_PIX_FMT_YUV422P;//PIX_FMT_YUYV422;//PIX_FMT_YUV420P;
        else if(encodeType == AV_CODEC_ID_MJPEG)
            pCodecCtx->pix_fmt =  PIX_FMT_YUVJ420P;
        else {
            pCodecCtx->pix_fmt =  PIX_FMT_YUV420P;
        }
        pCodecCtx->bit_rate = Bitrate;
        pCodecCtx->width = getWidth();
        pCodecCtx->height = getHeight();
        pVideoStream->time_base.den = fpsDenominator;
        pVideoStream->time_base.num = fpsNumerator;
        pCodecCtx->gop_size = fpsDenominator/fpsNumerator;
        pCodecCtx->time_base.den = fpsDenominator;
        pCodecCtx->time_base.num = fpsNumerator;
        tempExtensionCheck = fileName.mid(fileName.length()-3);
        if(pOutputFormat->video_codec == AV_CODEC_ID_H264 || pOutputFormat->video_codec == AV_CODEC_ID_VP8) {
            pCodecCtx->qmin = 15; // qmin = 10*
            pCodecCtx->qmax = 30; //qmax = 51 **
        }
        // open the codec
        if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0)
        {
            printf("could not open codec\n");
            return false;
        }

        //Allocate memory for output
        if(!initOutputBuf())
        {
            printf("Can't allocate memory for output bitstream\n");
            return false;
        }
        // Allocate the YUV frame
        if(!initFrame())
        {
            printf("Can't init frame\n");
            return false;
        }
    }

    pOutputFormat->audio_codec = AV_CODEC_ID_MP2;

    pdata = (struct paRecordData*)malloc(sizeof(struct paRecordData));
    if (pOutputFormat->audio_codec != AV_CODEC_ID_NONE){
       pAudioStream = add_audio_stream(pFormatCtx, pOutputFormat->audio_codec, fpsDenominator, fpsNumerator);
    }
    if (pAudioStream)
        open_audio(pAudioStream);


    set_sound(fpsDenominator, fpsNumerator);

    init_sound();

    // thread for audio
    int ret1 = pthread_create(&audThread, NULL, AudioLoop, NULL);

     if(ret1)
     {
         printf("Error - pthread_create() return code: %d\n",ret1);
         exit(1);
     }


    if (!(pOutputFormat->flags & AVFMT_NOFILE)) {
        if (avio_open(&pFormatCtx->pb, fileName.toStdString().c_str(), AVIO_FLAG_WRITE) < 0) {
            fprintf(stderr, "Could not open '%s'\n", fileName.toStdString().c_str());
            return 1;
        }
    }
    int ret = avformat_write_header(pFormatCtx,NULL);
    if(ret<0) {
        printf("Unable to record video...");
        return false;
    }
    ok=true;
    return true;
}

bool VideoEncoder::closeFile()
{
    if(!isOk())
        return false;


    av_write_trailer(pFormatCtx);

    // close_video

    capVid = 0;
    avcodec_close(pVideoStream->codec);
    freeFrame();
    freeOutputBuf();


    /* free the streams */

    for(unsigned int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        av_freep(&pFormatCtx->streams[i]->codec);
        av_freep(&pFormatCtx->streams[i]);
    }

    sws_freeContext(img_convert_ctx);

    // Close file
    avio_close(pFormatCtx->pb);

    // Free the stream
    av_free(pFormatCtx);

    initVars();

    //audio

   // finished = 1;
    if(pdata->audio_buff)
    {

        for(int j=0; j< AUDBUFF_NUM; j++)
        {
            for(int k=0; k<AUDBUFF_SIZE; k++)
            {
                free(pdata->audio_buff[j][k].frame);
            }
            free(pdata->audio_buff[j]);
            pdata->audio_buff[j] = NULL;
        }
    }

    free(pdata->recordedSamples);
    free(pdata->outbuf);
    if(pdata->pcm_sndBuff)
        free(pdata->pcm_sndBuff);
    pdata->pcm_sndBuff = NULL;
    free(pdata);



//    av_freep(paudioOutbuf);
//    av_frame_free(&pAudioFrame);
//    avcodec_free_frame(&pAudioFrame);
//    avcodec_close(paudioCodecCtx);


    err = Pa_StopStream(stream);
    if( err != paNoError ) {
        printf("%s", Pa_GetErrorText(err));
    }
    err = Pa_CloseStream(stream);
    if( err != paNoError ) {
        printf("%s", Pa_GetErrorText(err));
    }

    return true;
}


/**
   \brief Encode one frame

   The frame must be of the same size as specifie
**/
int VideoEncoder::encodeImage(const QImage &img)
{
    if(!isOk())
        return -1;
    capVid = 1;
    convertImage_sws(img);     // SWS conversion

    int got_packet = 0;
    int out_size = 0;
    pkt.data = NULL;
    pkt.size = 0;
    av_init_packet(&pkt);
    pkt.pts = pkt.dts = ppicture->pts;
    /* encode the image */
    int ret = avcodec_encode_video2(pCodecCtx, &pkt, ppicture, &got_packet);
    if (ret < 0) {
        fprintf(stderr, "Error encoding a video frame\n");
        exit(1);
    }
    if (got_packet) {
        if (pCodecCtx->coded_frame->pts != AV_NOPTS_VALUE)
            av_packet_rescale_ts(&pkt, pCodecCtx->time_base, pVideoStream->time_base);
        pkt.stream_index = pVideoStream->index;
        if((tempExtensionCheck) == "mkv") {
            i++;
            pkt.pts = (i*(1000/pCodecCtx->gop_size));
            pkt.dts = pkt.pts;
        }
        if(pCodecCtx->coded_frame->key_frame)
            pkt.flags |= AV_PKT_FLAG_KEY;

        /* Write the compressed frame to the media file. */
        out_size = av_write_frame(pFormatCtx, &pkt);
        av_free_packet(&pkt);
    }
    return out_size;
}


void VideoEncoder::initVars()
{
    ok=false;
    pFormatCtx=0;
    pOutputFormat=0;
    pCodecCtx=0;
    pVideoStream=0;
    pCodec=0;
    ppicture=0;
    outbuf=0;
    picture_buf=0;
    img_convert_ctx=0;
    i = 0;
}

bool VideoEncoder::initCodec()
{
    av_register_all();
    return true;
}

bool VideoEncoder::isSizeValid()
{
    if(getWidth()%8)
        return false;
    if(getHeight()%8)
        return false;
    return true;
}

unsigned VideoEncoder::getWidth()
{
    return Width;
}

unsigned VideoEncoder::getHeight()
{
    return Height;
}

bool VideoEncoder::isOk()
{
    return ok;
}


bool VideoEncoder::initOutputBuf()
{
    outbuf_size = getWidth()*getHeight()*3;
    outbuf = new uint8_t[outbuf_size];
    if(outbuf==0)
        return false;
    return true;
}

void VideoEncoder::freeOutputBuf()
{
    if(outbuf)
    {
        delete[] outbuf;
        outbuf=0;
    }
}

bool VideoEncoder::initFrame()
{
    ppicture = av_frame_alloc();
    if(ppicture==0)
        return false;

    int size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    picture_buf = new uint8_t[size];
    if(picture_buf==0)
    {
        av_free(ppicture);
        ppicture=0;
        return false;
    }

    // Setup the planes
    avpicture_fill((AVPicture *)ppicture, picture_buf,pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    return true;
}
void VideoEncoder::freeFrame()
{
    if(picture_buf)
    {
        delete[] picture_buf;
        picture_buf=0;
    }
    if(ppicture)
    {
        av_free(ppicture);
        ppicture=0;
    }
}

bool VideoEncoder::convertImage_sws(const QImage &img)
{
    // Check if the image matches the size
    if((unsigned)img.width()!=getWidth() || (unsigned)img.height()!=getHeight())
    {
        printf("Wrong image size!\n");
        return false;
    }
    if(img.format()!=QImage::Format_RGB32 && img.format() != QImage::Format_ARGB32)
    {
        printf("Wrong image format\n");
        return false;
    }

    img_convert_ctx = sws_getCachedContext(img_convert_ctx,getWidth(),getHeight(),PIX_FMT_RGB32,getWidth(),getHeight(),pCodecCtx->pix_fmt,SWS_FAST_BILINEAR, NULL, NULL, NULL);

    if (img_convert_ctx == NULL)
    {
        printf("Cannot initialize the conversion context\n");
        return false;
    }

    uint8_t *srcplanes[3];
    srcplanes[0]=(uint8_t*)img.bits();
    srcplanes[1]=0;
    srcplanes[2]=0;

    int srcstride[3];
    srcstride[0]=img.bytesPerLine();
    srcstride[1]=0;
    srcstride[2]=0;

    sws_scale(img_convert_ctx, srcplanes, srcstride,0, getHeight(), ppicture->data, ppicture->linesize);

    return true;
}





