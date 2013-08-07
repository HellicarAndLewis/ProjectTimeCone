#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	this->hitIndex = -1;

	gui.init();
	gui.addInstructions();
	
	ProjectTimeCone::Initialisation::LoadCameras(this->grabbers, [this] (int index, ofPtr<Grabber::Simple> grabber) {
		auto panel = gui.add(*grabber, string(grabber->getModelName()));
		panel->onDrawCropped.addListener([this, grabber, index, panel] (const Panels::BaseImage::DrawCroppedArguments & args) {
			ofPushMatrix();
			ofScale(args.size.x, args.size.y, 1.0f);


			//--
			//crosshair
			//
			ofPushStyle();
			ofSetColor(0);
			ofSetLineWidth(1.0f);
			ofLine(0.0f, 0.5f, 1.0f, 0.5f);
			ofLine(0.5f, 0.0f, 0.5f, 1.0f);

			ofSetColor(255);
			ofSetLineWidth(2.0f);
			ofLine(0.0f, 0.5f, 0.45f, 0.5f);
			ofLine(0.55f, 0.5f, 1.0f, 0.5f);
			ofLine(0.5f, 0.0f, 0.5f, 0.45f);
			ofLine(0.5f, 0.55f, 0.5f, 1.0f);
			
			ofPopStyle();
			//
			//--


			//--
			//drawing
			//
			for(auto & line : this->lines) {
				ofPushStyle();
				ofNoFill();
				
				//black
				ofSetLineWidth(3.0f);
				ofSetColor(0);
				ofBeginShape();
				for(auto & point : line) {
					ofVertex(point.x, point.y, 0.0f);
				}
				ofEndShape(false);

				//white
				ofSetLineWidth(1.0f);
				ofSetColor(255);
				ofBeginShape();
				for(auto & point : line) {
					ofVertex(point.x, point.y, 0.0f);
				}
				ofEndShape(false);
			}
			//
			//--
			ofPopMatrix();
		}, this);

		panel->onMouse.addListener([this, index, panel] (const MouseArguments & args) {
			if (args.isLocal() && panel->getZoomed() == ofxCvGui::Panels::BaseImage::ZoomFit) {
				if (args.action == MouseArguments::Pressed) {
					this->lines.push_back(vector<ofVec2f>());
					this->lines.back().push_back(ofVec2f(args.localNormalised));
					this->hitIndex = index;
				} else if (args.action == MouseArguments::Dragged) {
					if (index == this->hitIndex) {
						this->lines.back().push_back(ofVec2f(args.localNormalised));
					}
				}
			}
		}, this);
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
	if (key == OF_KEY_BACKSPACE && this->lines.size() > 0) {
		this->lines.resize(this->lines.size() - 1);
	}
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
}