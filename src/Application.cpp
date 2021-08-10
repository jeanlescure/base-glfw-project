#include "Application.h"

typedef struct Vertex
{
  vec2 pos;
  vec3 col;
} Vertex;

static const Vertex vertices[18] =
{
  { { -0.4330127018925f, -0.5f }, { 1.f, 0.f, 0.f } },
  { {  0.4330127018925f, -0.0f }, { 0.f, 1.f, 0.f } },
  { { -0.4330127018925f,  0.5f }, { 0.f, 0.f, 1.f } },
};

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";

Application *gApp;

Application::Application() {
  gApp = this;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    gApp->close();
  }
}

int Application::open() {
  if (!glfwInit()) {
    return 1;
  }

  glfwWindowHint(GLFW_DECORATED, 0);
  glfwWindowHint(GLFW_FLOATING, 1);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, !Debug);

  window = glfwCreateWindow(500, 500, "GL WINDOW", NULL, NULL);
  if (window == NULL) {
    return 2;
  }

  glfwSetWindowPos(window, 100, 300);

  glfwSetKeyCallback(window, key_callback);

  return 0;
}

void Application::close() {
  glfwSetWindowShouldClose(window, 1);
  should_close = true;
}

void Application::init() {
  clear_color[0] = 0.0;
  clear_color[1] = 0.0;
  clear_color[2] = 0.0;
  clear_color[3] = 0.0;

  fpsLimit = 1.0 / 60.0;
  lastFrameTime = 0;   // number of seconds since the last frame

  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
  glfwSwapInterval(1); // Enable vsync

  // NOTE: OpenGL error checks have been omitted for brevity

  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(vertex_shader);

  const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(fragment_shader);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  mvp_location = glGetUniformLocation(program, "MVP");
  const GLint vpos_location = glGetAttribLocation(program, "vPos");
  const GLint vcol_location = glGetAttribLocation(program, "vCol");

  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);
  glEnableVertexAttribArray(vpos_location);
  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*) offsetof(Vertex, pos));
  glEnableVertexAttribArray(vcol_location);
  glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void*) offsetof(Vertex, col));
}

void Application::runLoop() {
  while(!should_close) {
    now = glfwGetTime();

    if ((now - lastFrameTime) < fpsLimit) {
        std::chrono::milliseconds dura((int)(fpsLimit * 100));
        std::this_thread::sleep_for(dura);
    } else {
      glfwPollEvents();
      glfwGetFramebufferSize(window, &display_w, &display_h);
      const float ratio = display_w / (float) display_h;

      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
      glClear(GL_COLOR_BUFFER_BIT);

      glfwMakeContextCurrent(window);

      mat4x4 m, p, mvp;
      mat4x4_identity(m);
      mat4x4_rotate_Z(m, m, (float) glfwGetTime());
      mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      mat4x4_mul(mvp, p, m);

      glUseProgram(program);
      glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) &mvp);
      glBindVertexArray(vertex_array);
      glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));

      // draw your frame here
      glfwSwapBuffers(window);
      glFinish();

      // only set lastFrameTime when you actually draw something
      lastFrameTime = now;
    }
  }
}

void Application::cleanUp() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
