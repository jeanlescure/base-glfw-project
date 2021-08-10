#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <chrono>
#include <thread>
#include <string>

#include <linmath.h>

class Application {
  public:
    Application();

    int open();
    void close();

    void init();
    void runLoop();
    void cleanUp();

    int Debug = 0;

  private:
    bool should_close = false;
    double fpsLimit;
    double lastFrameTime;
    int display_w, display_h;
    vec4 clear_color;
    double now;
    GLFWwindow *window = nullptr;
    GLuint program;
    GLint mvp_location;
    GLuint vertex_array;
};

#endif //APPLICATION_H
