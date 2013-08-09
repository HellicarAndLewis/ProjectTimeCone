#include "Record.h"

using namespace ofxCvGui;
using namespace ofxMachineVision;

namespace Screens {
	//---------
	Record::Record(ofBaseDraws & preview) :
	preview(preview) {
		this->record = ofPtr<ofxCvGui::Utils::Button>(new ofxCvGui::Utils::Button());
		this->stop = ofPtr<ofxCvGui::Utils::Button>(new ofxCvGui::Utils::Button());
		
		this->record->setBounds(ofRectangle(50, (1080 - 180) + (180 - 100) / 2, 100, 100));
		this->stop->setBounds(ofRectangle(1920 - 150, (1080 - 180) + (180 - 100) / 2, 100, 100));

		this->record->onDrawUp += [] (DrawArguments & args) {
			ofxAssets::image("record_up").draw(0,0);
		};
		this->record->onDrawDown += [] (DrawArguments & args) {
			ofxAssets::image("record_hit").draw(0,0);
		};
		this->record->onHit += [this] (ofVec2f & args) {
			Poco::Path outputPath(ofToDataPath("D:\\DISK_STREAM\\", true));
			Poco::File(outputPath).remove(true);
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
			ofxAssets::image("stop_up").draw(0,0);
		};
		this->stop->onDrawDown += [] (DrawArguments & args) {
			ofxAssets::image("stop_hit").draw(0,0);
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