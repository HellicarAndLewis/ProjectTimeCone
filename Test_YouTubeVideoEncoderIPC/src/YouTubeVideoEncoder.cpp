#include "YouTubeVideoEncoder.h"
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <sstream>

// --------------------------------------------------------------

// when the encoding is ready this function gets called; next thing is adding it to the upload queue
void on_cmd_executed(VideoEncoderEncodeTask task, void* user) {
  YouTubeVideoEncoder* ytv = static_cast<YouTubeVideoEncoder*>(user);

  YouTubeVideo video;
  video.filename = task.video_filename;
  video.datapath = false;
  video.title = rx_get_date_time_string();
  
  ytv->yt_client.addVideoToUploadQueue(video);

  // @todo -> add callback
}


// --------------------------------------------------------------

YouTubeVideoEncoder::YouTubeVideoEncoder() 

#if defined(__APPLE__)
  :enc_client("/tmp/encoder.sock", false)
  ,yt_client("/tmp/youtube.sock", false)
#elif defined(WIN32) 
   :enc_client("\\\\.\\pipe\\encoder", false)
  ,yt_client("\\\\.\\pipe\\youtube", false)
#endif   
  ,frames_glob("*.jpg")

{
  RX_VERBOSE("YouTubeVideoEncoder");

  enc_client.cb_user = this;
  enc_client.cb_cmd_executed = on_cmd_executed;

  if(!enc_client.connect()) {
    RX_ERROR("Something went wrong while trying to connect to the video encoder server; is it running?");
  }

  if(!yt_client.connect()) {
    RX_ERROR("Something went wrong while trying to connect to the youtube uploader; is it running?");
  }

}

YouTubeVideoEncoder::~YouTubeVideoEncoder() {
}

bool YouTubeVideoEncoder::setAudioFile(std::string filename, bool datapath) {

  audio_file_path = filename;

  if(datapath) {
    audio_file_path = rx_to_data_path(filename);
  }

  if(!rx_file_exists(audio_file_path)) {
    RX_ERROR("Cannot find the audio file: %s", audio_file_path.c_str());
    return false;
  }

  return true;
}

bool YouTubeVideoEncoder::setFramesHeadDir(std::string dir, bool datapath) {

  frames_head_path = dir;

  if(datapath) {
    frames_head_path = rx_to_data_path(dir);
  }

  if(!rx_is_dir(frames_head_path)) {
    RX_ERROR("Cannot find frames head path");
    return false;
  }

  return true;
}


bool YouTubeVideoEncoder::setFramesTailDir(std::string dir, bool datapath) {

  frames_tail_path = dir;

  if(datapath) {
    frames_tail_path = rx_to_data_path(dir);
  }

  if(!rx_is_dir(frames_tail_path)) {
    RX_ERROR("Cannot find frames tail path");
    return false;
  }

  return true;
}

bool YouTubeVideoEncoder::encodeFrames(std::string path, bool datapath) {

  if(!audio_file_path.size() || !frames_head_path.size() || !frames_tail_path.size()) {
    RX_ERROR("Not setup correctly, call setAudioFile, setFramesHeadDir, setFramesTailDir");
    return false;
  }

  std::string body_path = path;
  if(datapath) {
    body_path =  rx_to_data_path(path);
  }

  if(!rx_is_dir(body_path)) {
    RX_ERROR("Cannot find the directory with the frames that we need to encode: %s", body_path.c_str());
    return false;
  }

  // name of the output file
  std::string vfile = "out.mov";

  // use custom command
  std::stringstream ss;
  std::string head_frames = rx_norm_path(frames_head_path +"/" +frames_glob);
  std::string tail_frames = rx_norm_path(frames_tail_path +"/" +frames_glob);
  std::string body_frames = rx_norm_path(body_path +"/" +frames_glob);
  std::string out_file = rx_norm_path(body_path +"/" +vfile);
  std::string audio_file = rx_norm_path(audio_file_path);

#if !defined(WIN32)
  ss << "cat " << head_frames  << " " 
     << body_frames << " "
     << tail_frames << " "
     << " | %s -y -v debug -f image2pipe -vcodec mjpeg -i - -i " << audio_file << " -acodec libmp3lame -qscale 20 -shortest -r 25 -map 0 -map 1 " << out_file;
#else   
  // -map 0, -map 1 doesnt work on windows, use `type` instead of `cat`
  ss << "type " << head_frames  << " " 
     << body_frames << " "
     << tail_frames << " "
     << " | %s -y -v debug -f image2pipe -vcodec mjpeg -i - -i " << audio_file << " -acodec libmp3lame -qscale 20 -shortest -r 25 " << out_file;

  rx_put_file_contents(rx_get_date_time_string() +"_exec.bat", ss.str(), true);
#endif
  

  VideoEncoderEncodeTask task;
  task.cmd = ss.str();
  task.dir = path;
  task.video_filename = out_file;

  enc_client.customCommand(task);

  return true;
}

void YouTubeVideoEncoder::update() {
  enc_client.update();
  yt_client.update();
}
