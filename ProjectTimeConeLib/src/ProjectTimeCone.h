#ifndef NO_YOUTUBE
	#ifdef OF_VERSION_MAJOR
		//#warning You should include ProjectTimeCone before oF (or <Windows.h>) because symbols clash in uv.h.
	#endif
	extern "C" {
		#include <uv.h>
	};
	#undef small
	#include "ProjectTimeCone/YouTube/EncodeAndUpload.h"
#endif

#include "ProjectTimeCone/Initialisation/Initialiser.h"
#include "ProjectTimeCone/TimeSpacePick/FrameSet.h"
#include "ProjectTimeCone/TimeSpacePick/FramePath.h"
#include "ProjectTimeCone/Interpolation/OpticalFlow.h"
#include "ProjectTimeCone/Interpolation/GPUOpticalFlow.h"
#include "ProjectTimeCone/Profiling/Timer.h"
#include "ProjectTimeCone/Utils/Disk.h"
