#include <videoencoder/VideoEncoderTypes.h>
#include <roxlu/core/Log.h>

void VideoEncoderEncodeTask::print() {
  RX_VERBOSE("dir: %s", dir.c_str());
  RX_VERBOSE("filemask: %s", filemask.c_str());
  RX_VERBOSE("video_filename: %s", video_filename.c_str());
}
