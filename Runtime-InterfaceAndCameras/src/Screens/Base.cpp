#include "Base.h"

using namespace ofxCvGui;

namespace Screens {
	//-------
	Base::Base() {
		this->setBounds(ofRectangle(0,0,1920,1080));
		this->onDraw.addListener([this] (DrawArguments & args) {
			this->draw(args);
		}, -1, this);
	}

	//-------
	Base::~Base() {
		this->onDraw.removeListeners(this);
	}

	//-------
	bool Base::getActive() const {
		return this->active;
	}
}