#ifndef ROXLU_YOUTUBE_SERVICE_H
#define ROXLU_YOUTUBE_SERVICE_H

extern "C" {
#  include <uv.h> 
}

#include <log/Log.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <youtube/YouTube.h>
#include <jansson.h>
#include <jansson/Jansson.h>

void signal_handler(uv_signal_t* handle, int signum);
void signal_kill_handler(uv_signal_t* handle, int signum);

class YouTubeService {
 public:
  YouTubeService();
  ~YouTubeService();
  bool setup();
  bool start();
  void update();
  void stop();
 public:
  roxlu::Log log;
  uv_loop_t* loop;
  uv_signal_t sig_int;
  uv_signal_t sig_kill;
  YouTube yt;
  YouTubeServerIPC server_ipc;
  static YouTubeService* ptr;
};


#endif
