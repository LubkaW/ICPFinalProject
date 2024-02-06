#include <GL/glew.h> // GLEW provides efficient run-time mechanisms for determining which OpenGL extensions are supported on the target platform.
#include <GLFW/glfw3.h> // library for app window creation
#include <glm/glm.hpp> // ibrary for math operations
#include <glm/ext.hpp>
#include <iostream>
#include <filesystem>
#include "stb_image.h" // loading image textures library
#include <random>
#include <opencv2\opencv.hpp>
#include <chrono>
#include <thread>




#include "GameApp.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Plane.h"
#include "Model.h"




/*


	- The graphics pipeline takes as input a set of 3D coordinates and transforms these to colored 2D pixels on your screen.
	- The graphics pipeline can be divided into several steps where each step requires the output of the previous step as its input.
	SHADERS = The processing cores run small programs on the GPU for each step of the pipeline. These small programs are called shaders.
	- Shaders are written in the OpenGL Shading Language (GLSL)
	- Have to setup atleast vertex and fragment shaders !

	VERTEX SHADER = Process endpoints of polygons.
	FRAGMENT (PIXEL) SHADER = Process how pixels between verticies looks like

	- A fragment in OpenGL is all the data required for OpenGL to render a single pixel.

	modes: glBufferData()
	GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
	GL_STATIC_DRAW: the data is set only once and used many times.
	GL_DYNAMIC_DRAW: the data is changed a lot and used many times.



*/


float randomFloatInRange(float min, float max) {
	// Set up random number generation
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);

	// Generate random float
	return dis(gen);
}

bool areVectorsInRange(glm::vec3 vector1, glm::vec3 vector2, float range) {
	double distanceSquared = 0.0;
	for (int i = 0; i < 3; ++i) {
		double diff = vector1[i] - vector2[i];
		distanceSquared += diff * diff;
	}
	double distance = std::sqrt(distanceSquared);
	return distance <= range;
}

glm::vec3 getRotationsFromVectors(const glm::vec3& front, const glm::vec3& up) {
	// Step 1: Normalize the vectors
	glm::vec3 normalizedFront = glm::normalize(front);
	glm::vec3 normalizedUp = glm::normalize(up);

	// Step 2: Calculate the right vector
	glm::vec3 right = glm::cross(normalizedUp, normalizedFront);

	// Step 3: Calculate the angles
	float yaw = atan2(normalizedFront.y, normalizedFront.x);
	float pitch = atan2(-normalizedFront.z, sqrt(normalizedFront.x * normalizedFront.x + normalizedFront.y * normalizedFront.y));
	float roll = atan2(right.z, normalizedUp.z);

	std::cout << "Rotations: " << yaw << "  " << pitch << "  " << roll << "  " << std::endl;

	// Return the rotations
	return glm::vec3(yaw, pitch, roll);
}

void GameApp::init_opencv()
{
	//open first available camera
	capture = cv::VideoCapture(cv::CAP_DSHOW);

	if (!capture.isOpened())
	{
		std::cerr << "no camera source? Fallback to video..." << std::endl;

		//open video file
		capture = cv::VideoCapture("resources/video.mkv");
		if (!capture.isOpened())
		{
			std::cerr << "no source?... " << std::endl;
			exit(EXIT_FAILURE);
		}
	}
}

