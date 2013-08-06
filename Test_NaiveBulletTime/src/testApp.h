#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

#define NO_YOUTUBE
#include "ProjectTimeCone.h"

using namespace ofxCvGui;
using namespace ofxMachineVision;
using namespace ProjectTimeCone;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void onControls(ofxUIEventArgs & args);
		vector<ofPtr<Grabber::Simple>> grabbers;
		ofxCvGui::Builder gui;
		ofPtr<ofxCvGui::Panels::Groups::Grid> previews;

		ofxUICanvas controls;

		float exposure;
		float gain;
		float focus;

		ofImage preview;
		ofxCvGui::PanelPtr outputPanel;
		bool autoCycle;
		float framesPerPosition;
		int selectedFrame;
		bool saveSet;
		bool clearLine;

		ofPolyline line;
};
