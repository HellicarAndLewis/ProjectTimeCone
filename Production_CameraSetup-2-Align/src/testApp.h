#pragma once
#define NO_YOUTUBE
#include "ProjectTimeCone.h"
#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

#define ORDER_FILENAME "../../../Data/order.bin"

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
		void keyPressed( int key );
		
		vector<ofPtr<ofxMachineVision::Grabber::Simple>> grabbers;
		ofxCvGui::Builder gui;

		vector<vector<ofVec2f>> lines;
		int hitIndex;
};
