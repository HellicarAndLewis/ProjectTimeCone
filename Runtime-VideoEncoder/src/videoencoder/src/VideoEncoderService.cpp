#include <videoencoder/VideoEncoderService.h>
#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>

VideoEncoderService::VideoEncoderService(std::string sockpath, std::string avconv) 
  :ipc(encoder, sockpath, false)  
  ,avconv(avconv)
  ,sockpath(sockpath)
{
  log.setup("videoencoder","");
  log.writeToConsole(true);
  log.writeToFile(true);
  log.maxi();
}

VideoEncoderService::~VideoEncoderService() {
}

bool VideoEncoderService::setup() {

  if(!rx_file_exists(avconv)) {
    RX_ERROR("Cannot find avconv at: %s", avconv.c_str());
    return false;
  }

  if(!encoder.setup(avconv)) {
    RX_ERROR("Cannot setup the encoder.");
    return false;
  }

  return true;
}

bool VideoEncoderService::start() {

  if(!ipc.start()) {
      RX_ERROR("Cannot start VideoEncoder IPC server");
      return false;
  }

  return true;
}

bool VideoEncoderService::stop() {
  
  if(!ipc.stop()) {
    RX_ERROR("Error while stopping the service");
    return false;
  }

  return true;
}

void VideoEncoderService::update() {
  ipc.update();
}



