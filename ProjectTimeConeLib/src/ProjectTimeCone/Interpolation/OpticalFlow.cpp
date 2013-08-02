#include "OpticalFlow.h"

using namespace cv;
using namespace ofxCv;

namespace ProjectTimeCone {
	namespace Interpolation {
		//---------
		OpticalFlow::OpticalFlow(int width, int height) {
			this->pyramidScale = 0.5f;
			this->numLevels = 4;
			this->windowSize = 40;
			this->numIterations = 3;
			this->polyN = 7;
			this->polySigma = 1.3;
			this->useGaussian = true;
			
			this->width = width;
			this->height = height;

			for(int j=0; j<height; j++) {
				for(int i=0; i<width; i++) {
					this->mesh.addVertex(ofVec3f(i, j, 0));
					this->mesh.addTexCoord(ofVec2f(i, j));
				}
			}

			this->AtoB = Mat(this->height, this->width, CV_32FC2);
			this->BtoA = Mat(this->height, this->width, CV_32FC2);
		}

		//---------
		void OpticalFlow::Interpolate(float x, ofPixels & A, ofPixels & B, ofPixels & result) {
			cvtColor(toCv(A), this->A, CV_RGB2GRAY);
			cvtColor(toCv(B), this->B, CV_RGB2GRAY);

			int flags = 0;
			if (this->useGaussian) {
				flags |= OPTFLOW_FARNEBACK_GAUSSIAN;
			}

			calcOpticalFlowFarneback(this->A, this->B, AtoB,
				this->pyramidScale, this->numLevels,
				this->windowSize, this->numIterations,
				this->polyN, this->polySigma, flags);
		
			calcOpticalFlowFarneback(this->B, this->A, BtoA,
				this->pyramidScale, this->numLevels,
				this->windowSize, this->numIterations,
				this->polyN, this->polySigma, flags);

			//now we need to render a displaced mesh to the fbo and read back to pixels
		}
	}
}