#include "Timer.h"

namespace ProjectTimeCone {
	namespace Profiling {
		//---------
		Timer::Action::Action(string name) {
			this->open = false;
			this->name = name;
			
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			this->frequency = (float) frequency.QuadPart;
		}

		//---------
		void Timer::Action::begin() {
			if (this->open) {
				ofLogError("Profiler::Timer::Action") << "Cannot begin as this action [" << this->name << "] is already running";
			} else {
				QueryPerformanceCounter(&this->startTime);
				this->open = true;
			}
		}

		//---------
		void Timer::Action::end() {
			if (!this->open) {
				ofLogError("Profiler::Timer::Action") << "Cannot end as this action [" << this->name << "] is not currently running";
			} else {
				this->open = false;
				LARGE_INTEGER endTime;
				QueryPerformanceCounter(&endTime);
				float difference = (float) (endTime.QuadPart - startTime.QuadPart) / this->frequency;
				this->durations.push_back(difference);
			}
		}

		//---------
		float Timer::Action::getDurationSum() const {
			float sum = 0;
			for(auto & duration : this->durations) {
				sum += duration;
			}
			return sum;
		}

		//---------
		float Timer::Action::getDurationMean() const {
			return this->getDurationSum() / (float) this->durations.size();
		}

		//---------
		void Timer::clear() {
			this->actions.clear();
		}

		//---------
		Timer::Action & Timer::operator[](string actionName) {
			auto it = this->actions.find(actionName);
			if (it == this->actions.end()) {
				//we need to add this action as it doesn't exist
				this->actions.insert(pair<string, Action>(actionName, Action(actionName)));
				it = this->actions.find(actionName);
			}

			return it->second;
		}

		//---------
		string Timer::getResultsString() const {
			stringstream result;

			float maxDuration = 0.0f;
			int longestName = 0;
			map<float, string> durations;
			for (auto & action : this->actions) {
				float duration = action.second.getDurationMean();
				durations.insert(pair<float, string>(duration, action.first));
				int nameLength = action.first.size();
				if (nameLength > longestName) {
					longestName = nameLength;
				}
				if (duration > maxDuration) {
					maxDuration = duration;
				}
			}

			for (auto & it : durations) {
				result << it.second;
				for(int i=0; i<longestName - it.second.size(); i++) {
					result << " ";
				}
				result << " : ";
				
				int barLength = it.first / maxDuration * 20.0f;
				for(int i=0; i<20; i++) {
					result << ((i <= barLength) ? "#" : ".");
				}
				
				result << "(" << ofToString(this->actions.at(it.second).getDurationSum(), 4) << "s) ";
				result << " " << ofToString(it.first, 4) << "s";
				
				result << endl;
			}

			return result.str();
		}
	}

	Profiling::Timer Profiler = Profiling::Timer();
}