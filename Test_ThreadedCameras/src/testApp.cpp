#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->mode = Waiting;

	gui.init();
	this->blankPanel = ofxCvGui::PanelPtr(new ofxCvGui::Panels::Base());
	gui.add(blankPanel);

	blankPanel->onDraw += [this] (DrawArguments & args) {
		string message;
		switch(this->mode) {
		case Waiting:
			message = "Waiting";
			break;
		case Ordering:
			message = "Ordering";
			break;
		}

		message += " [/]";
		AssetRegister.drawText(message, 0, args.parentBounds.height / 2 - 10, "", true, 20, args.parentBounds.width);
		
		stringstream orderString;
		for(auto index : this->order) {
			orderString << index << ", ";
		}
		AssetRegister.drawText(orderString.str(), 0, args.parentBounds.height / 2 + 20, "", true, 20, args.parentBounds.width);
	};

	blankPanel->onKeyboard += [this] (KeyboardArguments & args) {
		if (args.action == KeyboardArguments::Action::Pressed) {
			switch (args.key)
			{
			case ']':
			case '[':
				if (this->mode == Waiting)
					this->mode = Ordering;
				else
					this->mode = Waiting;
				break;
			}
		}
	};

	auto deviceList = ofVideoGrabber().listDevices();
	int index = 0;
	for(auto deviceItem : deviceList) {
		auto rawDevice = new ofxMachineVision::Device::VideoInputDevice(1280, 720);
		auto device = ofxMachineVision::DevicePtr(rawDevice);
		auto grabber = new ofxMachineVision::Grabber::Simple(device);
		grabber->open(deviceItem.id);
		grabber->startCapture();
		grabbers.push_back(grabber);

		//grabber->setExposure(0);
		//grabber->setGain(0);
		grabber->setFocus(0);

		auto panel = gui.add(*grabber, string(deviceItem.deviceName));

		panel->onDraw.addListener([grabber] (const DrawArguments &) {
			AssetRegister.drawText(ofToString(grabber->getFps()) + "fps", 20, 60);
		}, this);

		panel->onKeyboard.addListener([rawDevice] (const KeyboardArguments & key) {
			if (key.key == 's') {
				rawDevice->showSettings();
			}
		}, this);

		panel->onMouse.addListener([this, index] (const MouseArguments & args) {
			if (args.isLocalPressed()) {
				this->addIndex(index);
			}
		}, this);

		index++;
	}

	try {
		ofFile load("save.bin", ofFile::ReadOnly, true);
		if (load.getPocoFile().getSize() == sizeof(int) * this->grabbers.size()) {
			order.resize(grabbers.size());
			load.read((char*) &order[0], sizeof(int) * grabbers.size());
		}
		load.close();
	} catch(...) {
		ofLogWarning() << "Couldn't load camera order";
	}
}

//--------------------------------------------------------------
void testApp::update(){
	for(auto grabber : grabbers) {
		grabber->update();
	}

	if (order.size() == grabbers.size()) {
		auto rootGroup = this->gui.getController().getRootGroup();
		vector<PanelPtr> toRemove;
		int index = 0;
		for(auto panel : rootGroup->getElements()) {
			if (index > 0) {
				toRemove.push_back(panel);
			}
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
		ofFile("save.bin", ofFile::WriteOnly, true).writeFromBuffer(content);

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
	if (this->mode == Ordering && this->order.size() < grabbers.size())  {
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