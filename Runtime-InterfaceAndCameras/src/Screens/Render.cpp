#include "Render.h"
#include "Poco/DirectoryIterator.h"

using namespace ofxAssets;

namespace Screens {
	//---------
	Render::Render() {
		this->onUpdate += [this] (ofxCvGui::UpdateArguments&) {
			this->update();
		};
	}

	//---------
	void Render::render(Grid::BuildArgs& args) {
		this->buffer.allocate(1280, 720, OF_IMAGE_COLOR);
		this->framePath = args.framePath;
		
		//--
		//delete old files
		//
		Poco::Path path(ofToDataPath("frames_recording/", true));
		for(auto dirIt = Poco::DirectoryIterator(path); dirIt != Poco::DirectoryIterator(); dirIt++) {
			dirIt->remove(true);
		}
		//
		//--

		this->buildList = this->framePath->getBuildList("frames_recording/", FRAME_COUNT);
	}

	//---------
	void Render::update() {
		auto start = ofGetElapsedTimef();
		bool first = true; //should be unneccessary, but hey, just incase :)
		if (!this->buildList.empty()) {
			while (ofGetElapsedTimef() - start < (1.0f / 60.0f) || first) {
				auto buildItem = this->buildList.front();
				this->buildList.pop_front();
				this->framePath->buildFrame(buildItem, this->buffer);
				cout << buildItem.filename << endl;
				first = false;
			}
			if (this->buildList.empty()) {
				ofEventArgs args;
				this->onComplete(args);
			}
		}
	}

	//---------
	void Render::draw(ofxCvGui::DrawArguments& args) {
		image("progress_background").draw(0,0);

		//negative progress
		float progress = (float) buildList.size() / ((float) FRAME_COUNT);
		ofPushMatrix();
		ofTranslate(-1920.0f * progress, 0.0f);
		image("progress_bar").draw(0, 0.0f);
		ofPopMatrix();

		ofEnableAlphaBlending();
		image("progress_foreground").draw(0,0);
		ofDisableAlphaBlending();
	}
}