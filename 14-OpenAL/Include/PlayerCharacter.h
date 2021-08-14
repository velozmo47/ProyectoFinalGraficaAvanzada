
class PlayerCharacter
{
private:
	Model* model;
public:
	std::stack<Collectable> collected = {};
	AbstractModel::OBB playerCollider;
	float animationTime;

	PlayerCharacter() : model(nullptr)
	{
		PlayerCharacter(nullptr);
	}

	PlayerCharacter(Model* model) : model(model)
	{
		this->model = model;
		animationTime = 0;
	}

	AbstractModel::OBB& PlayerCollider()
	{
		return playerCollider;
	}

	void AddCollected(Collectable& collectable)
	{
		collected.push(collectable);
	}

	void ChangeAnimationIndex(int animationIndex)
	{
		if (animationIndex != model->getAnimationIndex())
		{
			model->setAnimationIndex(animationIndex);
			animationTime = 0;
		}
	}

	void Render(glm::mat4 modelMatrix, float deltaTime)
	{
		model->renderAnimation(modelMatrix, animationTime);
		animationTime += deltaTime;
	}
};
