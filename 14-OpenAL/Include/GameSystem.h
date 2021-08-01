class GameSystem
{
public:
	bool GameCompleted;
	std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collectablesOBB;

	void UpdateCollectables(std::vector<Collectable>& collectables, AbstractModel::OBB& character)
	{
		bool revision = true;

		for (int i = 0; i < collectables.size(); i++)
		{
			revision &= CheckCollectable(collectables[i], character);
		}

		GameCompleted = revision;

		if (GameCompleted)
		{
			std::cout << "Recolectables completos" << std::endl;
		}
	}

	bool CheckCollectable(Collectable& collectable, AbstractModel::OBB& character)
	{
		if (!collectable.collected)
		{
			collectable.model->render(collectable.modelMatrixCollider);
			collectable.collected = testOBBOBB(character, collectable.GetCollider());
		}

		return collectable.collected;
	}
};
