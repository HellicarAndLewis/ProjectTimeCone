#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

using namespace ofxCvGui;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		void addIndex(int index);

		vector<ofxMachineVision::Grabber::Simple*> grabbers;
		ofxCvGui::Builder gui;
		ofxCvGui::PanelPtr blankPanel;


		enum Mode {
			Waiting = 0,
			Ordering = 1
		};

		Mode mode;

		vector<int> order;
};
