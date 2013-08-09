#pragma once
#include "ProjectTimeCone.h"
#include "Base.h"

namespace Screens {
	class Grid : public Screens::Base {
	public:
		Grid();
		void draw(ofxCvGui::DrawArguments &) override;
		void load();

		ofxCvGui::Utils::LambdaStack<ofEventArgs> onRestart;
		ofxCvGui::Utils::LambdaStack<ofEventArgs> onComplete;
	protected:
		ofPtr<ofxCvGui::Utils::Button> restart;
		ofPtr<ofxCvGui::Utils::Button> render;

		ProjectTimeCone::TimeSpacePick::FrameSet frameSet;
		ProjectTimeCone::TimeSpacePick::FramePath framePath;
	};
}