#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->exposure = 500;
	this->gain = 5;
	this->focus = 0;

	this->device = DevicePtr(new Device::VideoInputDevice(1280, 720));
	this->grabber = new Grabber::Simple(this->device);
	this->grabber->open();
	this->grabber->startCapture();
	this->grabber->setExposure(this->exposure);
	this->grabber->setGain(this->gain);
	this->grabber->setFocus(this->focus);

	gui.init();
	auto livePanel = gui.add(this->grabber->getTextureReference(), "Live");
	gui.add(images, "Captures");

	tests.push_back(TestItem("JPEG Single Threaded", [] (vector<ofPixels>& images, string path) {
		int index = 0;
		for(auto & image : images) {
			ofSaveImage(image, path + "JPEG-ST" + ofToString(index) + ".jpg");
			index++;
		}
	}));

	tests.push_back(TestItem("JPEG Multi Threaded", [] (vector<ofPixels>& images, string path) {
		int count = images.size();
#pragma omp parallel for
		for(int index = 0; index < count; index++) {
			ofSaveImage(images[index], path + "JPEG-MT" + ofToString(index) + ".jpg");
		}
	}));

	tests.push_back(TestItem("PNG Single Threaded", [] (vector<ofPixels>& images, string path) {
		int index = 0;
		for(auto & image : images) {
			ofSaveImage(image, path + "PNG-ST" + ofToString(index) + ".png");
			index++;
		}
	}));

	tests.push_back(TestItem("PNG Multi Threaded", [] (vector<ofPixels>& images, string path) {
		int count = images.size();
#pragma omp parallel for
		for(int index = 0; index < count; index++) {
			ofSaveImage(images[index], path + "PNG-MT" + ofToString(index) + ".png");
		}
	}));

	tests.push_back(TestItem("BMP Single Threaded", [] (vector<ofPixels>& images, string path) {
		int index = 0;
		for(auto & image : images) {
			ofSaveImage(image, path + "BMP-ST" + ofToString(index) + ".bmp");
			index++;
		}
	}));

	tests.push_back(TestItem("BMP Multi Threaded", [] (vector<ofPixels>& images, string path) {
		int count = images.size();
#pragma omp parallel for
		for(int index = 0; index < count; index++) {
			ofSaveImage(images[index], path + "BMP-MT" + ofToString(index) + ".bmp");
		}
	}));

	tests.push_back(TestItem("RAW Single Threaded", [] (vector<ofPixels>& images, string path) {
		int count = images.size();
		for(int index = 0; index < count; index++) {
			auto & image = images[index];
			string filename = "BMP-ST" + ofToString(index) + ".bmp";
			ofstream file(ofToDataPath(path + filename).c_str(), ios::binary | ios::out);
			file.write((char*)image.getPixels(), image.getWidth() * image.getHeight() * 3);
		}
	}));

	tests.push_back(TestItem("RAW Multi Threaded", [] (vector<ofPixels>& images, string path) {
		int count = images.size();
#pragma omp parallel for
		for(int index = 0; index < count; index++) {
			auto & image = images[index];
			string filename = "BMP-MT" + ofToString(index) + ".bmp";
			ofstream file(ofToDataPath(path + filename).c_str(), ios::binary | ios::out);
			file.write((char*)image.getPixels(), image.getWidth() * image.getHeight() * 3);
		}
	}));

	livePanel->onKeyboard += [this] (KeyboardArguments& args) {
		if (args.key == 'c') {
			this->captureFrames();
		}
		if (args.key == 't') {
			this->runTests();
		}
	};
}

//--------------------------------------------------------------
void testApp::update(){
	this->grabber->update();
}

//--------------------------------------------------------------
void testApp::draw(){
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void testApp::captureFrames() {
	ofThread sleeper;
	images.reserve(120);
	while (images.size() < 120) {
		this->grabber->update();
		if (this->grabber->isFrameNew()) {
			images.push_back(this->grabber->getPixelsRef());
		} else {
			sleeper.sleep(10);
		}
	}
}

//--------------------------------------------------------------
void testApp::runTests() {
	auto path = ofSystemLoadDialog("Select folder for disk speed tests", true, ofToDataPath("")).getPath();
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	ofLogNotice() << "Saving to " << path;
	for(auto test : tests) {
		ofLogNotice() << "Running test " + test.name;

		LARGE_INTEGER start;
		LARGE_INTEGER end;
		QueryPerformanceCounter(&start);
		for (int i=0; i<5; i++) {
			test.test(this->images, path);
		}
		QueryPerformanceCounter(&end);

		float duration = (float) (end.QuadPart - start.QuadPart) / (float) frequency.QuadPart;
		test.time = duration / float(5 * this->images.size());
		ofLogNotice() << "Test complete in " << duration << "s";
		ofLogNotice() << "Average " << test.time << "s per image";
	}
}