#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ProjectTimeCone::Initialisation::Initialiser::LoadCameras(this->controllers,
		[this] (ofPtr<Initialisation::CameraController> controller) { }, 1280, 800);

	this->record = ofPtr<Screens::Record>(new Screens::Record(this->controllers[0]->grabber->getTextureReference()));
	this->grid= ofPtr<Screens::Grid>(new Screens::Grid());
	this->render = ofPtr<Screens::Render>(new Screens::Render());
	this->upload = ofPtr<Screens::Upload>(new Screens::Upload());
	this->record->reset();
	this->grid->init();

	this->gui.setRoot(this->record);

	for(auto controller : this->controllers) {
		auto streamer = Screens::Record::Streamer(new ofxMachineVision::Stream::DiskStreamer());
		streamer->setGrabber(controller->grabber);
		this->record->addStreamer(streamer);
	}

	this->record->onFinishRecording += [this] (string & path) {
		this->gui.setRoot(grid);
		grid->load();
		grid->clear();
		this->record->reset();
	};

	this->grid->onRestart += [this] (ofEventArgs&) {
		this->gui.setRoot(record);
	};
	this->grid->onRender += [this] (Screens::Grid::BuildArgs& args) {
		this->gui.setRoot(render);
		render->render(args);
	};

	this->render->onComplete += [this] (ofEventArgs&) {
		this->gui.setRoot(upload);
		upload->load("frames_recording");
	};

	this->upload->onReview += [this] (ofEventArgs&) {
		this->gui.setRoot(this->grid);
		this->grid->clear();
	};

	this->upload->onUpload += [this] (ofEventArgs&) {
		this->gui.setRoot(this->record);
		this->record->reset();
	};

	ofSetFullscreen(true);
}

//--------------------------------------------------------------
void testApp::update(){
	if (this->gui.getRoot() == this->record) {
		this->controllers[0]->grabber->update();
	}
	this->upload->updateYouTube();
}

//--------------------------------------------------------------
void testApp::draw(){
	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 30, 10);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
 
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
}