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

	this->interpolator = ofPtr<Interpolation::OpticalFlow>(new Interpolation::OpticalFlow(1280,720));
	this->intepolatorCachedA = "";
	this->intepolatorCachedB = "";
	this->A.allocate(1280, 720, OF_IMAGE_COLOR);
	this->B.allocate(1280, 720, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void testApp::update(){
	if (ofGetFrameNum() == 0) {
		//Initialisation::FillDevelopmentScreens();
	}

	if (!this->editing && this->positions.size() > 1) {
		this->searchTime = ofGetElapsedTimef() - beginPlaybackTime;

		//loop every 10 seconds (searchTime is normalized)
		searchTime -= floor(searchTime / 10.0f) * 10.0f;
		searchTime /= 10.0f;

		this->buildFrame(this->lengthToFrame(searchTime * totalLength));
	}
}

//--------------------------------------------------------------
void testApp::draw(){

	if (this->editing || this->positions.size() <= 1) {
		this->gridView.draw(0,0);
	} else {
		this->interpolator->getResultFbo().draw(0,0);
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
testApp::Frame testApp::lengthToFrame(float position) const {
	
	// no points
	if (this->positions.size() == 0) {
		ofLogError() << "We have no positions available to look up against.";
		return Frame();
	}
	
	// bang on the nose
	auto & exactPoint = this->positions.find(position);
	if (exactPoint != this->positions.end()) {
		return exactPoint->second;
	}

	auto & nextPoint = this->positions.upper_bound(position);

	//we're off the end
	if (nextPoint == this->positions.end()) {
		nextPoint--;
		return nextPoint->second;
	}

	//we're off the beginning
	if (nextPoint == this->positions.begin()) {
		return nextPoint->second;
	}

	//interpolate
	auto previousPoint = nextPoint;
	previousPoint--;
	
	Frame interpolated;

	interpolated.time = ofMap(position,
		previousPoint->first, nextPoint->first,
		previousPoint->second.time, nextPoint->second.time);
		
	interpolated.camera = ofMap(position,
		previousPoint->first, nextPoint->first,
		previousPoint->second.camera, nextPoint->second.camera);

	return interpolated;
}

//--------------------------------------------------------------
void testApp::getFrame(string path, ofPixels & pixels) {
	ifstream file(path, ios::binary | ios::in);
	file.read((char*) pixels.getPixels(), pixels.size());
	file.close();
}

//--------------------------------------------------------------
bool testApp::getFrame(int camera, float time, ofPixels & output) {
	auto & frames = files[camera];
	auto find = frames.find(time);

	if (camera < 0 || camera >= this->files.size() - 1) {
		ofLogError() << "Camera index " << camera << " out of bounds";
	}

	if (find == frames.end()) {
		//general case, we need to find the closest frame
		find = frames.lower_bound(time);
	}
	if (find == frames.end()) {
		find = frames.upper_bound(time);
	}
	if (find == frames.end()) {
		//no frame found. very strange situation
		ofLogError() << "No frame found for camera:" << camera << ", time:" << time;
		return false;
	}
	else {
		getFrame(find->second, output);
		return true;
	}
}

//--------------------------------------------------------------
bool testApp::cacheInterpolation(const Frame & frame) {
	//get frame times

	auto cameraIndexA = floor(frame.camera);
	auto cameraIndexB = ceil(frame.camera);

	if (cameraIndexA == cameraIndexB) {
		//no interpolation required
		return false;
	}

	if (cameraIndexB >= this->files.size()) {
		//if we're on the last camera we have to clamp
		cameraIndexB--;
	}

	auto & filesA = files[cameraIndexA];
	auto & filesB = files[cameraIndexB];

	auto & filenameA = filesA.lower_bound(frame.time)->second;
	auto & filenameB = filesB.lower_bound(frame.time)->second;

	bool needsCache = false;
	if (filenameA != intepolatorCachedA) {
		intepolatorCachedA = filenameA;
		getFrame(filenameA, A.getPixelsRef());
		A.update();
		needsCache = true;
	}
	if (filenameB != intepolatorCachedB) {
		intepolatorCachedB = filenameB;
		getFrame(filenameB, B.getPixelsRef());
		B.update();
		needsCache = true;
	}
	
	this->interpolator->UpdateFlow(A.getPixelsRef(), B.getPixelsRef());

	return true;
}

//--------------------------------------------------------------
void testApp::buildFrame(const Frame & frame, bool interpolate) {
	if (interpolate && this->cacheInterpolation(frame)) {
		float x = frame.camera;
		x -= floor(x); // strip to 0...1
		this->interpolator->UpdateResult(x, this->A.getTextureReference(), this->B.getTextureReference());
	} else {
		// just draw the A frame onto our interpolator's fbo
		auto fbo = this->interpolator->getResultFbo();
		fbo.begin();
		ofClear(0,0,0,0);
		A.draw(0,0);
		fbo.end();
	}

	this->cursor = frameToScreen(frame);
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
		this->positions.clear();
	}

	if (key == ' ') {
		Poco::Path outputPath(ofToDataPath("output/"));
		Poco::File(outputPath).createDirectories();

		ofPixels output;
		output.allocate(1280, 720, OF_PIXELS_RGB);
		for (int frame=0; frame<FRAME_LENGTH; frame++) {
			this->buildFrame(lengthToFrame(this->totalLength * (float) frame / (float) FRAME_LENGTH), true);
			this->interpolator->getResultFbo().readToPixels(output);

			string frameString = ofToString(frame);
			while(frameString.length() < 4) {
				frameString = "0" + frameString;
			}
			ofSaveImage(output, outputPath.toString() + frameString + ".png");
		}
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {

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
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button ) {
}