#pragma once
#include "Poco/Path.h"
#include <stdint.h>

namespace ProjectTimeCone {
	namespace Utils {
		class Disk {
		public:
			static uint64_t getFreeSpace(Poco::Path path);
		};
	}
}