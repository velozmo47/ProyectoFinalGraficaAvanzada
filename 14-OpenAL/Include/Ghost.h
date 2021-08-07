#pragma once

class Ghost {
	Model* model;
	Terrain* terrain;
	glm::mat4 modelMatrix;
	AbstractModel::OBB collider;
	int ghostState;
	glm::vec3 previousObjective;
	glm::vec3 nextObjective;

public:
	Ghost() : model (nullptr), terrain (nullptr)
	{
		Ghost(nullptr, nullptr, glm::vec3(0.0));
	}

	Ghost(Model* model, Terrain* terrain, glm::vec3 position) : model(model), terrain(terrain)
	{
		this->model = model;
		this->terrain = terrain;

		ghostState = 0;
		modelMatrix = glm::mat4(1.0);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5));
		nextObjective = glm::vec3(position.x, 0, position.z);
		previousObjective = nextObjective;
		modelMatrix[3] = glm::vec4(position, 1);
	}

	void UpdateGhost(Maze* maze, float deltaTime)
	{
		switch (ghostState)
		{
		case 0:
			glm::vec3 currentPosition = glm::vec3(modelMatrix[3][0], 0, modelMatrix[3][2]);
			currentPosition = MoveTowards(currentPosition, nextObjective, 2 * deltaTime);

			if (currentPosition == nextObjective)
			{
				int x = floor(0.5f + currentPosition.x / maze->CellSize());
				int z = floor(0.5f + currentPosition.z / maze->CellSize());

				int xPrevious = floor(0.5f + previousObjective.x / maze->CellSize());
				int zPrevious = floor(0.5f + previousObjective.z / maze->CellSize());

				MazeCell* mazeCell = maze->GetMazeCell(x, z);
				MazeCell* neighboor = mazeCell->RandomNeighboor(xPrevious, zPrevious);
				previousObjective = currentPosition;
				nextObjective = glm::vec3(neighboor->x * maze->CellSize(), 0, neighboor->y * maze->CellSize());

				std::cout << "X: " << std::to_string(x) << " Z: " << std::to_string(z) << std::endl;
				std::cout << "X: " << std::to_string(mazeCell->x) << " Z: " << std::to_string(mazeCell->y) << std::endl;
			}

			float yPos = terrain->getHeightTerrain(currentPosition.x, currentPosition.z);
			modelMatrix[3] = glm::vec4 (currentPosition, 1);
			modelMatrix[3][1] = yPos + 2;

			break;
		}

		model->render(modelMatrix);

		//glm::mat4 modelMatrixCollider = modelMatrix;
		//collider.u = glm::quat_cast(modelMatrixCollider);
		//modelMatrixCollider = glm::translate(modelMatrixCollider, model->getObb().c);
		//collider.c = glm::vec3(modelMatrixCollider[3]);
		//collider.e = model->getObb().e * glm::vec3(1.0);
	}

	static glm::vec3 MoveTowards(glm::vec3 current, glm::vec3 target, float maxDistanceDelta)
	{
		glm::vec3 a = target - current;
		float magnitude = glm::length(a);
		if (magnitude <= maxDistanceDelta || magnitude == 0.0f)
		{
			return target;
		}
		return current + a / magnitude * maxDistanceDelta;
	}
};