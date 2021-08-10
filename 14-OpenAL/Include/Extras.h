#pragma once
#include <glm/fwd.hpp>
#include <Headers/Model.h>
#include <Headers/Terrain.h>
class Extras
{
	Model* model;
	Terrain* terrain;
	glm::mat4 modelMatrix;
	AbstractModel::OBB collider;


public:
	

	AbstractModel::OBB& Collider() { return collider; }
	Extras() : model(nullptr), terrain(nullptr)
	{
		Extras(nullptr, nullptr, glm::vec3(0.0));
	}


	Extras(Model* model, Terrain* terrain, glm::vec3 position) : model(model), terrain(terrain)
	{
		this->model = model;
		this->terrain = terrain;
		modelMatrix = glm::mat4(1.0);
		modelMatrix[3] = glm::vec4(position, 1);
		modelMatrix[3][1] = terrain->getHeightTerrain(modelMatrix[3][0], modelMatrix[3][2]);
	}

	
	void RenderBarrel() {
		this->model->render(modelMatrix);	
	}

	void UpdateCollider(string nombre, std::map<std::string,std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >& collidersOBB)
	{
		glm::mat4 modelMatrixCollider = glm::mat4(modelMatrix);
		collider.u = glm::quat_cast(modelMatrixCollider);
		modelMatrixCollider = glm::translate(modelMatrixCollider, this->model->getObb().c);
		collider.c = glm::vec3(modelMatrixCollider[3]);
		collider.e = this->model->getObb().e * glm::vec3(1.0);
		addOrUpdateColliders(collidersOBB, nombre, collider, modelMatrixCollider);
	}
};

