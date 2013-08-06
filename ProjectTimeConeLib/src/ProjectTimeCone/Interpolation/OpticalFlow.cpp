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

			this->textureA.allocate(this->width, this->height, GL_RGB);
			this->textureB.allocate(this->width, this->height, GL_RGB);
			this->left.allocate(this->width, this->height, GL_RGBA);
			this->right.allocate(this->width, this->height, GL_RGBA);
			this->fbo.allocate(this->width, this->height, GL_RGB);

			this->reload();
		}

		//---------
		void OpticalFlow::Interpolate(float x, ofPixels & A, ofPixels & B, ofPixels & result) {
			this->UpdateFlow(A, B);
			this->textureA.loadData(A);
			this->textureB.loadData(B);
			this->UpdateResult(x, this->textureA, this->textureB);
			this->fbo.readToPixels(result);
		}

		//---------
		void OpticalFlow::UpdateFlow(ofPixels & A, ofPixels & B) {
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

			float* AtoBdataIn = (float*) AtoB.data;
			float* AtoBdataOut = this->AtoBimage.getPixels();
			float* BtoAdataIn = (float*) BtoA.data;
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
		}

		//---------
		void OpticalFlow::UpdateResult(float x, ofTexture & A, ofTexture & B) {
			//--
			//Draw points
			//
			this->displace.begin();
			ofEnableAlphaBlending();

			//A to B points
			this->left.begin();
			this->displace.setUniformTexture("imageDisplace", AtoBimage.getTextureReference(), 0);
			this->displace.setUniformTexture("texture", A, 1);
			this->displace.setUniform1f("x", x);
			this->mesh.drawVertices();
			this->left.end();

			//B to A points
			this->right.begin();
			this->displace.setUniformTexture("imageDisplace", BtoAimage.getTextureReference(), 0);
			this->displace.setUniformTexture("texture", B, 1);
			this->displace.setUniform1f("x", 1.0f - x);
			this->mesh.drawVertices();
			this->right.end();

			ofDisableAlphaBlending();
			this->displace.end();
			//
			//--
			

			//--
			//Crossfade
			//
			fbo.begin();
			ofClear(0,0,0,0);

			ofEnableAlphaBlending();
			morphFill.begin();
			
			//left
			morphFill.setUniformTexture("texture", this->left, 0);
			morphFill.setUniform1f("x", 1.0f);
			this->left.draw(0,0);

			//right
			morphFill.setUniformTexture("texture", this->right, 0);
			morphFill.setUniform1f("x", x);
			this->right.draw(0,0);

			morphFill.end();
			ofDisableAlphaBlending();
			fbo.end();
			//
			//--
		}

		//---------
		void OpticalFlow::reload() {
				this->displace.load("shaders/displace.vert", "shaders/constant.frag");
				this->morphFill.load("shaders/constant.vert", "shaders/morphFill.frag");
				ofLogNotice() << "Reloaded shaders.";
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
		}
	}
}