#include "testApp.h"

//--------------------------------------------------------------
CameraControls::CameraControls(int index) {
	this->index = index;
}

//--------------------------------------------------------------
void CameraControls::onControlChange(ofxUIEventArgs & args) {
	if(args.widget->getName() == "Exposure") {
		this->setExposure(this->exposure, true);
	} else if (args.widget->getName() == "Gain") {
		this->setGain(this->gain, true);
	} else if (args.widget->getName() == "Focus") {
		this->setFocus(this->focus, true);
	}
}

//--------------------------------------------------------------
void CameraControls::setExposure(float exposure, bool setOthers) {
	this->exposure = exposure;
	this->grabber->setExposure(exposure);
	if (setOthers && this->setAllFromThis) {
		this->onExposureChangeAll(exposure);
	}
	this->save();
}

//--------------------------------------------------------------
void CameraControls::setGain(float gain, bool setOthers) {
	this->gain = gain;
	this->grabber->setGain(gain);
	if (setOthers && this->setAllFromThis) {
		this->onGainChangeAll(gain);
	}
	this->save();
}

//--------------------------------------------------------------
void CameraControls::setFocus(float focus, bool setOthers) {
	this->focus = focus;
	this->grabber->setFocus(focus);
	if (setOthers && this->setAllFromThis) {
		this->onFocusChangeAll(focus);
	}
	this->save();
}

//--------------------------------------------------------------
string CameraControls::getFilename() const {
	return "../../../Data/camera" + ofToString(this->index) + ".xml";
}

//--------------------------------------------------------------
void CameraControls::load() {
	if (this->xml.load(this->getFilename())) {
		;
		this->setExposure(this->xml.getValue<float>("//Exposure"), false);
		this->setGain(this->xml.getValue<float>("//Gain"), false);
		this->setFocus(this->xml.getValue<float>("//Focus"), false);
		xml.setTo("//CameraSettings");
	} else {
		this->xml.clear();
		this->xml.addChild("CameraSettings");
		this->xml.setTo("//CameraSettings");
		this->xml.addValue("Exposure", ofToString(this->exposure));
		this->xml.addValue("Gain", ofToString(this->gain));
		this->xml.addValue("Focus", ofToString(this->focus));
		this->exposure = 500;
		this->focus = 0;
		this->gain = 0;
	}
}

//--------------------------------------------------------------
void CameraControls::save() {
	this->xml.setTo("//CameraSettings");
	this->xml.setValue("Exposure", ofToString(this->exposure));
	this->xml.setValue("Gain", ofToString(this->gain));
	this->xml.setValue("Focus", ofToString(this->focus));
	this->xml.save(this->getFilename());
}

//--------------------------------------------------------------
void testApp::setup(){

	this->hitIndex = -1;

	gui.init();
	gui.addInstructions();
	
	ProjectTimeCone::Initialisation::LoadCameras(this->grabbers,
	[this] (const Initialisation::InitialisedCameraArguments & args) {
		auto grabber = args.camera;
		auto panel = gui.add(*grabber, string(grabber->getModelName()));
		auto controls = new ofxUICanvas();

		this->controllers.push_back(ofPtr<CameraControls>(new CameraControls(args.index)));
		auto & controller = * this->controllers.back();
		panel->setWidgets(*controls);

		controls->addSlider("Exposure", 0, 1000, &controller.exposure);
		controls->addSlider("Gain", 0, 1, &controller.gain);
		controls->addSlider("Focus", 0, 1, &controller.focus);
		controls->addToggle("Set all from this", &controller.setAllFromThis);
		
		controller.setAllFromThis = false;
		controller.index = args.index;
		controller.grabber = grabber;
		controller.controls = ofPtr<ofxUICanvas>(controls);
		controller.load();

		ofAddListener(controls->newGUIEvent, &controller, &CameraControls::onControlChange);

		controller.onExposureChangeAll += [this] (float value) {
			for(auto & controller : this->controllers) {
				controller->setExposure(value, false);
			}
		};

		controller.onGainChangeAll += [this] (float value) {
			for(auto & controller : this->controllers) {
				controller->setGain(value, false);
			}
		};

		controller.onFocusChangeAll += [this] (float value) {
			for(auto & controller : this->controllers) {
				controller->setFocus(value, false);
			}
		};

		panel->onDraw += [grabber] (const ofxCvGui::DrawArguments&) {
			ofxCvGui::AssetRegister.drawText(ofToString(grabber->getFps()), 30, 60);
		};
	}, 1280, 800);
}

//--------------------------------------------------------------
void testApp::update(){
	for(auto grabber : grabbers) {
		grabber->update();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 30, 10);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
 
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
}