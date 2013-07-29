#ifndef ROXLU_VIDEO_ENCODER_CLIENT_IPC_H
#define ROXLU_VIDEO_ENCODER_CLIENT_IPC_H

extern "C" {
#  include <uv.h>
};

#include <string>
#include <uv/ClientIPC.h>
#include <videoencoder/VideoEncoderTypes.h>


void videoencoderclientipc_on_read(ClientIPC* ipc, void* user);

typedef void(*video_encoder_on_encoded_callback)(VideoEncoderEncodeTask task, void* user);         /* gets called when the server encoded the `task` */

class VideoEncoderClientIPC {
 public:
  VideoEncoderClientIPC(std::string sockfile, bool datapath);
  ~VideoEncoderClientIPC();
  void setup(video_encoder_on_encoded_callback encodedCB, void* user);                             /* setup the callbacks */
  bool connect();
  void update();
  void encode(VideoEncoderEncodeTask task);
 private:
  void writeCommand(uint32_t command, char* data, uint32_t nbytes);
 private:
  ClientIPC client;
 public:
  video_encoder_on_encoded_callback cb_encoded;                                                    /* the callback which is triggered when the server encoded a encoder task */
  void* cb_user;
};

#endif
