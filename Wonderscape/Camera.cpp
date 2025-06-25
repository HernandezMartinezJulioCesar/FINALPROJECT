#include "Camera.h"
#include <glm/gtx/norm.hpp> 
#include <vector>

//-----------------------
struct CollisionObject {
	glm::vec3 position;
	float radius;

	CollisionObject(glm::vec3 pos, float rad) : position(pos), radius(rad) {}
};

std::vector<CollisionObject> obstacles;
float collisionRadius = 0.989f;

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::Object(glm::vec3 position, float radius) {
	obstacles.push_back(CollisionObject(position, radius));
}
bool Camera::Verify(glm::vec3 newPosition) {
	// Área aproximada de colisión (ajustar según escena)
	const float APPROX_COLLISION_AREA = 10.1f * collisionRadius;
	const float APPROX_COLLISION_AREA_SQ = APPROX_COLLISION_AREA * APPROX_COLLISION_AREA;

	for (const auto& obstacle : obstacles) {
		const glm::vec3 diff = newPosition - obstacle.position;
		const float approxDistanceSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

		// Early exit para objetos muy lejanos
		if (approxDistanceSq > APPROX_COLLISION_AREA_SQ) continue;

		// Cálculo preciso solo para objetos cercanos
		const float minDistance = obstacle.radius + collisionRadius + 0.001f;
		const float minDistanceSq = minDistance * minDistance;

		if (approxDistanceSq < minDistanceSq) {
			return true;
		}
	}
	return false;
}
glm::vec3 Camera::AdjustedPosition(glm::vec3 desiredPos) {
	const float EPSILON = 0.001f;
	glm::vec3 adjustedPos = desiredPos;

	for (const auto& obstacle : obstacles) {
		glm::vec3 diff = adjustedPos - obstacle.position;
		float distanceSq = glm::dot(diff, diff);
		float minDistance = obstacle.radius + collisionRadius + EPSILON;
		float minDistanceSq = minDistance * minDistance;

		if (distanceSq >= minDistanceSq) continue;

		float distance = glm::sqrt(distanceSq);

		glm::vec3 pushDir;
		if (distance > EPSILON) {
			pushDir = diff / distance;
		}
		else {
			pushDir = glm::vec3(1.0f, 0.0f, 0.0f);
		}
		adjustedPos = obstacle.position + pushDir * minDistance;
		diff = adjustedPos - obstacle.position;
	}
	return adjustedPos;
}
float Camera::clamp(float val, float minVal, float maxVal) { return (val < minVal) ? minVal : (val > maxVal) ? maxVal : val; }
//----------------------------

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	// Sets new camera matrix
	cameraMatrix = projection * view;
}

void Camera::updateMatrixExplore(float FOVdeg, float nearPlane, float farPlane) {
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Orientaci n siempre hacia el targetPoint
	Orientation = glm::normalize(targetPoint - Position);
	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
	cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	// Exports camera matrix
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window)
{
	if (!inputsEnabled) {
		// Restore cursor if inputs are disabled
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
		return;
	}

	// -----------------------------------------------------
	glm::vec3 desiredPosition = Position;

	// Movimiento WASD (igual que antes, pero guardamos en desiredPosition)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		desiredPosition += speed * Orientation;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		desiredPosition += speed * -glm::normalize(glm::cross(Orientation, Up));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		desiredPosition += speed * -Orientation;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		desiredPosition += speed * glm::normalize(glm::cross(Orientation, Up));
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		desiredPosition += speed * Up;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		desiredPosition += speed * -Up;

	if (Verify(desiredPosition)) {
		Position = AdjustedPosition(desiredPosition);
	}
	else {
		Position = desiredPosition; // Si no hay colisión, nos movemos libremente
	}
	//--------------------------------------------------------------
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.0011f * 50;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.0011f * 25;
	}

	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
}

void Camera::OrbitInputs(GLFWwindow* window, float modelRadius, glm::vec2 screenCenter) {
	if (!inputsEnabled) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		return;
	}

	// Obtener posición actual del mouse
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	glm::vec2 mousePos(mouseX, mouseY);

	// Calcular distancia al centro
	float distanceToCenter = glm::distance(mousePos, screenCenter);

	// Solo procesar si el boton izquierdo esta presionado y el mouse esta dentro del area circular
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && distanceToCenter <= modelRadius) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Calcular movimiento relativo al centro
		float deltaX = mouseX - screenCenter.x;
		float deltaY = mouseY - screenCenter.y;

		// Ajustar sensibilidad segun el radio (opcional)
		float sensitivityFactor = 1.0f + (1.0f - (distanceToCenter / modelRadius)); // Mas sensible cerca del borde

		// Actualizar angulos orbitales
		orbitAngleY -= sensitivity * sensitivityFactor * deltaY / height;
		orbitAngleX += sensitivity * sensitivityFactor * deltaX / width;

		// Limitar angulo vertical (evitar volteretas)
		orbitAngleY = glm::clamp(orbitAngleY, -89.0f, 89.0f);

		// Calcular nueva posicion orbital
		Position.x = targetPoint.x + orbitRadius * cos(glm::radians(orbitAngleY)) * sin(glm::radians(orbitAngleX));
		Position.y = targetPoint.y + orbitRadius * sin(glm::radians(orbitAngleY));
		Position.z = targetPoint.z + orbitRadius * cos(glm::radians(orbitAngleY)) * cos(glm::radians(orbitAngleX));

		// Orientacion hacia el punto objetivo
		Orientation = glm::normalize(targetPoint - Position);

		// Centrar cursor
		glfwSetCursorPos(window, screenCenter.x, screenCenter.y);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}