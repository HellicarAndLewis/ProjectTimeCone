#pragma once
/*

  # YouTube::VideoEncoder

  Simple interface for the YouTubeUploader and VideoEncoder services.
  This class should be used to communicate with the Runtime-YouTube and
  Runtime-VideoEncoder applications.

  To setup:
  - Call setAudioFile() - pass in a mp3 audio file (format must be mp3) 
  - Call setFramesHeadDir() - pass in a directory that contains the JPEG frames for the head of the video
  - Call setFramesTailDir() - pass in a directory that contains the JPEG frames for the tail of the video
  - First call setup() with your callback

  To run:
  - Call update() as often as possible; putting it in your event loop would be a good place

  
  - Make sure to include libuv as the very first thing in your main.cpp:

  ````c++
  extern "C" {
  #  include <uv.h>
  }

  ````

*/

extern "C" {
# include <uv.h>
}

#include <videoencoder/VideoEncoder.h>
#include <youtube/YouTube.h>
#include <youtube/YouTubeUpload.h>
#include <string>

namespace ProjectTimeCone {
	namespace YouTube {
		typedef void(*yt_encode_callback)(VideoEncoderEncodeTask task, void* user);      /* callback typedef for events */

		void ytv_on_cmd_executed(VideoEncoderEncodeTask task, void* user);                   /* gets called when the encoder has encoded the given task, at this the avconv command has been executed and it's where you want to remove the raw frames */
		void ytv_on_uploaded(YouTubeVideo video, void* user);                                /* gets called when a video has been upload to youtube, at this point you might remove or move the video to another place */

		class EncodeAndUpload {
    public:
		  EncodeAndUpload();
		  ~EncodeAndUpload();

		  bool setup(yt_encode_callback onEncoded,                                        /* setup; onEncoded will be called when we've encoded the video */
                 youtube_upload_ready_callback onUploaded,
                 void* user);               
  
		  void update();                                                                  /* call this in your update function // event loop */

		  /* make sure to set the correct paths */
		  bool setAudioFile(std::string file, bool datapath);
		  bool setFramesHeadDir(std::string path, bool datapath);                          /* set the path to the directory that contains the head frames */
		  bool setFramesTailDir(std::string path, bool datapath);                          /* set the path to the directory that contains the tail frames */

		  /* encoding! */
		  bool encodeFrames(std::string path, bool datapath);                              /* perform an encode task */

		  /* some helpers */
		  bool removeEncodedVideoFile(YouTubeVideo video);                                 /* remove the encoded video file for the given YouTubeVideo */
		  bool removeInputFrames(VideoEncoderEncodeTask task);                             /* removes the raw frames */

    public:
		  VideoEncoderClientIPC enc_client;
		  YouTubeClientIPC yt_client;
		  std::string audio_file_path;                                                      /* abs path to the audio file, that is used to create the video */
		  std::string frames_head_path;                                                     /* the directory where we can find the frames for the "head" of the video.. intro */
		  std::string frames_tail_path;                                                     /* the directory where we can find the frames for the "tail" of the video.. outro */
		  std::string frames_glob;                                                          /* *.jpg, *.png etc.. */
		  yt_encode_callback cb_encoded;
		  youtube_upload_ready_callback cb_uploaded;
		  void* cb_user;
		};
	}
}
