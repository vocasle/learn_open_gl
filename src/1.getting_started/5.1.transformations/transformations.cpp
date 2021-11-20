//
// Created by vocasle on 11/7/21.
//

#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <set>
#include <deque>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
  UP, DOWN, LEFT, RIGHT, NONE
};

enum class Difficulty {
  LOW, MEDIUM, HIGH
};

enum class GridSize : uint8_t {
  SMALL = 8, MEDIUM = 16, BIG = 32
};

struct Camera {
  glm::mat4 view;
  glm::mat4 projection;
};

struct GameState {
  double start_time;
  double end_time;
  double tick_time;
  bool game_over;
  Difficulty difficulty;
  GridSize grid_size;
  static constexpr float grid_zoom_levels[] = {-8.0f, -15.0f, -40.0f};
  std::string song;

  Direction direction;
  Direction prev_direction;

  Camera camera;
};

float get_zoom_level(const GameState &g);

void update_difficulty(GameState &g, Difficulty d) {
  std::cout << "Updating difficulty to " << static_cast<int>(d) << std::endl;
  switch (d) {
  case Difficulty::LOW:g.song = "../assets/Greenberg.mp3";
    g.grid_size = GridSize::SMALL;
    g.tick_time = 1.0;
    break;
  case Difficulty::MEDIUM:g.song = "../assets/Drivin.mp3";
    g.grid_size = GridSize::MEDIUM;
    g.tick_time = 0.5;
    break;
  case Difficulty::HIGH:g.song = "../assets/Eastward.mp3";
    g.grid_size = GridSize::BIG;
    g.tick_time = 0.25;
    break;
  }
}

struct Point {
  int x;
  int y;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void process_input(GLFWwindow *window, GameState &state);

GLFWwindow *init_glfw();

VertexData init_vertices();

void play_audio(const char *file_name);

Point update_pos(const Point &pos, GameState &state) {
  Point cur_pos = pos;

  if (state.direction == Direction::RIGHT && state.prev_direction != Direction::LEFT)
    ++cur_pos.x;
  else if (state.direction == Direction::LEFT && state.prev_direction != Direction::RIGHT)
    --cur_pos.x;
  else if (state.direction == Direction::UP && state.prev_direction != Direction::DOWN)
    ++cur_pos.y;
  else if (state.direction == Direction::DOWN && state.prev_direction != Direction::UP)
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
  constexpr int max_grid_size = 32;
  constexpr float offset = max_grid_size / 2.0f;
  VertexData vertex_data;
  vertex_data.vertices.reserve(100);

  for (int i = 0; i <= max_grid_size; ++i) {
    vertex_data.vertices.push_back(-offset);
    vertex_data.vertices.push_back(-offset + i);
    vertex_data.vertices.push_back(offset);
    vertex_data.vertices.push_back(-offset + i);
  }

  for (int i = 0; i <= max_grid_size; ++i) {
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
  shader.set_mat4("model", glm::mat4(1.0f));
  glBindVertexArray(vertex_data.VAO);
  glDrawArrays(GL_LINES, 0, vertex_data.vertices.size());
}

void print_snake_pos(const std::deque<Point> &snake, const Point &meal) {
  for (const Point &p: snake)
    std::cout << "(x=" << p.x << ", y=" << p.y << ") ";
  std::cout << " meal(x=" << meal.x << ", y=" << meal.y << ")";
  std::cout << std::endl;
}

void draw_snake(const std::deque<Point> &deque_1, const Shader &shader, const VertexData &vertex_data) {
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

void init_game_state(GameState &state) {
  state.game_over = false;
  state.end_time = 0.0;
  state.start_time = glfwGetTime();

  state.camera.projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);;
  state.camera.view = glm::translate(glm::mat4(1.0f),
                                     glm::vec3(0.0f, 0.0f, get_zoom_level(state)));

  state.direction = state.prev_direction = Direction::NONE;
  update_difficulty(state, Difficulty::LOW);

}

bool is_frame_passed(const GameState &state) {
  return state.end_time - state.start_time > state.tick_time;
}

void setup_uniforms(const Shader &shader, GameState &state);

int main() {
  GLFWwindow *window = init_glfw();
  if (!window)
    return -1;

  Shader shader("5.1.shader.vs", "5.1.shader.fs");
  Shader grid_shader("5.1.line_shader.vs", "5.1.line_shader.fs");
  VertexData vertex_data = init_vertices();
  VertexData line_vertex_data = init_grid_vertices();

  GameState game;
  init_game_state(game);
  std::deque<Point> snake_parts(1, {1, 1});

//    std::thread audio(play_audio, "../assets/Greenberg.mp3");

  Point meal = spawn_meal(static_cast<int>(game.grid_size), snake_parts);
  bool ate_meal = false;

  while (!glfwWindowShouldClose(window)) {
    setup_uniforms(shader, game);
    setup_uniforms(grid_shader, game);
    process_input(window, game);
    game.end_time = glfwGetTime();

    if (is_frame_passed(game) && !game.game_over) {
      std::cout << "Frame took: " << game.end_time - game.start_time << std::endl;
      if (ate_meal)
        meal = spawn_meal(static_cast<int>(game.grid_size), snake_parts);

      Point p = update_pos(snake_parts[0], game);
      ate_meal = collided_with_meal(p, meal);
      move_snake(p, snake_parts, ate_meal);
      game.game_over = is_of_the_grid(p, static_cast<int>(game.grid_size)) || collided_with_self(snake_parts);
      game.start_time = game.end_time;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    draw_grid(grid_shader, line_vertex_data);
    draw_snake(snake_parts, shader, vertex_data);
    if (!ate_meal)
      draw_meal(shader, vertex_data, meal);

    glfwSwapBuffers(window);
    glfwPollEvents();

    game.prev_direction = game.direction;
  }

  glfwTerminate();
}

float get_zoom_level(const GameState &g) {
  switch (g.grid_size) {
  case GridSize::SMALL:return g.grid_zoom_levels[0];
  case GridSize::MEDIUM:return g.grid_zoom_levels[1];
  case GridSize::BIG:return g.grid_zoom_levels[2];
  default:return g.grid_zoom_levels[0];
  }
}

void setup_uniforms(const Shader &shader, GameState &state) {
  shader.use();
  state.camera.view = glm::translate(glm::mat4(1.0f),
                                     glm::vec3(0.0f, 0.0f, get_zoom_level(state)));
  shader.set_mat4("view", state.camera.view);
  shader.set_mat4("projection", state.camera.projection);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window, GameState &state) {
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
    state.direction = Direction::UP;
  else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    state.direction = Direction::DOWN;
  else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    state.direction = Direction::LEFT;
  else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    state.direction = Direction::RIGHT;
  else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    update_difficulty(state, Difficulty::LOW);
  else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    update_difficulty(state, Difficulty::MEDIUM);
  else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    update_difficulty(state, Difficulty::HIGH);
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
