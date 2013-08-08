#include "Initialiser.h"

using namespace ofxMachineVision;

namespace ProjectTimeCone {
	namespace Initialisation {
#pragma mark CameraController
		//--------------------------------------------------------------
		CameraController::CameraController(int index, ofPtr<ofxMachineVision::Grabber::Simple> grabber) {
			this->index = index;
			this->grabber = grabber;
			this->setAllFromThis = false;
			this->load();
		}

		//--------------------------------------------------------------
		void CameraController::onControlChange(ofxUIEventArgs & args) {
			if(args.widget->getName() == "Exposure") {
				this->setExposure(this->exposure, true);
			} else if (args.widget->getName() == "Gain") {
				this->setGain(this->gain, true);
			} else if (args.widget->getName() == "Focus") {
				this->setFocus(this->focus, true);
			}
		}

		//--------------------------------------------------------------
		void CameraController::setExposure(float exposure, bool setOthers) {
			this->exposure = exposure;
			this->grabber->setExposure(exposure);
			if (setOthers && this->setAllFromThis) {
				this->onExposureChangeAll(exposure);
			}
			this->save();
		}

		//--------------------------------------------------------------
		void CameraController::setGain(float gain, bool setOthers) {
			this->gain = gain;
			this->grabber->setGain(gain);
			if (setOthers && this->setAllFromThis) {
				this->onGainChangeAll(gain);
			}
			this->save();
		}

		//--------------------------------------------------------------
		void CameraController::setFocus(float focus, bool setOthers) {
			this->focus = focus;
			this->grabber->setFocus(focus);
			if (setOthers && this->setAllFromThis) {
				this->onFocusChangeAll(focus);
			}
			this->save();
		}

		//--------------------------------------------------------------
		string CameraController::getFilename() const {
			return "../../../Data/camera" + ofToString(this->index) + ".xml";
		}

		//--------------------------------------------------------------
		void CameraController::load() {
			if (this->xml.load(this->getFilename())) {
				;
				this->setExposure(this->xml.getValue<float>("//Exposure"), false);
				this->setGain(this->xml.getValue<float>("//Gain"), false);
				this->setFocus(this->xml.getValue<float>("//Focus"), false);
				xml.setTo("//CameraSettings");
			} else {
				this->xml.clear();
				this->xml.addChild("CameraSettings");
				this->xml.setTo("//CameraSettings");
				this->xml.addValue("Exposure", ofToString(this->exposure));
				this->xml.addValue("Gain", ofToString(this->gain));
				this->xml.addValue("Focus", ofToString(this->focus));
				this->exposure = 500;
				this->focus = 0;
				this->gain = 0;
			}
		}

		//--------------------------------------------------------------
		void CameraController::save() {
			this->xml.setTo("//CameraSettings");
			this->xml.setValue("Exposure", ofToString(this->exposure));
			this->xml.setValue("Gain", ofToString(this->gain));
			this->xml.setValue("Focus", ofToString(this->focus));
			this->xml.save(this->getFilename());
		}
#pragma mark Initialiser
		//---------
		void Initialiser::LoadCameras(std::vector<ofPtr<CameraController>> & controllers, std::function<void (ofPtr<CameraController>)> functor,
			int width, int height) {
			//order is a vector where
			// - order of vector is order of devices
			// - content of vector is index of devices
			vector<int> order;
			try {
				ofFile load(ORDER_FILENAME, ofFile::ReadOnly, true);
				int count = load.getPocoFile().getSize() / sizeof(int);
				order.resize(count);
				load.read((char*) &order[0], sizeof(int) * count);
				load.close();
			} catch(...) {
				ofSystemAlertDialog("Couldn't load camera order, please place save.bin into data folder.");
				std::exit(1);
			}

			videoInput deviceEnumerator;
			auto deviceCount = deviceEnumerator.listDevices();
			if (deviceCount != order.size()) {
				ofSystemAlertDialog("We've saved the order for a different number of cameras");
				std::exit(1);
			}

			controllers.resize(deviceCount);
		
			int index = 0;
			for(auto deviceIndex : order) {
				auto device = DevicePtr(new Device::VideoInputDevice(1280, 720));
				auto grabber = ofPtr<Grabber::Simple>(new Grabber::Simple(device));

				grabber->open(deviceIndex);
				grabber->startCapture();

				auto controller = ofPtr<CameraController>(new CameraController(index, grabber));
				functor(controller);

				controllers[index] = controller;
				index++;
			}
		}

		//---------
		void Initialiser::FillDevelopmentScreens() {
			ofSetWindowPosition(1920,0);
			ofSetWindowShape(1080 * 2, 1920);
		}
	}
}