using namespace std; 

class MazeCell
{
	std::vector<glm::mat4> wallsMatrix;	// pos and rotation
	std::vector<glm::mat4> pillarsMatrix;
	std::vector<MazeCell*> adyacentMazeCells;

public:
	int x;
	int y;
	bool hasTorch;
	glm::mat4 torchMatrix;

	MazeCell()
	{
		hasTorch = rand() % 100 < 60;
		x = 0;
		y = 0;
		torchMatrix = glm::mat4(1.0);
	}

	std::vector<MazeCell*>& AdyacentMazeCells() 
	{
		return adyacentMazeCells;
	}

	MazeCell* RandomNeighboor(int xPrevious, int yPrevious)
	{
		if (adyacentMazeCells.size() == 1)
		{
			return adyacentMazeCells[0];
		}

		std::vector<MazeCell*> aux_adyacents;
		MazeCell* neighboor;
		for (int i = 0; i < adyacentMazeCells.size(); i++)
		{
			neighboor = adyacentMazeCells[i];
			if (!(xPrevious == neighboor->x && yPrevious == neighboor->y))
			{
				aux_adyacents.push_back(neighboor);
			}
		}

		neighboor = aux_adyacents[rand() % aux_adyacents.size()];
		return neighboor;
	}

	void AddAdyacentCell(MazeCell* mazeCell)
	{
		adyacentMazeCells.push_back(mazeCell);
	}

	void AddWall(float rotation, glm::vec3 position)
	{
		AddModelMatrix(wallsMatrix, rotation, position);
	}

	void AddPilar(float rotation, glm::vec3 position)
	{
		AddModelMatrix(pillarsMatrix, rotation, position);
	}

	void AddModelMatrix(std::vector<glm::mat4>& matrix, float rotation, glm::vec3 position)
	{
		glm::mat4 modelMatrix = glm::mat4(1.0);
		modelMatrix[3] = glm::vec4(position, 1);
		modelMatrix = glm::rotate(modelMatrix, (float)glm::radians(rotation), glm::vec3(0.0, 1.0, 0.0));
		matrix.push_back(modelMatrix);
	}

	void DisplayCell(Model& modelNodo, Model& modelPared, Model& modelAntorcha, std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>& collidersOBB)
	{
		for (int i = 0; i < wallsMatrix.size(); i++)
		{
			modelPared.render(wallsMatrix[i]);
			UpdateCollider(modelPared, "Pared_" + std::to_string(x) + std::to_string(y) + "_" + std::to_string(i), wallsMatrix[i], collidersOBB);
		}

		for (int i = 0; i < pillarsMatrix.size(); i++)
		{
			modelNodo.render(pillarsMatrix[i]);
			UpdateCollider(modelNodo, "Pilar_" + std::to_string(x) + std::to_string(y) + "_" + std::to_string(i), pillarsMatrix[i], collidersOBB);
		}

		if (hasTorch)
		{
			modelAntorcha.render(torchMatrix);
		}
	}

	void UpdateCollider(Model& modelo, string nombre, glm::mat4 modelMatrix, std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >& collidersOBB)
	{
		AbstractModel::OBB collider;
		glm::mat4 modelMatrixCollider = glm::mat4(modelMatrix);
		collider.u = glm::quat_cast(modelMatrixCollider);
		modelMatrixCollider = glm::translate(modelMatrixCollider, modelo.getObb().c);
		collider.c = glm::vec3(modelMatrixCollider[3]);
		collider.e = modelo.getObb().e * glm::vec3(1.0);
		addOrUpdateColliders(collidersOBB, nombre, collider, modelMatrixCollider);
	}

	void CreateTorchMatrix()
	{
		torchMatrix = wallsMatrix[rand() % wallsMatrix.size()];
		glm::quat rotation = glm::quat_cast(torchMatrix);
		torchMatrix[3] += glm::vec4(rotation * glm::vec3(0, 0, -3.8), 0);
	}
};