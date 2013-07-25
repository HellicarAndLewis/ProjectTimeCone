#ifndef ROXLU_VIDEO_ENCODER_TYPES_H
#define ROXLU_VIDEO_ENCODER_TYPES_H

#undef nil
#include <msgpack.hpp>
#include <string>

enum VideoEncoderCommands {
  VE_CMD_ENCODE = 1,                       /* from client -> server; is used when the client wants to encode a video */
  VE_CMD_ENCODED = 2,                      /* from server -> client; when we received a VE_CMD_ENCODE command; */
};

struct VideoEncoderEncodeTask {
  void print();

  std::string dir;
  std::string filemask;
  std::string video_filename;

  MSGPACK_DEFINE(dir, filemask, video_filename);
};

#endif
