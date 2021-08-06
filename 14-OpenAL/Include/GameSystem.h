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

	void UpdateGameSystem(std::vector<Collectable>& collectables, AbstractModel::OBB& character, FontTypeRendering::FontTypeRendering* fontRendering)
	{
		switch (currentState)
		{
		case 0:
			break;
		case 1:
			UpdateCollectables(collectables, character, fontRendering);
			if (GameCompleted)
			{
				currentState++;
				nextCollectable = 0;
			}
			break;
		case 2:
			fontRendering->render("Juego terminado, gracias por jugar", -0.1, 0, 40, 1.0, 1.0, 0.0, 1.0);
			break;
		}
	}

	void UpdateCollectables(std::vector<Collectable>& collectables, AbstractModel::OBB& character, FontTypeRendering::FontTypeRendering* fontRendering)
	{
		collectables[nextCollectable].Effect(); 
		bool revision = CheckCollectable(collectables[nextCollectable], character);

		GameCompleted = revision && nextCollectable >= collectables.size();
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
