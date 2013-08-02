#include <Simulation.h>
#include <image/Image.h>
extern "C" {
# include <uv.h>  
}

#define W 1280
#define H 720

static void fill(unsigned char* pix, int ww, int hh, int r, int g, int b) {
  for(int i = 0; i < ww; ++i) {
    for(int j = 0; j < hh; ++j) {
      int dx = j * ww * 3 + i * 3;
      pix[dx + 0] = r;
      pix[dx + 1] = g;
      pix[dx + 2] = b;
    }
  }
}
Simulation::Simulation()
  :SimulationBase()
{
}

void Simulation::setup() {
  char fname[512];
  double num_red = 5 * 25;
  double num_green = 5 * 25;
  JPG jpg;
  unsigned char* pix = new unsigned char[W * H * 3];


  if(!jpg.setPixels(pix, W, H)) {
    RX_ERROR("Cannot set the pixels");
    ::exit(EXIT_FAILURE);
  }

  uint64_t t = uv_hrtime();


  for(int i = 0; i < num_red; ++i) {
    double p = double(i)/num_red;
    fill(pix, W, H, p * 255, 0, 0);
    jpg.setPixels(pix, W, H);

    sprintf(fname, "red/frame_%04d.jpg", i);
    jpg.save(fname, true);
  }

  fill(pix, W, H, 0, 255, 0);
  for(int i = 0; i < num_green; ++i) {
    double p = double(i)/num_green;
    fill(pix, W, H, 0, p * 255, 0);
    jpg.setPixels(pix, W, H);

    sprintf(fname, "green/frame_%04d.jpg", i);
    jpg.save(fname, true);
  }

  uint64_t d = uv_hrtime() - t;
  RX_VERBOSE("TOOK %ld nanos, %ld ms", d, (d / 1000000));

}

void Simulation::update() {
}

void Simulation::draw() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Simulation::onMouseUp(double x, double y, int button, int mods) {
}

void Simulation::onMouseDown(double x, double y, int button, int mods) {
}

void Simulation::onMouseDrag(double x, double y, double dx, double dy, int button) {
}

void Simulation::onMouseMove(double x, double y) {
}

void Simulation::onChar(unsigned int ch) {
}

void Simulation::onKeyDown(int key, int scancode, int mods) {
  if(key == GLFW_KEY_ESCAPE) {
    closeWindow();
  }
}

void Simulation::onKeyUp(int key, int scancode, int mods) {
}

void Simulation::onWindowClose() {
  ::exit(EXIT_SUCCESS);
}

