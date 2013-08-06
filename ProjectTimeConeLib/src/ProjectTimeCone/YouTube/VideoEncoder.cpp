#include "VideoEncoder.h"
#include <roxlu/core/Log.h>
#include <roxlu/core/Utils.h>
#include <sstream>

namespace ProjectTimeCone {
	namespace YouTube {
		// --------------------------------------------------------------

		// when the encoding is ready this function gets called; next thing is adding it to the upload queue
		void ytv_on_cmd_executed(VideoEncoderEncodeTask task, void* user) {
		  VideoEncoder* ytv = static_cast<VideoEncoder*>(user);

		  YouTubeVideo video;
		  video.filename = task.video_filename;
		  video.datapath = false;
		  video.title = rx_get_date_time_string();
  
		  ytv->yt_client.addVideoToUploadQueue(video);

		  if(ytv->cb_encoded) {
			ytv->cb_encoded(task, ytv->cb_user);
		  }
		}

		// when a video (doesnt have to be the last one) has been uploaded this function is called
		void ytv_on_uploaded(YouTubeVideo video, void* user) {
		  VideoEncoder* ytv = static_cast<VideoEncoder*>(user);
		  if(ytv->cb_uploaded) {
			ytv->cb_uploaded(video, ytv->cb_user);
		  }
		}

		// --------------------------------------------------------------

		VideoEncoder::VideoEncoder() 

		#if defined(__APPLE__)
		  :enc_client("/tmp/encoder.sock", false)
		  ,yt_client("/tmp/youtube.sock", false)
		#elif defined(WIN32) 
		   :enc_client("\\\\.\\pipe\\encoder", false)
		  ,yt_client("\\\\.\\pipe\\youtube", false)
		#endif   
		  ,frames_glob("*.jpg")
		  ,cb_user(NULL)
		  ,cb_encoded(NULL)
		  ,cb_uploaded(NULL)
		{
		  RX_VERBOSE("YouTubeVideoEncoder");

		  enc_client.cb_user = this;
		  enc_client.cb_cmd_executed = ytv_on_cmd_executed;

		  if(!enc_client.connect()) {
			RX_ERROR("Something went wrong while trying to connect to the video encoder server; is it running?");
		  }

		  if(!yt_client.connect()) {
			RX_ERROR("Something went wrong while trying to connect to the youtube uploader; is it running?");
		  }

		  yt_client.setUploadReadyCallback(ytv_on_uploaded, this);

		}

		VideoEncoder::~VideoEncoder() 
		{
		  cb_user = NULL;
		  cb_encoded = NULL;
		  cb_uploaded = NULL;
		  frames_glob = "";
		  audio_file_path = "";
		  frames_head_path = "";
		  frames_tail_path = "";
		}

		bool VideoEncoder::setup(yt_encode_callback onEncoded, 
										youtube_upload_ready_callback onUploaded,
										void* user) 
		{
		  cb_user = user;
		  cb_encoded = onEncoded;
		  cb_uploaded = onUploaded;
		  return true;
		}

		bool VideoEncoder::setAudioFile(std::string filename, bool datapath) {

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

		bool VideoEncoder::setFramesHeadDir(std::string dir, bool datapath) {

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


		bool VideoEncoder::setFramesTailDir(std::string dir, bool datapath) {

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

		bool VideoEncoder::encodeFrames(std::string path, bool datapath) {

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

			//  rx_put_file_contents(rx_get_date_time_string() +"_exec.bat", ss.str(), true);
		#endif
  
		  VideoEncoderEncodeTask task;
		  task.cmd = ss.str();
		  task.dir = body_path;
		  task.video_filename = out_file;

		  enc_client.customCommand(task);

		  return true;
		}

		void VideoEncoder::update() {
		  enc_client.update();
		  yt_client.update();
		}

		bool VideoEncoder::removeEncodedVideoFile(YouTubeVideo video) {

		  if(!rx_file_exists(video.filename)) {
			return false;
		  }

		  rx_file_remove(video.filename);
		  return true;
		}

		bool VideoEncoder::removeInputFrames(VideoEncoderEncodeTask task) {
		  std::vector<std::string> files = rx_get_files(task.dir, "jpg");
		  if(!files.size()) {
			RX_ERROR("Cannot find the raw input frames int the dir: '%s', check the file extension", task.dir.c_str());
			return false;
		  }

		  for(std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
			std::string f = *it;
			if(!rx_file_exists(f)) {
			  RX_ERROR("File removed already (!?) - '%s'", f.c_str());
			}
			else {
			  rx_file_remove(f);
			}
		  }

		  return true;
		}
	}
}