#pragma once

extern "C" {
# include <uv.h>
};

#include "ofMain.h"
#include <roxlu/core/Log.h>
#include <uv/WorkQueue.h>
#include <videoencoder/VideoEncoder.h>
#include <youtube/YouTube.h>

#define CAM_WIDTH 640
#define CAM_HEIGHT 480
#define ST_NONE 0
#define ST_SAVE_PNG 1
#define ST_CREATE_VIDEO 2
//#define ST_UPLOAD_VIDEO 3

struct PixelData {
  unsigned char* pixels;
  size_t nbytes;
  std::string filepath;
};

void save_frame_worker(void* user);                                    /* writes the pixels to a image file */
void save_frame_ready(void* user);                                     /* gets called when the pixels have been written */
void on_video_encoded(VideoEncoderEncodeTask task, void* user);        /* gets called when the `task` has been completed by the server */

class testApp : public ofBaseApp{
 public:
  testApp();
  void setup();
  void update();
  void draw();
		
  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);
 public:
  ofVideoGrabber grabber;
  int state;
  uint64_t grab_timeout;                 /* when should we grab a new image */
  uint64_t grab_delay;                   /* grab a new png every X-millis */
  int grab_frame_num;                    /* current frame num that is grabbed */
  int grab_max_frames;                   /* total number of frames to grab per session */
  WorkQueue grab_worker;                 /* is used to write the grabbed frames in a separate thread */
  std::string grab_dir;                  /* directory into which we write the video frames */
  VideoEncoderClientIPC enc_client;      /* communicates with the video encoder, this must be running! */
  YouTubeClientIPC yt_client;            /* the youtube client ipc; used to kick off youtube api stuff */
  YouTubeVideo yt_video;
};