cv::Point2f GameApp::find_center_normalized_hsv(cv::Mat& frame)
{
	// convert to grayscale, create threshold, sum white pixels
	// compute centroid of white pixels (average X,Y coordinate of all white pixels)
	cv::Point2f center;
	cv::Point2f center_normalized;
	double h_low = 80.0;
	double s_low = 50.0;
	double v_low = 50.0;

	double h_hi = 100.0;
	double s_hi = 255.0;
	double v_hi = 255.0;

	cv::Mat scene_hsv, scene_threshold;

	cv::cvtColor(frame, scene_hsv, cv::COLOR_BGR2HSV);

	cv::Scalar lower_threshold = cv::Scalar(h_low, s_low, v_low);
	cv::Scalar upper_threshold = cv::Scalar(h_hi, s_hi, v_hi);
	cv::inRange(scene_hsv, lower_threshold, upper_threshold, scene_threshold);

	int sy = 0, sx = 0, s = 0;
	for (int y = 0; y < frame.rows; y++) //y
	{
		for (int x = 0; x < frame.cols; x++) //x
		{
			// FIND THRESHOLD (value 0..255)
			if (scene_threshold.at<unsigned char>(y, x) < 255) {
				// set output pixel black

			}
			else {
				// set output pixel white
				sx += x;
				sy += y;
				s++;
			}
		}
	}

	center = cv::Point2f(sx / (float)s, sy / (float)s);
	center_normalized = cv::Point2f(center.x / frame.cols, center.y / frame.rows);

	//std::cout << "Center absolute: " << center << '\n';
	//std::cout << "Center normalized: " << center_normalized << '\n';

	return center_normalized;
}


