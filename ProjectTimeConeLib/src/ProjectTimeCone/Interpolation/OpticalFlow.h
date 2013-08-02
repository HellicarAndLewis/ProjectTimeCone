#pragma once

#include "Base.h"
#include "ofxCv.h"

#define PYRAMID_COUNT 2

namespace ProjectTimeCone {
	namespace Interpolation {
		class OpticalFlow : public Base {
		public:
			OpticalFlow(int width, int height);
			void UpdateFlow(ofImage & A, ofImage & B);
			void Interpolate(float x, ofImage & A, ofImage & B, ofPixels & result) override;
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

			ofFbo fbo;
			ofFbo pyramid[PYRAMID_COUNT];
			ofShader displace;
			ofMesh mesh;
		};
	}
}