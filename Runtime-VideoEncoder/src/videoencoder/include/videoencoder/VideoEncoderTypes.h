#ifndef ROXLU_VIDEO_ENCODER_TYPES_H
#define ROXLU_VIDEO_ENCODER_TYPES_H

#include <string>
#include <roxlu/io/Buffer.h>

struct VideoEncoderEncodeTask {
  void print();

  std::string dir;                        /* used with `VideoEncoder::encode()`, the directory where we can find the image sequence */
  std::string filemask;                   /* used with `VideoEncoder::encode()` the mask of the image sequence that is found in the `dir`, e.g. frame_%03.jpg */
  std::string video_filename;             /* used with `VideoEncoder::encode()`, output of the video file */

  std::string audio_filepath;             /* used with `VideoEncoder::addAudio()`, the audio file that you want to add to the video */
  std::string video_filepath;             /* used with `VideoEncoder::addAudio()`, the source video underneath you want to add the audio file */

  BUFFERIZE(dir, filemask, video_filename, 
            audio_filepath, video_filepath);
};

#endif
