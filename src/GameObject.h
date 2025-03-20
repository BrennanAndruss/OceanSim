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
};

struct Material
{
	Shader* shader;
	Texture* texture;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;

	Material();
	Material(Shader* shader, Texture* texture, glm::vec3 ambient, glm::vec3 diffuse, 
		glm::vec3 specular, float shininess);
	~Material();
};

class GameObject
{
public:
	GameObject();
	GameObject(Transform transform, Mesh* mesh, Material* material);
	~GameObject();

	void draw(glm::vec3 lightDir, glm::vec3 cameraPos);

private:
	Transform transform;
	Mesh* mesh;
	Material* material;

};

#endif // _GAMEOBJECT_H_