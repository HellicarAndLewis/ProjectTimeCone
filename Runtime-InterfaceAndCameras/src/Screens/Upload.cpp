#include "Upload.h"
#include "Poco/DirectoryIterator.h"

using namespace ofxAssets;
using namespace ofxCvGui;
namespace Screens {
	//---------
	Upload::Upload() {
		this->onUpdate += [this] (ofxCvGui::UpdateArguments&) {
			this->update();
		};

		this->review = ofPtr<ofxCvGui::Utils::Button>(new ofxCvGui::Utils::Button());
		this->upload = ofPtr<ofxCvGui::Utils::Button>(new ofxCvGui::Utils::Button());
		
		this->review->setBounds(ofRectangle(50, (1080 - 180) + (180 - 100) / 2, 100, 100));
		this->upload->setBounds(ofRectangle(1920 - 150, (1080 - 180) + (180 - 100) / 2, 100, 100));

		this->review->onDrawUp += [] (DrawArguments & args) {
			ofxAssets::image("review_up").draw(0,0);
		};
		this->review->onDrawDown += [] (DrawArguments & args) {
			ofxAssets::image("review_hit").draw(0,0);
		};
		this->review->onHit += [this] (ofVec2f&) {
			ofEventArgs args;
			this->onReview(args);
		};

		this->upload->onDrawUp += [] (DrawArguments & args) {
			ofxAssets::image("upload_up").draw(0,0);
		};
		this->upload->onDrawDown += [] (DrawArguments & args) {
			ofxAssets::image("upload_hit").draw(0,0);
		};
		this->upload->onHit += [this] (ofVec2f&) {
			this->youTube.encodeFrames(this->path, true);
			ofEventArgs args;
			this->onUpload(args);
		};

		if (!this->youTube.setAudioFile("audio.mp3", true)) {
			ofSystemAlertDialog("Audio file missing");
		}
		if (!this->youTube.setFramesHeadDir("frames_head", true)) {
			ofSystemAlertDialog("Header frames missing");
		}
		if (!this->youTube.setFramesTailDir("frames_tail", true)) {
			ofSystemAlertDialog("Tail frames missing");
		}

		this->add(this->review);
		this->add(this->upload);
	}

	//---------
	void Upload::load(const string& folder) {
		this->filenames.clear();
		Poco::Path path(ofToDataPath(folder + "/",true));
		auto dirIt = Poco::DirectoryIterator(path);
		auto dirEnd = Poco::DirectoryIterator();
		for(; dirIt != dirEnd; dirIt++) {
			this->filenames.push_back(dirIt->path());
		}
		
		this->startTime = ofGetElapsedTimef();
		this->path = folder;
	}

	//---------
	void Upload::update() {
		int frame = (ofGetElapsedTimef() - this->startTime) * 25.0f;
		this->preview.loadImage(this->filenames[frame % this->filenames.size()]);
	}

	//---------
	void Upload::updateYouTube() {
		this->youTube.update();
	}

	//---------
	void Upload::draw(ofxCvGui::DrawArguments& args) {
		this->preview.draw(0,0,1920,1080);
		image("bottomBar_background").draw(0,900);
	}
}