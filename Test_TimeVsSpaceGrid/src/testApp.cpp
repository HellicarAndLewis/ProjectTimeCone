#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->editing = true;
	this->totalLength = 0.0f;

	this->minTimestamp = std::numeric_limits<float>::max();
	this->maxTimestamp = std::numeric_limits<float>::min();

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

	this->gridView.allocate(1920,1080);
	int gridHeight = 12;
	int gridWidth = 12; //1920 / ((1080/12) * (1280 / 720)); //=12 because aspect ratio is same

	float thumbWidth = 1920.0f / (float) gridWidth;
	float thumbHeight = 1080.0f / (float) gridHeight;

	ofImage thumbLarge;
	thumbLarge.allocate(1280, 720, OF_IMAGE_COLOR);
	this->gridView.begin();
	for(int i=0; i<gridWidth; i++) {
		for(int j=0; j<gridHeight; j++) {
			auto frame = screenToFrame(ofVec2f(i * ofGetWidth() / gridWidth, j * ofGetHeight() / gridHeight));
			auto it = files[j].lower_bound(frame.time);

			if (it != files[j].end()) {
				ifstream file(it->second, ios::binary | ios::in);
				file.read( (char*) thumbLarge.getPixels(), thumbLarge.getPixelsRef().size());
				file.close();

				thumbLarge.update();
				thumbLarge.draw(thumbWidth * i, thumbHeight * j, thumbWidth, thumbHeight);
			}
		}
	}
	this->gridView.end();

	ofSetFullscreen(true);
}

//--------------------------------------------------------------
void testApp::update(){
	if (ofGetFrameNum() == 0) {
		//Initialisation::FillDevelopmentScreens();
	}

	if (!this->editing && this->positions.size() > 1) {
		this->searchTime = ofGetElapsedTimef() - beginPlaybackTime;

		//loop every 10 seconds
		searchTime -= floor(searchTime / 10.0f) * 10.0f;
		searchTime /= 10.0f;

		//search time is now in units of line length
		float searchLength = searchTime * totalLength;

		auto currentPosition = positions.find(searchLength);
		if (currentPosition == positions.end()) {
			//general case, no exact match, so interpolate

			auto lower = positions.lower_bound(searchLength);
			auto upper = positions.upper_bound(searchLength);

			Frame frame;
			frame.time = ofMap(searchLength, lower->first, upper->first, lower->second.time, upper->second.time);
			frame.camera = ofMap(searchLength, lower->first, upper->first, lower->second.camera, upper->second.camera);
			this->loadFrame(frame);
		} else {
			this->loadFrame(currentPosition->second);
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	if (preview.isAllocated()) {
		preview.draw(0,0,ofGetWidth(),ofGetHeight());
	}
	if (this->editing || this->positions.size() <= 1) {
		this->gridView.draw(0,0);
	}

	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(3.0f);
	ofBeginShape();
	ofSetColor(255, 100, 100);
	for(auto & point : positions) {
		auto screen = frameToScreen(point.second);
		ofVertex(screen.x, screen.y);
	}
	ofEndShape(false);
	auto currentPosition = positions.lower_bound(this->searchTime);
	ofFill();
	ofSetLineWidth(0);
	ofSetColor(200,0,0,100);
	ofCircle(cursor.x, cursor.y, 10.0f);
	ofPopStyle();
}

//--------------------------------------------------------------
testApp::Frame testApp::screenToFrame(const ofVec2f & screen) const {
	testApp::Frame frame;
	frame.time = screen.x / ofGetWidth() * (this->maxTimestamp - this->minTimestamp) + minTimestamp;
	frame.camera = ofClamp((screen.y * 13) / ofGetHeight() - 1, 0, 12);
	return frame;
}

//--------------------------------------------------------------
ofVec2f testApp::frameToScreen(const Frame & frame) const {
	return ofVec2f((frame.time - minTimestamp) / (maxTimestamp - minTimestamp) * ofGetWidth(),
		(frame.camera + 1.0f) * ofGetHeight() / 13);
}

//--------------------------------------------------------------
void testApp::loadFrame(const Frame & frame) {
	if (frame.camera < files.size()) {
		auto cameraFiles = files[frame.camera];
		auto find = cameraFiles.find(frame.time);
		if (find == cameraFiles.end()) {
			//this is general case, no exact match. find the frame before
			find = cameraFiles.lower_bound(frame.time);
		}
		if (find != cameraFiles.end()) {
			ifstream file(find->second, ios::binary | ios::in);
			file.read((char*) preview.getPixels(), preview.getPixelsRef().size());
			file.close();

			preview.update();
		}
		this->cursor = frameToScreen(frame);
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
	if (key == ' ') {
		this->editing = false;
		this->beginPlaybackTime = ofGetElapsedTimef();
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
	if (!this->editing) {
		auto frame = screenToFrame(ofVec2f(x, y));
		this->loadFrame(frame);
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {

	if (!this->editing) {
		positions.clear();
		positions.insert(pair<float,Frame>(0.0f, screenToFrame(ofVec2f(0, 0))));
		this->totalLength = 0.0f;
	}

	positions.insert(pair<float,Frame>(0.0f, screenToFrame(ofVec2f(x, y))));

	this->editing = true;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	if (editing) {
		auto frame = screenToFrame(ofVec2f(x, y));

		auto last = this->positions.end();
		last--;
		if (frame.time <= last->second.time) {
			frame.time = last->second.time;
		}

		//perform check on recomputed length in case we clamped
		float length = (frameToScreen(last->second) - frameToScreen(frame)).length();

		totalLength += length;

		positions.insert(pair<float,Frame>(totalLength, frame));

		this->loadFrame(frame);
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button ) {
}