#pragma once
#include "ofxCvGui.h"
#include "ofxAssets.h"

namespace Screens {
	class Base : public ofxCvGui::ElementGroup {
	public:
		Base();
		~Base();
		void setActive(bool active);
		bool getActive() const;
	protected:
		virtual void draw(ofxCvGui::DrawArguments &) = 0;
		bool active;
	};
}