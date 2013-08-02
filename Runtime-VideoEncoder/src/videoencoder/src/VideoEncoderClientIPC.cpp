#include <roxlu/io/Buffer.h>
#include <videoencoder/VideoEncoderClientIPC.h>

void video_encoder_client_ipc_on_encoded(std::string path, char* data, size_t nbytes, void* user) {
  if(nbytes > 0) {

    Buffer b(data, nbytes);
    VideoEncoderEncodeTask task;
    task.unpack(b);

    VideoEncoderClientIPC* ipc = static_cast<VideoEncoderClientIPC*>(user);
    if(ipc->cb_encoded) {
      ipc->cb_encoded(task, ipc->cb_user);
    }

    task.print();
  }
}

void video_encoder_client_ipc_on_audio_added(std::string path, char* data, size_t nbytes, void* user) {
  RX_VERBOSE("Audio added!");
  if(nbytes > 0) {

    Buffer b(data, nbytes);
    VideoEncoderEncodeTask task;
    task.unpack(b);

    VideoEncoderClientIPC* ipc = static_cast<VideoEncoderClientIPC*>(user);
    if(ipc->cb_audio_added) {
      ipc->cb_audio_added(task, ipc->cb_user);
    }
  }
}


void video_encoder_client_ipc_on_cmd_executed(std::string path, char* data, size_t nbytes, void* user) {
  RX_VERBOSE("Command executed  added!");
  if(nbytes > 0) {

    Buffer b(data, nbytes);
    VideoEncoderEncodeTask task;
    task.unpack(b);

    VideoEncoderClientIPC* ipc = static_cast<VideoEncoderClientIPC*>(user);
    if(ipc->cb_cmd_executed) {
      ipc->cb_cmd_executed(task, ipc->cb_user);
    }
  }
}


// ---------------------------------------------------------------------------------

VideoEncoderClientIPC::VideoEncoderClientIPC(std::string sockfile, bool datapath) 
  :client(sockfile, datapath)
  ,cb_user(NULL)
  ,cb_encoded(NULL)
  ,cb_audio_added(NULL)
  ,cb_cmd_executed(NULL)
{
  client.addMethod("/encoded", video_encoder_client_ipc_on_encoded, this);
  client.addMethod("/audio_added", video_encoder_client_ipc_on_audio_added, this);
  client.addMethod("/cmd_executed", video_encoder_client_ipc_on_cmd_executed, this);
}

VideoEncoderClientIPC::~VideoEncoderClientIPC() {
  cb_user = NULL;
  cb_encoded = NULL;
  cb_cmd_executed = NULL;
  cb_audio_added = NULL;
}

bool VideoEncoderClientIPC::connect() {
  return client.connect();
}

void VideoEncoderClientIPC::update() {
  client.update();
}

void VideoEncoderClientIPC::encode(VideoEncoderEncodeTask task) {
  Buffer b;
  task.pack(b);
  client.call("/encode", b.ptr(), b.size());
}

void VideoEncoderClientIPC::addAudio(VideoEncoderEncodeTask task) {
  RX_VERBOSE("Sending /add_audio command");

  Buffer b;
  task.pack(b);
  client.call("/add_audio", b.ptr(), b.size());
}

void VideoEncoderClientIPC::customCommand(VideoEncoderEncodeTask task) {
  RX_VERBOSE("Sending /cmd command");

  Buffer b;
  task.pack(b);
  client.call("/cmd", b.ptr(), b.size());
}
