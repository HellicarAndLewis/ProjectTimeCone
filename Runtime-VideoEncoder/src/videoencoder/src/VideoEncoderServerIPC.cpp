#include <roxlu/io/Buffer.h>
#include <videoencoder/VideoEncoder.h>
#include <videoencoder/VideoEncoderServerIPC.h>

void video_encoder_server_on_encode(std::string path, char* data, size_t nbytes, void* user) {
  RX_VERBOSE("/encode request received");

  VideoEncoderEncodeTask task;
  Buffer b(data, nbytes);
  task.unpack(b);
  task.print();

  VideoEncoderServerIPC* ipc = static_cast<VideoEncoderServerIPC*>(user);
  if(ipc->enc.encode(task)) {
    ipc->server.call("/encoded", data, nbytes);
  }

}

void video_encoder_server_on_add_audio(std::string path, char* data, size_t nbytes, void* user) {
  RX_VERBOSE("/add_audio received, with %ld bytes", nbytes);

  VideoEncoderEncodeTask task;
  Buffer b(data, nbytes);
  task.unpack(b);
  task.print();

  VideoEncoderServerIPC* ipc = static_cast<VideoEncoderServerIPC*>(user);
  if(ipc->enc.addAudio(task)) {
    RX_VERBOSE("Sending /audio_added");
    ipc->server.call("/audio_added", data, nbytes);
  }
  
}

void video_encoder_server_on_cmd(std::string path, char* data, size_t nbytes, void* user) {
  RX_VERBOSE("/cmd received");

  VideoEncoderEncodeTask task;
  Buffer b(data, nbytes);
  task.unpack(b);
  task.print();

  VideoEncoderServerIPC* ipc = static_cast<VideoEncoderServerIPC*>(user);
  if(ipc->enc.customCommand(task)) {
    RX_VERBOSE("Sending /cmd_executed");
    ipc->server.call("/cmd_executed", data, nbytes);
  }

}

VideoEncoderServerIPC::VideoEncoderServerIPC(VideoEncoder& enc, std::string sockfile, bool datapath)
  :enc(enc)
  ,server(sockfile, datapath)
{
  server.addMethod("/encode", video_encoder_server_on_encode, this);
  server.addMethod("/add_audio", video_encoder_server_on_add_audio, this);
}

VideoEncoderServerIPC::~VideoEncoderServerIPC() {

}

bool VideoEncoderServerIPC::start() {
  return server.start();
}

bool VideoEncoderServerIPC::stop() {
  return server.stop();
}

void VideoEncoderServerIPC::update() {
  server.update();
}
