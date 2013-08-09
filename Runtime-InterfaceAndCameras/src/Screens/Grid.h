#pragma once
#include "ProjectTimeCone.h"
#include "Base.h"

namespace Screens {
	class Grid : public Screens::Base {
	public:
		struct BuildArgs {
			ProjectTimeCone::TimeSpacePick::FramePath* framePath;
		};
		Grid();
		void init();
		void draw(ofxCvGui::DrawArguments &) override;
		void load();
		void clear();

		ofxCvGui::Utils::LambdaStack<ofEventArgs> onRestart;
		ofxCvGui::Utils::LambdaStack<BuildArgs> onRender;
	protected:
		ofPtr<ofxCvGui::Utils::Button> restart;
		ofPtr<ofxCvGui::Utils::Button> render;

		ProjectTimeCone::TimeSpacePick::FrameSet frameSet;
		ProjectTimeCone::TimeSpacePick::FramePath framePath;
	};
}