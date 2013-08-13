#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	gui.init();

	auto deviceList = ofVideoGrabber().listDevices();
	int index = 0;
	for(auto deviceItem : deviceList) {
		auto rawDevice = new ofxMachineVision::Device::VideoInputDevice(1280/2, 720/2);
		auto device = ofxMachineVision::DevicePtr(rawDevice);
		auto grabber = new ofxMachineVision::Grabber::Simple(device);
		this->grabbers.push_back(grabber);

		auto panel = this->gui.add(*grabber);

		panel->onMouse += [grabber, deviceItem] (ofxCvGui::MouseArguments& args) {
			if (args.isLocalPressed()) {
				if (!grabber->getIsDeviceOpen()) {
					grabber->open(deviceItem.id);
					grabber->startCapture();
				}
			}
		};
	}
}

//--------------------------------------------------------------
void testApp::update(){

	for(auto grabber : grabbers) {
		if (grabber->getIsDeviceOpen()) {
			grabber->update();
		}
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