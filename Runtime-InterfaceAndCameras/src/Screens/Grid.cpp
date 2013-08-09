#include "Grid.h"

using namespace ofxCvGui;
using namespace ofxAssets;

namespace Screens {
	//----------
	Grid::Grid() :
	framePath(frameSet) {
		this->restart = ofPtr<ofxCvGui::Utils::Button>(new ofxCvGui::Utils::Button());
		this->render = ofPtr<ofxCvGui::Utils::Button>(new ofxCvGui::Utils::Button());

		this->restart->setBounds(ofRectangle(50, (1080 - 180) + (180 - 100) / 2, 100, 100));
		this->render->setBounds(ofRectangle(1920 - 150, (1080 - 180) + (180 - 100) / 2, 100, 100));

		this->restart->onDrawUp += [] (DrawArguments&) {
			image("restart_up").draw(0,0);
		};
		this->restart->onDrawDown += [] (DrawArguments&) {
			image("restart_hit").draw(0,0);
		};
		this->restart->onHit += [this] (ofVec2f&) {
			if (this->framePath.size() == 0) {
				ofEventArgs args;
				this->onRestart(args);
			} else {
				this->framePath.clear();
				this->render->disable();
			}
		};

		this->render->onDrawUp += [] (DrawArguments&) {
			image("render_up").draw(0,0);
		};
		this->render->onDrawDown += [] (DrawArguments&) {
			image("render_hit").draw(0,0);
		};

		this->onMouse += [this] (MouseArguments& args) {
			if (args.action == MouseArguments::Pressed || args.action == MouseArguments::Dragged) {
				this->framePath.add(args.local);
				if (this->framePath.size() > 50) {
					this->render->enable();
				}
			}
		};

		this->frameSet.init(1920, 1080 - image("bottomBar_background").getHeight());

		this->add(this->restart);
		this->add(this->render);
		this->render->disable();
	}

	//----------
	void Grid::draw(DrawArguments &) {
		this->frameSet.getGridView().draw(0,0);
		this->framePath.drawPath();

		auto & bottomBar = ofxAssets::image("bottomBar_background");
		bottomBar.draw(0, this->getHeight() - bottomBar.getHeight());
	}

	//----------
	void Grid::load() {
		this->frameSet.load("D:\\DISK_STREAM\\");
	}
}
