#include "Disk.h"
#include <Windows.h>
#include <string>
#include <iostream>

namespace ProjectTimeCone {
	namespace Utils {
		//---------
		uint64_t Disk::getFreeSpace(Poco::Path path) {
			std::string pathString = path.toString();;
			ULARGE_INTEGER bytesAvailable;
			ULARGE_INTEGER totalBytes;
			ULARGE_INTEGER totalFreeBytes;
			GetDiskFreeSpaceEx(pathString.c_str(), &bytesAvailable, &totalBytes, &totalFreeBytes);

			return (uint64_t) bytesAvailable.QuadPart;
		}
	}
}