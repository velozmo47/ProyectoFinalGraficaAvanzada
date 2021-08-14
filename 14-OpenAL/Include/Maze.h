#include <cstdlib>
#include <iostream>
#include <stack>
#include "../Include/MazeCell.h"

using namespace std;

class Maze
{
private:
	int  m_nMazeWidth;  // 40
	int  m_nMazeHeight; // 25
	int* m_maze;
	Model* torch;
	std::vector<MazeCell> mazeCells = {};

	// Some bit fields for convenience
	enum
	{
		CELL_PATH_N = 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
	};

	// Algorithm variables
	int  m_nVisitedCells;
	stack<pair<int, int>> m_stack;	// (x, y) coordinate pairs
	float cellSize;

public:
	float CellSize() { return cellSize; }
	MazeCell* GetMazeCell(int x, int y) { return &(mazeCells[y * m_nMazeWidth + x]); }
	std::vector<glm::vec3> torchPositions = {};

	std::vector<glm::vec3>& GetTorchPositions() {
		return torchPositions;
	}

	Maze(int m_nMazeWidth, int m_nMazeHeight, float cellSize)
	{
		srand(static_cast <unsigned> (time(0)));

		this->m_nMazeWidth = m_nMazeWidth;
		this->m_nMazeHeight = m_nMazeHeight;
		this->cellSize = cellSize;
		m_maze = new int[m_nMazeWidth * m_nMazeHeight];
		mazeCells.resize(m_nMazeWidth * m_nMazeHeight);
		memset(m_maze, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));

		// Choose a starting cell
		int x = rand() % m_nMazeWidth;
		int y = rand() % m_nMazeHeight;
		m_stack.push(make_pair(x, y));
		m_maze[y * m_nMazeWidth + x] = CELL_VISITED;
		m_nVisitedCells = 1;

		CreateMaze();
	}

	void CreateMaze()
	{
		// Little lambda function to calculate index in a readable way
		auto offset = [&](int x, int y)
		{
			return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
		};

		// Do Maze Algorithm
		while (m_nVisitedCells < m_nMazeWidth * m_nMazeHeight)
		{
			// Create a set of unvisted neighbours
			vector<int> neighbours;

			// North neighbour
			if (m_stack.top().second > 0 && (m_maze[offset(0, -1)] & CELL_VISITED) == 0)
				neighbours.push_back(0);
			// East neighbour
			if (m_stack.top().first < m_nMazeWidth - 1 && (m_maze[offset(1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(1);
			// South neighbour
			if (m_stack.top().second < m_nMazeHeight - 1 && (m_maze[offset(0, 1)] & CELL_VISITED) == 0)
				neighbours.push_back(2);
			// West neighbour
			if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(3);

			// Are there any neighbours available?
			if (!neighbours.empty())
			{
				// Choose one available neighbour at random
				int next_cell_dir = neighbours[rand() % neighbours.size()];
				// Create a path between the neighbour and the current cell
				switch (next_cell_dir)
				{
				case 0: // North
					m_maze[offset(0, -1)] |= CELL_VISITED | CELL_PATH_S;
					m_maze[offset(0, 0)] |= CELL_PATH_N;
					m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
					break;

				case 1: // East
					m_maze[offset(+1, 0)] |= CELL_VISITED | CELL_PATH_W;
					m_maze[offset(0, 0)] |= CELL_PATH_E;
					m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
					break;

				case 2: // South
					m_maze[offset(0, +1)] |= CELL_VISITED | CELL_PATH_N;
					m_maze[offset(0, 0)] |= CELL_PATH_S;
					m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
					break;

				case 3: // West
					m_maze[offset(-1, 0)] |= CELL_VISITED | CELL_PATH_E;
					m_maze[offset(0, 0)] |= CELL_PATH_W;
					m_stack.push(make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
					break;

				}

				m_nVisitedCells++;
			}
			else
			{
				m_stack.pop(); // Backtrack
			}
		}

		CreateModelsMatrix();
	}

	void CreateModelsMatrix()
	{
		auto offset = [&](int x, int y)
		{
			return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
		};
		// Matriz de celda
		glm::mat4 modelMatrix = glm::mat4(1.0f);
		// Draw Maze
		for (int x = 0; x < m_nMazeWidth; x++)
		{
			for (int y = 0; y < m_nMazeHeight; y++)
			{				
				// Posicion de celda
				float xPos = x * cellSize;
				float yPos = y * cellSize;

				// Bloqueos 
				bool bloqueoNorte = !(m_maze[y * m_nMazeWidth + x] & CELL_PATH_N);
				bool bloqueoSur = !(m_maze[y * m_nMazeWidth + x] & CELL_PATH_S);
				bool bloqueoOeste = !(m_maze[y * m_nMazeWidth + x] & CELL_PATH_W);
				bool bloqueoEste = !(m_maze[y * m_nMazeWidth + x] & CELL_PATH_E);

				MazeCell* mazeCell = &mazeCells[y * m_nMazeWidth + x];
				mazeCell->x = x;
				mazeCell->y = y;

				if (bloqueoNorte)
				{
					mazeCell->AddWall(0.0f, glm::vec3(xPos, 0, yPos));
				}
				else
				{
					mazeCell->AddAdyacentCell(GetMazeCell(x, y - 1));
					if (!bloqueoOeste)
					{
						mazeCell->AddPilar(0.0f, glm::vec3(xPos, 0, yPos));
					}
					if (!bloqueoEste)
					{
						mazeCell->AddPilar(-90.0f, glm::vec3(xPos, 0, yPos));
					}
				}

				if (bloqueoSur)
				{
					if (y == m_nMazeHeight - 1)
					{
						mazeCell->AddWall(180.0f, glm::vec3(xPos, 0, yPos));
					}
					else
					{
						mazeCell->AddModelMatrix(mazeCell->wallsMatrix, 180.0f, glm::vec3(xPos, 0, yPos));
					}
				}
				else
				{
					mazeCell->AddAdyacentCell(GetMazeCell(x, y + 1));
					if (!bloqueoOeste)
					{
						mazeCell->AddPilar(90.0f, glm::vec3(xPos, 0, yPos));
					}
					if (!bloqueoEste)
					{
						mazeCell->AddPilar(-180.0f, glm::vec3(xPos, 0, yPos));
					}
				}

				if (bloqueoEste)
				{
					mazeCell->AddWall(-90.0f, glm::vec3(xPos, 0, yPos));
				}
				else {
					mazeCell->AddAdyacentCell(GetMazeCell(x + 1, y));
					
				}

				if (bloqueoOeste)
				{
					if (x == 0)
					{
						mazeCell->AddWall(90.0f, glm::vec3(xPos, 0, yPos));
					}
					else
					{
						mazeCell->AddModelMatrix(mazeCell->wallsMatrix, 90.0f, glm::vec3(xPos, 0, yPos));
					}
				}
				else {
					mazeCell->AddAdyacentCell(GetMazeCell(x - 1, y));
				}

				if (mazeCell->hasTorch)
				{
					mazeCell->CreateTorchMatrix();
					torchPositions.push_back(mazeCell->torchMatrix[3]);
				}
			}
		}
	}

	void DisplayMaze(Model& modelNodo, Model& modelPared, Model& modelAntorcha, std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>& collidersOBB)
	{
		for (int x = 0; x < m_nMazeWidth; x++)
		{
			for (int y = 0; y < m_nMazeHeight; y++)
			{
				MazeCell* mazeCell = &mazeCells[y * m_nMazeWidth + x];

				mazeCell->DisplayCell(modelNodo, modelPared, modelAntorcha, collidersOBB);
			}
		}
	}
};