void GameApp::thread_code(void)
{
	cv::Mat frame;

	try {
		while (true)
		{
			capture >> frame;

			if (frame.empty())
				throw std::exception("Empty file? Wrong path?");

			//cv::Point2f center_normalized = find_center_normalized(frame);
			cv::Point2f center_normalized = find_center_normalized_hsv(frame);

			centre = center_normalized;
			//fronta.push_back(center_normalized);

			//std::this_thread::sleep_for(std::chrono::milliseconds(100));

			if (koncime)
			{
				capture.release();
				break;
			}
		}
	}
	catch (std::exception const& e) {
		std::cerr << "App failed : " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}


//constructor
GameApp::GameApp() {

}

// init game window
// register window callbacks
// returns window object
GLFWwindow* GameApp::game_init_window() {

	init_opencv();
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ICPFinalGames", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	// make window current context
	glfwSetWindowUserPointer(window, this);
	glfwMakeContextCurrent(window);
	//Vsync 0-off 1-on
	glfwSwapInterval(1);
	// some window setups
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide cursor

	// init callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	return window;

}


int GameApp::run_game() {

	GLFWwindow* window = game_init_window();
	std::thread vlakno(&GameApp::thread_code, this);

	if (window == NULL)
		return -1;

	// ini 
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return -1;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));


	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// ------------------------------------
	ShaderProgram ourShader("resources/shaders/vertex_shader.vert", "resources/shaders/fragment_shader.frag");
	ShaderProgram lightShader("resources/shaders/light_vertex_shader.vert", "resources/shaders/light_fragment_shader.frag");

	// Just for info: Getm maximun num of vertex attributes supported by GPU
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	// load models
	// -----------
	Model plane_model = Model("resources/objects/plane/My_plane_z.obj");
	Model ground = Model("resources/objects/ground/ground.obj");
	//Model ourModel = Model("resources/objects/honey_jar/honey_jar.obj");
	Model qube = Model("resources/objects/cube_textured/cube_textured_opengl.obj");
	//Model qube = Model("resources/objects/wooden_map/Wooden.obj");
	Model light = Model("resources/objects/cube/cube_triangles_normals_tex.obj");
	/* MAIN PROGRAM LOOP */
	double previousTime = glfwGetTime();
	int frameCount = 0;

	//light position
	glm::vec3 lightPos(-0.2f, -1.0f, -0.3f);
	glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

	//baloon positions (right,up,backward)
	glm::vec3 baloon_positions[] = {
	glm::vec3(randomFloatInRange(-5.0f,-2.0f),  randomFloatInRange(0.0f,3.0f),randomFloatInRange(-5.0f,-2.0f)),
	glm::vec3(randomFloatInRange(-5.0f,-2.0f),   randomFloatInRange(0.0f,3.0f),randomFloatInRange(-1.5f,1.5f)),
	glm::vec3(randomFloatInRange(-5.0f,-2.0f),   randomFloatInRange(0.0f,3.0f),randomFloatInRange(2.0f,5.0f)),
	glm::vec3(randomFloatInRange(-1.5f,1.5f),   randomFloatInRange(0.0f,3.0f),randomFloatInRange(-5.0f,-2.0f)),
	glm::vec3(randomFloatInRange(-1.5f,1.5f),   randomFloatInRange(0.0f,3.0f),randomFloatInRange(-1.5f,1.5f)),
	glm::vec3(randomFloatInRange(-1.5f,1.5f),   randomFloatInRange(0.0f,3.0f),randomFloatInRange(2.0f,5.0f)),
	glm::vec3(randomFloatInRange(2.0f,5.0f),   randomFloatInRange(0.0f,3.0f),randomFloatInRange(-5.0f,-2.0f)),
	glm::vec3(randomFloatInRange(2.0f,5.0f),   randomFloatInRange(0.0f,3.0f),randomFloatInRange(-1.5f,1.5f)),
	glm::vec3(randomFloatInRange(2.0f,5.0f),   randomFloatInRange(0.0f,3.0f),randomFloatInRange(2.0f,5.0f)),
	};

	glm::vec3 flame_forwards[100];
	float flame_lifecycle[100];
	float flame_lifespan[100];


	for (unsigned int i = 0; i < 100; i++) {
		flame_forwards[i] = glm::vec3(randomFloatInRange(-10.0f, 10.0f), randomFloatInRange(-10.0f, 10.0f), randomFloatInRange(-50.0f, -30.0f));
		flame_lifecycle[i] = randomFloatInRange(0.01f, 0.02f);
		flame_lifespan[i] = randomFloatInRange(0.1f, 0.5f);
	}


	int baloon_cooldowns[] = { 0,0,0,0,0,0,0,0,0 };


	glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
	};



	while (!glfwWindowShouldClose(window))
	{

		// Measure FPS speed
		float currentFrame = glfwGetTime();
		// count delta time
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		frameCount++;
		// If a second has passed.
		if (currentFrame - previousTime >= 1.0)
		{
			//system("cls");
			// Display the frame count here any way you want.
			std::cout << "FPS: " << frameCount << std::endl;
			//std::cout << "Plane_pos: " << plane.Position[0]<< " " << plane.Position[1] << " " << plane.Position[2] << std::endl;
			//std::cout << "Forward_pos: " << plane.Front[0] << " " << plane.Front[1] << " " << plane.Front[2] << std::endl;
			std::cout << "Score: " << score << std::endl;
			//std::cout << "PlanePosition: " << plane.Position[0] << "  " << plane.Position[1] << "  " << plane.Position[2] << "  " << std::endl;
			//std::cout << "PlaneFront: " << plane.Front[0] << "  " << plane.Front[1] << "  " << plane.Front[2] << "  " << std::endl;
			//std::cout << "PlaneYPR: " << plane.Yaw << "  " << plane.Pitch << "  " << plane.Roll << "  " << std::endl;

			std::cout << "Centre: " << centre << std::endl;

			//std::cout << "CameraPosition: " << camera.Position[0] << "  " << camera.Position[1] << "  " << camera.Position[2] << "  " << std::endl;
			//std::cout << "CameraFront: " << camera.Front[0] << "  " << camera.Front[1] << "  " << camera.Front[2] << "  " << std::endl;
			//std::cout << "CameraYPR: " << camera.Yaw << "  " << camera.Pitch << "  " << camera.Roll << "  " << std::endl;


			frameCount = 0;
			previousTime = currentFrame;
		}

		// check keyboard inputs
		processInput(window);


		//game
		//movement
		//camera.Position += camera.Front * camera.MovementSpeed * 0.01f;
		plane.Position += plane.Front * plane.MovementSpeed * 0.001f;


		//colisions
		for (unsigned int i = 0; i < 9; i++)
		{
			if (areVectorsInRange(plane.Position + plane.Front * 0.1f, baloon_positions[i], 0.3f) == true) {
				if (baloon_cooldowns[i] == 0) {
					score += 1;
					baloon_cooldowns[i] = 1000;
				}

			}
		}
		if (plane.Position.y < 0) {
			std::cout << "Boom, havaroval jsi... Finalni skore: " << score << std::endl;
			glfwSetWindowShouldClose(window, true);
		}







		/* rendering commands ... */

		// first clear the colorbuffer and depth buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// light setup color
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);


		// don't forget to enable shader before setting uniforms
		ourShader.use();
		ourShader.setVec3("viewPos", camera.Position);
		ourShader.setFloat("material.shininess", 32.0f);


		/*
		   Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		   the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		   by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		   by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
		*/
		// directional light
		ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[0].constant", 1.0f);
		ourShader.setFloat("pointLights[0].linear", 0.09f);
		ourShader.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
		ourShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[1].constant", 1.0f);
		ourShader.setFloat("pointLights[1].linear", 0.09f);
		ourShader.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		ourShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[2].constant", 1.0f);
		ourShader.setFloat("pointLights[2].linear", 0.09f);
		ourShader.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		ourShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[3].constant", 1.0f);
		ourShader.setFloat("pointLights[3].linear", 0.09f);
		ourShader.setFloat("pointLights[3].quadratic", 0.032f);

		/* Going 3D */


		//fixed 3rd person view
		// 2. View matrix
		//glm::mat4 view =glm::mat4(1.0f);
		//view = glm::translate(view, -plane.Position);
		//view = glm::translate(view, plane.Front*1.0f);
		//view = plane.GetViewMatrixWP()*view;

		//working 3rd person view
		// 2. View matrix
		//glm::mat4 view = glm::mat4(1.0f);
		//view = glm::translate(view, -plane.Position);
		//view = glm::translate(view, plane.Front * 1.0f);
		//view = camera.GetViewMatrixWP() * view;

		glm::mat4 view = glm::mat4(1.0f);
		// 2. View matrix
		// 
		//far view
		if (activeView == 0) {
			view = camera.GetViewMatrix();
		}
		//fixed 3rd person view
		else if (activeView == 1) {
			view = plane.GetViewMatrix() * view;
			//view = glm::translate(view, -plane.Position);
			view = glm::translate(view, plane.Front * 1.5f);
			view = glm::translate(view, -plane.Up * 0.5f);

		}
		//rotating 3rd person view
		else if (activeView == 2) {
			view = glm::lookAt(plane.Position - camera.Front * 1.0f, plane.Position, camera.Up) * view;
		}
		//


		// 3. Projection matrix
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("view", view);
		ourShader.setMat4("projection", projection);


		/* TRANSOFRAMTION */

		glm::mat4 model = glm::mat4(1.0f);
		// baloons
		for (unsigned int i = 0; i < 9; i++)
		{
			if (baloon_cooldowns[i] == 0) {
				model = glm::mat4(1.0f);
				model = glm::translate(model, baloon_positions[i]);
				float angle = 20.0f * i;
				model = glm::scale(model, glm::vec3(0.1f)); // Make it a smaller plane
				ourShader.setMat4("model", model);
				//qube.Draw(ourShader);
			}
			else {
				baloon_cooldowns[i] -= 1;
			}

		}
		//ground
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(10.0f)); // Make it a smaller plane
		ourShader.setMat4("model", model);
		ground.Draw(ourShader);


		//zcube test
		//model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, 10.0f));
		//model = glm::scale(model, glm::vec3(1.0f));
		//ourShader.setMat4("model", model);
		//qube.Draw(ourShader);

		//plane
		model = glm::mat4(1.0f);
		model = glm::translate(model, plane.Position);
		model = glm::rotate(model, glm::radians(plane.Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -glm::radians(plane.Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		//Planeview = plane.GetViewMatrixWP()*model;
		model = glm::scale(model, glm::vec3(0.01f)); // Make it a smaller plane
		ourShader.setMat4("model", model);
		plane_model.Draw(ourShader);

		//flame
		glm::mat4 flame_model = glm::mat4(1.0f);
		model = glm::mat4(1.0f);
		model = glm::translate(model, plane.Position);

		model = glm::translate(model, -plane.Front * 0.65f);
		model = glm::translate(model, plane.Up * 0.03f);
		model = glm::scale(model, glm::vec3(0.01f));
		model = glm::rotate(model, glm::radians(plane.Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -glm::radians(plane.Pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		lightShader.use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		for (unsigned int i = 0; i < 100; i++) {
			flame_lifecycle[i] += 0.01f;
			if (flame_lifecycle[i] > flame_lifespan[i]) {
				flame_forwards[i] = glm::vec3(randomFloatInRange(-10.0f, 10.0f), randomFloatInRange(-10.0f, 10.0f), randomFloatInRange(-50.0f, -30.0f));
				flame_lifecycle[i] = randomFloatInRange(0.01f, 0.02f);
				flame_lifespan[i] = randomFloatInRange(0.1f, 0.5f);
			}
			flame_model = glm::translate(model, flame_forwards[i] * flame_lifecycle[i]);
			flame_model = glm::scale(flame_model, glm::vec3(0.8f));
			lightShader.setMat4("model", flame_model);
			light.Draw(lightShader);
		}

		//flame test
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f));
		for (unsigned int i = 0; i < 100; i++) {
			flame_lifecycle[i] += 0.01f;
			if (flame_lifecycle[i] > flame_lifespan[i]) {
				flame_forwards[i] = glm::vec3(randomFloatInRange(-10.0f, 10.0f), randomFloatInRange(-10.0f, 10.0f), randomFloatInRange(-50.0f, -30.0f));
				flame_lifecycle[i] = randomFloatInRange(0.01f, 0.02f);
				flame_lifespan[i] = randomFloatInRange(0.1f, 0.5f);
			}
			flame_model = glm::translate(model, flame_forwards[i] * flame_lifecycle[i]);
			flame_model = glm::scale(flame_model, glm::vec3(0.8f));
			lightShader.setMat4("model", flame_model);
			light.Draw(lightShader);
		}


		/* Light */
		// don't forget to use the corresponding shader program first (to set the uniform)
		lightShader.use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightShader.setMat4("model", model);
			light.Draw(lightShader);
		}

		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	koncime = true;
	vlakno.join();
	return 0;


}


//destructor
GameApp::~GameApp() {


	glfwTerminate();
	std::cout << "Destructor was called." << std::endl;

}


/* Defined callbacks and key process inputs for game window */

// define key inputs
void GameApp::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		plane.ProcessKeyboard(P_FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		plane.ProcessKeyboard(P_BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		plane.ProcessKeyboard(P_LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		plane.ProcessKeyboard(P_RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		activeView = 0;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		activeView = 1;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		activeView = 2;

}

// callback on resize window
void GameApp::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// callback on mouse movement
void GameApp::mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {

	// Retrieve the user pointer
	GameApp* gameApp = static_cast<GameApp*>(glfwGetWindowUserPointer(window));
	if (gameApp) {

		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		float xoffset = xpos - gameApp->lastX;
		float yoffset = gameApp->lastY - ypos; // reversed since y-coordinates go from bottom to top

		gameApp->lastX = xpos;
		gameApp->lastY = ypos;
		gameApp->camera.ProcessMouseMovement(xoffset, yoffset);
		//gameApp->plane.ProcessMouseMovement(xoffset, yoffset);

	}
}


// scroll callback
void GameApp::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

	// Retrieve the user pointer
	GameApp* gameApp = static_cast<GameApp*>(glfwGetWindowUserPointer(window));

	// Call the class-specific function
	if (gameApp)
		gameApp->camera.ProcessMouseScroll(static_cast<float>(yoffset));

}
