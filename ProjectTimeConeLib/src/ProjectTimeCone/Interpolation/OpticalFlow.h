#pragma once

#include "Base.h"
#include "ofxCv.h"

namespace ProjectTimeCone {
	namespace Interpolation {
		class OpticalFlow : public Base {
		public:
			OpticalFlow(int width, int height);
			void Interpolate(float x, ofPixels & A, ofPixels & B, ofPixels & result) override;

			float pyramidScale;
			float numLevels;
			float windowSize;
			float numIterations;
			float polyN;
			float polySigma;
			bool useGaussian;
		protected:
			int width, height;
			cv::Mat A, B;
			cv::Mat AtoB, BtoA;
			
			ofFbo fbo;
			ofShader displace;
			ofMesh mesh;
		};
	}
}