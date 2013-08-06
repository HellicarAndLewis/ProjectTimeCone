#include "Timer.h"

namespace ProjectTimeCone {
	namespace Profiling {
		//---------
		Timer::Action::Action() {
			this->open = false;
			
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			this->frequency = (float) frequency.QuadPart;
		}

		//---------
		void Timer::Action::begin() {
			if (this->open) {
				ofLogError("Profiler::Timer::Action") << "Cannot begin as this action is already running";
			} else {
				QueryPerformanceCounter(&this->startTime);
				this->open = true;
			}
		}

		//---------
		void Timer::Action::end() {
			if (!this->open) {
				ofLogError("Profiler::Timer::Action") << "Cannot end as this action is not currently running";
			} else {
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
		Timer::Action & Timer::operator[](string actionName) {
			auto it = this->actions.find(actionName);
			if (it == this->actions.end()) {
				//we need to add this action as it doesn't exist
				this->actions.insert(pair<string, Action>(actionName, Action()));
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
				durations.insert(pair<float, string>(action.second.getDurationMean(), action.first));
				int nameLength = action.first.size();
				if (nameLength > longestName) {
					longestName = nameLength;
				}
			}

			for (auto & it : durations) {
				result << it.second;
				int requiredTabs = (int) ceil(float(it.second.size() -  longestName)/4.0f);
				for(int i=0; i<requiredTabs; i++) {
					result << "\t";
				}
				result << " : ";
				int barLength = it.first * 20 / maxDuration;
				for(int i=0; i<20; i++) {
					cout << (i <= barLength) ? "#" : " ";
				}
				cout << " " << ofToString(it.first, 4) << "s ";
				cout << "(" << ofToString(this->actions.at(it.second).getDurationSum(), 4) << "s)";
				cout << endl;
			}

			return result.str();
		}
	}
}