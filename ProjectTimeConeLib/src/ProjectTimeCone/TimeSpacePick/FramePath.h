#pragma once
#include "FrameSet.h"
#include "ofMain.h"

namespace ProjectTimeCone {
	namespace TimeSpacePick {
		class FramePath {
		public:
			FramePath(FrameSet&);
			void clear();
			ofVec2f lengthToScreen(float position) const;
			ofVec2f getPositionOnPath(float normalised);
			size_t size() const;
			void drawPath();

			void add(const ofVec2f& screen);

			void buildFrames(string path, int length, ofImage& buffer);
		protected:
			FrameSet& frameSet;
			map<float, ofVec2f> positions;
			float totalLength;
		};
	}
}