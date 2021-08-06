#include <algorithm>
#include <random>

class GameSystem
{
public:
	bool GameCompleted;
	int nextCollectable;
	int currentState = 0; // 0 - MainMenú

	GameSystem(std::vector<Collectable>& collectables)
	{
		currentState = 0;
		GameCompleted = false;
		auto rng = std::default_random_engine{};
		std::shuffle(std::begin(collectables), std::end(collectables), rng);
		nextCollectable = 0;
	}

	int UpdateCollectables(std::vector<Collectable>& collectables, AbstractModel::OBB& character)
	{
		if (GameCompleted)
		{
			std::cout << "Recolectables completos" << std::endl;
			return 1;
		}

		collectables[nextCollectable].Effect(); 
		bool revision = CheckCollectable(collectables[nextCollectable], character);

		GameCompleted = revision && nextCollectable >= collectables.size();
		return 0;
	}

	bool CheckCollectable(Collectable& collectable, AbstractModel::OBB& character)
	{
		if (!collectable.collected)
		{
			collectable.model->render(collectable.modelMatrixCollider);
			collectable.collected = testOBBOBB(character, collectable.GetCollider());
			if (collectable.collected)
			{
				nextCollectable++;
			}
		}

		return collectable.collected;
	}

	void EnterPress()
	{
		if (currentState == 0)
		{
			currentState = 1;
			std::cout << "Inicia  el juego" << std::endl;
		}
	}
};
