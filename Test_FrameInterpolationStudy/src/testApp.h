#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"
#include "ofxCv.h"

using namespace ofxCvGui;
using namespace ofxCv;
using namespace cv;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void onControls(ofxUIEventArgs & args);
		void updateFlow(int selection);
		void updateFlow();

		ofxCvGui::Builder gui;
		vector<ofPixels> images;

		float pyramidScale;
		float numLevels;
		float windowSize;
		float numIterations;
		float polyN;
		float polySigma;
		bool useGaussian;
		float viewScaleLow;
		float viewScaleHigh;

		int width, height;
		Mat flow;
		Mat grayLHS, grayRHS;
		ofFloatImage flowPreview;

		ofxUICanvas controls;

		ofMesh pointCloud;
		ofTexture texture;
};
