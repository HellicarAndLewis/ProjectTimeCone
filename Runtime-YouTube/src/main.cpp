#include <iostream>
#include <log/Log.h>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <youtube/YouTube.h>
#include <jansson.h>

bool must_run;

void signal_handler(uv_signal_t* handle, int signum) {
  must_run = false;
  uv_signal_stop(handle);
  RX_WARNING("Received signal");
}

int main() {
  // Enable logging
  // ------------------------------------------------------------------
  roxlu::Log log;
  log.setup("youtube", "");
  log.writeToConsole(true);
  log.maxi();
  log.mini();

  // Load configuration
  // ------------------------------------------------------------------

  std::string config_file = rx_to_data_path("youtube.cfg");
  if(!rx_file_exists(config_file)) {
    RX_ERROR("Please create the youtube.cfg file; see README.md for more info");
    ::exit(EXIT_FAILURE);
  }

  // Validate config
  // ------------------------------------------------------------------
  std::string client_id;
  std::string client_secret;
  std::string auth_code;
  const char* client_id_cstr;
  const char* client_secret_cstr;
  const char* auth_code_cstr ;

  json_error_t err;
  json_t* json = json_load_file(config_file.c_str(), 0, &err);
  if(!json) {
    RX_ERROR("Error with json: %s (%d)", err.text, err.line);
    ::exit(EXIT_FAILURE);
  }

  json_unpack(json, "{s:s,s:s,s:s}",
              "client_id", &client_id_cstr, 
              "client_secret", &client_secret_cstr, 
              "auth_code", &auth_code_cstr);

  client_id = client_id_cstr;
  client_secret = client_secret_cstr;
  auth_code = auth_code_cstr;
  json_decref(json);

  if(!client_id.size() || !client_secret.size() || !auth_code.size()) {
    RX_ERROR("One of the configuration options is empty! Did you set the auth code? See html/index.html and readme.");
    ::exit(EXIT_FAILURE);
  }

  // Setup YouTube
  // ------------------------------------------------------------------
  YouTube yt;
  if(!yt.setup(client_id, client_secret)) {
    RX_ERROR("Cannot setup the youtube API wrapper.");
    ::exit(EXIT_FAILURE);
  }

  if(!yt.hasAccessToken()) {
    RX_VERBOSE("Fetching access token");
    if(!yt.exchangeAuthCode(auth_code)) {
      RX_ERROR("Error while trying to exchagne the auth code for an access token");
      ::exit(EXIT_FAILURE);
    }
  }

  // YouTubeIPC
  // ------------------------------------------------------------------
#if defined(__APPLE__)
  YouTubeServerIPC server_ipc(yt, "/tmp/youtube.sock", false);
#elif defined(WIN32)
  YouTubeServerIPC server_ipc(yt, "\\\\.\\pipe\\youtube", false);
#else
#  error Only tested on Mac and Win
#endif

  if(!server_ipc.start()) {
    RX_ERROR("Cannot start the ipc server");
    ::exit(EXIT_FAILURE);
  }

  // Signal handler
  // ------------------------------------------------------------------
  uv_loop_t* loop = uv_loop_new();
  uv_signal_t sig;
  uv_signal_init(loop, &sig);
  uv_signal_start(&sig, signal_handler, SIGINT);

  // Main loop
  // ------------------------------------------------------------------
  must_run = true;

  while(must_run) {
    yt.update();
    server_ipc.update();
    uv_run(loop, UV_RUN_NOWAIT);
  }

  if(!server_ipc.stop()) {
    RX_ERROR("Stopping the YouTubeServerIPC failed.");
  }

  return EXIT_SUCCESS;
};

