#pragma once
#include "Grid.h"
#include "Base.h"

#include "ofxMachineVision.h"

#define FRAME_COUNT 25 * 10

namespace Screens {
	class Render : public Base {
	public:
		Render();
		void render(Grid::BuildArgs&);
		void update();
		void draw(ofxCvGui::DrawArguments &) override;
		ofxCvGui::Utils::LambdaStack<ofEventArgs&> onComplete;
	protected:
		ProjectTimeCone::TimeSpacePick::FramePath* framePath;
		ProjectTimeCone::TimeSpacePick::FramePath::BuildList buildList;
		ofImage buffer;
	};
}