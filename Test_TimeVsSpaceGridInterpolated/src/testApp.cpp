#include "testApp.h"
#include "Poco/DirectoryIterator.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->editing = true;
	this->totalLength = 0.0f;

	this->minTimestamp = std::numeric_limits<float>::max();
	this->maxTimestamp = std::numeric_limits<float>::min();

	auto result = ofSystemLoadDialog("Select recording folder", true);
	if (!result.bSuccess) {
		ofExit();
	}
	this->inputFolder = Poco::Path(result.getPath() + "/");

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

			auto dirIt = Poco::DirectoryIterator(folder.getAbsolutePath());
			auto dirEnd = Poco::DirectoryIterator();

			//add items to file map
			float timeOffset = 0;
			for (;dirIt != dirEnd; dirIt++) {
				auto path = Poco::Path(dirIt->path());

				auto timestamp = ofToFloat(path.getBaseName()) / 1e6;

				if (currentFolder.size() == 0) {
					//set the time offset as the first timestamp
					timeOffset = timestamp;
					this->timeOffsets.push_back(timeOffset);
				}
				timestamp -= timeOffset;

				currentFolder.insert(std::pair<float, string>(timestamp, path.toString()));
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

	auto flow = new Interpolation::GPUOpticalFlow(1280,720);
	flow->UpdateParameters();
	this->interpolator = ofPtr<Interpolation::OpticalFlow>(flow);
	this->intepolatorCachedA = "";
	this->intepolatorCachedB = "";
	this->A.allocate(1280, 720, OF_IMAGE_COLOR);
	this->B.allocate(1280, 720, OF_IMAGE_COLOR);
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
	if (ofGetFrameNum() == 0) {
		//Initialisation::FillDevelopmentScreens();
	}

	if (!this->editing && this->positions.size() > 1) {
		this->searchTime = ofGetElapsedTimef() - beginPlaybackTime;

		//loop every 10 seconds (searchTime is normalized)
		searchTime -= floor(searchTime / 10.0f) * 10.0f;
		searchTime /= 10.0f;

		this->buildFrame(this->lengthToFrame(searchTime * totalLength), true);
	}

	this->youTubeEncoder.update();
}

//--------------------------------------------------------------
void testApp::draw(){

	if (this->editing || this->positions.size() <= 1) {
		this->gridView.draw(0,0);
	} else {
		this->result.draw(0,0, ofGetWidth(), ofGetHeight());
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
	frame.camera = ofClamp((screen.y * 13) / ofGetHeight() - 1, 0, 11);
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
	Profiler["Load frame from disk"].begin();
	ifstream file(path, ios::binary | ios::in);
	file.read((char*) pixels.getPixels(), pixels.size());
	file.close();
	Profiler["Load frame from disk"].end();
}

//--------------------------------------------------------------
bool testApp::getFrame(int camera, float time, ofPixels & output) {
	auto & frames = files[camera];
	auto find = frames.find(time);

	if (camera < 0 || camera >= this->files.size()) {
		ofLogError() << "Camera index " << camera << " out of bounds";
	}

	if (frames.size() == 0) {
		ofLogError() << "No frames available for camera " << camera;
		return false;
	}

	if (find == frames.end()) {
		//general case, we need to find the closest frame
		find = frames.lower_bound(time);
	}
	if (find == frames.end()) {
		//we want to look back in time to find the last frame
		find--;
	}

	getFrame(find->second, output);
	return true;
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
		Profiler["Load frame to GPU"].begin();
		A.update();
		Profiler["Load frame to GPU"].end();
		needsCache = true;
	}
	if (filenameB != intepolatorCachedB) {
		intepolatorCachedB = filenameB;
		getFrame(filenameB, B.getPixelsRef());
		Profiler["Load frame to GPU"].begin();
		B.update();
		Profiler["Load frame to GPU"].end();
		needsCache = true;
	}
	
	this->interpolator->UpdateFlow(A.getPixelsRef(), B.getPixelsRef());

	return true;
}

//--------------------------------------------------------------
void testApp::buildFrame(const Frame & frame, bool interpolate) {
	bool needsInterpolation = interpolate;
	if (interpolate) {
		needsInterpolation = this->cacheInterpolation(frame);
	}
	
	if (needsInterpolation) {
		float x = frame.camera;
		x -= floor(x); // strip to 0...1
		this->interpolator->UpdateResult(x, this->A.getTextureReference(), this->B.getTextureReference());
		this->interpolator->getResultFbo().readToPixels(result.getPixelsRef());
	} else {
		this->getFrame(frame.camera, frame.time, this->result.getPixelsRef());
	}

	this->result.update();

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
		this->editing = false;
		this->beginPlaybackTime = ofGetElapsedTimef();
	}

	if (key == 'b') {
		Poco::Path outputPath(ofToDataPath("frames_recording/"));
		Poco::File(outputPath).remove(true);
		Poco::File(outputPath).createDirectories();

		ofPixels output;
		output.allocate(1280, 720, OF_PIXELS_RGB);

		Profiler.clear();

		Profiler["Build frames"].begin();
		for (int frame=0; frame<FRAME_LENGTH; frame++) {
			Profiler["Compute frame"].begin();
			auto x = lengthToFrame(this->totalLength * (float) frame / (float) FRAME_LENGTH);
			this->buildFrame(x, true);
			Profiler["Compute frame"].end();

			Profiler["Save frame"].begin();
			string frameString = ofToString(frame);
			while(frameString.length() < 4) {
				frameString = "0" + frameString;
			}

			frameString = "frame_" + frameString;
			auto filename = outputPath.toString() + frameString + ".jpg";

			this->result.saveImage(filename);
			cout << frameString << endl;
			Profiler["Save frame"].end();
		}
		Profiler["Build frames"].end();

		Profiler["Dispatch encoding"].begin();
		this->youTubeEncoder.encodeFrames("frames_recording", true);
		Profiler["Dispatch encoding"].end();

		cout << Profiler.getResultsString();
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