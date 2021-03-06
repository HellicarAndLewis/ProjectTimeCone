#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	this->width = 1280;
	this->height = 720;

	ofDirectory dir;
	dir.listDir(".");
	for(auto file : dir.getFiles()) {
		if (ofToUpper(file.getExtension()) == "PNG") {
			ofPixels pixels;
			ofLoadImage(pixels, file.getFileName());
			images.push_back(pixels);
		}
	}

	A = this->images[0];
	B = this->images[1];

	A.update();
	B.update();

	this->interpolation = ofPtr<Interpolation::OpticalFlow>(new Interpolation::GPUOpticalFlow(this->width, this->height));
	this->interpolation->UpdateFlow(A, B);

	this->gui.init();
	this->gui.add(A, "A");
	this->gui.add(B, "B");
	this->gui.add(interpolation->getAtoB(), "A to B");
	this->gui.add(interpolation->getBtoA(), "B to A");
	this->outputPanel = this->gui.add(interpolation->getResultFbo(), "Output");

	this->outputPanel->onKeyboard += [this] (KeyboardArguments & args) {
		if (args.key == ' ') {
			this->interpolation->reload();
		}
		if (args.key == 'o') {
			Profiler.clear();
			this->interpolation->UpdateFlow(A, B);
			this->interpolation->UpdateResult(x, this->A.getTextureReference(), this->B.getTextureReference());
			cout << Profiler.getResultsString();
		}
	};

	this->outputPanel->onMouse += [this] (MouseArguments & args) {
		this->x = args.localNormalised.x;
		this->interpolation->UpdateResult(x, this->A.getTextureReference(), this->B.getTextureReference());
	};
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void testApp::onControls(ofxUIEventArgs & args) {

}