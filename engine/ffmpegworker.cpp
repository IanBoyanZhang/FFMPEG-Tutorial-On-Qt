#include "ffmpegworker.h"
#include <QDebug>
//#ifndef INT64_C
//#define INT64_C(c) (c ## LL)
//#define UINT64_C(c) (c ## ULL)
//#endif
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}

FFmpengWorker::FFmpengWorker(QObject *parent) : QObject(parent)
{
}
FFmpengWorker* FFmpengWorker::instance(){
    static FFmpengWorker* instance = new FFmpengWorker();
    return instance;
}

QList<QPixmap>* FFmpengWorker::getScreenCaps(QString fileUrl){
    // Initalizing these to NULL prevents segfaults!
    AVFormatContext   *pFormatCtx = NULL;
    int               i, videoStream;
    AVCodecContext    *pCodecCtxOrig = NULL;
    AVCodecContext    *pCodecCtx = NULL;
    AVCodec           *pCodec = NULL;
    AVFrame           *pFrame = NULL;
    AVFrame           *pFrameRGB = NULL;
    AVPacket          packet;
    int               frameFinished;
    int               numBytes;
    uint8_t           *buffer = NULL;
    struct SwsContext *sws_ctx = NULL;
    QList<QPixmap>* capsList = new QList<QPixmap>();

    // Register all formats and codecs
    av_register_all();

    // Open video file
    if(avformat_open_input(&pFormatCtx, fileUrl.toStdString().data(), NULL, NULL)!=0)
      return NULL; // Couldn't open file

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
      return NULL; // Couldn't find stream information

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, fileUrl.toStdString().data(), 0);

    // Find the first video stream
    videoStream=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
      if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
        videoStream=i;
        break;
      }
    if(videoStream==-1)
      return NULL; // Didn't find a video stream

    // Get a pointer to the codec context for the video stream
    pCodecCtxOrig=pFormatCtx->streams[videoStream]->codec;
    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtxOrig->codec_id);
    if(pCodec==NULL) {
      fprintf(stderr, "Unsupported codec!\n");
      return NULL; // Codec not found
    }
    // Copy context
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
      fprintf(stderr, "Couldn't copy codec context");
      return NULL; // Error copying codec context
    }

    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
      return NULL; // Could not open codec

    // Allocate video frame
    pFrame=av_frame_alloc();

    // Allocate an AVFrame structure
    pFrameRGB=av_frame_alloc();
    if(pFrameRGB==NULL)
      return capsList;

    // Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(AVPixelFormat::AV_PIX_FMT_RGB24, pCodecCtx->width,
                    pCodecCtx->height);
    buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

    // Assign appropriate parts of buffer to image planes in pFrameRGBframeRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pFrameRGB, buffer, AVPixelFormat::AV_PIX_FMT_RGB24,
           pCodecCtx->width, pCodecCtx->height);

    // initialize SWS context for software scaling
    sws_ctx = sws_getContext(pCodecCtx->width,
                 pCodecCtx->height,
                 pCodecCtx->pix_fmt,
                 pCodecCtx->width,
                 pCodecCtx->height,
                 AVPixelFormat::AV_PIX_FMT_RGB24,
                 SWS_BILINEAR,
                 NULL,
                 NULL,
                 NULL
                 );

    // Read frames and save first five frames to disk
    i=0;
    while(av_read_frame(pFormatCtx, &packet)>=0) {
      // Is this a packet from the video stream?
      if(packet.stream_index==videoStream) {
        // Decode video frame
        avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

        // Did we get a video frame?
        if(frameFinished) {
      // Convert the image from its native format to RGB

            sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                      pFrame->linesize, 0, pCodecCtx->height,
                      pFrameRGB->data, pFrameRGB->linesize);
            // Save the frame to disk
            if(++i<=5){
                qDebug()<<"Capturing screen"<<(i);
                QImage img(pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB888);
                for( int y = 0; y < img.height(); ++y )
                   memcpy( img.scanLine(y), pFrameRGB->data[0]+y * pFrameRGB->linesize[0], img.width() * 3 );
                //scale it smoothly
                img = img.scaled(QSize(256,256),Qt::KeepAspectRatio,Qt::SmoothTransformation);
                capsList->append(QPixmap::fromImage(img));
            }
            else {
                // Free the packet that was allocated by av_read_frame
                av_free_packet(&packet);
                break;
            }

        }
      }

      // Free the packet that was allocated by av_read_frame
      av_free_packet(&packet);
    }

    // Free the RGB image
    av_free(buffer);
    av_frame_free(&pFrameRGB);

    // Free the YUV frame
    av_frame_free(&pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);
    avcodec_close(pCodecCtxOrig);

    // Close the video file
    avformat_close_input(&pFormatCtx);
    qDebug()<<"Capture screens completed";

    return capsList;
}

