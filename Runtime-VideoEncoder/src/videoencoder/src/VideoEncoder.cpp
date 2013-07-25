#include <roxlu/core/Utils.h>
#include <roxlu/core/Log.h>
#include <videoencoder/VideoEncoder.h>
#include <sstream>

VideoEncoder::VideoEncoder() {

}

VideoEncoder::~VideoEncoder() {
}

bool VideoEncoder::setup(std::string convpath) {
  if(!rx_file_exists(convpath)) {
    RX_ERROR("Cannot find the convert util: %s", convpath.c_str());
    return false;
  }

  conv_util = convpath;
  return true;
}

bool VideoEncoder::encode(VideoEncoderEncodeTask task) {

  if(!conv_util.size()) {
    RX_ERROR("Convert util (avconv) not set; call setup first");
    return false;
  }

  if(!task.dir.size()) {
    RX_ERROR("Wrong directory");
    return false;
  }

  if(!task.filemask.size()) {
    RX_ERROR("Wrong filemask");
    return false;
  }

  if(!task.video_filename.size()) {
    RX_ERROR("No task.video_filename given.");
    return false;
  }


  if(task.dir[task.dir.size()-1] != '/') {
    task.dir.push_back('/');
  }


  std::vector<std::string> files = rx_get_files(task.dir, "jpg");
  if(!files.size()) {
    RX_ERROR("No files found in dir.");
    return false;
  }

#if 0
  for(std::vector<std::string>::iterator it = files.begin(); it != files.end() ; ++it) {
    RX_VERBOSE("%s", (*it).c_str());
  }
#endif

  int num_frames = files.size();

  std::stringstream ss;
  ss << conv_util << " -f image2 -i " << task.dir << task.filemask << " -vcodec h264 -crf 1 -r 24 " << task.dir << task.video_filename;
  std::string cmd = ss.str();
  RX_VERBOSE("Command: %s", cmd.c_str());
  system(cmd.c_str());
  RX_VERBOSE("Lets encode: %s (%ld files)", task.dir.c_str(), files.size());
  return true;
}
