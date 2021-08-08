
class PlayerCharacter
{
private:
	Model* model;
public:
	std::stack<Collectable> collected = {};
	AbstractModel::OBB playerCollider;

	PlayerCharacter() : model(nullptr)
	{
		//PlayerCharacter(nullptr);
	}

	PlayerCharacter(Model* model) : model(model)
	{
		this->model = model;
	}

	AbstractModel::OBB& PlayerCollider()
	{
		return playerCollider;
	}

	void AddCollected(Collectable& collectable)
	{
		collected.push(collectable);
	}

	void LostCollectable()
	{
		//if (collected.size() > 0)
		//{
		//	Collectable lost = collected.top();
		//	collected.pop();

		//	gameSystem->AddCollectable(lost);
		//}
	}
};
