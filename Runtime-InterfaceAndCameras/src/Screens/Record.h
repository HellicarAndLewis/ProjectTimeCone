#pragma once
#include "Base.h"
#include "ofxMachineVision.h"

namespace Screens {
	class Record : public Base {
	public:
		typedef ofPtr<ofxMachineVision::Stream::DiskStreamer> Streamer;
		Record(ofBaseDraws & preview);
		void draw(ofxCvGui::DrawArguments &) override;
		void reset();
		void addStreamer(Streamer);
		ofxCvGui::Utils::LambdaStack<string> onFinishRecording;
	protected:
		ofBaseDraws & preview;
		ofPtr<ofxCvGui::Utils::Button> record;
		ofPtr<ofxCvGui::Utils::Button> stop;

		vector<Streamer> streamers;
		string path;
	};
}