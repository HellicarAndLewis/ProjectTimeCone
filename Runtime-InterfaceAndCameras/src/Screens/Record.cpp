#include "ProjectTimeCone.h"
#include "Record.h"

using namespace ofxCvGui;
using namespace ofxMachineVision;

namespace Screens {
	//---------
	Record::Record(ofBaseDraws & preview) :
	preview(preview) {
		this->onUpdate += [this] (UpdateArguments&) {
			this->update();
		};

		this->record = ofPtr<ofxCvGui::Utils::Button>(new ofxCvGui::Utils::Button());
		this->stop = ofPtr<ofxCvGui::Utils::Button>(new ofxCvGui::Utils::Button());
		
		this->record->setBounds(ofRectangle(50, 1080 + ((190 - 137) / 2) - 190, 137, 137));
		this->stop->setBounds(ofRectangle(1920 - 50 - 137, 1080 + ((190 - 137) / 2) - 190, 137, 137));

		this->record->onDrawUp += [] (DrawArguments & args) {
			ofEnableAlphaBlending();
			ofxAssets::image("record_up").draw(0,0);
			ofDisableAlphaBlending();
		};
		this->record->onDrawDown += [] (DrawArguments & args) {
			ofEnableAlphaBlending();
			ofxAssets::image("record_hit").draw(0,0);
			ofDisableAlphaBlending();
		};
		this->record->onHit += [this] (ofVec2f & args) {
			Poco::Path outputPath(ofToDataPath("D:\\DISK_STREAM\\", true));
			auto file = Poco::File(outputPath);
			if (file.exists()) {
				file.remove(true);
			}
			this->path = outputPath.toString();
			int index = 0;
			for(auto streamer : streamers) {
				auto path = outputPath;
				path.pushDirectory(ofToString(index));

				auto device = (Device::VideoInputDevice*) streamer->getGrabber()->getDevice().get();
				device->resetTimestamp();

				streamer->setOutputFolder(path.toString());
				streamer->start();
				index++;
			}

			this->record->disable();
			this->stop->enable();
		};

		this->stop->onDrawUp += [] (DrawArguments & args) {
			ofEnableAlphaBlending();
			ofxAssets::image("stop_up").draw(0,0);
			ofDisableAlphaBlending();
		};
		this->stop->onDrawDown += [] (DrawArguments & args) {
			ofEnableAlphaBlending();
			ofxAssets::image("stop_hit").draw(0,0);
			ofDisableAlphaBlending();
		};
		this->stop->onHit += [this] (ofVec2f & args) {
			for(auto streamer : this->streamers) {
				streamer->stop();
			}
			this->onFinishRecording(this->path);
		};

		this->add(this->record);
		this->add(this->stop);
		this->stop->disable();
	}

	//---------
	void Record::update() {
		if (this->stop->getEnabled()) {
			auto space = ProjectTimeCone::Utils::Disk::getFreeSpace("D:\\");
			if (space < 10000000000) {
				ofVec2f dummy;
				this->stop->onHit(dummy);
			}
		}
	}

	//---------
	void Record::draw(ofxCvGui::DrawArguments & args) {
		this->preview.draw(0,0,this->getWidth(), this->getHeight());
	
		auto & bottomBar = ofxAssets::image("bottomBar_background");
		bottomBar.draw(0, this->getHeight() - bottomBar.getHeight());
	}

	//---------
	void Record::reset() {
		this->record->enable();
		this->stop->disable();
	}

	//---------
	void Record::addStreamer(Streamer streamer) {
		this->streamers.push_back(streamer);
	}
}