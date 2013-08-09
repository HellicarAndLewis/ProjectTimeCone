#pragma once
#include "ProjectTimeCone.h"
#include "Base.h"
#include "ofxAssets.h"

namespace Screens {
	class Upload : public Base {
	public:
		Upload();
		void load(const string & folder);
		void update();
		void updateYouTube();
		void draw(ofxCvGui::DrawArguments &) override;
		ofxCvGui::Utils::LambdaStack<ofEventArgs> onReview;
		ofxCvGui::Utils::LambdaStack<ofEventArgs> onUpload;
	protected:
		ofPtr<ofxCvGui::Utils::Button> review;
		ofPtr<ofxCvGui::Utils::Button> upload;

		vector<string> filenames;
		ofImage preview;
		float startTime;
		ProjectTimeCone::YouTube::EncodeAndUpload youTube;
		string path;
	};
}