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
		grabber->open(deviceItem.id);
		grabber->startCapture();
		grabbers.push_back(grabber);

		//grabber->setExposure(0);
		//grabber->setGain(0);
		grabber->setFocus(0);

		auto panel = gui.add(*grabber, string(deviceItem.deviceName));

		panel->onDraw.addListener([this, grabber, index, panel] (const DrawArguments &) {
			AssetRegister.drawText(ofToString(grabber->getFps()) + "fps", 20, 60);

			for(int i=0; i<this->order.size(); i++) {
				if (this->order[i] == index) {
					//draw index if we have it
					AssetRegister.drawText(ofToString(i),0, panel->getHeight() / 2 - 20, "", true, 40, panel->getWidth());
				}
			}
		}, this);

		panel->onKeyboard.addListener([rawDevice] (const KeyboardArguments & key) {
			if (key.key == 's') {
				rawDevice->showSettings();
			}
		}, this);

		panel->onMouse.addListener([this, index] (const MouseArguments & args) {
			if (args.isLocalPressed() && args.button == 0) {
				this->addIndex(index);
			}
		}, this);

		index++;
	}

	/*
	//we presume we want to start from scratch
	try {
		ofFile load(ORDER_FILENAME, ofFile::ReadOnly, true);
		if (load.getPocoFile().getSize() == sizeof(int) * this->grabbers.size()) {
			order.resize(grabbers.size());
			load.read((char*) &order[0], sizeof(int) * grabbers.size());
		}
		load.close();
	} catch(...) {
		ofLogWarning() << "Couldn't load camera order";
	}
	*/
}

//--------------------------------------------------------------
void testApp::update(){

	if (ofGetFrameNum() == 0) {
		ofSetWindowPosition(0,0);
		ofSetWindowShape(1080*2, 1920);
	}

	for(auto grabber : grabbers) {
		grabber->update();
	}

	if (order.size() == grabbers.size()) {
		auto rootGroup = this->gui.getController().getRootGroup();
		vector<PanelPtr> toRemove;
		int index = 0;
		for(auto panel : rootGroup->getElements()) {
			toRemove.push_back(panel);
			index++;
		}
		for(auto panel : toRemove) {
			rootGroup->drop(panel);
		}
		int newIndex = 0;
		for(auto index : order) {
			auto panel = toRemove[index];
			rootGroup->add(panel);
			panel->onMouse.removeListeners(this);
			panel->onMouse.addListener([this, newIndex] (const MouseArguments & args) {
				if (args.isLocalPressed()) {
					this->addIndex(newIndex);
				}
			}, this);
			newIndex++;
		}

		ofBuffer content;
		for(auto index : order) {
			content.append( (char*) &index, sizeof(int));
		}
		ofFile(ORDER_FILENAME, ofFile::WriteOnly, true).writeFromBuffer(content);

		order.clear();
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
void testApp::addIndex(int index) {
	if (this->order.size() < grabbers.size())  {
		bool contains = false;
		for(auto find : order) {
			if (find == index) {
				contains = true;
				break;
			}
		}
		if (contains == false) {
			order.push_back(index);
		}
	}
}