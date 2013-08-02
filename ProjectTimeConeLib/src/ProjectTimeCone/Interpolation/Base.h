#pragma once

#include "ofPixels.h"

namespace ProjectTimeCone {
	namespace Interpolation {
		class Base {
		public:
			virtual ~Base() { }
			virtual void Interpolate(float x, ofPixels & A, ofPixels & B, ofPixels & result) = 0;
		};
	}
}