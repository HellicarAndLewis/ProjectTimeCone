#pragma once
#include <functional>
#include <vector>

#include "ofMain.h"
#include "ofxMachineVision.h"

#define ORDER_FILENAME "../../../Data/order.bin"

namespace ProjectTimeCone {
	class Initialisation {
	public:
		static void LoadCameras(std::vector<ofPtr<ofxMachineVision::Grabber::Simple>>&, std::function<void (int, ofPtr<ofxMachineVision::Grabber::Simple>)>,
			float exposure, float gain, float focus,
			int width, int height);

		static void FillDevelopmentScreens();
	};
};