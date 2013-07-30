#include <stdio.h>
#include <string>
#include <youtube/YouTube.h>
#include <jansson/Jansson.h>

bool must_run = true;

void sh(int signum);

int main() {

  // get necessary config
  // -----------------------------------------------
  Jansson j;
  if(!j.load("youtube.cfg", true)) {
    RX_ERROR("Make sure that you created the youtube.cfg; we're using that config to authorize ourself");
    return EXIT_FAILURE;
  }
  
  std::string client_id;
  std::string client_secret;
  std::string auth_code;

  j.getString("/client_id", client_id);
  j.getString("/client_secret", client_secret);
  j.getString("/auth_code", auth_code);

  if(!client_id.size() || !client_secret.size() || !auth_code.size()) {
    RX_ERROR("One of the configuration options is empty! Did you set the auth code? See html/index.html and readme.");
    return EXIT_FAILURE;
  }

  // test an upload with some garbage json 
  // -----------------------------------------------
  YouTube yt;
  if(!yt.setup(client_id, client_secret)) {
    RX_ERROR("Cannot setup the youtube API handler");
    return EXIT_FAILURE;
  }

  if(!yt.hasAccessToken()) {
    RX_VERBOSE("Fetching access token");
    if(!yt.exchangeAuthCode(auth_code)) {
      RX_ERROR("Cannot update the access token");
      return EXIT_FAILURE;
    }
  }

  YouTubeVideo video;
  video.title = "some title";
  video.filename = rx_to_data_path("test.mov");
  video.datapath = false;
  video.video_resource_json = "_invalid_json_"; 
  video.bytes_total = rx_get_file_size(video.filename);

  YouTubeUploadStart uploader;
  if(uploader.start(video, yt.getAccessToken())) {
    RX_ERROR("TEST FAILED, uploader should return false on error");
  }
  else {
    RX_ERROR("TEST SUCCEEDED!");
  }

  return EXIT_SUCCESS;
}

void sh(int signum) {
  RX_VERBOSE("Received signal; stopping");
  must_run = false;
}
