//
// Created by vocasle on 11/24/21.
//

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader.h"
#include "utility.h"
#include "vertex_array.h"
#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"
#include "texture.h"

void process_input(GLFWwindow* window, Camera& camera, double delta_time)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.update_pos(Direction::FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.update_pos(Direction::BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.update_pos(Direction::LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.update_pos(Direction::RIGHT, delta_time);

	camera.toggle_acceleration(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
}

struct Light {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct Material {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

int main()
{
	constexpr int win_width = 800;
	constexpr int win_height = 600;
	GLFWwindow* window = init_gl_context(win_width, win_height);
	if (!window) {
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	const glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
	Camera camera(1.0f, camera_pos);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(window, &camera);
	glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
		static bool first_time = false;
		static double last_x = 0.0;
		static double last_y = 0.0;
		if (const auto c = static_cast<Camera*>(glfwGetWindowUserPointer(w))) {
			if (first_time) {
				last_x = x;
				last_y = y;
				first_time = false;
			}
			c->update_euler_angles(x - last_x, last_y - y);
			last_x = x;
			last_y = y;
		}
		});

	Shader lighting_shader("5.1.lighting.vert", "5.1.lighting.frag");
	Shader object_shader("object.vert", "object.frag");

	const std::vector<float> vertices = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	constexpr unsigned int MAX_POSITIONS = 10;
	constexpr glm::vec3 cube_positions[MAX_POSITIONS] = {
		{0.0f,  0.0f,  0.0f},
		{2.0f,  5.0f, -15.0f},
		{-1.5f, -2.2f, -2.5f},
		{-3.8f, -2.0f, -12.3f},
		{2.4f, -0.4f, -3.5f},
		{-1.7f,  3.0f, -7.5f},
		{1.3f, -2.0f, -2.5f},
		{1.5f,  2.0f, -2.5f},
		{1.5f,  0.2f, -1.5f},
		{-1.3f,  1.0f, -1.5f}
	};

	constexpr unsigned int NUM_PONT_LIGHTS = 4;
	constexpr glm::vec3 point_lights_positions[NUM_PONT_LIGHTS] = {
		{0.7f,  0.2f,  2.0f},
		{2.3f, -3.3f, -4.0f},
		{-4.0f,  2.0f, -12.0f},
		{0.0f,  0.0f, -3.0f}
	};

	glm::mat4 cube_world_positions[MAX_POSITIONS];

	for (unsigned int i = 0; i < MAX_POSITIONS; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cube_positions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		cube_world_positions[i] = model;
	}

	VertexArray object_va;
	VertexBuffer vb(vertices.data(), sizeof(float) * vertices.size());
	VertexBufferLayout vbl;
	vbl.add_element<float>(3);
	vbl.add_element<float>(3);
	vbl.add_element<float>(2);
	object_va.add_buffer(vb, vbl);

	GL_CALL(glEnable(GL_DEPTH_TEST));

	const glm::mat4 projection = glm::perspective(glm::radians(45.0f), win_width / static_cast<float>(win_height), 0.1f,
		100.0f);

	double begin = glfwGetTime();
	double end = 0.0;
	double time_span = 0.0;
	float angle = 1.0f;

	Light light{ {1.2f, 1.0f, 2.0f}, glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f) };
	Material bronze{
		glm::vec3(0.2125f, 0.1275f, 0.054f),
		glm::vec3(0.714f, 0.4284f, 0.18144f),
		glm::vec3(0.393548f, 0.271906f, 0.166721f),
		0.2f * 128.0f
	};

	Material chrome{
		glm::vec3(0.25f),
		glm::vec3(0.4f),
		glm::vec3(0.774597f),
		0.6f * 128.0f
	};

	Material m = bronze;
	Texture diffuse_map("../../assets/container2.png");
	Texture specular_map("../../assets/container2_specular.png");

	while (!glfwWindowShouldClose(window)) {
		end = glfwGetTime();
		time_span = end - begin;
		begin = end;
		process_input(window, camera, time_span);
		angle += static_cast<float>(time_span);

		GL_CALL(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
		GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		object_shader.use();
		object_shader.set_mat4("view", camera.get_view());
		object_shader.set_mat4("projection", projection);
		object_shader.set_vec3("view_pos", camera_pos);
		object_shader.set_int("material.diffuse", 0);
		object_shader.set_int("material.specular", 1);
		object_shader.set_float("material.shininess", m.shininess);
		object_shader.set_vec3("dir_light.direction", {-0.2f, -1.0f, -0.3f});
		object_shader.set_vec3("dir_light.ambient", glm::vec3(0.05f));
		object_shader.set_vec3("dir_light.diffuse", glm::vec3(0.4f));
		object_shader.set_vec3("dir_light.specular", glm::vec3(0.5f));
		for (unsigned int i = 0; i < NUM_PONT_LIGHTS; ++i) {
			object_shader.set_float(format("point_lights[{}].constant", i), 1.0f);
			object_shader.set_float(format("point_lights[{}].linear", i), 0.09f);
			object_shader.set_float(format("point_lights[{}].quadratic", i), 0.032f);
			object_shader.set_vec3(format("point_lights[{}].position", i), point_lights_positions[i]);
			object_shader.set_vec3(format("point_lights[{}].ambient", i), light.ambient);
			object_shader.set_vec3(format("point_lights[{}].diffuse", i), light.diffuse);
			object_shader.set_vec3(format("point_lights[{}].specular", i), light.specular);
		}
		object_va.bind();
		GL_CALL(glActiveTexture(GL_TEXTURE0));
		diffuse_map.bind();
		GL_CALL(glActiveTexture(GL_TEXTURE1));
		specular_map.bind();
		for (const auto& cube_world_position : cube_world_positions) {
			object_shader.set_mat4("model", cube_world_position);
			GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
		}

		lighting_shader.use();
		lighting_shader.set_mat4("view", camera.get_view());
		lighting_shader.set_mat4("projection", projection);
		for (unsigned int i = 0; const auto &point_lights_position : point_lights_positions) {
			glm::mat4 light_model(1.0f);
			light_model = glm::translate(light_model, point_lights_position);
			light_model = glm::scale(light_model, glm::vec3(0.2f));
			lighting_shader.set_mat4("model", light_model);
			lighting_shader.set_vec3("u_light.ambient", light.ambient);
			lighting_shader.set_vec3("u_light.diffuse", light.diffuse);
			lighting_shader.set_vec3("u_light.specular", light.specular);
			GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 36));
		}
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
