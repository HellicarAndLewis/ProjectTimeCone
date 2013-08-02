#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

#include "ProjectTimeCone.h"

using namespace ofxCvGui;
using namespace ProjectTimeCone;

class testApp : public ofBaseApp{

	struct Frame {
		float time;
		float camera;
	};

public:
	void setup();
	void update();
	void draw();

	Frame screenToFrame(const ofVec2f &) const;
	ofVec2f frameToScreen(const Frame &) const;
	void loadFrame(const Frame &);

	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void keyPressed( int key );
	void mouseMoved( int x, int y );
	void mousePressed( int x, int y, int button) override;
	void mouseDragged( int x, int y, int button ) override;
	void mouseReleased(int x, int y, int button ) override;

	Poco::Path inputFolder;

	vector<string> folders;
	vector<map<float, string>> files;
	vector<float> timeOffsets;
	float minTimestamp;
	float maxTimestamp;

	map<float, Frame> positions; // length, frame
	ofVec2f cursor; // screen

	float totalLength;
	float beginPlaybackTime;

	bool editing;
	float searchTime;

	ofFbo gridView;
	ofImage preview;
};
