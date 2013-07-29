#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	gui.init();

	auto deviceList = ofVideoGrabber().listDevices();
	for(auto device : deviceList) {
		auto grabber = new ofVideoGrabber();
		grabber->setDeviceID(device.id);
		grabber->initGrabber(1280, 720, true);
		gui.add(*grabber, device.deviceName);
		devices.push_back(ofPtr<ofVideoGrabber>(grabber));
	}

	if (deviceList.size() == 0) {
		ofSystemAlertDialog("No cameras found )':");
		ofExit();
	}
}

//--------------------------------------------------------------
void testApp::update(){
	for(auto device : devices) {
		device->update();
	}
}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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
