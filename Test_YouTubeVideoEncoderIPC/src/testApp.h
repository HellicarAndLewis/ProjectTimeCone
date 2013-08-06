#pragma once

#include "YouTubeVideoEncoder.h"
#include "ofMain.h"

void on_video_encoded(VideoEncoderEncodeTask task, void *user);
void on_video_uploaded(YouTubeVideo video, void* user);

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    YouTubeVideoEncoder yt;
};
