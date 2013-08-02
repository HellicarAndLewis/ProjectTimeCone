#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

#include "ProjectTimeCone.h"

using namespace ofxCvGui;
using namespace ofxMachineVision;
using namespace ProjectTimeCone;

class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	ofVec2f screenToFrame(const ofVec2f &) const;
	ofVec2f frameToScreen(const ofVec2f &) const;
	void loadFrame(const ofVec2f &);

	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void keyPressed( int key );
	void mouseMoved( int x, int y );
	void mousePressed( int x, int y, int button) override;
	void mouseDragged( int x, int y, int button ) override;
	void mouseReleased(int x, int y, int button ) override;

	ofxCvGui::Builder gui;

	Poco::Path inputFolder;

	vector<string> folders;
	vector<map<float, string>> files;
	vector<float> timeOffsets;
	float minTimestamp;
	float maxTimestamp;
	ofImage preview;

	map<float, ofVec2f> positions;
	float recordingStart;
	float recordingEnd;
	bool pressed;
	float position;
	float searchTime;

	vector<vector<ofVec2f>> highlight;
};
