#include"Model.h"
#include "Texture.h"
#include <algorithm> 

const unsigned int width = 1920;
const unsigned int height = 1080;
float rgb(int color) { return color / 255.0f; }
float clamp(float val, float minVal, float maxVal) { return std::max(minVal, std::min(val, maxVal)); }

Camera* globalCamera = nullptr;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_P && action == GLFW_PRESS && globalCamera != nullptr)
	{
		std::cout << "Camera Position: ("
			<< globalCamera->Position.x << ", "
			<< globalCamera->Position.y << ", "
			<< globalCamera->Position.z << ")\n";

		std::cout << "Camera Orientation: ("
			<< globalCamera->Orientation.x << ", "
			<< globalCamera->Orientation.y << ", "
			<< globalCamera->Orientation.z << ")\n";
	}
}

void ModelosEscenas()
{
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
	// Error check if the window fails to create
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, width, height);

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	// Take care of all the light related things
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	shaderProgram.Activate();
	glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(width, height, glm::vec3(0.0f, 10.0f, 1.0f));
	bool enZonaLimitada = false;
	glm::vec3 minLimite, maxLimite;
	globalCamera = &camera;

	/*
	* I'm doing this relative path thing in order to centralize all the resources into one folder and not
	* duplicate them between tutorial folders. You can just copy paste the resources from the 'Resources'
	* folder and then give a relative path from this folder to whatever resource you want to get to.
	* Also note that this requires C++17, so go to Project Properties, C/C++, Language, and select C++17
	*/
	// Load in a model
	Model model("Modelos/maravillas/scene.gltf");

	Texture Maravilla("Modelos/maravillas/textures/base_baseColor.png", "albedo", 0);
	//Original code from the tutorial
	// Model model("models/bunny/scene.gltf");

	for (auto& mesh : model.meshes) {
		mesh.textures.clear();
		mesh.textures.push_back(Maravilla);
	}

	glfwSetKeyCallback(window, keyCallback);
	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the backgroundrgb(154, 209, 255)
		glClearColor(rgb(154), rgb(209), rgb(209), 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Handles camera inputs
		camera.Inputs(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		// Draw a model
		model.Draw(shaderProgram, camera);

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		{
			Texture Maravilla("Modelos/maravillas/textures/Material.010_baseColor.png", "albedo", 0);
			for (auto& mesh : model.meshes) {
				mesh.textures.clear();
				mesh.textures.push_back(Maravilla);
			}

			// Nueva posición dentro del cubo
			camera.Position = glm::vec3(-23.8616, 7.84555, 33.0579);
			camera.Orientation = glm::normalize(glm::vec3(0.488122, -0.642787, 0.590388)); // Opcional: orientación deseada

			enZonaLimitada = true;

			/*minLimite = glm::vec3(-23.8616, 13.3356, 33.0579);
			maxLimite = glm::vec3(0.080306, -0.996192, 0.0338641);*/
		}

		/*if (enZonaLimitada)
		{
			camera.Position.x = clamp(camera.Position.x, minLimite.x, maxLimite.x);
			camera.Position.y = clamp(camera.Position.y, minLimite.y, maxLimite.y);
			camera.Position.z = clamp(camera.Position.z, minLimite.z, maxLimite.z);
		}*/

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			enZonaLimitada = false;
		}

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
}