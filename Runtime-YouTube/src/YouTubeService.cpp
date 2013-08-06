#include "YouTubeService.h"

YouTubeService* YouTubeService::ptr = NULL;

// ------------------------------------------------------------

YouTubeService::YouTubeService() 
#if defined(__APPLE__)
  :server_ipc(yt, "/tmp/youtube.sock", false)
#elif defined(WIN32)
   :server_ipc(yt, "\\\\.\\pipe\\youtube", false)
#else
#  error Only tested on Mac and Win
#endif
{
  ptr = this;
}

YouTubeService::~YouTubeService() {

}

bool YouTubeService::setup() {
  
  // Enable logging
  // ------------------------------------------------------------------
  log.setup("youtube", "");
  log.writeToConsole(true);
  log.writeToFile(true);
  log.maxi();
  //  log.mini();

  // Load configuration
  // ------------------------------------------------------------------
  std::string client_id;
  std::string client_secret;
  std::string auth_code;

  Jansson j;
  if(!j.load("youtube.cfg", true)) {
    RX_ERROR("Cannot load youtube.cfg");
    return false;
  }
  j.getString("/client_id", client_id);
  j.getString("/client_secret", client_secret);
  j.getString("/auth_code", auth_code);

  if(!client_id.size() || !client_secret.size() || !auth_code.size()) {
    RX_ERROR("One of the configuration options is empty! Did you set the auth code? See html/index.html and readme.");
    return false;
  }

  // Setup YouTube
  // ------------------------------------------------------------------
  if(!yt.setup(client_id, client_secret)) {
    RX_ERROR("Cannot setup the youtube API wrapper.");
    return false;
  }

  if(!yt.hasAccessToken()) {
    RX_VERBOSE("Exchanging auth code");
    if(!yt.exchangeAuthCode(auth_code)) {
      RX_ERROR("Error while trying to exchagne the auth code for an access token");
      return false;
    }
  }

  return true;
}

void YouTubeService::update() {
  yt.update();
  server_ipc.update();
}

bool YouTubeService::start() {

  if(!server_ipc.start()) {
    RX_ERROR("Cannot start the ipc server");
    return false;
  }

  return true;
}

void YouTubeService::stop() {
  if(!server_ipc.stop()) {
    RX_ERROR("Stopping the YouTubeServerIPC failed.");
  }
}
