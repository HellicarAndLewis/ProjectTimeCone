#pragma once
#include <functional>
#include <vector>

#include "ofMain.h"
#include "ofxMachineVision.h"
#include "ofxCvGui.h"

#define ORDER_FILENAME "../../../Data/order.bin"

namespace ProjectTimeCone {
	namespace Initialisation {
		class CameraController {
		public:
			CameraController(int index, ofPtr<ofxMachineVision::Grabber::Simple> grabber);
			void onControlChange(ofxUIEventArgs &);

			void setExposure(float, bool allowTriggerOthers);
			void setGain(float, bool allowTriggerOthers);
			void setFocus(float, bool allowTriggerOthers);
	
			string getFilename() const;
			void load();
			void save();

			ofxCvGui::Utils::LambdaStack<float> onExposureChangeAll;
			ofxCvGui::Utils::LambdaStack<float> onGainChangeAll;
			ofxCvGui::Utils::LambdaStack<float> onFocusChangeAll;

			int index;
			float exposure;
			float gain;
			float focus;
			bool setAllFromThis;
			ofXml xml;

			ofPtr<ofxUICanvas> controls;
			ofPtr<ofxMachineVision::Grabber::Simple> grabber;
		};

		class Initialiser {
		public:
			static void LoadCameras(std::vector<ofPtr<CameraController>>&, std::function<void (ofPtr<CameraController>)>,
				int width, int height);

			static void FillDevelopmentScreens();
		};
	}
}