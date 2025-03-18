#include "Camera.h"

#include <iostream>

Camera::Camera() : position(glm::vec3(0.0f, 0.0f, 0.0f)),
	velocity(glm::vec3(0.0f, 0.0f, 0.0f)), accelRate(0.25f), pitch(0.0f),
	yaw(-90.0f), sensitivity(0.05f), matricesUboID(0)
{
	updateCameraVectors();
}

Camera::Camera(glm::vec3 position, int* screenWidth, int* screenHeight) 
	: position(position), screenWidth(screenWidth), screenHeight(screenHeight),
	velocity(glm::vec3(0.0f, 0.0f, 0.0f)), accelRate(0.25f), pitch(0.0f), 
	yaw(-90.0f), sensitivity(0.05f), matricesUboID(0)
{
	updateCameraVectors();
}

Camera::~Camera() {}

void Camera::updatePosition(glm::vec3 moveDirection, float deltaTime)
{
	// std::cout << "shit happen???\n";

	if (moveDirection.x != 0.0f || moveDirection.y != 0.0f || moveDirection.z != 0.0f)
	{
		// Convert move direction to camera space
		moveDirection = moveDirection.x * right + moveDirection.y * up 
			+ moveDirection.z * front;

		// Normalize move direction
		moveDirection = glm::normalize(moveDirection);

		velocity += moveDirection * accelRate;
	}
	
	// Update position and dampen velocity
	position += velocity * deltaTime;
	velocity *= 0.95f;

	updateView();
}

void Camera::updateRotation(float xOffset, float yOffset)
{
	yaw += xOffset;
	pitch += yOffset;
	
	// Constrain pitch
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	updateCameraVectors();
	updateView();
}

void Camera::updateCameraVectors()
{
	// Convert from degrees to radians
	float p = glm::radians(pitch);
	float r = glm::radians(yaw);

	// Calculate new orthonormal basis vectors
	glm::vec3 front;
	front.x = cos(p) * cos(r);
	front.y = sin(p);
	front.z = cos(p) * sin(r);
	this->front = glm::normalize(front);
	this->right = glm::normalize(glm::cross(this->front, glm::vec3(0.0f, 1.0f, 0.0f)));
	this->up = glm::cross(this->right, this->front);
}

glm::vec3 Camera::getPosition() const
{
	return position;
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position + front, up);
}

void Camera::setupMatricesUbo()
{
	// Initialize UBO and allocate memory for two matrices
	glGenBuffers(1, &matricesUboID);
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUboID);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Bind UBO to binding point 0
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, matricesUboID, 0, 2 * sizeof(glm::mat4));

	// Store the projection matrix
	float aspect = (float)(*screenWidth) / (*screenHeight);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUboID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Store the view matrix
	glm::mat4 view = getViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUboID);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), 
		sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::updatePerspective()
{
	// Update the projection matrix
	float aspect = (float)(*screenWidth) / (*screenHeight);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUboID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::updateView()
{
	// Update the view matrix
	glm::mat4 view = getViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUboID);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), 
		sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
