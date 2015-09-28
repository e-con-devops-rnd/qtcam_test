#ifndef H264DECODER_H
#define H264DECODER_H


/* checking version compatibility */
#define LIBAVCODEC_VER_AT_LEAST(major,minor)  (LIBAVCODEC_VERSION_MAJOR > major || \
                                              (LIBAVCODEC_VERSION_MAJOR == major && \
                                               LIBAVCODEC_VERSION_MINOR >= minor))

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/rational.h"
#include "libavutil/avstring.h"
#include "libswscale/swscale.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/dict.h"
}


class H264Decoder
{

public:
   H264Decoder();
   ~H264Decoder();
   void closeFile();
   bool initH264Decoder(unsigned width,unsigned height);
   int decodeH264(uint8_t *out_buf, uint8_t *in_buf, int buf_size);

protected:
      AVCodecContext *pH264CodecCtx;
      AVCodec *pH264Codec;
      AVFrame *pH264picture;
      uint8_t *h264pictureBuf;
      int h264PictureSize;

      void initVars();
      bool initCodec();

      //free a frame
      void freeFrame();

};

#endif // H264DECODER_H
