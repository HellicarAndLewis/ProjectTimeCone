#ifndef NO_YOUTUBE
	extern "C" {
		#include <uv.h>
	};
	#undef small
	#include "ProjectTimeCone/YouTube/VideoEncoder.h"
#endif

#include "ProjectTimeCone/Initialisation/Initialiser.h"
#include "ProjectTimeCone/Interpolation/OpticalFlow.h"
#include "ProjectTimeCone/Interpolation/GPUOpticalFlow.h"
#include "ProjectTimeCone/Profiling/Timer.h"

