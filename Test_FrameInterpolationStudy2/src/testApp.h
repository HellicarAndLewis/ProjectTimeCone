#pragma once
#define NO_YOUTUBE
#include "ProjectTimeCone.h"

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"
#include "ofxCv.h"

using namespace ofxCvGui;
using namespace ofxCv;
using namespace cv;
using namespace ProjectTimeCone;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void onControls(ofxUIEventArgs & args);

		ofPtr<Interpolation::OpticalFlow> interpolation;

		ofxCvGui::Builder gui;
		PanelPtr outputPanel;

		vector<ofPixels> images;

		ofImage A, B;
		float x;
		float width, height;
};
