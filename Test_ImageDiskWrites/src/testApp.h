#pragma once
#include <thread>

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

using namespace ofxCvGui;
using namespace ofxMachineVision;

class TestItem {
public:
	typedef function<void (vector<ofPixels>&, string)> Functor;
	TestItem(string name, Functor test) {
		this->test = test;
		this->name = name;
	}
	string name;
	Functor test;
	float time;
};

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void captureFrames();
		void runTests();

		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofxCvGui::Builder gui;
		DevicePtr device;
		Grabber::Simple * grabber;
		float exposure;
		float gain;
		float focus;

		vector<TestItem> tests;
		vector<ofPixels> images;
		thread testRunner;
};
