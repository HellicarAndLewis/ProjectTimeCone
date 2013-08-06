extern "C" {
# include <uv.h>
};

#include <roxlu/core/Log.h>
#include "ofMain.h"
#include "testApp.h"

static void on_log(int level, void* user, int line, const char* function, const char* fmt, va_list args) {
  printf("[%d] - ", level);
  vprintf(fmt, args);
  printf("\n");
}


//========================================================================
int main( ){
  rx_log_set_callback(on_log, NULL);

	ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new testApp());

}
