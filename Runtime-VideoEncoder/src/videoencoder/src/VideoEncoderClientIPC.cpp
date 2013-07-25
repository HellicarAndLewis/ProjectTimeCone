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
      msgpack::unpacked unp;
      msgpack::unpack(&unp, &ipc->buffer[offset], nbytes);

      if(cmd == VE_CMD_ENCODED) {
        VideoEncoderEncodeTask task;

        msgpack::object obj = unp.get();
        try {
          obj.convert(&task);
          task.print();
          ipc->buffer.erase(ipc->buffer.begin(), ipc->buffer.begin() + offset + nbytes);
        }
        catch(std::exception& ex) {
          RX_ERROR("Error while trying to decode: %s", ex.what());
          ipc->buffer.clear(); // @todo - test if this is correct; we're basically destroying the buffer; but we shouldn't arrive here
          break;
        }

        // notify caller
        if(client->cb_encoded) {
          RX_VERBOSE("{ %p }", client->cb_user);
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
  RX_VERBOSE("[ %p ]", cb_user);
}

bool VideoEncoderClientIPC::connect() {
  return client.connect();
}

void VideoEncoderClientIPC::update() {
  client.update();
}

void VideoEncoderClientIPC::encode(VideoEncoderEncodeTask task) {
  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, task);
  writeCommand(VE_CMD_ENCODE, sbuf.data(), sbuf.size());
}

void VideoEncoderClientIPC::writeCommand(uint32_t command, char* data, uint32_t nbytes) {
  client.write((char*)&command, sizeof(command));
  client.write((char*)&nbytes, sizeof(nbytes));
  client.write(data, nbytes);
}
