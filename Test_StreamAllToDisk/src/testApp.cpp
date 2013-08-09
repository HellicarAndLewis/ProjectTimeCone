#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
#ifdef BUILD_DEBUG
	ofSystemAlertDialog("You are building in debug mode, this app may not work properly if not built with optimisations turned on");
#endif

	this->exposure = 500;
	this->gain = 5;
	this->focus = 0;
	this->doRecord = false;
	this->clearLine = false;

	gui.init();
	auto dialogResult = ofSystemLoadDialog("Select output folder", true);
	if (!dialogResult.bSuccess) {
		ofExit();
	}
	this->outputFolder = Poco::Path(dialogResult.getPath() + "/");

	ProjectTimeCone::Initialisation::Initialiser::LoadCameras(this->controllers, [this] (ofPtr<ProjectTimeCone::Initialisation::CameraController> controller) {
		auto grabber = controller->grabber;
		PanelPtr panel = this->gui.add(grabber->getTextureReference(), grabber->getModelName());
		panel->onDraw.addListener([this, grabber, panel] (DrawArguments & args) {
			AssetRegister.drawText(ofToString(grabber->getFps()), 20, 60);

			ofPushMatrix();
			ofScale(panel->getWidth(), panel->getHeight(), 1.0f);
			ofPushStyle();
			ofSetLineWidth(2.0f);
			ofSetColor(255);
			ofNoFill();
			this->line.draw();
			ofPopStyle();
			ofPopMatrix();
		}, this);
		panel->onMouse += [this] (MouseArguments& args) {
			if (args.isLocal() && (args.action == MouseArguments::Pressed || args.action == MouseArguments::Dragged)) {
				this->line.addVertex(args.localNormalised);
			}
		};

		auto streamer = ofPtr<Stream::DiskStreamer>(new Stream::DiskStreamer());
		streamer->setGrabber(grabber);
		this->streamers.push_back(streamer);
	}, 1280, 720);

	
	this->controlPanel = PanelPtr(new Panels::Base());
	gui.add(this->controlPanel);
	this->controls.setPosition(10, 60);
	this->controlPanel->setWidgets(this->controls);
	this->controls.addFPS();
	this->controls.addToggle("Record", &this->doRecord);
	this->controls.addButton("Clear line", &this->clearLine);
	ofAddListener(this->controls.newGUIEvent, this, &testApp::onControls);
}

//--------------------------------------------------------------
void testApp::update(){
	if (ofGetFrameNum() == 0) {
		ofSetWindowPosition(0,0);
		ofSetWindowShape(1080*2, 1920);
	}

	for(auto controller : this->controllers) {
		controller->grabber->update();
	}

	if (this->clearLine) {
		this->clearLine = false;
		this->line.clear();
	}
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
void testApp::onControls(ofxUIEventArgs & args) {
	for(auto controller : controllers) {
		if (args.widget->getName() == "Record") {
			if (this->doRecord) {
				auto thisRecordingPath = this->outputFolder;
				thisRecordingPath.pushDirectory(ofToString(ofGetHours()) + "." + ofToString(ofGetMinutes()));
				for (int i=0; i<streamers.size(); i++) {
					auto path = thisRecordingPath;
					path.pushDirectory(ofToString(i));

					auto device = (Device::VideoInputDevice*) this->controllers[i]->grabber->getDevice().get();
					device->resetTimestamp();
					streamers[i]->setOutputFolder(path.toString());
					streamers[i]->start();
				} 
			} else {
				for(auto streamer : streamers) {
					streamer->stop();
				}
			}
		}
	}
}