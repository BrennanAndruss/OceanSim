#pragma once

#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <vector>
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"


struct Transform
{
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 scale;

	Transform();
	Transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);
	~Transform();

	glm::mat4 getCompositeTransform() const;
	glm::mat4 getCompositeTransform(glm::vec3 center) const;
};

struct Material
{
	Shader* shader;
	Texture* difTexture;
	Texture* specTexture;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Material();
	Material(Shader* shader, glm::vec3 ambient, Texture* difTexture,
		Texture* specTexture, float shininess);
	Material(Shader* shader, glm::vec3 ambient, glm::vec3 diffuse, 
		glm::vec3 specular, float shininess);
	~Material();
};

struct GameObject
{
	Transform transform;
	Mesh* mesh;
	Material* material;

	GameObject();
	GameObject(Transform transform, Mesh* mesh, Material* material);
	~GameObject();

	void draw(glm::vec3 lightDir, glm::vec3 cameraPos, glm::mat4* modelMat = nullptr);
};

#endif // _GAMEOBJECT_H_