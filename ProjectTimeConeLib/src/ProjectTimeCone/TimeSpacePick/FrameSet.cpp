#include "FrameSet.h"
#include "Poco/DirectoryIterator.h"

#define WIDTH 1280
#define HEIGHT 720

namespace ProjectTimeCone {
	namespace TimeSpacePick {
		//---------
		void FrameSet::init(float screenWidth, float screenHeight) {
			this->interpolator = ofPtr<Interpolation::GPUOpticalFlow>(new Interpolation::GPUOpticalFlow(WIDTH, HEIGHT));
			this->interpolator->UpdateParameters();
			this->frameA.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
			this->frameB.allocate(WIDTH, HEIGHT, OF_IMAGE_COLOR);
			this->screenWidth = screenWidth;
			this->screenHeight = screenHeight;
			this->gridView.allocate(this->screenWidth,this->screenHeight);
		}

		//---------
		void FrameSet::load(string path) {
			this->minTimestamp = std::numeric_limits<float>::max();
			this->maxTimestamp = std::numeric_limits<float>::min();

			this->folder = Poco::Path(path);
			this->cameras.clear();
			this->cameraFolders.clear();
			this->timeOffsets.clear();

			ofDirectory listDir;
			listDir.listDir(this->folder.toString());
			listDir.sort();
			auto folders = listDir.getFiles();
			for(auto folder : folders) {
				if (folder.isDirectory()) {
					ofLogNotice() << "Adding camera " << folder.getAbsolutePath();
					this->cameraFolders.push_back(folder.getBaseName());
					this->cameras.push_back(map<float, string>());
					auto & currentFolder = this->cameras.back();

					auto dirIt = Poco::DirectoryIterator(folder.getAbsolutePath());
					auto dirEnd = Poco::DirectoryIterator();

					//add items to file map
					float timeOffset = 0;
					for (;dirIt != dirEnd; dirIt++) {
						auto path = Poco::Path(dirIt->path());

						auto timestamp = ofToFloat(path.getBaseName()) / 1e6;

						if (currentFolder.size() == 0) {
							//set the time offset as the first timestamp
							timeOffset = timestamp;
							this->timeOffsets.push_back(timeOffset);
						}
						timestamp -= timeOffset;

						currentFolder.insert(std::pair<float, string>(timestamp, path.toString()));
						if (timestamp > maxTimestamp) {
							maxTimestamp = timestamp;
						}
						if (timestamp < minTimestamp) {
							minTimestamp = timestamp;
						}
					}
				}
			}

			//--
			//shave maxTimestamp back to common max
			for(auto camera : cameras) {
				auto last = camera.end();
				last--;
				if (last->first < maxTimestamp) {
					maxTimestamp = last->first;
				}
			}
			//
			//--

			int gridHeight = 12;
			int gridWidth = this->screenWidth / ((this->screenHeight/12) * (1280.0f / 720.0f)); //=12 because aspect ratio is same

			float thumbWidth = this->screenWidth / (float) gridWidth;
			float thumbHeight = this->screenHeight / (float) gridHeight;

			ofImage thumbLarge;
			thumbLarge.allocate(1280, 720, OF_IMAGE_COLOR);
			this->gridView.begin();
			for(int i=0; i<gridWidth; i++) {
				for(int j=0; j<gridHeight; j++) {
					auto frame = this->screenToTimeSpace(ofVec2f(i * this->screenWidth / gridWidth, j * this->screenHeight / gridHeight));
					auto it = this->cameras[j].lower_bound(frame.time);

					if (it != this->cameras[j].end()) {
						getFrame(it->second, thumbLarge.getPixelsRef());
						thumbLarge.update();
						thumbLarge.draw(thumbWidth * i, thumbHeight * j, thumbWidth, thumbHeight);
					}
				}
			}
			this->gridView.end();
		}

