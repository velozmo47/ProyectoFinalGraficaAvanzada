#ifndef MAZE_H
#define MAZE_H

#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_DLL
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllexport))
#else
#define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
#endif
#else
#ifdef __GNUC__
#define DLL_PUBLIC __attribute__ ((dllimport))
#else
#define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
#endif
#endif
#define DLL_LOCAL
#else
#if __GNUC__ >= 4
#define DLL_PUBLIC __attribute__ ((visibility ("default")))
#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define DLL_PUBLIC
#define DLL_LOCAL
#endif
#endif

#include "Headers/Colisiones.h"
#include "Headers/Model.h"
#include <iostream>
#include <stack>
using namespace std;

class DLL_PUBLIC Maze
{
private:
	int  m_nMazeWidth;  // 40
	int  m_nMazeHeight; // 25
	int* m_maze;

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
	int  m_nPathWidth;
	float cellSize;

public:
	Maze(int m_nMazeWidth, int m_nMazeHeight, int m_nPathWidth, float cellSize);
	bool OnUserUpdate(Model& modelNodo, Model& modelPared, std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >& collidersOBB);
	void CrearCollider(Model& modelo, string nombre, glm::mat4 modelMatrix, std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >& collidersOBB);
	glm::mat4 DibujarModelo(Model& modelo, float rotacion, glm::vec3 posicion);
};

#endif /* MAZE_H */