#pragma once
#include "ProjectTimeCone.h"
#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

#include "Screens/Controller.h"
#include "Screens/Record.h"
#include "Screens/Grid.h"
#include "Screens/Render.h"
#include "Screens/Upload.h"

using namespace ofxCvGui;
using namespace ofxMachineVision;
using namespace ProjectTimeCone;

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void keyPressed( int key );
		
		vector<ofPtr<Initialisation::CameraController>> controllers;

		ofPtr<Screens::Record> record;
		ofPtr<Screens::Grid> grid;
		ofPtr<Screens::Render> render;
		ofPtr<Screens::Upload> upload;

		Screens::Controller gui;
};
