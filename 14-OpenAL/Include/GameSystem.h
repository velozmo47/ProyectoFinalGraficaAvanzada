#include <algorithm>
#include <random>

class GameSystem
{
public:
	bool collectedCoin;
	bool GameCompleted;
	int lives=3;
	int currentState = 0; // 0 - MainMenú 
	std::vector<Collectable> collectables;
	PlayerCharacter* playerCharacter;

	GameSystem() : playerCharacter(nullptr)
	{
		GameSystem(std::vector<Collectable> (), nullptr);
	}

	GameSystem(std::vector<Collectable> collectables, PlayerCharacter* playerCharacter) : playerCharacter(playerCharacter)
	{
		this->playerCharacter = playerCharacter;
		currentState = 0;
		GameCompleted = false;
		lives = 3;

		auto rng = std::default_random_engine{};
		std::shuffle(std::begin(collectables), std::end(collectables), rng);

		for (int i = 0; i < collectables.size(); i++)
		{
			this->collectables.push_back(collectables[i]);
		}
	}

	void UpdateGameSystem(PlayerCharacter* player, FontTypeRendering::FontTypeRendering* fontRendering)
	{
		switch (currentState)
		{
		case 0:
			break;
		case 1:
			UpdateCollectables(player, fontRendering);
			if (GameCompleted)
				currentState = 2;
			break;
		case 2:
			fontRendering->render("Juego terminado, gracias por jugar", -0.5, 0, 40, 1.0, 1.0, 0.0, 1.0);
			break;
		}
	}

	void UpdateCollectables(PlayerCharacter* player, FontTypeRendering::FontTypeRendering* fontRendering)
	{
		collectedCoin= CheckCollectable(collectables.back(), player);

		GameCompleted = collectables.size() <= 0;
	}

	bool CheckCollectable(Collectable& collectable, PlayerCharacter* player)
	{
		collectable.Display();
		bool collected = testOBBOBB(player->PlayerCollider(), collectable.GetCollider());
		if (collected)
		{
			playerCharacter->collected.push(collectable);
			collectables.pop_back();
		}

		return collected;
	}

	void EnterPress()
	{
		if (currentState == 0)
		{
			currentState = 1;
			std::cout << "Inicia el juego" << std::endl;
		}
		if (currentState == 2 && (lives <= 0 || collectables.size() <= 0))
		{
			currentState = 3;
			std::cout << "Reinicia el juego" << std::endl;
		}
	}

	void LostCollectable()
	{
		if (currentState == 1) {
			lives--;
			if (playerCharacter->collected.size() > 0)
			{
				collectables.insert(collectables.begin(), playerCharacter->collected.top());
				playerCharacter->collected.pop();
			}
			
		}
	}
};

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

static float MoveTowards(float current, float target, float maxDistanceDelta)
{
	float a = target - current;
	float magnitude = glm::abs(a);
	if (magnitude <= maxDistanceDelta || magnitude == 0.0f)
	{
		return target;
	}
	return current + a / magnitude * maxDistanceDelta;
}