//
// Created by vocasle on 11/7/21.
//

#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <deque>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <set>

#include "miniaudio.h"

#include "shader.h"

struct VertexData {
  uint VAO = 0;
  uint VBO = 0;
  uint EBO = 0;
  std::vector<float> vertices;
  std::vector<uint> indices;
};

enum class Direction {
  up, down, left, right, none
};

struct Controller {
  Direction direction = Direction::none;
  Direction prev_direction = Direction::none;
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 projection;
  float angle = 0.0f;
  std::deque<glm::mat4> positions;
};

struct Point {
  int x;
  int y;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void update_controls(Controller &controller);

void process_input(GLFWwindow *window, Controller &controller);

GLFWwindow *init_glfw();

VertexData init_vertices();

void play_audio(const char *file_name);

void draw_piece(uint program_id, const VertexData &vertex_data, const glm::mat4 &view);

Controller init_controller();

void setup_uniforms(const Shader &shader, Controller &controller);

Point update_pos(const Point &pos, Controller &c) {
  Point cur_pos = pos;

  if (c.direction == Direction::right && c.prev_direction != Direction::left)
    ++cur_pos.x;
  else if (c.direction == Direction::left && c.prev_direction != Direction::right)
    --cur_pos.x;
  else if (c.direction == Direction::up && c.prev_direction != Direction::down)
    ++cur_pos.y;
  else if (c.direction == Direction::down && c.prev_direction != Direction::up)
    --cur_pos.y;
  return cur_pos;
}

bool is_of_the_grid(const Point &cur_pos, int grid_size) {
  return !(cur_pos.x > -grid_size / 2 && cur_pos.x <= grid_size / 2 && cur_pos.y > -grid_size / 2
      && cur_pos.y <= grid_size / 2);
}

void move_snake(const Point &p, std::deque<Point> &snake_parts, bool save_last_segment) {
  if (!save_last_segment)
    snake_parts.pop_back();
  snake_parts.push_front(p);
}

bool collided_with_meal(const Point &point, const Point &meal) {
  return point.x == meal.x && point.y == meal.y;
}

bool operator==(const Point &lhs, const Point &rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool collided_with_self(const std::deque<Point> &snake_parts) {
  if (snake_parts.size() == 1)
    return false;

  for (auto it = std::begin(snake_parts) + 1; it != std::end(snake_parts); ++it)
    if (it->x == snake_parts.front().x && it->y == snake_parts.front().y)
      return true;
  return false;
}

Point spawn_meal(int grid_size, const std::deque<Point> &snake_parts) {

  std::set<int> busy_x;
  std::set<int> busy_y;

  for (auto p: snake_parts) {
    busy_x.insert(p.x);
    busy_y.insert(p.y);
  }

  std::set<int> grid;
  for (int i = -grid_size / 2 + 1; i <= grid_size / 2; ++i) {
    grid.insert(i);
  }

  std::vector<int> free_x;
  std::vector<int> free_y;
  std::set_difference(
      grid.begin(), grid.end(),
      busy_x.begin(), busy_x.end(),
      std::inserter(free_x, free_x.end())
  );
  std::set_difference(
      grid.begin(), grid.end(),
      busy_y.begin(), busy_y.end(),
      std::inserter(free_y, free_y.end())
  );

  int x = !free_x.empty() ? free_x[std::rand() % free_x.size()] : snake_parts[std::rand() % snake_parts.size()].x;
  int y = !free_y.empty() ? free_y[std::rand() % free_y.size()] : snake_parts[std::rand() % snake_parts.size()].y;

  return {x, y};
}

VertexData init_grid_vertices() {
  constexpr float offset = 4.0;
  VertexData vertex_data;
  vertex_data.vertices.reserve(100);

  for (int i = 0; i < 10; ++i) {
    vertex_data.vertices.push_back(-offset);
    vertex_data.vertices.push_back(-offset + i);
    vertex_data.vertices.push_back(offset);
    vertex_data.vertices.push_back(-offset + i);
  }

  for (int i = 0; i < 10; ++i) {
    vertex_data.vertices.push_back(-offset + i);
    vertex_data.vertices.push_back(-offset);
    vertex_data.vertices.push_back(-offset + i);
    vertex_data.vertices.push_back(offset);
  }

  glGenVertexArrays(1, &vertex_data.VAO);
  glBindVertexArray(vertex_data.VAO);

  glGenBuffers(1, &vertex_data.VBO);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_data.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.vertices.size(), &vertex_data.vertices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  return vertex_data;
}

void draw_grid(Shader &shader, const VertexData &vertex_data) {
  shader.use();
  glBindVertexArray(vertex_data.VAO);
  glDrawArrays(GL_LINES, 0, vertex_data.vertices.size());
}

void print_snake_pos(const std::deque<Point> &snake, const Point &meal) {
  for (const Point &p: snake)
    std::cout << "(x=" << p.x << ", y=" << p.y << ") ";
  std::cout << " meal(x=" << meal.x << ", y=" << meal.y << ")";
  std::cout << std::endl;
}

void draw_snake(const std::deque<Point> &deque_1, const Shader &shader, const VertexData &vertex_data,
                const Controller &controller) {
  shader.use();

  for (auto p: deque_1) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(p.x - 0.5f, p.y - 0.5f, 0.0f));
//    model = glm::scale(model, glm::vec3(0.99f));
    shader.set_mat4("model", model);
    shader.set_vec4("in_color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    glBindVertexArray(vertex_data.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
  }

}

void draw_meal(const Shader &shader, const VertexData &vertex_data, const Point &meal) {
  shader.use();
  glBindVertexArray(vertex_data.VAO);
  shader.set_vec4("in_color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
  shader.set_mat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(meal.x - 0.5f, meal.y - 0.5f, 0.0f)));
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
int main() {
  GLFWwindow *window = init_glfw();
  if (!window)
    return -1;

  Controller c;
  Controller c2;
  Shader shader("5.1.shader.vs", "5.1.shader.fs");
  Shader line_shader("5.1.line_shader.vs", "5.1.line_shader.fs");
  VertexData vertex_data = init_vertices();
  setup_uniforms(line_shader, c);
  setup_uniforms(shader, c2);

  std::vector<std::vector<bool>> grid(10, std::vector<bool>(10, false));
  std::deque<Point> snake_parts(1, {1, 1});
  double start_time = glfwGetTime();
  double end_time = 0;

////    std::thread audio(play_audio, "../assets/Greenberg.mp3");

  // Make snake peace twice smaller to fit into the grid and move to the
  VertexData line_vertex_data = init_grid_vertices();
  c2.model = glm::scale(c2.model, glm::vec3(0.5));
  c2.model = glm::translate(c2.model, glm::vec3(1.0f, 1.0f, 0.0f));
  shader.set_mat4("model", c2.model);
  bool game_over = false;
  constexpr int grid_size = 8;
  Point meal = spawn_meal(grid_size, snake_parts);
  bool ate_meal = false;
  constexpr double tick_interval = 1.0;

  while (!glfwWindowShouldClose(window)) {
    process_input(window, c2);
    end_time = glfwGetTime();

    if (end_time - start_time > tick_interval / 2 && !game_over) {
      if (ate_meal)
        meal = spawn_meal(grid_size, snake_parts);

      Point p = update_pos(snake_parts[0], c2);
      ate_meal = collided_with_meal(p, meal);
      move_snake(p, snake_parts, ate_meal);
      print_snake_pos(snake_parts, meal);
      game_over = is_of_the_grid(p, grid_size) || collided_with_self(snake_parts);
      start_time = end_time;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    draw_grid(line_shader, line_vertex_data);
    draw_snake(snake_parts, shader, vertex_data, c2);
    if (!ate_meal)
      draw_meal(shader, vertex_data, meal);

    glfwSwapBuffers(window);
    glfwPollEvents();

    c2.prev_direction = c2.direction;
  }

  glfwTerminate();
}

void setup_uniforms(const Shader &shader, Controller &controller) {
  glm::mat4 model(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

  glm::mat4 view(1.0f);
  view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  shader.use();
  GLint model_loc = glGetUniformLocation(shader.program_id, "model");
  GLint view_loc = glGetUniformLocation(shader.program_id, "view");
  GLint projection_loc = glGetUniformLocation(shader.program_id, "projection");

  glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

  controller.view = view;
  controller.model = model;
  controller.projection = projection;
}

void draw_piece(uint program_id, const VertexData &vertex_data, const glm::mat4 &view) {
  const GLint view_loc = glGetUniformLocation(program_id, "view");
  const GLint color_loc = glGetUniformLocation(program_id, "in_color");
  glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
  const glm::vec4 color(1.0f);
  glUniform4fv(color_loc, 1, glm::value_ptr(color));

  glBindVertexArray(vertex_data.VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

float direction_as_angle(Direction direction) {
  switch (direction) {
  case Direction::up:return glm::radians(90.0f);
  case Direction::down:return glm::radians(270.0f);
  case Direction::left:return glm::radians(180.0f);
  case Direction::right:return glm::radians(0.0f);
  default:return 0.0f;
  }
}

void update_controls(Controller &controller) {
  float speed = 2.0f;
  const glm::vec3 z_axis(0.0f, 0.0f, 1.0f);

  controller.view = glm::translate(controller.view, glm::vec3(speed, 0.0f, 0.0f));
  const bool is_moving_along_x_axis =
      (controller.direction == Direction::left || controller.direction == Direction::right)
          && (controller.prev_direction == Direction::left || controller.prev_direction == Direction::right);
  const bool is_moving_along_y_axis =
      (controller.direction == Direction::up || controller.direction == Direction::down)
          && (controller.prev_direction == Direction::up || controller.prev_direction == Direction::down);
  if (controller.direction != Direction::none && controller.direction != controller.prev_direction
      && !is_moving_along_x_axis && !is_moving_along_y_axis) {
    float angle = direction_as_angle(controller.direction);
    controller.view = glm::rotate(controller.view, angle - controller.angle, z_axis);
    controller.angle = angle;
  }

  controller.positions.push_back(controller.view);
  controller.prev_direction = controller.direction;
}

void process_input(GLFWwindow *window, Controller &controller) {
  static bool fill = true;
  static bool e_pressed = false;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    e_pressed = true;
  else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE && e_pressed) {
    fill = !fill;
    glPolygonMode(GL_FRONT_AND_BACK, fill ? GL_FILL : GL_LINE);
    e_pressed = false;
  } else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    controller.direction = Direction::up;
  else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    controller.direction = Direction::down;
  else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    controller.direction = Direction::left;
  else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    controller.direction = Direction::right;
}

VertexData init_vertices() {
  VertexData vertex_data;
  vertex_data.vertices = {
      -0.5f, -0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      -0.5f, 0.5f, 0.0f,
      0.5f, 0.5f, 0.0f
  };

  vertex_data.indices = {
      0, 1, 2,
      1, 2, 3
  };

  glGenVertexArrays(1, &vertex_data.VAO);
  glBindVertexArray(vertex_data.VAO);

  glGenBuffers(1, &vertex_data.VBO);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_data.VBO);
  glBufferData(GL_ARRAY_BUFFER, vertex_data.vertices.size() * sizeof(float), &vertex_data.vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &vertex_data.EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_data.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertex_data.indices.size() * sizeof(uint), &vertex_data.indices[0],
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void *>(NULL));
  glEnableVertexAttribArray(0);

  return vertex_data;
}

GLFWwindow *init_glfw() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return nullptr;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    return nullptr;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  return window;
}

Controller init_controller() {
  Controller controller;
  return controller;
}

void clamp_translation(glm::mat4 &trans) {
  constexpr float max = 50.0f;
  if (trans[3][0] > max)
    trans[3][0] = -max;
  else if (trans[3][0] < -max)
    trans[3][0] = max;

  if (trans[3][1] > max)
    trans[3][1] = -max;
  else if (trans[3][1] < -max)
    trans[3][1] = max;
}

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
  ma_bool32 isLooping = MA_TRUE;

  ma_decoder *pDecoder = (ma_decoder *) pDevice->pUserData;
  if (pDecoder == NULL) {
    return;
  }

  /*
  A decoder is a data source which means you can seemlessly plug it into the ma_data_source API. We can therefore take advantage
  of the "loop" parameter of ma_data_source_read_pcm_frames() to handle looping for us.
  */
  ma_data_source_read_pcm_frames(pDecoder, pOutput, frameCount, NULL, isLooping);

  (void) pInput;
}

void play_audio(const char *file_name) {
  ma_result result;
  ma_decoder decoder;
  ma_device_config deviceConfig;
  ma_device device;

  result = ma_decoder_init_file(file_name, NULL, &decoder);
  if (result != MA_SUCCESS) {
    return;
  }

  deviceConfig = ma_device_config_init(ma_device_type_playback);
  deviceConfig.playback.format = decoder.outputFormat;
  deviceConfig.playback.channels = decoder.outputChannels;
  deviceConfig.sampleRate = decoder.outputSampleRate;
  deviceConfig.dataCallback = data_callback;
  deviceConfig.pUserData = &decoder;

  if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
    printf("Failed to open playback device.\n");
    ma_decoder_uninit(&decoder);
    return;
  }

  if (ma_device_start(&device) != MA_SUCCESS) {
    printf("Failed to start playback device.\n");
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
    return;
  }

  printf("Press Enter to quit...");
  getchar();

  ma_device_uninit(&device);
  ma_decoder_uninit(&decoder);
}
