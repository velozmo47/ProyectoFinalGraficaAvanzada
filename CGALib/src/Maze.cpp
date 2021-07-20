#include "Headers/Maze.h"
#include "Headers/Colisiones.h"

Maze::Maze(int m_nMazeWidth, int m_nMazeHeight, int m_nPathWidth, float cellSize)
{
	this->m_nMazeWidth = m_nMazeWidth;
	this->m_nMazeHeight = m_nMazeHeight;
	this->cellSize = cellSize;
	m_maze = new int[m_nMazeWidth * m_nMazeHeight];
	memset(m_maze, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));
	this->m_nPathWidth = m_nPathWidth;
	
	// Choose a starting cell
	int x = rand() % m_nMazeWidth;
	int y = rand() % m_nMazeHeight;
	m_stack.push(make_pair(x, y));
	m_maze[y * m_nMazeWidth + x] = CELL_VISITED;
	m_nVisitedCells = 1;
}

bool Maze::OnUserUpdate(Model& modelNodo, Model& modelPared, std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>& collidersOBB)
{
	// Little lambda function to calculate index in a readable way
	auto offset = [&](int x, int y)
	{
		return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
	};

	// Do Maze Algorithm
	if (m_nVisitedCells < m_nMazeWidth * m_nMazeHeight)
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

	if (m_nVisitedCells < m_nMazeWidth * m_nMazeHeight) { return false; }

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	// Draw Maze
	for (int x = 0; x < m_nMazeWidth; x++)
	{
		for (int y = 0; y < m_nMazeHeight; y++)
		{
			float xPos = (x * (m_nPathWidth)) * cellSize;
			float yPos = (y * (m_nPathWidth)) * cellSize;
			modelMatrix[3] = glm::vec4(xPos, 0, yPos, 1);

			bool bloqueoNorte = !(m_maze[y * m_nMazeWidth + x] & CELL_PATH_N);
			bool bloqueoSur = !(m_maze[y * m_nMazeWidth + x] & CELL_PATH_S);
			bool bloqueoOeste = !(m_maze[y * m_nMazeWidth + x] & CELL_PATH_W);
			bool bloqueoEste = !(m_maze[y * m_nMazeWidth + x] & CELL_PATH_E);

			glm::mat4 modelMatrix;

			if (bloqueoNorte)
			{
				modelMatrix = DibujarModelo(modelPared, 0.0f, glm::vec3(xPos, 0, yPos));
				CrearCollider(modelPared, "ParedNorte " + std::to_string(x) + " " + std::to_string(y), modelMatrix, collidersOBB);
			}
			else
			{
				if (!bloqueoOeste)
				{
					modelMatrix = DibujarModelo(modelNodo, 0.0f, glm::vec3(xPos, 0, yPos));
					CrearCollider(modelNodo, "BloqueNW " + std::to_string(x) + " " + std::to_string(y), modelMatrix, collidersOBB);
				}
				if (!bloqueoEste)
				{
					modelMatrix = DibujarModelo(modelNodo, -90.0f, glm::vec3(xPos, 0, yPos));
					CrearCollider(modelNodo, "BloqueNE " + std::to_string(x) + " " + std::to_string(y), modelMatrix, collidersOBB);
				}
			}

			if (bloqueoSur)
			{
				modelMatrix = DibujarModelo(modelPared, 180.0f, glm::vec3(xPos, 0, yPos));
				CrearCollider(modelPared, "ParedSur " + std::to_string(x) + " " + std::to_string(y), modelMatrix, collidersOBB);
			}
			else
			{
				if (!bloqueoOeste)
				{
					modelMatrix = DibujarModelo(modelNodo, 90.0f, glm::vec3(xPos, 0, yPos));
					CrearCollider(modelNodo, "BloqueSW " + std::to_string(x) + " " + std::to_string(y), modelMatrix, collidersOBB);
				}
				if (!bloqueoEste)
				{
					modelMatrix = DibujarModelo(modelNodo, -180.0f, glm::vec3(xPos, 0, yPos));
					CrearCollider(modelNodo, "BloqueSE " + std::to_string(x) + " " + std::to_string(y), modelMatrix, collidersOBB);
				}
			}

			if (bloqueoEste)
			{
				modelMatrix = DibujarModelo(modelPared, -90.0f, glm::vec3(xPos, 0, yPos));
				CrearCollider(modelPared, "ParedEste " + std::to_string(x) + " " + std::to_string(y), modelMatrix, collidersOBB);
			}

			if (bloqueoOeste)
			{
				modelMatrix = DibujarModelo(modelPared, 90.0f, glm::vec3(xPos, 0, yPos));
				CrearCollider(modelPared, "ParedOeste " + std::to_string(x) + " " + std::to_string(y), modelMatrix, collidersOBB);
			}
		}
	}

	return true;
}

glm::mat4 Maze::DibujarModelo(Model& modelo, float rotacion , glm::vec3 posicion)
{
	glm::mat4 modelMatrix = glm::mat4(1.0);
	modelMatrix[3] = glm::vec4(posicion, 1);
	modelMatrix = glm::rotate(modelMatrix, (float)glm::radians(rotacion), glm::vec3(0.0, 1.0, 0.0));
	modelo.render(modelMatrix);

	return modelMatrix;
}

void Maze::CrearCollider(Model& modelo, string nombre, glm::mat4 modelMatrix, std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >& collidersOBB)
{
	AbstractModel::OBB collider;
	glm::mat4 modelMatrixCollider = glm::mat4(modelMatrix);
	//modelMatrixCollider = glm::translate(modelMatrixCollider, posicion);
	//modelMatrixCollider = glm::rotate ()
	collider.u = glm::quat_cast(modelMatrixCollider);
	modelMatrixCollider = glm::translate(modelMatrixCollider, modelo.getObb().c);
	collider.c = glm::vec3(modelMatrixCollider[3]);
	collider.e = modelo.getObb().e * glm::vec3(1.0);
	addOrUpdateColliders(collidersOBB, nombre, collider, modelMatrixCollider);
}
