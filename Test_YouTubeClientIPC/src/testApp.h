#pragma once

/*

    # Test_YouTubeClientIPC
    
    Quick 'n dirty test application that writes frames from webcam to 
    disc, then encodes them, adds audio (all through ipc, using Runtime-VideoEncoder and
    Runtime-YouTube). The frames are saved in a separate thread; the threaded writer should
    implement the task-paradigm which would make it way cleanier to shutdown the 
    thread. 

    AUTOMATED_UPLOADS:  uncomment if you want to perform automated stress tests
    USE_RAW_IPC: use the somewhat older code for IPC with the video encoder and uploader

 */

extern "C" {
#  include <uv.h>
};

#include "ofMain.h"
#include <roxlu/core/Log.h>
#include <uv/WorkQueue.h>
#include <videoencoder/VideoEncoder.h>
#include <youtube/YouTube.h>
#include <roxlu/io/RingBuffer.h>
//#include "YouTubeVideoEncoder.h"
#include "EncodeAndUpload.h"

#define CAM_WIDTH 1280
#define CAM_HEIGHT 720

#define ST_NONE 0
#define ST_SAVE_PNG 1
#define ST_WAIT_ON_THREAD 2
#define ST_CREATE_VIDEO 3

#define AUTOMATED_UPLOADS 
//#define USE_RAW_IPC     /* if you want to use the VideoEncoderClientIPC and YouTube ipc directly instead of the wrapper YouTubeVideoEncoder */

//using namespace ProjectTimeCone::YouTube;

struct PixelData {
  size_t nbytes;
  std::string filepath;
  size_t offset;
  RingBuffer* buffer;
};

void encoder_thread_callback(void* user);

class EncoderThread {                                                    /* quick 'n dirty threaded writer */
 public:
  EncoderThread(); 
  ~EncoderThread();
  void start();
  void addFrame(PixelData* pd);
  void stop();
  void lock() { uv_mutex_lock(&mutex); }
  void unlock() { uv_mutex_unlock(&mutex); }
  bool stillWorking() { return still_working; } 
  void join() {   uv_thread_join(&thread_id); } 
  int getNumFramesInQueue() { lock(); int i = num_added_frames; unlock(); return i; } 
 public:
  int num_added_frames;
  std::vector<PixelData*> data;
  uv_mutex_t mutex;
  uv_thread_t thread_id;
  volatile bool must_stop;
  volatile bool still_working;
};

void on_video_encoded(VideoEncoderEncodeTask task, void* user);        /* gets called when the `task` has been completed by the server */
void on_audio_added(VideoEncoderEncodeTask task, void* user);          /* gets called when the `task` has been completed */
void on_cmd_executed(VideoEncoderEncodeTask task, void* user);         /* 2nd approach to encode the video; we use a custom command that we send to the VideoEncoderIPCServer; we used this approach to be a bit more flexible with the encoder/avconv command */

class testApp : public ofBaseApp{
 public:
  testApp();
  ~testApp();
  void setup();
  void update();
  void draw();
  void exit();
		
  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);
 private:
  void initiateGrabbing();
 public:
 
  ofVideoGrabber grabber;
  int state;
  uint64_t grab_timeout;                 /* when should we grab a new image */
  uint64_t grab_delay;                   /* grab a new png every X-millis */
  int grab_frame_num;                    /* current frame num that is grabbed */
  int grab_max_frames;                   /* total number of frames to grab per session */
  std::string grab_dir;                  /* directory into which we write the video frames */
#if defined(USE_RAW_IPC)
  VideoEncoderClientIPC enc_client;      /* communicates with the video encoder, this must be running! */
  YouTubeClientIPC yt_client;            /* the youtube client ipc; used to kick off youtube api stuff */
#endif
  YouTubeVideo yt_video;
  RingBuffer pixel_buffer;               /* we preallocate some frames to speed things up */
  EncoderThread encoder_thread;          /* quick 'n dirty threaded image write; blocks when we need to stop and there are still iamges in the queue */

  //  ProjectTimeCone::YouTube::YouTubeVideoEncoder yt;
  ProjectTimeCone::YouTube::EncodeAndUpload  yt;

#if defined(AUTOMATED_UPLOADS)
  uint64_t automated_timeout;            /* when we should start another grab process */
  uint64_t automated_delay;              /* duration between grab processes in millis */
#endif  
};
