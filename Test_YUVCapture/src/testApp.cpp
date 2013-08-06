#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	device.setRequestedMediaSubType(VI_MEDIASUBTYPE_MJPG);
	device.setupDevice(0, 1280, 720);

	this->output.allocate(1280, 720, OF_IMAGE_COLOR);
	this->gui.init();
	this->gui.add(output, "Output");
}

//--------------------------------------------------------------
void testApp::update(){
	if (device.isFrameNew(0)) {
		device.getPixels(0, this->output.getPixels(), true, true);
		this->output.update();
	}

}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
