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
			this->AtoBimage.allocate(this->width, this->height, OF_IMAGE_COLOR);
			this->BtoAimage.allocate(this->width, this->height, OF_IMAGE_COLOR);
			this->reload();

			this->fbo.allocate(this->width, this->height, GL_RGBA8, 2);

			float pyrWidth = this->width;
			float pyrHeight = this->height;
			for(int i=0; i<PYRAMID_COUNT; i++) {
				this->pyramid[i].allocate(pyrWidth, pyrHeight, GL_RGBA8);
				pyrWidth /= 2.0f;
				pyrHeight /= 2.0f;
			}
		}

		//---------
		void OpticalFlow::UpdateFlow(ofImage & A, ofImage & B) {
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
		}

		//---------
		void OpticalFlow::Interpolate(float x, ofImage & A, ofImage & B, ofPixels & result) {
			float* AtoBdataIn = (float*) AtoB.data;
			float* AtoBdataOut = this->AtoBimage.getPixels();
			float* BtoAdataIn = (float*) AtoB.data;
			float* BtoAdataOut = this->BtoAimage.getPixels();
			for(int i=0; i<this->width * this->height; i++) {
				*AtoBdataOut++ = *AtoBdataIn++;
				*AtoBdataOut++ = *AtoBdataIn++;
				*AtoBdataOut++ = 0.0f;

				*BtoAdataOut++ = *BtoAdataIn++;
				*BtoAdataOut++ = *BtoAdataIn++;
				*BtoAdataOut++ = 0.0f;
			}
			
			this->AtoBimage.update();
			this->BtoAimage.update();

			//--
			//Create pyramid fbo's
			//
			ofPushMatrix();
			for (int i=0; i<PYRAMID_COUNT; i++) {
				pyramid[i].begin();
				ofClear(0,0,0,0);
				float scale = 1.0f / pow(2.0f, (float) i);
				ofScale(scale, scale);
				this->drawPoints(x, A.getTextureReference(), B.getTextureReference());
				pyramid[i].end();
			}
			ofPopMatrix();
			//
			//--


			fbo.begin();
			ofClear(0,0,0,0);

			//--
			//background cross fade
			//
			ofPushStyle();
			ofEnableAlphaBlending();

			ofSetColor(255, 255, 255, 255.0f * (1.0f - x));
			A.draw(0,0);
			ofSetColor(255, 255, 255, 255.0f * x);
			B.draw(0,0);

			ofDisableAlphaBlending();
			ofPopStyle();
			//
			//--

			
			//--
			//pyramids
			//
			ofEnableAlphaBlending();
			for (int i=PYRAMID_COUNT - 1; i>=0; i--) {
				this->pyramid[i].draw(0, 0, this->width, this->height);
			}
			ofDisableAlphaBlending();
			//
			//--

			fbo.end();

			fbo.draw(0,0);
		}

		//---------
		void OpticalFlow::reload() {
				this->displace.load("shaders/displace.vert", "shaders/constant.frag");
				ofLogNotice() << "Reloaded displacement shader.";
		}
		
		//---------
		ofFloatImage & OpticalFlow::getAtoB() {
			return this->AtoBimage;
		}

		//----------
		ofFloatImage & OpticalFlow::getBtoA() {
			return this->BtoAimage;
		}

		//----------
		ofFbo & OpticalFlow::getResultFbo() {
			return this->fbo;
		}

		//----------
		void OpticalFlow::drawPoints(float x, ofTexture & A, ofTexture & B) {
			this->displace.begin();

			ofEnableAlphaBlending();

			//--
			//A to B points
			//
			this->displace.setUniformTexture("imageDisplace", AtoBimage.getTextureReference(), 0);
			this->displace.setUniformTexture("texture", A, 1);
			this->displace.setUniform1f("alpha", 1.0f - x);
			this->displace.setUniform1f("x", x);

			this->mesh.drawVertices();
			//
			//--

			//--
			//B to A points
			//
			this->displace.setUniformTexture("imageDisplace", BtoAimage.getTextureReference(), 0);
			this->displace.setUniformTexture("texture", B, 1);
			this->displace.setUniform1f("alpha", x);
			this->displace.setUniform1f("x", 1.0f - x);

			this->mesh.drawVertices();
			//
			//--

			ofDisableAlphaBlending();

			this->displace.end();
		}
	}
}