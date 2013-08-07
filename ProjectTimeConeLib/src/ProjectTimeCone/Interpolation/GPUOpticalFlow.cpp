#include "GPUOpticalFlow.h"

using namespace cv;
using namespace cv::gpu;
using namespace ofxCv;

namespace ProjectTimeCone {
	namespace Interpolation {
		//---------
		GPUOpticalFlow::FlowResult::FlowResult(int width, int height) {
			try
			{
				this->gpuMat = GpuMat(height, width, CV_32FC1);
				this->cpuMat = Mat(height, width, CV_32FC1);
			} catch (cv::Exception e) {
				ofLogError("GPUOpticalFlow") << e.msg;
			}
			this->width = width;
			this->height = height;
		}

		//---------
		void GPUOpticalFlow::FlowResult::download() {
			this->gpuMat.download(this->cpuMat);
		}

		//---------
		void GPUOpticalFlow::FlowResult::upload() {
			this->image.setFromPixels((float*) this->cpuMat.data, this->width, this->height, OF_IMAGE_GRAYSCALE);
			this->image.update();
		}

		//---------
		GPUOpticalFlow::GPUOpticalFlow(int width, int height) :
			OpticalFlow(width, height),
			AtoBX(width, height), AtoBY(width, height),
			BtoAX(width, height), BtoAY(width, height)
		{
				this->gpuA = GpuMat(height, width, CV_8UC1);
				this->gpuB = GpuMat(height, width, CV_8UC1);

				this->AtoBimage.allocate(width, height, OF_IMAGE_GRAYSCALE);
				this->BtoAimage.allocate(width, height, OF_IMAGE_GRAYSCALE);

				this->reload();
		}
		
		//---------
		GPUOpticalFlow::~GPUOpticalFlow() {
			this->flow.releaseMemory();
		}

		//---------
		void GPUOpticalFlow::UpdateFlow(ofPixels & A, ofPixels & B) {
			Profiler["Convert colour"].begin();
			cvtColor(toCv(A), this->A, CV_RGB2GRAY);
			cvtColor(toCv(B), this->B, CV_RGB2GRAY);

			gpuA.upload(this->A);
			gpuB.upload(this->B);
			Profiler["Convert colour"].end();
			
			Profiler["Calculate optical flow"].begin();
			this->flow(this->gpuA, this->gpuB, this->AtoBX.gpuMat, this->AtoBY.gpuMat);
			this->flow(this->gpuB, this->gpuA, this->BtoAX.gpuMat, this->BtoAY.gpuMat);
			Profiler["Calculate optical flow"].end();

			Profiler["Download optical flow gpu->cpu"].begin();
			this->AtoBX.download();
			this->AtoBY.download();
			this->BtoAX.download();
			this->BtoAY.download();
			Profiler["Download optical flow gpu->cpu"].end();

			Profiler["Upload optical flow cpu->gpu"].begin();
			this->AtoBX.upload();
			this->AtoBY.upload();
			this->BtoAX.upload();
			this->BtoAY.upload();
			Profiler["Upload optical flow cpu->gpu"].end();
		}

		//---------
		void GPUOpticalFlow::UpdateResult(float x, ofTexture & A, ofTexture & B) {
			
			//--
			//Draw points
			//
			Profiler["Draw displaced points"].begin();
			this->displaceXY.begin();
			ofEnableAlphaBlending();

			//A to B points
			this->left.begin();
			ofClear(0,0,0,0);
			this->displaceXY.setUniformTexture("imageDisplaceX", AtoBX.image.getTextureReference(), 0);
			this->displaceXY.setUniformTexture("imageDisplaceY", AtoBY.image.getTextureReference(), 1);
			this->displaceXY.setUniformTexture("texture", A, 2);
			this->displaceXY.setUniform1f("x", x);
			this->mesh.drawFaces();
			this->left.end();

			//B to A points
			this->right.begin();
			ofClear(0,0,0,0);
			this->displaceXY.setUniformTexture("imageDisplaceX", BtoAX.image.getTextureReference(), 0);
			this->displaceXY.setUniformTexture("imageDisplaceY", BtoAY.image.getTextureReference(), 1);
			this->displaceXY.setUniformTexture("texture", B, 2);
			this->displaceXY.setUniform1f("x", 1.0f - x);
			this->mesh.drawFaces();
			this->right.end();

			ofDisableAlphaBlending();
			this->displaceXY.end();
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
			morphFill.begin();
			morphFill.setUniform1i("stepDistance", 2);
			
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
			result.end();
			Profiler["Crossfade and fill"].end();
			//
			//--
		}

		//---------
		void GPUOpticalFlow::UpdateParameters() {
			this->flow.pyrScale = this->pyramidScale;
			this->flow.numLevels = this->numLevels;
			this->flow.winSize = this->windowSize;
			this->flow.numIters = this->numIterations;
			this->flow.polyN = this->polyN;
			this->flow.polySigma = this->polySigma;
			//doesn't support useGaussian
		}

		//---------
		void GPUOpticalFlow::reload() {
			OpticalFlow::reload();
			this->displaceXY.load("shaders/displaceXY.vert", "shaders/constant.frag");
		}
	}
}