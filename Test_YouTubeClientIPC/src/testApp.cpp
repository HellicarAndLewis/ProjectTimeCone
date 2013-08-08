#include "testApp.h"

//--------------------------------------------------------------
#if !defined(USE_RAW_IPC)
//namespace ProjectTimeCone {
//  namespace YouTube {

static void yt_on_video_encoded(VideoEncoderEncodeTask task, void *user) {
  testApp* app = static_cast<testApp*>(user);
  app->yt.removeInputFrames(task);
}

static void yt_on_video_uploaded(YouTubeVideo video, void* user) {
  testApp* app = static_cast<testApp*>(user);
  app->yt.removeEncodedVideoFile(video);
}

//  }} // namespaces
#endif

//--------------------------------------------------------------

void encoder_thread_callback(void* user) {

  EncoderThread* et = static_cast<EncoderThread*>(user);
  std::vector<PixelData*> work_data;

  et->still_working = true;

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

      et->lock();
        et->num_added_frames--;
      et->unlock();

    }

    work_data.clear();

    // only stop when all files have been created.
    if(et->must_stop) {
      et->lock();
      if(!et->data.size()) {
        et->unlock();
        et->still_working = false;
        break;
      }
      et->unlock();
    }
  }
}

EncoderThread::EncoderThread()
  :must_stop(false)
  ,still_working(false)
  ,num_added_frames(0)
{
  uv_mutex_init(&mutex);
}

EncoderThread::~EncoderThread() {

  for(std::vector<PixelData*>::iterator it = data.begin(); it != data.end(); ++it) {
    delete *it;
  }

  data.clear();

  uv_mutex_destroy(&mutex);
  still_working = false;
  must_stop = true;
  num_added_frames = 0;
}

void EncoderThread::start() {

  for(std::vector<PixelData*>::iterator it = data.begin(); it != data.end(); ++it) {
    delete *it;
  }

  data.clear();
  num_added_frames = 0;
  must_stop = false;
  uv_thread_create(&thread_id, encoder_thread_callback, this);
}

void EncoderThread::stop() {
  must_stop = true;
}

void EncoderThread::addFrame(PixelData* pd) {
  lock();
    data.push_back(pd);
    num_added_frames++;
  unlock();
}


//--------------------------------------------------------------

void on_video_encoded(VideoEncoderEncodeTask task, void* user) {
  RX_VERBOSE("Video file created: %s", task.video_filename.c_str());

  task.video_filepath = task.dir +"/" +task.video_filename;
  task.audio_filepath = rx_to_data_path("audio.mp3");
  task.video_filename = "output2.mov";
#if USE_RAW_IPC
  testApp* app = static_cast<testApp*>(user);
  app->enc_client.addAudio(task);
#endif
}

void on_audio_added(VideoEncoderEncodeTask task, void* user) {
  RX_VERBOSE("Audio added");
  
  // remove files once the audio has been added
  std::vector<std::string> files = rx_get_files(task.dir, "jpg");
  if(!files.size()){
    RX_ERROR("Cannot find files after encoding (!)");
  }
  else {
    for(std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it) {
      std::string f = *it;
      if(!rx_file_exists(f)) {
        RX_ERROR("File removed(!?): %s", f.c_str());
      }
      else {
        rx_file_remove(f);
        RX_WARNING("Removed: %s", f.c_str());
      }
    
    }
  }
  task.print();


  YouTubeVideo video;
  video.filename = task.dir +"/" +task.video_filename;
  video.datapath = false;
  video.title = rx_get_date_time_string();
#if USE_RAW_IPC
  testApp* app = static_cast<testApp*>(user);
  app->yt_client.addVideoToUploadQueue(video);
#endif
}

void on_cmd_executed(VideoEncoderEncodeTask task, void* user) {
  RX_VERBOSE("COMMAND EXECUTED!");
  task.print();
  YouTubeVideo video;
  video.filename = task.dir +"/" +task.video_filename;
  video.datapath = false;
  video.title = rx_get_date_time_string();
  
#if USE_RAW_IPC
  testApp* app = static_cast<testApp*>(user);
  app->yt_client.addVideoToUploadQueue(video);
#endif
}

//--------------------------------------------------------------
testApp::testApp()
#if USE_RAW_IPC
#  if defined(__APPLE__)
    :enc_client("/tmp/encoder.sock", false)
    ,yt_client("/tmp/youtube.sock", false)
#  elif defined(WIN32) 
     :enc_client("\\\\.\\pipe\\encoder", false)
    ,yt_client("\\\\.\\pipe\\youtube", false)
#  endif   
  ,pixel_buffer(CAM_WIDTH * CAM_HEIGHT * 3 * 400) // 400 frames 
#else
   :pixel_buffer(CAM_WIDTH * CAM_HEIGHT * 3 * 400) // 400 frames 
#endif
{
}

testApp::~testApp() {
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

#if USE_RAW_IPC
  enc_client.cb_user = this;
  enc_client.cb_encoded = on_video_encoded;
  enc_client.cb_audio_added = on_audio_added;
  enc_client.cb_cmd_executed = on_cmd_executed;

  if(!enc_client.connect()) {
    RX_ERROR("Cannot connect to the video encoder ipc, make sure that you start it; we will try to connect later");
  }

  if(!yt_client.connect()) {
    RX_ERROR("Cannot connect to the youtube ipc, make sure that you start it; we will try to connect later");
  }
#else 
  if(!yt.setAudioFile("audio.mp3", true)) {
    RX_ERROR("Couldn't find the audio.mp3");
  }
  if(!yt.setFramesHeadDir("red", true)) {
    RX_ERROR("Couldn't find the test directory with the head frames");
  }
  if(!yt.setFramesTailDir("green", true)) {
    RX_ERROR("Couldn't find the test directory with the tail frames");
  }

  if(!yt.setup(yt_on_video_encoded, yt_on_video_uploaded, this)) {
    // handle setup error .... 
  }
#endif

#if defined(AUTOMATED_UPLOADS)
  automated_delay = 60 * 1000;  // a new video every minute
  automated_timeout = rx_millis();
#endif
}

