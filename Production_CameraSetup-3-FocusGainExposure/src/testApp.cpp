#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){

	this->hitIndex = -1;

	gui.init();
	gui.addInstructions();
	gui.add(this->combined, "Strip of each");

	ProjectTimeCone::Initialisation::Initialiser::LoadCameras(this->controllers,
		[this] (ofPtr<Initialisation::CameraController> controller) {
		auto grabber = controller->grabber;
		auto panel = gui.add(*grabber, string(grabber->getModelName()));

		auto controls = new ofxUICanvas();
		panel->setWidgets(*controls);
		controller->controls = ofPtr<ofxUICanvas>(controls);

		controls->addSlider("Exposure", 0, 1000, &controller->exposure);
		controls->addSlider("Gain", 0, 1, &controller->gain);
		controls->addSlider("Focus", 0, 1, &controller->focus);
		controls->addToggle("Set all from this", &controller->setAllFromThis);

		ofAddListener(controls->newGUIEvent, &*controller, &Initialisation::CameraController::onControlChange);

		controller->onExposureChangeAll += [this] (float value) {
			for(auto & controller : this->controllers) {
				controller->setExposure(value, false);
			}
		};

		controller->onGainChangeAll += [this] (float value) {
			for(auto & controller : this->controllers) {
				controller->setGain(value, false);
			}
		};

		controller->onFocusChangeAll += [this] (float value) {
			for(auto & controller : this->controllers) {
				controller->setFocus(value, false);
			}
		};

		panel->onDraw += [grabber] (const ofxCvGui::DrawArguments&) {
			ofxCvGui::AssetRegister.drawText(ofToString(grabber->getFps()), 30, 60);
		};
	}, 1280, 800);

	this->combined.allocate(20 * this->controllers.size(), 800);
}

//--------------------------------------------------------------
void testApp::update(){
	int index = 0;
	for(auto controller : this->controllers) {
		controller->grabber->update();

		for(int i=0; i<800; i++) {
			memcpy(this->combined.getPixels() + 20 * 3 * i, controller->grabber->getPixels(), 3 * 20);
		}
	}
	this->combined.update();
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