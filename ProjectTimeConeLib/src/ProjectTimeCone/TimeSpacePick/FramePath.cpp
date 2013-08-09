#include "FramePath.h"

namespace ProjectTimeCone {
	namespace TimeSpacePick {
		//----------
		FramePath::FramePath(FrameSet& frameSet) :
		frameSet(frameSet) {
			this->clear();
		}

		//----------
		void FramePath::clear() {
			this->positions.clear();
			this->add(ofVec2f(0,0));
		}

		//----------
		ofVec2f FramePath::lengthToScreen(float position) const {
			// no points
			if (this->positions.size() == 0) {
				ofLogError() << "We have no positions available to look up against.";
				return ofVec2f();
			}
	
			// bang on the nose
			auto & exactPoint = this->positions.find(position);
			if (exactPoint != this->positions.end()) {
				return exactPoint->second;
			}

			auto & nextPoint = this->positions.upper_bound(position);

			//we're off the end
			if (nextPoint == this->positions.end()) {
				nextPoint--;
				return nextPoint->second;
			}

			//we're off the beginning
			if (nextPoint == this->positions.begin()) {
				return nextPoint->second;
			}

			//interpolate
			auto previousPoint = nextPoint;
			previousPoint--;
	
			ofVec2f interpolated;

			interpolated.x = ofMap(position,
				previousPoint->first, nextPoint->first,
				previousPoint->second.x, nextPoint->second.x);
		
			interpolated.y = ofMap(position,
				previousPoint->first, nextPoint->first,
				previousPoint->second.y, nextPoint->second.y);

			return interpolated;
		}
		
		//----------
		ofVec2f FramePath::getPositionOnPath(float normalised) {
			return this->lengthToScreen(normalised * totalLength);
		}

		//----------
		size_t FramePath::size() const {
			return this->positions.size();
		}

		//----------
		void FramePath::drawPath() {
			//--
			//gray bar
			//
			float lastX = 0.0f;
			for(auto & point : this->positions) {
				lastX = point.second.x;
			}
			ofPushStyle();
			ofSetColor(0,0,0,100.0f);
			ofEnableAlphaBlending();
			ofRect(0,0,lastX,1080.0f);
			ofDisableAlphaBlending();
			ofPopStyle();
			//
			//--
			
			
			//--
			//path
			//
			ofPushStyle();
			ofNoFill();
			ofSetLineWidth(3.0f);
			ofBeginShape();
			ofSetColor(255, 100, 100);
			for(auto & point : this->positions) {
				ofVertex(point.second.x, point.second.y);
			}
			ofEndShape(false);
			ofPopStyle();
			//
			//--
		}

		//----------
		void FramePath::add(const ofVec2f& constScreen) {
			if (!this->frameSet.insideScreen(constScreen)) {
				return;
			}

			ofVec2f screen = constScreen;
			if (this->positions.size() > 0) {
				auto last = this->positions.end();
				last--;
				if (screen.x <= last->second.x) {
					screen.x = last->second.x;
				}

				//perform check on recomputed length in case we clamped
				float length = (last->second - screen).length();

				totalLength += length;
			} else {
				totalLength = 0;
			}
			positions.insert(pair<float,ofVec2f>(totalLength, screen));
		}

		//----------
		void FramePath::buildFrames(string path, int length, ofImage& buffer) {
			Poco::Path outputPath(ofToDataPath(path));
			Poco::File(outputPath).remove(true);
			Poco::File(outputPath).createDirectories();

			for (int frame=0; frame<length; frame++) {
				auto screen = this->lengthToScreen(this->totalLength * (float) frame / (float) length);
				auto timeSpace = this->frameSet.screenToTimeSpace(screen);
				this->frameSet.getInterpolatedFrame(timeSpace, buffer);

				string frameString = ofToString(frame);
				while(frameString.length() < 4) {
					frameString = "0" + frameString;
				}

				frameString = "frame_" + frameString;
				auto filename = outputPath.toString() + frameString + ".jpg";

				buffer.saveImage(filename);
				cout << frameString << endl;
			}
		}
	}
}