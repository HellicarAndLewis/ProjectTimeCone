#pragma once
#include "ofMain.h"
#include "ofxCvGui.h"

namespace Screens {
	class Controller {
	public:
		Controller();
		~Controller();
		void addListeners();
		void removeListeners();

		void setRoot(ofxCvGui::ElementPtr);
		ofxCvGui::ElementPtr getRoot();
	protected:
		////
		//actions
		////
		//
		void update(ofEventArgs &args);
		void draw(ofEventArgs &args);
		void mouseMoved(ofMouseEventArgs &args);
		void mousePressed(ofMouseEventArgs &args);
		void mouseReleased(ofMouseEventArgs &args);
		void mouseDragged(ofMouseEventArgs &args);
		void keyPressed(ofKeyEventArgs &args);
		void windowResized(ofResizeEventArgs &args);
		//
		////

		ofxCvGui::ElementPtr root;
		ofVec2f mouseCached;
	};
}