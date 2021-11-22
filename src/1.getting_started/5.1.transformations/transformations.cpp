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
#include "stb_image.h"

struct VertexData {
  uint VAO = 0;
  uint VBO = 0;
  uint EBO = 0;
  uint texture = 0;
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

struct Point {
  int x;
  int y;
};

struct GameState {
  double start_time;
  double end_time;
  double tick_time;
  bool game_over;
  Difficulty difficulty;
  Difficulty prev_difficulty;
  GridSize grid_size;
  static constexpr float grid_zoom_levels[] = {-12.0f, -20.0f, -40.0f};
  std::string song;

  Direction direction;
  Direction prev_direction;

  Point velocity;

  Camera camera;

  ma_decoder decoder;
  ma_device_config device_config;
  ma_device device;

  std::deque<Point> snake_parts;
  Point meal;
  bool ate_meal;

  float texture_angle;
};

bool should_update_grid(const GameState &state) {
  return state.prev_difficulty != state.difficulty;
}

float get_zoom_level(const GameState &g);
void play_audio(GameState &);

void reset_game(GameState &g) {
  g.game_over = false;
  g.end_time = 0.0;
  g.start_time = glfwGetTime();
  g.direction = g.prev_direction = Direction::NONE;
  g.velocity = {0, 0};
  g.snake_parts = std::deque<Point>(1, {1, 1});
  g.ate_meal = true;
  play_audio(g);
  g.texture_angle = 0.0f;
}

void update_difficulty(GameState &g, Difficulty d) {
  std::cout << "Updating difficulty to " << static_cast<int>(d) << std::endl;
  g.difficulty = d;
  switch (d) {
  case Difficulty::LOW:g.song = "../assets/Greenberg.mp3";
    g.grid_size = GridSize::SMALL;
    g.tick_time = 0.5;
    break;
  case Difficulty::MEDIUM:g.song = "../assets/Drivin.mp3";
    g.grid_size = GridSize::MEDIUM;
    g.tick_time = 0.125;
    break;
  case Difficulty::HIGH:g.song = "../assets/Eastward.mp3";
    g.grid_size = GridSize::BIG;
    g.tick_time = 0.05;
    break;
  }

  reset_game(g);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void process_input(GLFWwindow *window, GameState &state);

GLFWwindow *init_glfw();

VertexData init_vertices();

Point update_pos(const Point &pos, GameState &state) {
  Point cur_pos = pos;

  if (state.prev_direction != state.direction
      && (state.direction == Direction::RIGHT && state.prev_direction != Direction::LEFT
          || state.direction == Direction::LEFT && state.prev_direction != Direction::RIGHT
          || state.direction == Direction::UP && state.prev_direction != Direction::DOWN
          || state.direction == Direction::DOWN && state.prev_direction != Direction::UP)) {
    state.velocity = {0, 0};
    if (state.direction == Direction::RIGHT) {
      state.texture_angle = 90.0f;
      ++state.velocity.x;
    }
    else if (state.direction == Direction::LEFT) {
      state.texture_angle = 270.0f;
      --state.velocity.x;
    }
    else if (state.direction == Direction::UP) {
      state.texture_angle = 180.0f;
      ++state.velocity.y;
    }
    else if (state.direction == Direction::DOWN) {
      state.texture_angle = 0.0f;
      --state.velocity.y;
    }
    state.prev_direction = state.direction;
  }
  cur_pos.x += state.velocity.x;
  cur_pos.y += state.velocity.y;

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
  for (int i = -grid_size / 2 + 1; i <= grid_size / 2; ++i)
    grid.insert(i);

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

  assert(x >= -grid_size / 2 && x <= grid_size / 2);
  assert(y >= -grid_size / 2 && y <= grid_size / 2);
  return {x, y};
}

std::vector<float> generate_grid(uint32_t max_grid_size) {
  const float offset = max_grid_size / 2.0f;
  std::vector<float> vertices;
  vertices.reserve(max_grid_size * 4 * 2);

  for (int i = 0; i <= max_grid_size; ++i) {
    vertices.push_back(-offset);
    vertices.push_back(-offset + i);
    vertices.push_back(offset);
    vertices.push_back(-offset + i);
  }

  for (int i = 0; i <= max_grid_size; ++i) {
    vertices.push_back(-offset + i);
    vertices.push_back(-offset);
    vertices.push_back(-offset + i);
    vertices.push_back(offset);
  }

  return vertices;
}

VertexData init_grid_vertices() {
  constexpr int max_grid_size = 8;
  VertexData vertex_data;
  vertex_data.vertices = generate_grid(max_grid_size);

  glGenVertexArrays(1, &vertex_data.VAO);
  glBindVertexArray(vertex_data.VAO);

  glGenBuffers(1, &vertex_data.VBO);
  glBindBuffer(GL_ARRAY_BUFFER, vertex_data.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.vertices.size(), &vertex_data.vertices[0], GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  return vertex_data;
}

void draw_grid(Shader &shader, VertexData &vertex_data, bool update_grid, uint32_t grid_size) {
  shader.use();
  shader.set_mat4("model", glm::mat4(1.0f));
  glBindVertexArray(vertex_data.VAO);

  if (update_grid) {
    vertex_data.vertices = generate_grid(static_cast<int>(grid_size));
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(double) * vertex_data.vertices.size(),
                 &vertex_data.vertices[0],
                 GL_DYNAMIC_DRAW);
  }

  glDrawArrays(GL_LINES, 0, vertex_data.vertices.size());
}

void print_snake_pos(const std::deque<Point> &snake, const Point &meal) {
  for (const Point &p: snake)
    std::cout << "(x=" << p.x << ", y=" << p.y << ") ";
  std::cout << " meal(x=" << meal.x << ", y=" << meal.y << ")";
  std::cout << std::endl;
}

void draw_snake(const std::deque<Point> &parts,
                const Shader &shader,
                const VertexData &vertex_data,
                const GameState &game) {
  shader.use();
  for (auto p: parts) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(p.x - 0.5f, p.y - 0.5f, 0.0f));
//    model = glm::scale(model, glm::vec3(0.99f));
    shader.set_mat4("model", model);
    shader.set_vec4("in_color", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    shader.set_float("in_rotation", glm::radians(game.texture_angle));
    glBindTexture(GL_TEXTURE_2D, vertex_data.texture);
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

  state.difficulty = state.prev_difficulty = Difficulty::LOW;
  state.direction = state.prev_direction = Direction::NONE;
  state.grid_size = GridSize::SMALL;
  state.song = "../assets/Greenberg.mp3";
  state.tick_time = 0.5;
  state.velocity = {0, 0};
  state.snake_parts = std::deque<Point>(1, {1, 1});
  state.ate_meal = false;
  state.texture_angle = 0.0f;
}

bool is_frame_passed(const GameState &state) {
  return state.end_time - state.start_time > state.tick_time;
}

void setup_uniforms(const Shader &shader, GameState &state);

void debug_draw(const Shader &shader, const VertexData &vertex_data, const GameState &game);
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
//  std::thread audio(play_audio, std::ref(game));

  while (!glfwWindowShouldClose(window)) {
    setup_uniforms(shader, game);
    setup_uniforms(grid_shader, game);
    process_input(window, game);
    game.end_time = glfwGetTime();

    if (is_frame_passed(game) && !game.game_over) {
//      print_snake_pos(game.snake_parts, game.meal);
      if (game.ate_meal)
        game.meal = spawn_meal(static_cast<int>(game.grid_size), game.snake_parts);

      Point p = update_pos(game.snake_parts[0], game);
      game.ate_meal = collided_with_meal(p, game.meal);
      move_snake(p, game.snake_parts, game.ate_meal);
      game.game_over = is_of_the_grid(p, static_cast<int>(game.grid_size)) || collided_with_self(game.snake_parts);
      game.start_time = game.end_time;
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    draw_grid(grid_shader, line_vertex_data, should_update_grid(game), static_cast<int>(game.grid_size));
    draw_snake(game.snake_parts, shader, vertex_data, game);
    if (!game.ate_meal)
      draw_meal(shader, vertex_data, game.meal);

    //debug_draw(shader, vertex_data, game);

    glfwSwapBuffers(window);
    glfwPollEvents();

    game.prev_difficulty = game.difficulty;
  }

  ma_device_uninit(&game.device);
  ma_decoder_uninit(&game.decoder);
  glfwTerminate();
}
void debug_draw(const Shader &shader, const VertexData &vertex_data, const GameState &game) {
  shader.use();
  shader.set_vec4("in_color", glm::vec4(1.0f));
  shader.set_mat4("view", game.camera.view);
  shader.set_mat4("projection", game.camera.projection);
  shader.set_mat4("model", glm::mat4(1.0f));

  glBindVertexArray(vertex_data.VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

float get_zoom_level(const GameState &g) {
  switch (g.grid_size) {
  case GridSize::SMALL:return GameState::grid_zoom_levels[0];
  case GridSize::MEDIUM:return GameState::grid_zoom_levels[1];
  case GridSize::BIG:return GameState::grid_zoom_levels[2];
  default:return GameState::grid_zoom_levels[0];
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
      -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
      -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // top left
      0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // top right
      0.5f, -0.5f, 0.0f, 1.0f, 0.0f // bottom right
  };

  vertex_data.indices = {
      0, 1, 2,
      2, 3, 0
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

  glGenTextures(1, &vertex_data.texture);
  glBindTexture(GL_TEXTURE_2D, vertex_data.texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int32_t width;
  int32_t height;
  int32_t nr_channels;
  unsigned char *data = stbi_load("../assets/ant.png", &width, &height, &nr_channels, 0);
  if (!data)
    std::cerr << "Failed to load texture from ../assets/ant.png" << std::endl;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(NULL));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

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

void play_audio(GameState &game) {
  if (ma_device_is_started(&game.device)) {
    ma_device_stop(&game.device);
    ma_decoder_uninit(&game.decoder);
  }

  ma_result result;
  result = ma_decoder_init_file(game.song.c_str(), NULL, &game.decoder);
  if (result != MA_SUCCESS) {
    return;
  }

  game.device_config = ma_device_config_init(ma_device_type_playback);
  game.device_config.playback.format = game.decoder.outputFormat;
  game.device_config.playback.channels = game.decoder.outputChannels;
  game.device_config.sampleRate = game.decoder.outputSampleRate;
  game.device_config.dataCallback = data_callback;
  game.device_config.pUserData = &game.decoder;

  if (ma_device_init(NULL, &game.device_config, &game.device) != MA_SUCCESS) {
    printf("Failed to open playback device.\n");
    ma_decoder_uninit(&game.decoder);
    return;
  }

  if (ma_device_start(&game.device) != MA_SUCCESS) {
    printf("Failed to start playback device.\n");
    ma_device_uninit(&game.device);
    ma_decoder_uninit(&game.decoder);
    return;
  }
}
