#pragma once

extern "C" {
#include <uv.h>
};
#undef small

#include <windows.h>
#include "ofMain.h"

namespace ProjectTimeCone {
	namespace Profiling {
		class Timer {
			class Action {
			public:
				Action(string name);
				void begin();
				void end();
				float getDurationSum() const;
				float getDurationMean() const;
			protected:
				string name;
				bool open;
				LARGE_INTEGER startTime;
				float frequency;
				vector<float> durations;
			};
		public:
			void clear();
			Action & operator[](string actionName);
			string getResultsString() const;
		protected:
			map<string, Action> actions;
		};
	}

	extern Profiling::Timer Profiler;
}