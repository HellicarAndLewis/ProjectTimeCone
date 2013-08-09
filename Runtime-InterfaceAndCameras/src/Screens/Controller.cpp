#include "Controller.h"

using namespace ofxCvGui;

namespace Screens {
	//----------
	Controller::Controller() {
		this->addListeners();
	}

	//----------
	Controller::~Controller() {
		this->removeListeners();
	}

	//----------
	void Controller::addListeners() {
		ofAddListener(ofEvents().update, this, &Controller::update);
		ofAddListener(ofEvents().draw, this, &Controller::draw);
		ofAddListener(ofEvents().mouseMoved, this, &Controller::mouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &Controller::mousePressed);
		ofAddListener(ofEvents().mouseReleased, this, &Controller::mouseReleased);
		ofAddListener(ofEvents().mouseDragged, this, &Controller::mouseDragged);
		ofAddListener(ofEvents().keyPressed, this, &Controller::keyPressed);	
		ofAddListener(ofEvents().windowResized, this, &Controller::windowResized);
	}

	//----------
	void Controller::removeListeners() {
		ofRemoveListener(ofEvents().update, this, &Controller::update);
		ofRemoveListener(ofEvents().draw, this, &Controller::draw);
		ofRemoveListener(ofEvents().mouseMoved, this, &Controller::mouseMoved);
		ofRemoveListener(ofEvents().mousePressed, this, &Controller::mousePressed);
		ofRemoveListener(ofEvents().mouseReleased, this, &Controller::mouseReleased);
		ofRemoveListener(ofEvents().mouseDragged, this, &Controller::mouseDragged);
		ofRemoveListener(ofEvents().keyPressed, this, &Controller::keyPressed);	
		ofRemoveListener(ofEvents().windowResized, this, &Controller::windowResized);
	}

	//----------
	void Controller::setRoot(ElementPtr root) {
		this->root = root;
	}

	//----------
	ElementPtr Controller::getRoot() {
		return this->root;
	}

	//----------
	void Controller::update(ofEventArgs& args) {
		if (this->root == ElementPtr())
			return;
		root->update();
	}

	//----------
	void Controller::draw(ofEventArgs& args) {
		if (this->root == ElementPtr())
			return;
		DrawArguments action(ofRectangle(0,0,ofGetWidth(),ofGetHeight()), true);
		root->draw(action);
	}

	//----------
	void Controller::mouseMoved(ofMouseEventArgs &args) {
		if (this->root == ElementPtr())
			return;
		MouseArguments action(MouseArguments(args, MouseArguments::Moved, root->getBounds(), this->root));
		this->root->mouseAction(action);
	}
	
	//----------
	void Controller::mousePressed(ofMouseEventArgs &args) {
		if (this->root == ElementPtr())
			return;
		MouseArguments action(MouseArguments(args, MouseArguments::Pressed, root->getBounds(), this->root));
		this->root->mouseAction(action);
        this->mouseCached = action.global;
	}
	
	//----------
	void Controller::mouseReleased(ofMouseEventArgs &args) {
		if (this->root == ElementPtr())
			return;
		MouseArguments action(args, MouseArguments::Released, root->getBounds(), this->root);
		this->root->mouseAction(action);
	}
	
	//----------
	void Controller::mouseDragged(ofMouseEventArgs &args) {
		if (this->root == ElementPtr())
			return;
        MouseArguments action(args, MouseArguments::Dragged, root->getBounds(), this->root, this->mouseCached);
		this->root->mouseAction(action);
        this->mouseCached = action.global;
	}
	
	//----------
	void Controller::keyPressed(ofKeyEventArgs &args) {
		if (this->root == ElementPtr())
			return;
		KeyboardArguments action(args, KeyboardArguments::Pressed, this->root);
		this->root->keyboardAction(action);
	}
	
	//----------
	void Controller::windowResized(ofResizeEventArgs &args) {
		if (this->root == ElementPtr())
			return;
		ofRectangle bounds(0,0,ofGetWidth(), ofGetHeight());
		root->setBounds(bounds);
	}

}