#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->exposure = 500;
	this->gain = 5;
	this->focus = 0;
	this->autoCycle = false;
	this->selectedFrame = 0;
	this->framesPerPosition = 1;

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

	vector<int> toAdd;
	for(auto deviceItem : deviceList) {
		toAdd.push_back(deviceItem.id);
	}

	for(auto index : order) {
		auto rawDevice = new ofxMachineVision::Device::VideoInputDevice(1280, 720);
		auto device = ofxMachineVision::DevicePtr(rawDevice);
		auto grabber = new ofxMachineVision::Grabber::Simple(device);
		grabber->open(toAdd[index]);
		grabber->startCapture();
		grabber->setExposure(this->exposure);
		grabber->setGain(this->gain);
		grabber->setFocus(this->focus);

		this->grabbers.push_back(grabber);
		auto panel = this->gui.add(grabber->getTextureReference(), grabber->getModelName());
		panel->onDraw.addListener([grabber] (DrawArguments &) {
			AssetRegister.drawText(ofToString(grabber->getFps()), 20, 60);
		}, this);
	}
	
	this->outputPanel = gui.add(preview);
	this->controls.setPosition(10, 60);
	this->outputPanel->setWidgets(this->controls);
	this->controls.addFPS();
	this->controls.addSlider("Exposure", 0, 1000, &this->exposure);
	this->controls.addSlider("Gain", 0, 1.0f, &this->gain);
	this->controls.addSlider("Focus", 0, 1.0f, &this->focus);
	this->controls.addToggle("Cycle", &this->autoCycle);
	this->controls.addSlider("Frames per position", 1, 20, &framesPerPosition);
	this->controls.addButton("Save set", &this->saveSet);
	ofAddListener(this->controls.newGUIEvent, this, &testApp::onControls);

	this->outputPanel->onMouse.addListener([this] (MouseArguments& args) {
		if (args.isLocal()) {
			this->selectedFrame = args.localNormalised.x * this->grabbers.size();
		}
	}, this);
}

//--------------------------------------------------------------
void testApp::update(){
	for (int i=0; i<grabbers.size(); i++) {
		grabbers[i]->update();
	}

	int index = this->autoCycle ? ofGetFrameNum() / framesPerPosition: selectedFrame;
	preview.setFromPixels(this->grabbers[index % this->grabbers.size()]->getPixelsRef());
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
		if (args.widget->getName() == "Exposure")
			grabber->setExposure(this->exposure);
		if (args.widget->getName() == "Gain")
			grabber->setGain(this->gain);
		if (args.widget->getName() == "Focus")
			grabber->setFocus(this->focus);
		if (args.widget->getName() == "Save set") {
			if (this->saveSet) {
				for(int i=0; i<grabbers.size(); i++) {
					string filename = ofToString(i);
					while(filename.size() < 3) {
						filename = "0" + filename;
					}
					filename += ".png";
					ofSaveImage(grabbers[i]->getPixelsRef(), filename);
				}
				this->saveSet = false;
			}
		}
	}
}