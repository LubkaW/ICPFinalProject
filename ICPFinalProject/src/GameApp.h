#pragma once

#include <GL/glew.h> // GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform.
#include <GLFW/glfw3.h> // library for app window creation
#include <glm/glm.hpp> // ibrary for math operations
#include <glm/ext.hpp>

#include "Camera.h"

class GameApp {

public:
	// settings
	const unsigned int SCR_WIDTH = 800;
	const unsigned int SCR_HEIGHT = 600;

	// timing
	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame


	// camera
	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;
	bool firstMouse = true;
	Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.3f));


	GameApp();
	int run_game();
	~GameApp();

	// callbacks method definition
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
	GLFWwindow* game_init_window();
	void processInput(GLFWwindow* window);
	
};
