/*

  # VideoEncoder

  This application doesn't do much, except that it starts a avconv/ffmpeg/ process
  which will encode a image sequence into a video file.

 */
#ifndef ROXLU_VIDEO_ENCODER_H
#define ROXLU_VIDEO_ENCODER_H

#include <string>
#include <videoencoder/VideoEncoderTypes.h>
#include <videoencoder/VideoEncoderServerIPC.h>
#include <videoencoder/VideoEncoderClientIPC.h>

class VideoEncoder {
 public:
  VideoEncoder();
  ~VideoEncoder();
  bool setup(std::string convpath);                            /* sets the path to the convert util (avconv) */
  bool encode(VideoEncoderEncodeTask task);                    /* this will encode all the images in the directory `task.dir` (full path). The task.filemask should be something like: frame_%04d.jpg */
  bool addAudio(VideoEncoderEncodeTask task);           /* add audio to a video */
 private:
  bool isSetup();                                              /* checks if we've been setup correctly and we can use avconv */
 private:
  std::string conv_util;
};

#endif
