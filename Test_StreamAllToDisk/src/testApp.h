#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

#include "ProjectTimeCone.h"

using namespace ofxCvGui;
using namespace ofxMachineVision;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void onControls(ofxUIEventArgs & args);

		vector<ofPtr<Grabber::Simple>> grabbers;
		vector<ofPtr<ofxMachineVision::Stream::DiskStreamer>> streamers;
		ofxCvGui::Builder gui;

		PanelPtr controlPanel;
		ofxUICanvas controls;

		float exposure;
		float gain;
		float focus;

		bool clearLine;
		ofPolyline line;

		bool doRecord;

		Poco::Path outputFolder;

};
