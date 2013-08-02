#pragma once

#include "Base.h"

namespace ProjectTimeCone {
	namespace Interpolation {
		class OpticalFlow : public Base {
		public:
			void Interpolate(float x, const ofPixels & A, const ofPixels & B, ofPixels & result) override;
		};
	}
}