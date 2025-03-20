#include "HierarchyNode.h"
#include <iostream>


HierarchyNode::HierarchyNode() : gameObject(nullptr) {}

HierarchyNode::HierarchyNode(GameObject* gameObject) : gameObject(gameObject) {}

HierarchyNode::~HierarchyNode() {}

void HierarchyNode::addChild(HierarchyNode* child)
{
	children.push_back(child);
}

void HierarchyNode::drawHierarchyFromRoot(glm::vec3 lightDir, glm::vec3 cameraPos)
{
	// Compose the model matrix for the current node
	glm::mat4 model = gameObject->transform.getCompositeTransform();

	// Recursively draw the children
	for (HierarchyNode* child : children)
	{
		child->drawHierarchy(model, lightDir, cameraPos);
	}
}

void HierarchyNode::drawHierarchy(glm::mat4 parentModel, glm::vec3 lightDir, glm::vec3 cameraPos)
{
	// Compose the model matrix for the current node
	glm::mat4 model = parentModel * gameObject->transform.getCompositeTransform();
	gameObject->draw(lightDir, cameraPos, &model);

	// Recursively draw the children
	for (HierarchyNode* child : children)
	{
		child->drawHierarchy(model, lightDir, cameraPos);
	}
}

void HierarchyNode::clearHierarchy()
{
	for (HierarchyNode* child : children)
	{
		child->clearHierarchy();
		delete child;
	}
	children.clear();
}
