#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	this->pyramidScale = 0.5;
	this->numLevels = 4;
	this->windowSize = 8;
	this->numIterations = 2;
	this->polyN = 7;
	this->polySigma = 1.5;
	this->useGaussian = false;
	this->viewScaleLow = -10.0f;
	this->viewScaleHigh = 10.0f;

	ofDirectory dir;
	dir.listDir(".");
	for(auto file : dir.getFiles()) {
		if (ofToUpper(file.getExtension()) == "PNG") {
			ofPixels pixels;
			ofLoadImage(pixels, file.getFileName());
			images.push_back(pixels);
		}
	}
	texture.allocate(images[0]);

	this->width = images[0].getWidth();
	this->height = images[0].getHeight();

	flow = Mat(this->height, this->width, CV_32FC2);
	flowPreview.allocate(this->width, this->height, OF_IMAGE_COLOR);

	gui.init();
	auto imagesPanel = gui.add(images, "Images");
	auto nextPanel = gui.add(images, "Next");
	auto flowPanel = gui.add(flowPreview, "Flow");
	flowPanel->setWidgets(this->controls);

	this->controls.addSlider("Pyramid scale", 0, 1, &this->pyramidScale);
	this->controls.addSlider("Number of levels", 1, 10, &this->numLevels);
	this->controls.addSlider("Window size", 1, 20, &this->windowSize);
	this->controls.addSlider("Iterations", 1, 10, &this->numIterations);
	this->controls.addSlider("Poly N", 1, 20, &this->polyN);
	this->controls.addSlider("Poly Sigma", 1, 2, &this->polySigma);
	this->controls.addToggle("Use Gaussian", &this->useGaussian);
	this->controls.addRangeSlider("View scale", -100.0f, 100.0f, &viewScaleLow, &viewScaleHigh);
	ofAddListener(this->controls.newGUIEvent, this, &testApp::onControls);
	imagesPanel->onSelectionChange.addListener([this, nextPanel] (int selection) {
		this->updateFlow(selection);
		nextPanel->setSelectionIndex(selection + 1);
		texture.loadData(images[selection]);
	}, this);
	imagesPanel->setSelectionIndex(0);

	auto meshPanel = PanelPtr(new Panels::Base());
	gui.add(meshPanel);
	meshPanel->onDraw.addListener([this, meshPanel] (DrawArguments& args) {
		ofPushMatrix();
		ofScale(meshPanel->getWidth() / this->width,
			meshPanel->getHeight() / this->height);
		texture.bind();
		pointCloud.drawVertices();
		texture.unbind();
		ofPopMatrix();
	}, this);
	meshPanel->onMouse.addListener([this] (MouseArguments& args) {
		if (args.isLocal() && (args.action == MouseArguments::Dragged || args.action == MouseArguments::Pressed)) {
			ofVec3f* vertices = this->pointCloud.getVerticesPointer();
			ofVec2f* displacement = (ofVec2f*) this->flow.data;
			for (int j=0; j<this->height; j++) {
				for(int i=0; i<this->width; i++) {
					*vertices++ = ofVec3f(i, j, 0) + *displacement++ * args.localNormalised.x;
				}
			}
		}
	}, this);

	for (int j=0; j<this->height; j++) {
		for(int i=0; i<this->width; i++) {
			this->pointCloud.addVertex(ofVec3f(i, j, 0));
			this->pointCloud.addTexCoord(ofVec2f(i, j));
		}
	}
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
	this->updateFlow();
}

//--------------------------------------------------------------
void testApp::updateFlow(int selection) {
	auto & lhs = images[selection];
	auto & rhs = images[(selection + 1) % this->images.size()];
	cvtColor(toCv(lhs), grayLHS, CV_RGB2GRAY);
	cvtColor(toCv(rhs), grayRHS, CV_RGB2GRAY);
	this->updateFlow();
}

//--------------------------------------------------------------
void testApp::updateFlow() {
	try {

		int flags = 0;//OPTFLOW_USE_INITIAL_FLOW;
		flags |= OPTFLOW_FARNEBACK_GAUSSIAN; //optional
		calcOpticalFlowFarneback(grayLHS, grayRHS, flow,
			this->pyramidScale, this->numLevels, this->windowSize, this->numIterations,
			this->polyN, this->polySigma, flags);

		const float scale = this->viewScaleHigh - this->viewScaleLow;
		float * input = (float*) flow.data;
		float * output = flowPreview.getPixels();
		for(int i=0; i<width; i++) {
			for(int j=0; j<height; j++) {
				*output++ = (*input++ - this->viewScaleLow) / scale;
				*output++ = (*input++ - this->viewScaleLow) / scale;
				*output++ = -*(output-1);
			}
		}
		flowPreview.update();
	} catch (cv::Exception e) {

	}
}