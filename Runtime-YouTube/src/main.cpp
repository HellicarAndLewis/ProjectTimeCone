#include <iostream>
#include <utils/Utils.h>
#include "YouTubeService.h"

void youtube_sighandler(int signum, void* user);
void yt_start(void* user);
void yt_update(void* user);
void yt_stop(void* user);

SignalHandler sighandler;
YouTubeService youtube;

int main() {

  sighandler.setup();

  if(!youtube.setup()) {
    RX_ERROR("Failed to setup the youtube service.");
    return EXIT_FAILURE;
  }
  
  rx_windows_service_setup("YouTubeUploader", yt_start, yt_update, yt_stop, NULL);
  rx_windows_service_start();

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
