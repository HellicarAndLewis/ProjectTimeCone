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

			int index = 0;
			for(int j=0; j<height; j++) {
				for(int i=0; i<width; i++) {
					this->mesh.addVertex(ofVec3f(i, j, 0));
					this->mesh.addTexCoord(ofVec2f(i, j));
					if (i < width - 1 && j < height - 1) {
						this->mesh.addIndex(index);
						this->mesh.addIndex(index + width);
						this->mesh.addIndex(index + 1);

						this->mesh.addIndex(index + 1);
						this->mesh.addIndex(index + width);
						this->mesh.addIndex(index + width + 1);
					}
					index++;
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
			this->interim.allocate(this->width, this->height, GL_RGB);
			this->result.allocate(this->width, this->height, GL_RGB);

			this->reload();
		}

		//---------
		void OpticalFlow::Interpolate(float x, ofPixels & A, ofPixels & B, ofPixels & result) {
			this->UpdateFlow(A, B);
			this->textureA.loadData(A);
			this->textureB.loadData(B);
			this->UpdateResult(x, this->textureA, this->textureB);
			this->result.readToPixels(result);
		}

		//---------
		void OpticalFlow::UpdateFlow(ofPixels & A, ofPixels & B) {
			cvtColor(toCv(A), this->A, CV_RGB2GRAY);
			cvtColor(toCv(B), this->B, CV_RGB2GRAY);

			int flags = 0;
			if (this->useGaussian) {
				flags |= OPTFLOW_FARNEBACK_GAUSSIAN;
			}

			//--
			//
			Profiler["Calculate optical flow"].begin();

			calcOpticalFlowFarneback(this->A, this->B, AtoB,
				this->pyramidScale, this->numLevels,
				this->windowSize, this->numIterations,
				this->polyN, this->polySigma, flags);
		
			calcOpticalFlowFarneback(this->B, this->A, BtoA,
				this->pyramidScale, this->numLevels,
				this->windowSize, this->numIterations,
				this->polyN, this->polySigma, flags);

			Profiler["Calculate optical flow"].end();
			//
			//--


			//--
			//
			Profiler["Load flow to GPU"].begin();

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

			Profiler["Load flow to GPU"].end();
			//
			//--
		}

		//---------
		void OpticalFlow::UpdateResult(float x, ofTexture & A, ofTexture & B) {
			//--
			//Draw points
			//
			Profiler["Draw displaced points"].begin();
			this->displace.begin();
			ofEnableAlphaBlending();

			//A to B points
			this->left.begin();
			this->displace.setUniformTexture("imageDisplace", AtoBimage.getTextureReference(), 0);
			this->displace.setUniformTexture("texture", A, 1);
			this->displace.setUniform1f("x", x);
			this->mesh.drawFaces();
			this->left.end();

			//B to A points
			this->right.begin();
			this->displace.setUniformTexture("imageDisplace", BtoAimage.getTextureReference(), 0);
			this->displace.setUniformTexture("texture", B, 1);
			this->displace.setUniform1f("x", 1.0f - x);
			this->mesh.drawFaces();
			this->right.end();

			ofDisableAlphaBlending();
			this->displace.end();
			Profiler["Draw displaced points"].end();
			//
			//--
			

			//--
			//Crossfade
			//
			Profiler["Crossfade and fill"].begin();
			result.begin();
			ofClear(0,0,0,0);

			ofEnableAlphaBlending();
			
			//left
			this->left.draw(0,0);

			//right
			this->right.draw(0,0);

			ofDisableAlphaBlending();
			result.end();
			Profiler["Crossfade and fill"].end();
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
			return this->result;
		}

		//----------
		void OpticalFlow::drawPoints(float x, ofTexture & A, ofTexture & B) {
		}
	}
}