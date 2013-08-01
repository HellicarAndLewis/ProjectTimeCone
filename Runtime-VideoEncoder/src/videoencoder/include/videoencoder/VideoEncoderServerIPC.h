/*

  # VideoEncoderIPC

  IPC wrapper so you can run the encoder in another process

 */

#ifndef ROXLU_VIDEO_ENCODER_SERVER_IPC_H
#define ROXLU_VIDEO_ENCODER_SERVER_IPC_H

#include <uv/IPC.h>
#include <videoencoder/VideoEncoderTypes.h>

void video_encoder_server_on_encode(std::string path, char* data, size_t nbytes, void* user);
void video_encoder_server_on_add_audio(std::string path, char* data, size_t nbytes, void* user);
void video_encoder_server_on_cmd(std::string path, char* data, size_t nbytes, void* user);

class VideoEncoder;
class VideoEncoderServerIPC {
 public:
  VideoEncoderServerIPC(VideoEncoder& enc, std::string sock, bool datapath);
  ~VideoEncoderServerIPC();
  bool start();
  bool stop();
  void update();
 public:
  VideoEncoder& enc;
  ServerIPC server;
};

#endif
