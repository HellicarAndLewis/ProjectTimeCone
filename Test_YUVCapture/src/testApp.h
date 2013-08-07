#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"

#define ORDER_FILENAME "../../../Data/order.bin"

using namespace ofxCvGui;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		videoInput device;
		
		ofImage output;

		ofxCvGui::Builder gui;
};
