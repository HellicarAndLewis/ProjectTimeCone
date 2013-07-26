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

#if defined(WIN32)
  std::string avconv_path = rx_get_exe_path() +"\\avconv\\win\\avconv.exe";
  std::string socket_path = "\\\\.\\pipe\\encoder";
#elif defined(__APPLE__)
  std::string avconv_path = rx_get_exe_path() +"/avconv/mac/avconv";
  std::string socket_path = "/tmp/encoder.sock";
#else
#  error Not tested on this OS
#endif

  if(!rx_file_exists(avconv_path)) {
    RX_ERROR("Cannot find avconv at: %s", avconv_path.c_str());
    ::exit(EXIT_FAILURE);
  }

  if(!enc.setup(avconv_path)) {
    RX_ERROR("Cannot find the convert util");
    ::exit(EXIT_FAILURE);
  }

  VideoEncoderServerIPC enc_server(enc, socket_path, false);

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

