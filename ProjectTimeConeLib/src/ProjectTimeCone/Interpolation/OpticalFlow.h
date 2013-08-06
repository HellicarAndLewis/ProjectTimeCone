#pragma once

#include "../Profiling/Timer.h"
#include "Base.h"
#include "ofxCv.h"

namespace ProjectTimeCone {
	namespace Interpolation {
		class OpticalFlow : public Base {
		public:
			OpticalFlow(int width, int height);
			
			void Interpolate(float x, ofPixels & A, ofPixels & B, ofPixels & result) override;
			
			void UpdateFlow(ofPixels & A, ofPixels & B);
			void UpdateResult(float x, ofTexture & A, ofTexture & B);
			void reload();

			ofFloatImage & getAtoB();
			ofFloatImage & getBtoA();

			ofFbo & getResultFbo();

			float pyramidScale;
			float numLevels;
			float windowSize;
			float numIterations;
			float polyN;
			float polySigma;
			bool useGaussian;
		protected:
			void drawPoints(float x, ofTexture & A, ofTexture & B);

			int width, height;
			cv::Mat A, B;
			cv::Mat AtoB, BtoA;
			
			ofFloatImage AtoBimage;
			ofFloatImage BtoAimage;

			ofTexture textureA, textureB;
			ofFbo left, right;
			ofFbo fbo;
			ofShader displace;
			ofShader morphFill;
			ofMesh mesh;
		};
	}
}