#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->exposure = 500;
	this->gain = 5;
	this->focus = 0;
	this->updatePreviews;

	gui.init();

	vector<int> order;
	try {
		ofFile load("save.bin", ofFile::ReadOnly, true);
		int count = load.getPocoFile().getSize() / sizeof(int);
		order.resize(count);
		load.read((char*) &order[0], sizeof(int) * count);
		load.close();
	} catch(...) {
		ofSystemAlertDialog("Couldn't load camera order, please place save.bin into data folder.");
		ofExit();
	}

	auto deviceList = ofVideoGrabber().listDevices();
	if (deviceList.size() != order.size()) {
		ofSystemAlertDialog("We've saved the order for a different number of cameras");
		ofExit();
	}

	vector<ofxMachineVision::Grabber::Simple*> toAdd;
	for(auto deviceItem : deviceList) {
		auto rawDevice = new ofxMachineVision::Device::VideoInputDevice(1280, 720);
		auto device = ofxMachineVision::DevicePtr(rawDevice);
		auto grabber = new ofxMachineVision::Grabber::Simple(device);
		grabber->open(deviceItem.id);
		grabber->startCapture();
		toAdd.push_back(grabber);

		grabber->setExposure(this->exposure);
		grabber->setGain(this->gain);
		grabber->setFocus(this->focus);
	}

	previews = gui.addGrid();
	for(auto index : order) {
		auto grabber = toAdd[index];
		this->grabbers.push_back(grabber);
		auto panel = PanelPtr(new ofxCvGui::Panels::Draws(*grabber));
		panel->setCaption(grabber->getModelName());
		previews->add(panel);
		panel->onDraw.addListener([grabber] (DrawArguments&) {
			AssetRegister.drawText(ofToString(grabber->getFps(), 1) + "fps", 20, 50);
		}, this);
	}
	
	this->outputPanel = gui.add(preview);
	this->outputPanel->setWidgets(this->controls);
	this->controls.addFPS();
	this->controls.addSlider("Exposure", 0, 1000, &this->exposure);
	this->controls.addSlider("Gain", 0, 1.0f, &this->gain);
	this->controls.addSlider("Focus", 0, 1.0f, &this->focus);
	this->controls.addToggle("Update previews", &this->updatePreviews);

	ofAddListener(this->controls.newGUIEvent, this, &testApp::onControls);
}

//--------------------------------------------------------------
void testApp::update(){
	for(auto grabber : grabbers) {
		grabber->update();
	}

	preview.setFromPixels(grabbers[(ofGetFrameNum() / 4)% grabbers.size()]->getPixelsRef());
	preview.update();
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

//--------------------------------------------------------------
void testApp::onControls(ofxUIEventArgs & args) {
	for(auto grabber : grabbers) {
		grabber->setExposure(this->exposure);
		grabber->setGain(this->gain);
		grabber->setFocus(this->focus);

		grabber->setUseTexture(this->updatePreviews);
	}
}