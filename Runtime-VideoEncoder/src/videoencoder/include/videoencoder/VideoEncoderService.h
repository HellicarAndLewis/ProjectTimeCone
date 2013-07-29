#ifndef ROXLU_VIDEO_ENCODER_SERVICE_H
#define ROXLU_VIDEO_ENCODER_SERVICE_H

#include <log/Log.h>
#include <videoencoder/VideoEncoder.h>
#include <videoencoder/VideoEncoderServerIPC.h>
#include <string>

class VideoEncoderService {
 public:
  VideoEncoderService(std::string sockpath, std::string avconv);
  ~VideoEncoderService();
  bool setup();
  bool start();
  bool stop();
  void update();
 private:
  roxlu::Log log;
  VideoEncoder encoder;
  VideoEncoderServerIPC ipc;
  std::string avconv;
  std::string sockpath;
};
#endif

