#pragma once
#include "ProjectTimeCone/Interpolation/GPUOpticalFlow.h"

#include "ofMain.h"

namespace ProjectTimeCone {
	namespace TimeSpacePick {
		struct TimeSpace {
			float time;
			float camera;
		};

		class FrameSet {
		public:
			void init(float screenWidth, float screenHeight);
			void load(string path);

			TimeSpace screenToTimeSpace(const ofVec2f &) const;
			ofVec2f TimeSpaceToScreen(const TimeSpace &) const;
			bool insideScreen(const ofVec2f&) const;

			static void getFrame(string filename, ofPixels &);
			bool getFrame(int camera, float time, ofPixels &);
			void getInterpolatedFrame(TimeSpace&, ofPixels&);

			ofFbo & getGridView();

		protected:
			bool cacheInterpolation(TimeSpace&);

			vector<map<float, string>> cameras;
			vector<string> cameraFolders;
			vector<float> timeOffsets;
			float minTimestamp, maxTimestamp;
			Poco::Path folder;
			ofFbo gridView;

			ofPtr<Interpolation::GPUOpticalFlow> interpolator;
			string cachedFilenameA;
			string cachedFilenameB;
			ofImage frameA;
			ofImage frameB;

			float screenWidth, screenHeight;
		};
	}
}