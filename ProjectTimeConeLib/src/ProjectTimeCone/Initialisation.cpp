#include "Initialisation.h"

using namespace ofxMachineVision;

namespace ProjectTimeCone {
	//---------
	Initialisation::InitialisedCameraArguments::InitialisedCameraArguments(int index, ofPtr<ofxMachineVision::Grabber::Simple> camera) {
		this->index = index;
		this->camera = camera;
	}

	//---------
	void Initialisation::LoadCameras(std::vector<ofPtr<Grabber::Simple>> & grabbers, std::function<void (const Initialisation::InitialisedCameraArguments&)> functor,
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

		grabbers.resize(deviceCount);
		
		int index = 0;
		for(auto deviceIndex : order) {
			auto device = DevicePtr(new Device::VideoInputDevice(1280, 720));
			auto grabber = ofPtr<Grabber::Simple>(new Grabber::Simple(device));

			grabber->open(deviceIndex);
			grabber->startCapture();

			functor(InitialisedCameraArguments(index, grabber));

			grabbers[index] = grabber;
			index++;
		}
	}

	//---------
	void Initialisation::FillDevelopmentScreens() {
		ofSetWindowPosition(1920,0);
		ofSetWindowShape(1080 * 2, 1920);
	}
}