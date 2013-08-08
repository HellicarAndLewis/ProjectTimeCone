#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

#define ORDER_FILENAME "../../../Data/order.bin"

using namespace ofxCvGui;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void keyPressed( int key );
		
		void addIndex(int index);

		vector<ofxMachineVision::Grabber::Simple*> grabbers;
		ofxCvGui::Builder gui;

		//
		vector<int> order;
		bool saved;
};
