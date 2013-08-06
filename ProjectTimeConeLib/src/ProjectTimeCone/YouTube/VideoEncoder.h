#pragma once

extern "C" {
# include <uv.h>
}

#include <videoencoder/VideoEncoder.h>
#include <youtube/YouTube.h>
#include <string>

namespace ProjectTimeCone {
	namespace YouTube {
		void on_cmd_executed(VideoEncoderEncodeTask task, void* user);         /* gets called when the encoder has encoded the given task */

		class VideoEncoder {
		 public:
		  VideoEncoder();
		  ~VideoEncoder();

		  void update();                                                       /* call this in your update function // event loop */

		  /* make sure to set the correct paths */
		  bool setAudioFile(std::string file, bool datapath);
		  bool setFramesHeadDir(std::string path, bool datapath);              /* set the path to the directory that contains the head frames */
		  bool setFramesTailDir(std::string path, bool datapath);              /* set the path to the directory that contains the tail frames */

		  /* encoding! */
		  bool encodeFrames(std::string path, bool datapath);                   /* perform an encode task */

		 public:
		  VideoEncoderClientIPC enc_client;
		  YouTubeClientIPC yt_client;
		  std::string audio_file_path;                                          /* abs path to the audio file, that is used to create the video */
		  std::string frames_head_path;                                         /* the directory where we can find the frames for the "head" of the video.. intro */
		  std::string frames_tail_path;                                         /* the directory where we can find the frames for the "tail" of the video.. outro */
		  std::string frames_glob;                                              /* *.jpg, *.png etc.. */

		};
	}
}