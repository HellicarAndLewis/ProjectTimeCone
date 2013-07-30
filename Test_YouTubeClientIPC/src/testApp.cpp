#include "testApp.h"

void encoder_thread_callback(void* user) {

  EncoderThread* et = static_cast<EncoderThread*>(user);
  std::vector<PixelData*> work_data;

  while(true) {

    et->lock();
      std::copy(et->data.begin(), et->data.end(), std::back_inserter(work_data));
      et->data.clear();
    et->unlock();

    for(std::vector<PixelData*>::iterator it = work_data.begin(); it != work_data.end(); ++it) {

      PixelData* pd = *it;
      ofImage img;
      img.setUseTexture(false);
      img.setFromPixels((unsigned char*)pd->buffer->getReadPtr(), CAM_WIDTH, CAM_HEIGHT, OF_IMAGE_COLOR);
      img.saveImage(pd->filepath);
      pd->buffer->drain(pd->nbytes);

      RX_VERBOSE("Saved: %s", pd->filepath.c_str());
      delete pd;
      pd = NULL;

    }
    work_data.clear();

    // only stop when all files have been created.
    if(et->must_stop) {
      et->lock();
      if(!et->data.size()) {
        et->unlock();
        break;
      }
      et->unlock();
    }
  }
}

EncoderThread::EncoderThread()
  :must_stop(false)
{
  uv_mutex_init(&mutex);
}

EncoderThread::~EncoderThread() {

  stop();

  for(std::vector<PixelData*>::iterator it = data.begin(); it != data.end(); ++it) {
    delete *it;
  }

  data.clear();

  uv_mutex_destroy(&mutex);
}

void EncoderThread::start() {

  for(std::vector<PixelData*>::iterator it = data.begin(); it != data.end(); ++it) {
    delete *it;
  }

  data.clear();

  must_stop = false;
  uv_thread_create(&thread_id, encoder_thread_callback, this);
}

void EncoderThread::stop() {
  must_stop = true;
  uv_thread_join(&thread_id);
}

void EncoderThread::addFrame(PixelData* pd) {
  lock();
    data.push_back(pd);
  unlock();
}


//--------------------------------------------------------------

void on_video_encoded(VideoEncoderEncodeTask task, void* user) {
  RX_VERBOSE("Video file created: %s", task.video_filename.c_str());

  task.video_filepath = task.dir +"/" +task.video_filename;
  task.audio_filepath = rx_to_data_path("audio.mp3");
  task.video_filename = "output2.mov";

  testApp* app = static_cast<testApp*>(user);
  app->enc_client.addAudio(task);
}

void on_audio_added(VideoEncoderEncodeTask task, void* user) {
  RX_VERBOSE("Audio added");

  YouTubeVideo video;
  video.filename = task.dir +"/" +task.video_filename;
  video.datapath = false;
  video.title = "automated";

  testApp* app = static_cast<testApp*>(user);
  app->yt_client.addVideoToUploadQueue(video);
}

//--------------------------------------------------------------
testApp::testApp()
#if defined(__APPLE__)
  :enc_client("/tmp/encoder.sock", false)
  ,yt_client("/tmp/youtube.sock", false)
#elif defined(WIN32) 
   :enc_client("\\\\.\\pipe\\encoder", false)
  ,yt_client("\\\\.\\pipe\\youtube", false)
#endif   
  ,pixel_buffer(CAM_WIDTH * CAM_HEIGHT * 3 * 400) // 400 frames 
{
}

//--------------------------------------------------------------
void testApp::setup(){
  ofSetWindowTitle("YouTube IPC tester");

  grabber.setVerbose(true);
  grabber.setDesiredFrameRate(30);
  grabber.setDeviceID(0);

  if(!grabber.initGrabber(CAM_WIDTH, CAM_HEIGHT)) {
    grabber.setDeviceID(1);
    if(!grabber.initGrabber(CAM_WIDTH, CAM_HEIGHT)) {
      RX_ERROR("Cannot find a camera");
      ::exit(EXIT_FAILURE);
    }
  }
  
  state = ST_NONE;
  grab_timeout = 0;
  grab_delay = (1.0 / 25.0) * 1000;
  grab_frame_num = 0;
  grab_max_frames = 10;

  enc_client.cb_user = this;
  enc_client.cb_encoded = on_video_encoded;
  enc_client.cb_audio_added = on_audio_added;

  if(!enc_client.connect()) {
    RX_ERROR("Cannot connect to the video encoder ipc, make sure that you start it; we will try to connect later");
  }

  if(!yt_client.connect()) {
    RX_ERROR("Cannot connect to the youtube ipc, make sure that you start it; we will try to connect later");
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

      PixelData* pd = new PixelData;
      pd->nbytes = CAM_WIDTH * CAM_HEIGHT * 3;
      pd->buffer = &pixel_buffer;
      pd->offset = pixel_buffer.getWriteIndex();
      pd->filepath = grab_dir + "/" +fname;

      pixel_buffer.write((const char*)grabber.getPixels(), pd->nbytes);

      encoder_thread.addFrame(pd);

      grab_frame_num++;

      if(grab_frame_num >= grab_max_frames) {
        state = ST_CREATE_VIDEO;
      }
    }
  }
  else if(state == ST_CREATE_VIDEO) {

    RX_VERBOSE("Waiting a bit till the encoder thread has written all files.");
    encoder_thread.stop();

    RX_VERBOSE("Encode.");
    VideoEncoderEncodeTask task;
    task.dir = ofToDataPath(grab_dir, true);
    task.filemask = "frame_%04d.jpg";
    task.video_filename = "output.mp4";
    enc_client.encode(task);
    state = ST_NONE;
    grab_frame_num = 0;
  }

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
    std::stringstream ss;
    ss << "Current state: 0, just drawing. (max frames: " << grab_max_frames << ")";
    ofDrawBitmapString(ss.str(), 10, 13);
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
  ofRect(0,CAM_HEIGHT-20,CAM_WIDTH,20);
  ofSetHexColor(0xFFFFFF);
  ofDrawBitmapString("0 = just draw, 1 = create video, + = more frames, - = less frames", 10, CAM_HEIGHT - 5);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
  // Start grabbing frames
  if(key == '1') {
    if(state != ST_NONE) {
      RX_ERROR("Cannot start grabber because the current is not 'none'");
    }
    else {
      state = ST_SAVE_PNG;
      grab_timeout = (uv_hrtime()/1000000) + grab_delay;
      grab_dir = "frames/" +rx_get_date_time_string(); // rx_strftime("frames/%F-%H-%M-%S");
      if(!rx_create_path(rx_to_data_path(grab_dir))) {
        RX_ERROR("Cannot create destination dir: %s", grab_dir.c_str());
      }

      encoder_thread.start();
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
  else if(key == '+' || key == '=') {
    grab_max_frames++;
    if(grab_max_frames > 400) {
      grab_max_frames = 400;
    }
  }
  else if(key == '-' || key == '_') {
    if(grab_max_frames > 1) {
      grab_max_frames--;
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
