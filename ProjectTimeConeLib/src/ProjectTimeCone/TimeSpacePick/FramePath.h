#pragma once
#include "FrameSet.h"
#include "ofMain.h"

namespace ProjectTimeCone {
	namespace TimeSpacePick {
		class FramePath {
		public:
			struct FrameToBuild {
				TimeSpace timeSpace;
				string filename;
			};
			typedef deque<FrameToBuild> BuildList;

			FramePath(FrameSet&);
			void clear();
			ofVec2f lengthToScreen(float position) const;
			ofVec2f getPositionOnPath(float normalised);
			size_t size() const;
			void drawPath();

			void add(const ofVec2f& screen);

			BuildList getBuildList(string path, int length) const;
			void buildFrames(string path, int length, ofImage& buffer) const;
			void buildFrame(FrameToBuild&, ofImage& buffer) const;
		protected:
			FrameSet& frameSet;
			map<float, ofVec2f> positions;
			float totalLength;
			ofImage buffer;
		};
	}
}