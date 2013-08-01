#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->exposure = 500;
	this->gain = 5;
	this->focus = 0;
	this->doRecord = false;
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

	Poco::Path outputPath(ofSystemLoadDialog("Select output folder", true).getPath());

	for(auto index : toAdd) {
		auto device = DevicePtr(new Device::VideoInputDevice(1280, 720));
		auto grabber = ofPtr<Grabber::Simple>(new Grabber::Simple(device));
		grabber->open(index);
		grabber->startCapture();
		grabber->setExposure(this->exposure);
		grabber->setGain(this->gain);
		grabber->setFocus(this->focus);

		this->grabbers.push_back(grabber);
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

		auto streamer = ofPtr<Stream::DiskStreamer>(new Stream::DiskStreamer());
		streamer->setGrabber(grabber);
		auto path = outputPath;
		path.pushDirectory(ofToString(streamers.size()));
		streamer->setOutputFolder(path.toString());
		this->streamers.push_back(streamer);
	}
	
	this->controlPanel = PanelPtr(new Panels::Base());
	gui.add(this->controlPanel);
	this->controls.setPosition(10, 60);
	this->controlPanel->setWidgets(this->controls);
	this->controls.addFPS();
	this->controls.addToggle("Record", &this->doRecord);
	this->controls.addSlider("Exposure", 0, 1000, &this->exposure);
	this->controls.addSlider("Gain", 0, 1.0f, &this->gain);
	this->controls.addSlider("Focus", 0, 1.0f, &this->focus);
	this->controls.addButton("Clear line", &this->clearLine);
	ofAddListener(this->controls.newGUIEvent, this, &testApp::onControls);
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
		if (args.widget->getName() == "Record") {
			if (this->doRecord) {
				for(auto streamer : streamers) {
					streamer->start();
				} 
			} else {
				for(auto streamer : streamers) {
					streamer->stop();
				}
			}
		}
	}
}