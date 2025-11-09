#include "GameObject.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


#pragma region Transform

Transform::Transform() 
	: translation(glm::vec3(0.0f)), rotation(glm::vec3(0.0f)), scale(glm::vec3(1.0f)) {}

Transform::Transform(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
	: translation(translation), rotation(rotation), scale(scale) {}

Transform::~Transform() {}

// Returns the composited transformation matrix
// Rotates around the origin
glm::mat4 Transform::getCompositeTransform() const
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, translation);
	
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	
	model = glm::scale(model, scale);

	return model;
}

// Returns the composited transformation matrix
// Rotates around the center of the object
glm::mat4 Transform::getCompositeTransform(glm::vec3 center) const
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, -center);

	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	model = glm::scale(model, center + translation);

	return model;
}

#pragma endregion


#pragma region Material

Material::Material() : shader(nullptr), difTexture(nullptr), specTexture(nullptr), 
	ambient(glm::vec3(0.0f)), diffuse(glm::vec3(0.0f)), specular(glm::vec3(0.0f)), 
	shininess(0.0f) {}

Material::Material(Shader* shader, glm::vec3 ambient, glm::vec3 diffuse,
	glm::vec3 specular, float shininess) : shader(shader), difTexture(nullptr),
	specTexture(nullptr), ambient(ambient), diffuse(diffuse), specular(specular), 
	shininess(shininess) {}

Material::Material(Shader* shader, glm::vec3 ambient, Texture* difTexture,
	Texture* specTexture, float shininess) : shader(shader), difTexture(difTexture),
	specTexture(specTexture), ambient(ambient), diffuse(glm::vec3(0.0f)), 
	specular(glm::vec3(0.0f)), shininess(shininess) {}

Material::~Material() {}

#pragma endregion


#pragma region GameObject

GameObject::GameObject() 
	: transform(Transform()), mesh(nullptr), material(nullptr) {}

GameObject::GameObject(Transform transform, Mesh* mesh, Material* material) 
	: transform(transform), mesh(mesh), material(material) {}

GameObject::~GameObject() {}

void GameObject::draw(glm::vec3 lightDir, glm::vec3 cameraPos, glm::mat4* modelMat)
{
	// Set generic shader uniforms
	material->shader->bind();
	if (modelMat)
	{
		material->shader->setMat4("model", *modelMat);
	}
	else
	{
		material->shader->setMat4("model", transform.getCompositeTransform());
	}
	material->shader->setVec3("lightDir", lightDir);
	material->shader->setVec3("cameraPos", cameraPos);

	// Set material properties
	material->shader->setVec3("matAmb", material->ambient);
	if (material->difTexture)
	{
		GLint handle = glGetUniformLocation(material->shader->getPid(), "matDif");
		material->difTexture->bind(handle);
	}
	else
	{
		material->shader->setVec3("matDif", material->diffuse);
	}

	if (material->specTexture)
	{
		GLint handle = glGetUniformLocation(material->shader->getPid(), "matSpec");
		material->difTexture->bind(handle);
	}
	else
	{
		material->shader->setVec3("matSpec", material->specular);
	}
	
	mesh->draw();

	// Unbind resources
	material->shader->unbind();
	if (material->difTexture)
		material->difTexture->unbind();
}

#pragma endregion
