#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->exposure = 500;
	this->gain = 5;
	this->focus = 0;
	this->autoCycle = false;
	this->selectedFrame = 0;
	this->framesPerPosition = 1;
	this->saveSet = false;
	this->clearLine = false;

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

	this->grabbers.resize(toAdd.size());
//#pragma omp parallel for num_threads(2) //issues initialising
	for(int i=0; i<toAdd.size(); i++) {
		int index = toAdd[i];
		auto rawDevice = new ofxMachineVision::Device::VideoInputDevice(1280, 720);
		auto device = ofxMachineVision::DevicePtr(rawDevice);
		auto grabber = new ofxMachineVision::Grabber::Simple(device);
		grabber->open(index);
		grabber->startCapture();
		grabber->setExposure(this->exposure);
		grabber->setGain(this->gain);
		grabber->setFocus(this->focus);

		this->grabbers[i] = grabber;
#pragma omp critical
		{
			PanelPtr panel = this->gui.add(grabber->getTextureReference(), grabber->getModelName());
			panel->onDraw.addListener([this, grabber, panel] (DrawArguments & args) {
				AssetRegister.drawText(ofToString(grabber->getFps()), 20, 60);

				ofPushMatrix();
				ofScale(panel->getWidth(), panel->getHeight(), 1.0f);
				ofPushStyle();
				ofSetLineWidth(2.0f);
				ofSetColor(255);
				ofNoFill();
				this->line.draw();
				ofPopStyle();
				ofPopMatrix();
			}, this);
			panel->onMouse += [this] (MouseArguments& args) {
				if (args.isLocal() && (args.action == MouseArguments::Pressed || args.action == MouseArguments::Dragged)) {
					this->line.addVertex(args.localNormalised);
				}
			};
		}
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
	this->controls.addButton("Clear line", &this->clearLine);
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

	if (this->clearLine) {
		this->clearLine = false;
		this->line.clear();
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