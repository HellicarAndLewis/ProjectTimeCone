#include <iostream>
#include <utils/Utils.h>
#include "YouTubeService.h"

void youtube_sighandler(int signum, void* user);
void yt_start(void* user);
void yt_update(void* user);
void yt_stop(void* user);

SignalHandler sighandler;
YouTubeService youtube;

#if !defined(WIN32)
bool must_run = true;
#endif

int main() {

#if defined(__APPLE__)
  sighandler.cb_all = youtube_sighandler;
  sighandler.setup();
#endif

  if(!youtube.setup()) {
    RX_ERROR("Failed to setup the youtube service.");
    return EXIT_FAILURE;
  }

#if defined(WIN32)
  rx_windows_service_setup("YouTubeUploader", yt_start, yt_update, yt_stop, NULL);
  rx_windows_service_start();
#else
  yt_start(NULL);
  while(must_run) {
    yt_update(NULL);
  }
  yt_stop(NULL);
#endif

  return EXIT_SUCCESS;
};

void yt_start(void* user) {
  RX_VERBOSE("Service starts");
  youtube.start();
}
  
void yt_update(void* user) {
  youtube.update();
}
  
void yt_stop(void* user) {
  RX_VERBOSE("Service stops");
  youtube.stop();
}

#if defined(__APPLE__)
void youtube_sighandler(int signum, void* user) {
  must_run = false;
}
#endif
