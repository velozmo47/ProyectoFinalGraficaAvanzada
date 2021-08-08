
class Ghost {
	Model* model;
	Terrain* terrain;
	glm::mat4 modelMatrix;
	AbstractModel::OBB collider;
	int ghostState;
	float floatingHeight;
	float rotInterp;
	float yOffset;
	glm::vec3 previousObjective;
	glm::vec3 nextObjective;
	float coolDown;
	float maxCoolDown;

public:
	AbstractModel::OBB& Collider() { return collider; }

	Ghost() : model (nullptr), terrain (nullptr)
	{
		Ghost(nullptr, nullptr, glm::vec3(0.0));
	}

	Ghost(Model* model, Terrain* terrain, glm::vec3 position) : model(model), terrain(terrain)
	{
		this->model = model;
		this->terrain = terrain;

		ghostState = 0;
		floatingHeight = (rand() % 100) / 100;
		rotInterp = 0;
		coolDown = 0;
		maxCoolDown = 5.0f;
		yOffset = 0;
		modelMatrix = glm::mat4(1.0);
		nextObjective = glm::vec3(position.x, 0, position.z);
		previousObjective = nextObjective;
		modelMatrix[3] = glm::vec4(position, 1);
	}

	void UpdateGhost (Maze* maze, float deltaTime, GameSystem* gameSystem)
	{
		glm::vec3 currentPosition = glm::vec3(modelMatrix[3][0], 0, modelMatrix[3][2]);

		switch (ghostState)
		{
		case 0:
			UpdateObjective(currentPosition, maze, deltaTime);
			UpdatePosition(currentPosition, deltaTime, 0.0f);
			UpdateCollider();
			TestPlayerContact(gameSystem);
			break;
		case 1:
			RotateTowardsObjective(deltaTime, currentPosition);
			UpdatePosition(currentPosition, deltaTime, 0.0f);
			UpdateCollider();
			TestPlayerContact(gameSystem);
			break;
		case 2:
			CoolDown(deltaTime);
			UpdatePosition(currentPosition, deltaTime, 5.0f);
			UpdateCollider();
			break;
		}
	}

	void CoolDown(float deltaTime)
	{
		coolDown += deltaTime;
		if (coolDown >= maxCoolDown)
		{
			coolDown = 0;
			ghostState = 1;
		}
	}

	void UpdatePosition(glm::vec3& currentPosition, float deltaTime, float height)
	{
		float yPos = terrain->getHeightTerrain(currentPosition.x, currentPosition.z);
		modelMatrix[3] = glm::vec4(currentPosition, 1);
		modelMatrix[3][1] = yPos + FloatingOffset(0.5f, 0.5f * deltaTime) + VerticalOffset(height, deltaTime);
		model->render(modelMatrix);
	}

	void UpdateCollider()
	{
		glm::mat4 modelMatrixCollider = modelMatrix;
		collider.u = glm::quat_cast(modelMatrixCollider);
		modelMatrixCollider = glm::translate(modelMatrixCollider, model->getObb().c);
		collider.c = glm::vec3(modelMatrixCollider[3]);
		collider.e = model->getObb().e * glm::vec3(1.0);
	}

	void TestPlayerContact(GameSystem* gameSystem)
	{
		bool contact = testOBBOBB(gameSystem->playerCharacter->PlayerCollider(), collider);
		if (contact)
		{
			ghostState = 2;
			coolDown = 0;
			gameSystem->LostCollectable();
		}
	}

	void RotateTowardsObjective(float deltaTime, glm::vec3& currentPosition)
	{
		rotInterp = glm::clamp(rotInterp + 0.5f * deltaTime, 0.0f, 1.0f);
		glm::vec3 direction = glm::normalize(nextObjective - currentPosition);
		glm::quat desiredRot = glm::quatLookAtLH(direction, glm::vec3(0, 1, 0));
		modelMatrix = glm::mat4_cast(glm::slerp(glm::quat_cast(modelMatrix), desiredRot, rotInterp * rotInterp * rotInterp));

		float dotProduct = glm::dot(direction, glm::quat_cast(modelMatrix) * glm::vec3(0, 0, 1));
		if (dotProduct > 0.999f && dotProduct < 1.001f)
		{
			ghostState = 0;
			rotInterp = 0;
		}
	}

	void UpdateObjective(glm::vec3& currentPosition, Maze* maze, float deltaTime)
	{
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

			ghostState = 1;
		}
	}

	float VerticalOffset(float height, float speed)
	{
		yOffset = MoveTowards(yOffset, height, speed);
		return yOffset;
	}

	float FloatingOffset(float height, float speed)
	{
		floatingHeight += speed;
		if (floatingHeight >= 1)
			floatingHeight = 0;
		return height * sinf(floatingHeight * 2.0f * std::_Pi);
	}
};