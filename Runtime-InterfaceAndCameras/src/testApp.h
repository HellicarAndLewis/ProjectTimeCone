#pragma once
#define NO_YOUTUBE
#include "ProjectTimeCone.h"
#include "ofMain.h"
#include "ofxCvGui.h"
#include "ofxMachineVision.h"

#include "Screens/Controller.h"
#include "Screens/Record.h"
#include "Screens/Grid.h"
#include "Screens/Render.h"

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

		Screens::Controller gui;
};
