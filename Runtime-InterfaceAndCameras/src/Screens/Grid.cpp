#include "Grid.h"

using namespace ofxCvGui;
using namespace ofxAssets;

namespace Screens {
	//----------
	Grid::Grid() :
	framePath(frameSet) {
		this->restart = ofPtr<ofxCvGui::Utils::Button>(new ofxCvGui::Utils::Button());
		this->render = ofPtr<ofxCvGui::Utils::Button>(new ofxCvGui::Utils::Button());

		this->restart->setBounds(ofRectangle(50, 1080 + ((190 - 137) / 2) - 190, 137, 137));
		this->render->setBounds(ofRectangle(1920 - 50 - 137, 1080 + ((190 - 137) / 2) - 190, 137, 137));

		this->restart->onDrawUp += [] (DrawArguments&) {
			ofEnableAlphaBlending();
			image("restart_up").draw(0,0);
			ofDisableAlphaBlending();
		};
		this->restart->onDrawDown += [] (DrawArguments&) {
			ofEnableAlphaBlending();
			image("restart_hit").draw(0,0);
			ofDisableAlphaBlending();
		};
		this->restart->onHit += [this] (ofVec2f&) {
			if (this->framePath.size() <= 1) {
				ofEventArgs args;
				this->onRestart(args);
			} else {
				this->framePath.clear();
				this->render->disable();
			}
		};

		this->render->onDrawUp += [] (DrawArguments&) {
			ofEnableAlphaBlending();
			image("render_up").draw(0,0);
			ofDisableAlphaBlending();
		};
		this->render->onDrawDown += [] (DrawArguments&) {
			ofEnableAlphaBlending();
			image("render_hit").draw(0,0);
			ofDisableAlphaBlending();
		};
		this->render->onHit += [this] (ofVec2f&) {
			BuildArgs args;
			args.framePath = &this->framePath;
			this->onRender(args);
		};

		this->onMouse += [this] (MouseArguments& args) {
			if (args.action == MouseArguments::Pressed || args.action == MouseArguments::Dragged) {
				this->framePath.add(args.local);
				if (this->framePath.size() > 20 || this->framePath.getPositionOnPath(1.0f).x > 0.5f) {
					this->render->enable();
				}
			}
		};

		this->add(this->restart);
		this->add(this->render);
		this->render->disable();
	}
	
	//----------
	void Grid::init() {
		float y = 1080 - image("bottomBar_background").getHeight();
		//!!DEBUG
		//hard coded, should look up on image
		this->frameSet.init(1920, 900);
		//!!DEBUG
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

	//----------
	void Grid::clear() {
		this->framePath.clear();
		this->render->disable();
	}
}
