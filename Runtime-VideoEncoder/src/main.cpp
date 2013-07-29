#include <iostream>
#include <stdio.h>
#include <roxlu/core/Log.h>
#include <utils/Utils.h>
#include <videoencoder/VideoEncoder.h>
#include <videoencoder/VideoEncoderService.h>

#if defined(WIN32)
  std::string avconv_path = rx_get_exe_path() +"\\avconv\\win\\avconv.exe";
  std::string socket_path = "\\\\.\\pipe\\encoder";

#elif defined(__APPLE__)
  std::string avconv_path = rx_get_exe_path() +"/avconv/mac/avconv";
  std::string socket_path = "/tmp/encoder.sock";

#else
#  error VideoEncoder is not tested on this OS
#endif


SignalHandler sighandler;
VideoEncoderService service(socket_path, avconv_path);

void encoder_start(void* user);
void encoder_update(void* user);
void encoder_stop(void* user);

int main() {
  RX_VERBOSE("Runtime-VideoEncoder");

  sighandler.setup();

  if(!service.setup()) {
    RX_ERROR("Cannot setup the VideoEncoder service");
    return EXIT_FAILURE;
  }


  rx_windows_service_setup("VideoEncoder", encoder_start, encoder_update, encoder_stop, NULL);
  rx_windows_service_start();

  return EXIT_SUCCESS;
};

void encoder_start(void* user) {
  service.start();
}

void encoder_update(void* user) {
  service.update();
}

void encoder_stop(void* user) {
  service.stop();
}
