#pragma once

#include "OpticalFlow.h"
#include <opencv2/gpu/gpu.hpp>

namespace ProjectTimeCone {
	namespace Interpolation {
		class GPUOpticalFlow : public OpticalFlow {
			class FlowResult
			{
			public:
				FlowResult(int width, int height);
				void download();
				void upload();

				cv::gpu::GpuMat gpuMat;
				cv::Mat cpuMat;
				ofTexture texture;
				int width, height;
			};
		public:
			GPUOpticalFlow(int width, int height);
			void UpdateFlow(ofPixels & A, ofPixels & B) override;
			void UpdateResult(float x, ofTexture & A, ofTexture & B) override;
			void UpdateParameters();
			void reload();
		protected:
			cv::gpu::GpuMat gpuA, gpuB;

			FlowResult AtoBX, AtoBY;
			FlowResult BtoAX, BtoAY;

			cv::gpu::FarnebackOpticalFlow flow;

			ofShader displaceXY;
		};
	}
}