#pragma once
#include "Grid.h"
#include "Base.h"

#include "ofxMachineVision.h"

namespace Screens {
	class Render : public Base {
	public:
		void draw(ofxCvGui::DrawArguments &) override { }
		void render();
		ofxCvGui::Utils::LambdaStack<ofEventArgs&> onComplete;
	protected:
	};
}