#include <roxlu/io/Buffer.h>
#include <videoencoder/VideoEncoderClientIPC.h>

void videoencoderclientipc_on_read(ClientIPC* ipc, void* user) {

  int cmd;
  uint32_t nbytes;
  VideoEncoderClientIPC* client = static_cast<VideoEncoderClientIPC*>(user);
  size_t offset = sizeof(cmd) + sizeof(nbytes);

  while(ipc->buffer.size() > offset) {
    
    memcpy((char*)&cmd, &ipc->buffer[0], sizeof(cmd));
    memcpy((char*)&nbytes, &ipc->buffer[sizeof(cmd)], sizeof(nbytes));

    if(ipc->buffer.size() - offset >= nbytes) {

      Buffer in;
      in.putBytes(&ipc->buffer[offset], nbytes);

      if(cmd == VE_CMD_ENCODED) {
        VideoEncoderEncodeTask task;
        in >> task.dir >> task.filemask >> task.video_filename;

        task.print();

        ipc->buffer.erase(ipc->buffer.begin(), ipc->buffer.begin() + offset + nbytes);

        // notify caller
        if(client->cb_encoded) {
          client->cb_encoded(task, client->cb_user);
        }

      }
      else {
        RX_ERROR("Unhandled command: %ld, flushing the buffer!", cmd);
        ipc->buffer.clear();
      }
    }
    else {
      break;
    }
  }
}

// ---------------------------------------------------------------------------------


VideoEncoderClientIPC::VideoEncoderClientIPC(std::string sockfile, bool datapath) 
  :client(sockfile, datapath)
  ,cb_user(NULL)
  ,cb_encoded(NULL)
{
  client.setup(NULL, videoencoderclientipc_on_read, this);
}

VideoEncoderClientIPC::~VideoEncoderClientIPC() {
  cb_user = NULL;
  cb_encoded = NULL;
}

void VideoEncoderClientIPC::setup(video_encoder_on_encoded_callback encodedCB, void* user) {
  cb_encoded = encodedCB;
  cb_user = user;
}

bool VideoEncoderClientIPC::connect() {
  return client.connect();
}

void VideoEncoderClientIPC::update() {
  client.update();
}

void VideoEncoderClientIPC::encode(VideoEncoderEncodeTask task) {
  Buffer buf;
  buf << task.dir << task.filemask << task.video_filename;
  writeCommand(VE_CMD_ENCODE, buf.ptr(), buf.size());
}

void VideoEncoderClientIPC::writeCommand(uint32_t command, char* data, uint32_t nbytes) {
  client.write((char*)&command, sizeof(command));
  client.write((char*)&nbytes, sizeof(nbytes));
  client.write(data, nbytes);
}
