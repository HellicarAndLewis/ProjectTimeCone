#include <videoencoder/VideoEncoder.h>
#include <videoencoder/VideoEncoderServerIPC.h>

void videoencoder_server_ipc_on_read(ConnectionIPC* con, void* user) {
  int cmd;
  uint32_t nbytes;
  VideoEncoderServerIPC* server = static_cast<VideoEncoderServerIPC*>(user);
  size_t offset = sizeof(cmd) + sizeof(nbytes);

  while(con->buffer.size() > offset) {
    
    memcpy((char*)&cmd, &con->buffer[0], sizeof(cmd));
    memcpy((char*)&nbytes, &con->buffer[sizeof(cmd)], sizeof(nbytes));

    if(con->buffer.size() - offset >= nbytes) {
      msgpack::unpacked unp;
      msgpack::unpack(&unp, &con->buffer[offset], nbytes);

      if(cmd == VE_CMD_ENCODE) {
        VideoEncoderEncodeTask task;

        msgpack::object obj = unp.get();
        try {
          obj.convert(&task);
          task.print();
          con->buffer.erase(con->buffer.begin(), con->buffer.begin() + offset + nbytes);
        }
        catch(std::exception& ex) {
          RX_ERROR("Error while trying to decode: %s", ex.what());
          con->buffer.clear(); // @todo - test if this is correct; we're basically destroying the buffer; but we shouldn't arrive here
          break;
        }

        // encode the video + notify the caller on sucess
        if(server->enc.encode(task)) {
          msgpack::sbuffer sbuf;
          msgpack::pack(sbuf, task);
          server->writeCommand(con, VE_CMD_ENCODED, sbuf.data(), sbuf.size());
        }

      }
      else {
        RX_ERROR("Unhandled command: %ld, flushing the buffer!", cmd);
        con->buffer.clear();
      }
    }
    else {
      break;
    }
  }
}

VideoEncoderServerIPC::VideoEncoderServerIPC(VideoEncoder& enc, std::string sockfile, bool datapath)
  :enc(enc)
  ,server(sockfile, datapath)
{
  server.setup(videoencoder_server_ipc_on_read, this);
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

void VideoEncoderServerIPC::writeCommand(ConnectionIPC* ipc, uint32_t command, char* data, uint32_t nbytes) {
  ipc->write((char*)&command, sizeof(command));
  ipc->write((char*)&nbytes, sizeof(nbytes));
  ipc->write((char*)data, nbytes);
}
