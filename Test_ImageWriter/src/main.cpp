#include <iostream>

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


int main() {
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


#if 0 
  // setup GLFW
  if(!rx_glfw_init()) {
    exit(EXIT_FAILURE);
  }

  // create window.
  GLFWwindow* win = rx_glfw_create_window(1280, 720, "Simulation", NULL, NULL);
  if(!win) {
    exit(EXIT_FAILURE);
  }

  // create simulation app.
  Simulation sim;
  rx_glfw_setup_simulation(&sim, win);

  while(!glfwWindowShouldClose(win)) {

    sim.update();
    sim.draw();

    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  glfwTerminate();
#endif
  return EXIT_SUCCESS;
};

