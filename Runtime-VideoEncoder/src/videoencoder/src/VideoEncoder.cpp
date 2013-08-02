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

  if(!isSetup()) {
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
  ss << conv_util << " -f image2 -i " << task.dir << task.filemask 
     << " -vcodec h264 "
    //     << " -crf 15 "
     << " -qscale 20 "
     << " -r 25 " 
     << " -y "  // force overwrite
     << task.dir << task.video_filename;
  std::string cmd = ss.str();

  RX_VERBOSE("Command: %s", cmd.c_str());

  system(cmd.c_str());

  RX_VERBOSE("Lets encode: %s (%ld files)", task.dir.c_str(), files.size());
  return true;
}

bool VideoEncoder::addAudio(VideoEncoderEncodeTask task) {

  if(!isSetup()) {
    return false;
  }

  if(!task.audio_filepath.size()) {
    RX_ERROR("task.audio_filepath not set. ");
    return false;
  }

  if(!rx_file_exists(task.audio_filepath)) {
    RX_ERROR("Cannot find the audio file you want to add to the video: `%s`", task.audio_filepath.c_str());
    return false;
  }

  if(!task.video_filepath.size()) {
    RX_ERROR("task.video_filepath not set");
    return false;
  }

  if(!rx_file_exists(task.video_filepath)) {
    RX_ERROR("Cannot add audio to `%s` because we cannot find the file", task.video_filepath.c_str());
    return false;
  }

  std::string output = task.dir +"/" +task.video_filename;

  std::stringstream ss;
  ss << conv_util 
     << " -v debug " 
     << " -i " << task.audio_filepath 
     << " -i " << task.video_filepath 
     << " -acodec libmp3lame "
     << " -qscale 20 "
     << " -shortest "
     << " -y "  // force overwrite
     << output;

  //     << " -strict experimental "
  //     << " -c copy " 
  //     << " -t 00:00:10 "


  std::string cmd = ss.str();

  RX_VERBOSE("Command: %s", cmd.c_str());

  int r = system(cmd.c_str());

  RX_VERBOSE("Added audio to `%s` (%d)", task.video_filepath.c_str(), r);

  return true;
}

bool VideoEncoder::customCommand(VideoEncoderEncodeTask task) {

  if(!isSetup()) {
    return false;
  }

  if(!task.cmd.size()) {
    RX_VERBOSE("No command found.");
    return false;
  }

  size_t needed_len = task.cmd.size() + conv_util.size() + 32; 
  if(needed_len > 1024 * 10) {
    RX_ERROR("The command length exceeds 1024 * 10; a bit too much");
    return false;
  }

  char* cstr = new char[needed_len];
  sprintf(cstr, task.cmd.c_str(), conv_util.c_str());
  
  //  std::stringstream ss;
  //   ss << conv_util << " " << task.cmd;
   //  std::string cmd = ss.str();
  int r = system(cstr);

  RX_VERBOSE("Executed custom command: %s, result: %d", cstr, r);
  delete[] cstr;
  cstr = NULL;

  return true;
}

bool VideoEncoder::isSetup() {

  if(!conv_util.size()) {
    RX_ERROR("Cannot find the avconv util; did you call setup()?");
    return false;
  }

  if(!rx_file_exists(conv_util)) {
    RX_ERROR("Cannot find the avconv util; file doesn't exist");
    return false;
  }

  return true;
}
