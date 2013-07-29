#include <stdio.h>
#include  <youtube/YouTube.h>

bool must_run = true;

void sh(int signum);

int main() {
  YouTubeClientIPC yt_client("/tmp/nonexisting_socket", false);
  
  signal(SIGINT, sh);

  YouTubeVideo video;
  video.filename = rx_to_data_path("test.mov");
  video.datapath = false;
  video.title = "automated";
  yt_client.addVideoToUploadQueue(video);

  while(must_run) {
    yt_client.update();
  }

  return EXIT_SUCCESS;
}

void sh(int signum) {
  RX_VERBOSE("Received signal; stopping");
  must_run = false;
}
