#include <iostream>
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <videoencoder/VideoEncoderClientIPC.h>


#if defined(WIN32)
  std::string socket_path = "\\\\.\\pipe\\encoder";

#elif defined(__APPLE__)
  std::string socket_path = "/tmp/encoder.sock";

#else
#  error VideoEncoder is not tested on this OS
#endif

bool must_run = true;

void sh(int signum);

void on_encoded(std::string path, char* data, size_t nbytes, void* user) {
  RX_VERBOSE("/encoded called!");
  Buffer b(data, nbytes);
  VideoEncoderEncodeTask task;
  task.unpack(b);
  task.print();
}

int main() {

  RX_VERBOSE("Client test");

  signal(SIGTERM, sh);

  uint64_t encode_delay = 25 * 1000; // wait X-millis until we fire an encode request
  uint64_t encode_timeout = uv_hrtime()/1000000 + 5000; 

  VideoEncoderClientIPC client_ipc(socket_path, false);

  if(!client_ipc.connect()) {
    RX_VERBOSE("Cannot connect to the encoder server");
    return EXIT_FAILURE;
  }

  VideoEncoderEncodeTask task;
  task.dir = rx_to_data_path("test_frames/");
  task.filemask = "frame_%04d.jpg";
  task.video_filename = "output.mov";
  
  while(must_run) {
    uint64_t now = uv_hrtime() / 1000000;
    if(now >= encode_timeout) {
      RX_VERBOSE("Encode a video...");
      client_ipc.encode(task);
      encode_timeout = now + encode_delay;
    }
    client_ipc.update();
  }

  return EXIT_SUCCESS;
}

void sh(int signum) {
  must_run = false;
}