void FFmpengWorker::displayToScreen(QString fileUrl){
    QtConcurrent::run([=]{
        // Initalizing these to NULL prevents segfaults!
        AVFormatContext   *pFormatCtx = NULL;
        int               i, videoStream;
        AVCodecContext    *pCodecCtxOrig = NULL;
        AVCodecContext    *pCodecCtx = NULL;
        AVCodec           *pCodec = NULL;
        AVFrame           *pFrame = NULL;
        AVFrame           *pFrameRGB = NULL;
        AVPacket          packet;
        int               frameFinished;
        int               numBytes;
        uint8_t           *buffer = NULL;
        struct SwsContext *sws_ctx = NULL;

        // Register all formats and codecs
        av_register_all();

        // Open video file
        if(avformat_open_input(&pFormatCtx, fileUrl.toStdString().data(), NULL, NULL)!=0)
          return ; // Couldn't open file

        // Retrieve stream information
        if(avformat_find_stream_info(pFormatCtx, NULL)<0)
          return ; // Couldn't find stream information

        // Dump information about file onto standard error
        av_dump_format(pFormatCtx, 0, fileUrl.toStdString().data(), 0);

        // Find the first video stream
        videoStream=-1;
        for(i=0; i<pFormatCtx->nb_streams; i++)
          if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
            videoStream=i;
            break;
          }
        if(videoStream==-1)
          return ; // Didn't find a video stream

        // Get a pointer to the codec context for the video stream
        pCodecCtxOrig=pFormatCtx->streams[videoStream]->codec;
        // Find the decoder for the video stream
        pCodec=avcodec_find_decoder(pCodecCtxOrig->codec_id);
        if(pCodec==NULL) {
          fprintf(stderr, "Unsupported codec!\n");
          return ; // Codec not found
        }
        // Copy context
        pCodecCtx = avcodec_alloc_context3(pCodec);
        if(avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
          fprintf(stderr, "Couldn't copy codec context");
          return ; // Error copying codec context
        }

        // Open codec
        if(avcodec_open2(pCodecCtx, pCodec, NULL)<0)
          return ; // Could not open codec

        // Allocate video frame
        pFrame=av_frame_alloc();

        // Allocate an AVFrame structure
        pFrameRGB=av_frame_alloc();
        if(pFrameRGB==NULL)
          return ;

        // Determine required buffer size and allocate buffer
        numBytes=avpicture_get_size(AVPixelFormat::AV_PIX_FMT_RGB24, pCodecCtx->width,
                        pCodecCtx->height);
        buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

        // Assign appropriate parts of buffer to image planes in pFrameRGBframeRGB
        // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
        // of AVPicture
        avpicture_fill((AVPicture *)pFrameRGB, buffer, AVPixelFormat::AV_PIX_FMT_RGB24,
               pCodecCtx->width, pCodecCtx->height);

        // initialize SWS context for software scaling
        sws_ctx = sws_getContext(pCodecCtx->width,
                     pCodecCtx->height,
                     pCodecCtx->pix_fmt,
                     pCodecCtx->width,
                     pCodecCtx->height,
                     AVPixelFormat::AV_PIX_FMT_RGB24,
                     SWS_BILINEAR,
                     NULL,
                     NULL,
                     NULL
                     );

        // Read frames and save first five frames to disk
        i=0;
        while(av_read_frame(pFormatCtx, &packet)>=0) {
          // Is this a packet from the video stream?
          if(packet.stream_index==videoStream) {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Did we get a video frame?
            if(frameFinished) {
          // Convert the image from its native format to RGB
                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGB->data, pFrameRGB->linesize);
                // Get the pixmap to screen
                QImage img(pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB888);
                for( int y = 0; y < img.height(); ++y )
                   memcpy( img.scanLine(y), pFrameRGB->data[0]+y * pFrameRGB->linesize[0], img.width() * 3 );
                //scale it smoothly
                img = img.scaled(QSize(720,480),Qt::KeepAspectRatio,Qt::SmoothTransformation);

                //emit signal for a new image
                emit displayScreenUpdate(img);
            }
          }

          // Free the packet that was allocated by av_read_frame
          av_free_packet(&packet);
        }

        // Free the RGB image
        av_free(buffer);
        av_frame_free(&pFrameRGB);

        // Free the YUV frame
        av_frame_free(&pFrame);

        // Close the codecs
        avcodec_close(pCodecCtx);
        avcodec_close(pCodecCtxOrig);

        // Close the video file
        avformat_close_input(&pFormatCtx);
        qDebug()<<"Capture screens completed";
        emit displayFinished();
    });
}
