#pragma once

#include <GL/glew.h> // GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform.
#include <GLFW/glfw3.h> // library for app window creation
#include <glm/glm.hpp> // ibrary for math operations
#include <glm/ext.hpp>

#include <opencv2\opencv.hpp>

#include "Camera.h"
#include "Plane.h"

class GameApp {

public:
	cv::Point2f find_center_normalized_hsv(cv::Mat& frame);
	void draw_cross_normalized(cv::Mat& img, cv::Point2f center_relative, int size);


	cv::Point2f centre;
	// settings
	const unsigned int SCR_WIDTH = 1920;
	const unsigned int SCR_HEIGHT = 1080;

	// timing
	float deltaTime = 0.0f;	// Time between current frame and last frame
	float lastFrame = 0.0f; // Time of last frame
	int score = 0;
	int activeView = 1;
	int controllMode = 0; //0=arrows,1=tracking
	bool GameFreeze = false;
	// camera
	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;
	bool firstMouse = true;
	Camera camera = Camera(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);

	// plane(position/front/yaw/pitch)
	Plane plane = Plane(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f);


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
	void ObjectDetection(void);
	void init_opencv();
	cv::VideoCapture capture;
	std::atomic<bool> GameEnd = false;

};