//--------------------------------------------------------------
void testApp::update(){
#if defined(AUTOMATED_UPLOADS)
  {
    uint64_t now = rx_millis();
    if(automated_timeout && now > automated_timeout) {
      
      if(state == ST_NONE && !encoder_thread.getNumFramesInQueue()) { 
        automated_timeout = now + automated_delay;
        grab_max_frames = rx_random(40, 60);
        initiateGrabbing();
        RX_VERBOSE("START AUTOMATED SEQ");
      }
    }
  }
#endif 

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
        state = ST_WAIT_ON_THREAD;
        encoder_thread.stop();
      }

    }
  }
  else if(state == ST_WAIT_ON_THREAD) {
    if(encoder_thread.getNumFramesInQueue() == 0) {
      state = ST_CREATE_VIDEO;
    }
  }
  else if(state == ST_CREATE_VIDEO) {

    RX_VERBOSE("Waiting a bit till the encoder thread has written all files.");
   
    encoder_thread.join();

#if 0
    // use default command
    VideoEncoderEncodeTask task;
    task.dir = ofToDataPath(grab_dir, true);
    task.filemask = "frame_%04d.jpg";
    task.video_filename = "output.mp4";
    enc_client.encode(task);
#else

#  if USE_RAW_IPC
    // use custom command
    std::stringstream ss;
    std::string head_frames = rx_norm_path(rx_to_data_path("red/*.jpg"));
    std::string tail_frames = rx_norm_path(rx_to_data_path("green/*.jpg"));
    std::string body_frames = rx_norm_path(rx_to_data_path(grab_dir) +"/*.jpg");
    std::string out_file = rx_norm_path(rx_to_data_path(grab_dir) +"/out.mov");
    std::string audio_file = rx_norm_path(rx_to_data_path("audio.mp3"));

#   if !defined(WIN32)
    ss << "cat " << head_frames  << " " 
       << body_frames << " "
       << tail_frames << " "
       << " | %s -y -v debug -f image2pipe -vcodec mjpeg -i - -i " << audio_file << " -acodec libmp3lame -qscale 20 -shortest -r 25 -map 0 -map 1 " << out_file;
#   else   
    ss << "type " << head_frames  << " " 
       << body_frames << " "
       << tail_frames << " "
       << " | %s -y -v debug -f image2pipe -vcodec mjpeg -i - -i " << audio_file << " -acodec libmp3lame -qscale 20 -shortest -r 25 " << out_file;

    rx_put_file_contents(rx_get_date_time_string() +"_exec.bat", ss.str(), true);
#   endif

    
    RX_VERBOSE("%s\n%s\n%s\n", head_frames.c_str(), tail_frames.c_str(), body_frames.c_str());
    // cat frames/*.jpg frames_a/*.jpg | ./avconv -y -v debug -f image2pipe -vcodec mjpeg -i - -r 25 -map 0 out.mov

    VideoEncoderEncodeTask task;
    task.cmd = ss.str();
    task.dir = ofToDataPath(grab_dir, true);
    task.filemask = "frame_%04d.jpg";
    task.video_filename = "out.mov";

    enc_client.customCommand(task);



    state = ST_NONE;
    grab_frame_num = 0;
  }
  enc_client.update();
  yt_client.update();
# else // USE_RAW_IPC
     yt.encodeFrames(grab_dir, true);
     state = ST_NONE;
     grab_frame_num = 0;
  } // st = ST_CREATE_VIDEO

  yt.update();

# endif
#endif 
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
  // waiting on thread to finish
  else if(state == ST_WAIT_ON_THREAD) {
    ofSetHexColor(0xFABD4A);
    ofRect(0,0,CAM_WIDTH,20);
    ofSetHexColor(0xFFFFFF);

    std::stringstream ss;
    ss << "Waiting on thread to finish writing files: " << encoder_thread.getNumFramesInQueue();
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
#if !defined(AUTOMATED_UPLOADS)
  // Start grabbing frames
  if(key == '1') {
    if(state != ST_NONE) {
      RX_ERROR("Cannot start grabber because the current is not 'none'");
    }
    else {
      initiateGrabbing();
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
#else
  RX_VERBOSE("Not handling key input when in automated mode");
#endif
}

void testApp::exit() {
  encoder_thread.stop();
  encoder_thread.join();
}

void testApp::initiateGrabbing() {

  if(encoder_thread.stillWorking()) {
    RX_VERBOSE("Cannot start a new grabbing sequence because the previous one isn't ready yet.");
    return;
  }

  state = ST_SAVE_PNG;
  grab_timeout = (uv_hrtime()/1000000) + grab_delay;
  grab_dir = "frames/" +rx_get_date_time_string(); // rx_strftime("frames/%F-%H-%M-%S");
  if(!rx_create_path(rx_to_data_path(grab_dir))) {
    RX_ERROR("Cannot create destination dir: %s", grab_dir.c_str());
  }

  encoder_thread.start();
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
