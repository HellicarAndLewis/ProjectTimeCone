#pragma once
#define NO_YOUTUBE
#include "ProjectTimeCone.h"
#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

using namespace ofxCvGui;
using namespace ofxMachineVision;
using namespace ProjectTimeCone;

class CameraControls {
public:
	CameraControls(int index);
	void onControlChange(ofxUIEventArgs &);

	void setExposure(float, bool allowTriggerOthers);
	void setGain(float, bool allowTriggerOthers);
	void setFocus(float, bool allowTriggerOthers);
	
	string getFilename() const;
	void load();
	void save();

	ofxCvGui::Utils::LambdaStack<float> onExposureChangeAll;
	ofxCvGui::Utils::LambdaStack<float> onGainChangeAll;
	ofxCvGui::Utils::LambdaStack<float> onFocusChangeAll;

	int index;
	float exposure;
	float gain;
	float focus;
	bool setAllFromThis;
	ofXml xml;

	ofPtr<ofxUICanvas> controls;
	ofPtr<ofxMachineVision::Grabber::Simple> grabber;
};

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void keyPressed( int key );
		
		vector<ofPtr<Grabber::Simple>> grabbers;
		vector<ofPtr<CameraControls>> controllers;
		ofxCvGui::Builder gui;

		int hitIndex;
};
