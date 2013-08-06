#include "testApp.h"
#include <roxlu/core/Log.h>


//--------------------------------------------------------------

void on_video_encoded(VideoEncoderEncodeTask task, void *user) {
  testApp* app = static_cast<testApp*>(user);
  app->yt.removeInputFrames(task);
}

void on_video_uploaded(YouTubeVideo video, void* user) {
  testApp* app = static_cast<testApp*>(user);
  app->yt.removeEncodedVideoFile(video);
}

//--------------------------------------------------------------
void testApp::setup(){

  if(!yt.setAudioFile("audio.mp3", true)) {
    // handle ... check the audio file..
  }
  if(!yt.setFramesHeadDir("png/frames_head", true)) {
    // handle ... create the frames head dir
  }
  if(!yt.setFramesTailDir("png/frames_tail", true)) {
    // handle ... create the frames tail dir
  }

  if(!yt.setup(on_video_encoded, on_video_uploaded, this)) {
    // handle setup error .... 
  }
}

//--------------------------------------------------------------
void testApp::update(){
  // call this often :) 
  yt.update();
}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
  if(key == ' ') {
    yt.encodeFrames("png/frames_recording", true);
  }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

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
