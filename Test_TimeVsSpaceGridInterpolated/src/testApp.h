#pragma once

#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

#include "ProjectTimeCone.h"

#define FRAME_LENGTH (25 * 10)

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
	Frame lengthToFrame(float position) const;
	
	static void getFrame(string filename, ofPixels &);
	bool getFrame(int camera, float time, ofPixels & output);

	bool cacheInterpolation(const Frame &); //returns true if we need to interpolate
	void buildFrame(const Frame &, bool interpolate=false);

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

	ofPtr<Interpolation::OpticalFlow> interpolator;
	string intepolatorCachedA, intepolatorCachedB; //filesnames of cached content
	ofImage A, B;
};
