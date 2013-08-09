#include "testApp.h"
#include "Poco/DirectoryIterator.h"

//--------------------------------------------------------------
testApp::testApp() :
framePath(frameSet) {
}

//--------------------------------------------------------------
void testApp::setup(){
	this->editing = true;
	this->frameSet.init(1920, 1080);
	
	auto result = ofSystemLoadDialog("Select recording folder", true);
	if (!result.bSuccess) {
		ofExit();
	}
	this->frameSet.load(result.getPath() + "/");

	this->result.allocate(1280, 720, OF_IMAGE_COLOR);

	if (!this->youTubeEncoder.setAudioFile("audio.mp3", true)) {
		ofSystemAlertDialog("Audio file missing");
	}
	if (!this->youTubeEncoder.setFramesHeadDir("frames_head", true)) {
		ofSystemAlertDialog("Header frames missing");
	}
	if (!this->youTubeEncoder.setFramesTailDir("frames_tail", true)) {
		ofSystemAlertDialog("Tail frames missing");
	}
}

//--------------------------------------------------------------
void testApp::update(){

	if (!this->editing && this->framePath.size() > 1) {
		float searchTime = ofGetElapsedTimef() - beginPlaybackTime;

		//loop every 10 seconds (searchTime is normalized)
		searchTime -= floor(searchTime / 10.0f) * 10.0f;
		searchTime /= 10.0f;

		auto screen = this->framePath.getPositionOnPath(searchTime);
		this->cursor = screen;
		auto timeSpace = this->frameSet.screenToTimeSpace(screen);
		this->frameSet.getInterpolatedFrame(timeSpace, this->result.getPixelsRef());
		this->result.update();
	}

	this->youTubeEncoder.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	if (this->editing || this->framePath.size() <= 1) {
		this->frameSet.getGridView().draw(0,0);
	} else {
		this->result.draw(0,0, ofGetWidth(), ofGetHeight());
	}

	this->framePath.drawPath();

	ofPushStyle();
	ofFill();
	ofSetLineWidth(0);
	ofSetColor(200,0,0,100);
	ofCircle(cursor.x, cursor.y, 10.0f);
	ofPopStyle();
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	if (key == 'f') {
		ofToggleFullscreen();
	}
	
	if (key == OF_KEY_BACKSPACE) {
		this->framePath.clear();
	}

	if (key == ' ') {
		this->editing = false;
		this->beginPlaybackTime = ofGetElapsedTimef();
	}

	if (key == 'b') {
		this->framePath.buildFrames("frames_recording/", FRAME_LENGTH, this->result);
		this->result.update();
		this->youTubeEncoder.encodeFrames("frames_recording", true);
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	if (!this->editing) {
		this->framePath.clear();
		this->editing = true;
	}
	this->framePath.add(ofVec2f(x, y));
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	if (editing) {
		auto screen = ofVec2f(x, y);
		this->framePath.add(screen);
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button ) {
}