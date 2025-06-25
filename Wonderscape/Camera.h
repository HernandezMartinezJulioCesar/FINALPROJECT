#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include"shaderClass.h"

class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);

	glm::vec3 targetPoint = glm::vec3(0.0f); // Punto fijo de mira (B)
	float orbitRadius = 3.7f; // Distancia entre c mara y punto B
	float orbitAngleX = 0.0f; //  ngulo horizontal
	float orbitAngleY = 0.0f; //  ngulo vertical

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Controls whether camera inputs are processed
	bool inputsEnabled = true;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.1f;
	float sensitivity = 50.0f;

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position);

	// Updates the camera matrix to the Vertex Shader
	void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
	//
	void updateMatrixExplore(float FOVdeg, float nearPlane, float farPlane);
	// Exports the camera matrix to a shader
	void Matrix(Shader& shader, const char* uniform);
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
	//
	void OrbitInputs(GLFWwindow* window, float modelRadius, glm::vec2 screenCenter);
	//------------------------------------
	float clamp(float val, float minVal, float maxVal);
	void Object(glm::vec3 position, float radius); //add object
	bool Verify(glm::vec3 newPosition); // Verify colission
	glm::vec3 AdjustedPosition(glm::vec3 desiredPos);
};
#endif