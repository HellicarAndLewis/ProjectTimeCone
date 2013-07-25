#include "testApp.h"

void save_frame_ready(void* user) {
  PixelData* pd = static_cast<PixelData*>(user);
  RX_VERBOSE("Ready with grabbing.");
}

void save_frame_worker(void* user) {
  PixelData* pd = static_cast<PixelData*>(user);

  ofImage img;
  img.setUseTexture(false);
  img.setFromPixels(pd->pixels, CAM_WIDTH, CAM_HEIGHT, OF_IMAGE_COLOR);
  img.saveImage(pd->filepath);

  delete[] pd->pixels;
  pd->pixels = NULL;

  RX_VERBOSE("Saved: %s", pd->filepath.c_str());
}

//--------------------------------------------------------------

void on_video_encoded(VideoEncoderEncodeTask task, void* user) {
  RX_VERBOSE("Video file created: %s", task.video_filename.c_str());
  testApp* app = static_cast<testApp*>(user);


#if 0
  app->yt_video.filename = task.dir +task.video_filename;
  app->yt_video.datapath = false;
  app->yt_video.title = "automated";
  app->state = ST_UPLOAD_VIDEO;
#else
  YouTubeVideo video;
  video.filename = task.dir +task.video_filename;
  video.datapath = false;
  video.title = "automated";
  app->yt_client.addVideoToUploadQueue(video);
#endif
  RX_VERBOSE("{ yt_client: %p }", &app->yt_client);

  
}

//--------------------------------------------------------------
testApp::testApp()
  :enc_client("/tmp/encoder.sock", false)
  ,yt_client("/tmp/youtube.sock", false)
{
}

//--------------------------------------------------------------
void testApp::setup(){
  ofSetWindowTitle("YouTube IPC tester");

  grabber.setVerbose(true);
  grabber.setDeviceID(0);
  grabber.setDesiredFrameRate(30);
  grabber.initGrabber(CAM_WIDTH, CAM_HEIGHT);
  
  state = ST_NONE;
  grab_timeout = 0;
  grab_delay = (1.0 / 5.0) * 1000;
  grab_frame_num = 0;
  grab_max_frames = 10;

  enc_client.setup(on_video_encoded, this);

  if(!enc_client.connect()) {
    RX_ERROR("Cannot connect to the video encoder ipc");
    ::exit(EXIT_FAILURE);
  }

  if(!yt_client.connect()) {
    RX_ERROR("Cannot connect to the youtube ipc");
    ::exit(EXIT_FAILURE);
  }
}

//--------------------------------------------------------------
void testApp::update(){
  grabber.update();

  if(state == ST_SAVE_PNG) {
    uint64_t now = (uv_hrtime()/1000000);
    if(now >= grab_timeout) {
      grab_timeout = now + grab_delay;

      char fname[1024];
      sprintf(fname, "frame_%04d.jpg", grab_frame_num);
      RX_VERBOSE("File: %s", fname);

      PixelData* pd = new PixelData;
      pd->nbytes = CAM_WIDTH * CAM_HEIGHT * 3;
      pd->pixels = new unsigned char[pd->nbytes];
      pd->filepath = grab_dir + "/" +fname;

      memcpy(pd->pixels, grabber.getPixels(), pd->nbytes);
      grab_worker.addWorker(save_frame_worker, save_frame_ready, pd);
      grab_frame_num++;

      if(grab_frame_num >= grab_max_frames) {
        state = ST_CREATE_VIDEO;
      }
    }
  }
  else if(state == ST_CREATE_VIDEO) {
    VideoEncoderEncodeTask task;
    task.dir = ofToDataPath(grab_dir, true);
    task.filemask = "frame_%04d.jpg";
    task.video_filename = "output.mov";
    enc_client.encode(task);
    state = ST_NONE;
    grab_frame_num = 0;
  }
  /*
  else if(state == ST_UPLOAD_VIDEO) {
    RX_VERBOSE("OK UPLOADING!");
    yt_client.addVideoToUploadQueue(yt_video);
    state = ST_NONE;
  }
  */
  grab_worker.update();
  enc_client.update();
  yt_client.update();
}

//--------------------------------------------------------------
void testApp::draw(){
  grabber.draw(0, 0, CAM_WIDTH, CAM_HEIGHT);
  
  // no state... default
  if(state == ST_NONE) {
    ofSetHexColor(0xBF2431);
    ofRect(0,0,CAM_WIDTH,20);
    ofSetHexColor(0xFFFFFF);
    ofDrawBitmapString("Current state: 0, just drawing.", 10, 13);
  }
  // grabbing
  else if(state == ST_SAVE_PNG) {
    ofSetHexColor(0x2A4557);
    ofRect(0,0,CAM_WIDTH,20);
    ofSetHexColor(0xFFFFFF);

    std::stringstream ss;
    ss << "Current state: 1, grabbing images (" << grab_frame_num << "/" << grab_max_frames << ")";
    std::string str = ss.str();
    ofDrawBitmapString(str.c_str(), 10, 13);
  }

  // info
  ofSetHexColor(0x37D1BB);
  ofRect(0,460,CAM_WIDTH,20);
  ofSetHexColor(0xFFFFFF);
  ofDrawBitmapString("0 = just draw, 1 = create video", 10, CAM_HEIGHT - 5);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
  if(key == '1') {
    if(state != ST_NONE) {
      RX_ERROR("Cannot start grabber because the current is not 'none'");
    }
    else {
      state = ST_SAVE_PNG;
      grab_timeout = (uv_hrtime()/1000000) + grab_delay;
      grab_dir = rx_strftime("frames/%F-%H-%M-%S");
      if(!rx_create_path(rx_to_data_path(grab_dir))) {
        RX_ERROR("Cannot create destination dir: %s", grab_dir.c_str());
      }
    }
  }
  else if(key == '2') {
    if(state != ST_SAVE_PNG) {
      RX_ERROR("We can only create a video after we've grabbed some frames");
    }
    else {
      state = ST_CREATE_VIDEO;
    }
  }

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
