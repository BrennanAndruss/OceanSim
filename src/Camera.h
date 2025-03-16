#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera
{
public:
	Camera();
	Camera(glm::vec3 position, int* screenWidth, int* screenHeight);
	~Camera();

	void updatePosition(glm::vec3 moveDirection, float deltaTime);
	void updateRotation(float xOffset, float yOffset);

	glm::mat4 getViewMatrix() const;
	void setupMatricesUbo();
	void updatePerspective();
	void updateView();

	float sensitivity;

private:
	glm::vec3 position;
	glm::vec3 velocity;
	float accelRate;

	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;

	float pitch;
	float yaw;

	int* screenWidth;
	int* screenHeight;
	GLuint matricesUboID;

	void updateCameraVectors();
};

#endif // CAMERA_H