/*

  # VideoEncoderClientIPC

  The `VideoEncoderClientIPC` allows connect to the `VideoEncoderServerIPC` and 
  enqueue encoding tasks. The `VideoEncoderServerIPC` will encode an video based
  on the information it finds in the `VideoEncoderEncodeTask` object.
  

 */
#ifndef ROXLU_VIDEO_ENCODER_CLIENT_IPC_H
#define ROXLU_VIDEO_ENCODER_CLIENT_IPC_H

extern "C" {
#  include <uv.h>
};

#include <string>
#include <uv/IPC.h>
#include <videoencoder/VideoEncoderTypes.h>

void video_encoder_client_ipc_on_encoded(std::string path, char* data, size_t nbytes, void* user); /* our callback we add to the `ClientIPC` member */
void video_encoder_client_ipc_on_audio_added(std::string path, char* data, size_t nbyts, void* user); /* our callback that gets called when we receive /audio_added from the server */

typedef void(*video_encoder_callback)(VideoEncoderEncodeTask task, void* user);         /* gets called when the server encoded the `task` */

class VideoEncoderClientIPC {
 public:
  VideoEncoderClientIPC(std::string sockfile, bool datapath);
  ~VideoEncoderClientIPC();
  //  void setup(video_encoder_callback encodedCB, void* user);                                        /* setup the callbacks */
  bool connect();                                                                                  /* connect to the VideoEncoder IPC server */
  void update();                                                                                   /* call this as often as possible */
  void encode(VideoEncoderEncodeTask task);                                                        /* encode the given video task; when ready the callback set by `setup()` will be called   */
  void addAudio(VideoEncoderEncodeTask task);                                                      /* combine video and audio, make sure to set `video_filepath` and `audio_filepath` */
 private:
  ClientIPC client;                                                                                /* our ipc helper which does all of the hard work for us */
 public:
  video_encoder_callback cb_encoded;                                                               /* the callback which is triggered when the server encoded a encoder task */
  video_encoder_callback cb_audio_added;
  void* cb_user;                                                                                   /* gets passed into the callback */
};

#endif
