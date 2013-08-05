#include "testApp.h"
#include <roxlu/core/Log.h>

//--------------------------------------------------------------
void testApp::setup(){

  if(!yt.setAudioFile("audio.mp3", true)) {
    // handle ... check the audio file..
  }
  if(!yt.setFramesHeadDir("frames_head", true)) {
    // handle ... create the frames head dir
  }
  if(!yt.setFramesTailDir("frames_tail", true)) {
    // handle ... create the frames tail dir
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
    yt.encodeFrames("frames_recording", true);
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