		//----------
		TimeSpace FrameSet::screenToTimeSpace(const ofVec2f & screen) const {
			TimeSpace x;
			x.time = screen.x / this->screenWidth * (this->maxTimestamp - this->minTimestamp) + minTimestamp;
			x.camera = ofClamp((screen.y * 13) / this->screenHeight - 1, 0, 11);
			return x;
		}

		//----------
		ofVec2f FrameSet::TimeSpaceToScreen(const TimeSpace & x) const {
			return ofVec2f((x.time - minTimestamp) / (maxTimestamp - minTimestamp) * this->screenWidth,
				(x.camera + 1.0f) * this->screenHeight / 13);
		}

		//----------
		bool FrameSet::insideScreen(const ofVec2f& screen) const {
			return screen.x >=0 && screen.y >=0 && screen.x <= this->screenWidth && screen.y <= this->screenHeight;
		}

		//----------
		void FrameSet::getFrame(string filename, ofPixels & pixels) {
			ifstream file(filename, ios::binary | ios::in);
			file.read((char*) pixels.getPixels(), pixels.size());
			file.close();
		}

		//----------
		bool FrameSet::getFrame(int camera, float time, ofPixels& output) {
			if (camera < 0 || camera >= this->cameras.size()) {
				ofLogError() << "Camera index " << camera << " out of bounds";
				return false;
			}

			auto & files = this->cameras[camera];
			auto find = files.find(time);

			if (files.size() == 0) {
				ofLogError() << "No frames available for camera " << camera;
				return false;
			}

			if (find == files.end()) {
				//general case, we need to find the closest frame
				find = files.lower_bound(time);
			}
			if (find == files.end()) {
				//we want to look back in time to find the last frame
				find--;
			}

			getFrame(find->second, output);
			return true;
		}

		//----------
		void FrameSet::getInterpolatedFrame(TimeSpace& timeSpace, ofPixels& result) {
			if (this->cacheInterpolation(timeSpace)) {
				float x = timeSpace.camera;
				x -= floor(x); // strip to 0...1
				this->interpolator->UpdateResult(x, this->frameA.getTextureReference(), this->frameB.getTextureReference());
				this->interpolator->getResultFbo().readToPixels(result);
			} else {
				this->getFrame(timeSpace.camera, timeSpace.time, result);
			}
		}

		//----------
		ofFbo & FrameSet::getGridView() {
			return this->gridView;
		}

		//----------
		bool FrameSet::cacheInterpolation(TimeSpace& timeSpace) {
			//get frame times

			auto cameraIndexA = floor(timeSpace.camera);
			auto cameraIndexB = ceil(timeSpace.camera);

			if (cameraIndexA == cameraIndexB) {
				//no interpolation required
				return false;
			}

			if (cameraIndexB >= this->cameras.size()) {
				//if we're on the last camera we have to clamp
				cameraIndexB--;
			}

			auto & filesA = cameras[cameraIndexA];
			auto & filesB = cameras[cameraIndexB];

			auto fileA = filesA.lower_bound(timeSpace.time);
			auto fileB = filesB.lower_bound(timeSpace.time);

			if (fileA == filesA.end()) {
				fileA--;
			}
			if (fileB == filesB.end()) {
				fileB--;
			}

			auto & filenameA = fileA->second;
			auto & filenameB = fileB->second;

			bool needsCache = false;
			if (filenameA != this->cachedFilenameA) {
				this->getFrame(filenameA, this->frameA.getPixelsRef());
				this->frameA.update();
				this->cachedFilenameA = filenameA;
				needsCache = true;
			}
			if (filenameB != cachedFilenameB) {
				getFrame(filenameB, this->frameB.getPixelsRef());
				this->frameB.update();
				this->cachedFilenameB = filenameB;
				needsCache = true;
			}
	
			this->interpolator->UpdateFlow(this->frameA.getPixelsRef(), this->frameB.getPixelsRef());
			return true;
		}
	}
}