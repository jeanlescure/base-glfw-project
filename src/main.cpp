#include "Application.h"

static void glfw_error_callback(int error, const char *description);

int main(int, char**)
{
  glfwSetErrorCallback(glfw_error_callback);

  Application &app = *new Application;

  int openState = app.open();

  if (openState != 0) {
    return openState;
  }

  app.init();

  app.runLoop();

  app.cleanUp();

  return 0;
}

static void glfw_error_callback(int error, const char *description) {
  printf("Glfw Error %d: %s\n", error, description);
}
