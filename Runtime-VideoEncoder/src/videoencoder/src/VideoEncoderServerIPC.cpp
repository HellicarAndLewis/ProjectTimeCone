#include <roxlu/io/Buffer.h>
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
      Buffer buf;
      buf.putBytes(&con->buffer[offset], nbytes);

      if(cmd == VE_CMD_ENCODE) {
        VideoEncoderEncodeTask task;
        buf >> task.dir >> task.filemask >> task.video_filename;
        task.print();
        con->buffer.erase(con->buffer.begin(), con->buffer.begin() + offset + nbytes);

        // encode the video + notify the caller on sucess
        if(server->enc.encode(task)) {
          Buffer out;
          out << task.dir << task.filemask << task.video_filename;
          server->writeCommand(con, VE_CMD_ENCODED, out.ptr(), out.size());
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
