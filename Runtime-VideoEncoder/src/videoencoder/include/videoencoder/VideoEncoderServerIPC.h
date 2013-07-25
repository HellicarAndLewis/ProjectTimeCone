/*

  # VideoEncoderIPC

  IPC wrapper so you can run the encoder in another process

 */
#ifndef ROXLU_VIDEO_ENCODER_SERVER_IPC_H
#define ROXLU_VIDEO_ENCODER_SERVER_IPC_H

#include <uv/ServerIPC.h>
#include <videoencoder/VideoEncoderTypes.h>

void videoencoder_server_ipc_on_read(ConnectionIPC* con, void* user);

class VideoEncoder;
class VideoEncoderServerIPC {
 public:
  VideoEncoderServerIPC(VideoEncoder& enc, std::string sock, bool datapath);
  ~VideoEncoderServerIPC();
  bool start();
  bool stop();
  void update();
 public:                                                                                   /* internally used; but should be public so we can access them in callbacks */
  void writeCommand(ConnectionIPC* ipc, uint32_t command, char* data, uint32_t nbytes);
 public:
  VideoEncoder& enc;
  ServerIPC server;
};

#endif
