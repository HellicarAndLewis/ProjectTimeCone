#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->pressed = false;
	this->minTimestamp = std::numeric_limits<float>::max();
	this->maxTimestamp = std::numeric_limits<float>::min();

	gui.init();
	
	this->inputFolder = Poco::Path(ofSystemLoadDialog("Select recording folder", true).getPath() + "/");
	ofDirectory listDir;
	listDir.listDir(this->inputFolder.toString());
	listDir.sort();
	auto folders = listDir.getFiles();
	for(auto folder : folders) {
		if (folder.isDirectory()) {
			ofLogNotice() << "Adding camera " << folder.getAbsolutePath();
			this->folders.push_back(folder.getBaseName());
			this->files.push_back(map<float, string>());
			auto & currentFolder = this->files.back();

			ofDirectory listImages;
			listImages.listDir(folder.getAbsolutePath());
			auto files = listImages.getFiles();
			float timeOffset;
			for (auto file : files) {
				auto timestamp = ofToFloat(file.getBaseName()) / 1e6;
				if (currentFolder.size() == 0) {
					//set the time offset as the first timestamp
					timeOffset = timestamp;
					this->timeOffsets.push_back(timeOffset);
				}
				timestamp -= timeOffset;

				currentFolder.insert(std::pair<float, string>(timestamp, file.getAbsolutePath()));
				if (timestamp > maxTimestamp) {
					maxTimestamp = timestamp;
				}
				if (timestamp < minTimestamp) {
					minTimestamp = timestamp;
				}
			}
		}
	}

	this->preview.allocate(1280, 720, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void testApp::update(){
	if (ofGetFrameNum() == 0) {
		//Initialisation::FillDevelopmentScreens();
	}

	if (!this->pressed && this->positions.size() > 1) {
		this->searchTime = ofGetElapsedTimef() - recordingEnd;
		searchTime -= floor(searchTime / 10.0f) * 10.0f;

		searchTime *= (recordingEnd - recordingStart) / 10.0f;
		auto currentPosition = positions.lower_bound(searchTime);
		if (currentPosition != positions.end()) {
			this->loadFrame(currentPosition->second);
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if (preview.isAllocated()) {
		preview.draw(0,0,ofGetWidth(),ofGetHeight());
	}
	ofPushStyle();
	auto cameraCount = files.size();
	for(int i=0; i<cameraCount; i++) {
		auto & camera = files[i];
		for(auto & frame : camera) {
			float timestamp = frame.first;
			auto screen = frameToScreen(ofVec2f(timestamp, i));
			ofVec3f position(screen.x, screen.y, 2.0f);

			ofFill();
			ofSetLineWidth(0.0f);
			ofSetColor(0);
			ofCircle(position.x, position.y, position.z);
			ofSetLineWidth(1.0f);
			ofNoFill();
			ofSetColor(255);
			ofCircle(position.x, position.y, position.z);
		}
	}

	ofSetLineWidth(3.0f);
	ofSetColor(255, 100, 100);
	ofBeginShape();
	for(auto & point : positions) {
		auto screen = frameToScreen(point.second);
		ofVertex(screen.x, screen.y);
	}
	ofEndShape(false);
	auto currentPosition = positions.lower_bound(this->searchTime);
	if (currentPosition != positions.end()) {
		ofFill();
		ofSetLineWidth(0);
		ofSetColor(200,0,0,100);
		auto screen = frameToScreen(currentPosition->second);
		ofCircle(screen.x, screen.y, 10.0f);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
ofVec2f testApp::screenToFrame(const ofVec2f & screen) const {
	return ofVec2f(screen.x / ofGetWidth() * (this->maxTimestamp - this->minTimestamp) + minTimestamp,
		ofClamp((screen.y * 13) / ofGetHeight() - 1, 0, 12));
}

//--------------------------------------------------------------
ofVec2f testApp::frameToScreen(const ofVec2f & frame) const {
	return ofVec2f((frame.x - minTimestamp) / (maxTimestamp - minTimestamp) * ofGetWidth(),
		(frame.y + 1) * ofGetHeight() / 13);
}

//--------------------------------------------------------------
void testApp::loadFrame(const ofVec2f & frame) {
	if (frame.y < files.size()) {
		auto cameraFiles = files[frame.y];
		auto file = cameraFiles.lower_bound(frame.x);
		if (file != cameraFiles.end()) {
			//we've found an image
			ifstream file(file->second, ios::binary | ios::in);
			file.read((char*) preview.getPixels(), preview.getPixelsRef().size());
			file.close();

			preview.update();
		}
	}
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
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
	ofVec2f frame = screenToFrame(ofVec2f(x, y));
	this->loadFrame(frame);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	ofVec2f frame = screenToFrame(ofVec2f(x, y));

	this->pressed = true;
	this->recordingStart = ofGetElapsedTimef();
	positions.clear();
	positions.insert(pair<float,ofVec2f>(0.0f, frame));
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	ofVec2f frame = screenToFrame(ofVec2f(x, y));

	auto last = this->positions.end();
	last--;
	float lastFrame = last->second.x;
	if (frame.x <= lastFrame) {
		frame.x = lastFrame;
	}

	float time = ofGetElapsedTimef() - this->recordingStart;
	positions.insert(pair<float,ofVec2f>(time, frame));
	this->recordingEnd = time + this->recordingStart;
	this->position = time;

	this->loadFrame(frame);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button ) {
	this->pressed = false;
}