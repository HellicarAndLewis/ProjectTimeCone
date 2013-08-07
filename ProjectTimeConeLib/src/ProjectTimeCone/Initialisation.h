#pragma once
#include <functional>
#include <vector>

#include "ofMain.h"
#include "ofxMachineVision.h"

#define ORDER_FILENAME "../../../Data/order.bin"

namespace ProjectTimeCone {
	class Initialisation {
	public:
		class InitialisedCameraArguments {
		public:
			InitialisedCameraArguments(int index, ofPtr<ofxMachineVision::Grabber::Simple> camera);
			int index;
			ofPtr<ofxMachineVision::Grabber::Simple> camera;
		};
		static void LoadCameras(std::vector<ofPtr<ofxMachineVision::Grabber::Simple>>&, std::function<void (const InitialisedCameraArguments&)>,
			int width, int height);

		static void FillDevelopmentScreens();
	};
};