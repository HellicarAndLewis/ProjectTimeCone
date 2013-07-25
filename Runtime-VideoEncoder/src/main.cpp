#include <iostream>
#include <stdio.h>
#include <roxlu/core/Log.h>
#include <videoencoder/VideoEncoder.h>

bool must_run;

void signal_handler(uv_signal_t* handle, int signum) {
  RX_WARNING("Received signal");
  must_run = false;
  uv_signal_stop(handle);
}

int main() {
  RX_VERBOSE("Runtime-VideoEncoder");

  VideoEncoder enc;

  if(!enc.setup("/Users/roxlu/Documents/programming/nissan/apps/NissanBulletTime/Runtime-VideoEncoder/bin/avconv")) {
    RX_ERROR("Cannot find the convert util");
    ::exit(EXIT_FAILURE);
  }

  std::string sockfile = "/tmp/encoder.sock";
  VideoEncoderServerIPC enc_server(enc, sockfile, false);

  if(!enc_server.start()) {
    RX_ERROR("Cannot start the video encoder server");
    ::exit(EXIT_FAILURE);
  }

  uv_loop_t* loop = uv_loop_new();
  uv_signal_t sig;
  uv_signal_init(loop, &sig);
  uv_signal_start(&sig, signal_handler, SIGINT);

  must_run = true;

  while(must_run) {
    enc_server.update();
    uv_run(loop, UV_RUN_NOWAIT);    
  }

  // enc.encode("/Users/roxlu/Documents/programming/nissan/apps/NissanBulletTime/Test0-YouTubeClientIPC/bin/data/", "frame_%04d.jpg");
  return EXIT_SUCCESS;
};

