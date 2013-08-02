#pragma once

#include "ofImage.h"

namespace ProjectTimeCone {
	namespace Interpolation {
		class Base {
		public:
			virtual ~Base() { }
			virtual void Interpolate(float x, ofImage & A, ofImage & B, ofPixels & result) = 0;
		};
	}
}