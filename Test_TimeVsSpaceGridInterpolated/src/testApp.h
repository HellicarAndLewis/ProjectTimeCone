#pragma once

#include "ProjectTimeCone.h"
#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"


#define FRAME_LENGTH (25 * 10)

using namespace ofxCvGui;
using namespace ProjectTimeCone;

class testApp : public ofBaseApp{
public:
	testApp();
	void setup();
	void update();
	void draw();


	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void keyPressed( int key );
	void mouseMoved( int x, int y );
	void mousePressed( int x, int y, int button) override;
	void mouseDragged( int x, int y, int button ) override;
	void mouseReleased(int x, int y, int button ) override;

	Poco::Path inputFolder;

	ProjectTimeCone::TimeSpacePick::FrameSet frameSet;
	ProjectTimeCone::TimeSpacePick::FramePath framePath;

	float beginPlaybackTime;
	ofVec2f cursor; // screen

	bool editing;
	float searchTime;

	ofImage result;

	ProjectTimeCone::YouTube::EncodeAndUpload youTubeEncoder;
};
