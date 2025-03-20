#pragma once

#ifndef HIERARCHY_NODE_H
#define HIERARCHY_NODE_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GameObject.h"


class HierarchyNode
{
public:
	GameObject* gameObject;
	std::vector<HierarchyNode*> children;

	HierarchyNode();
	HierarchyNode(GameObject* gameObject);
	~HierarchyNode();

	void addChild(HierarchyNode* child);
	void drawHierarchyFromRoot(glm::vec3 lightDir, glm::vec3 cameraPos);
	void drawHierarchy(glm::mat4 parentModel, glm::vec3 lightDir, glm::vec3 cameraPos);
	void clearHierarchy();
};

#endif // HIERARCHY_NODE_H