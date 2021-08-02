#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>
#include <algorithm>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// Maze
#include "../Include/Maze.h"
#include "../Include/Collectable.h"
#include "../Include/GameSystem.h"
#include "../Include/MainMenu.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;
int totalSpot;

GLFWwindow* window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = 2.0;
float distanceFromTargetOffset;
float Ghost=3.0f;
float rotGhost = 0.0f;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Cylinder cylinderRay(10, 10, 0.05, 0.05);
std::vector<glm::vec3> lampPosition{};

// Girl
Model girlModelAnimate;
Terrain terrain(-1, -1, 150, 4, "../Textures/Heightmap.png");

Model modelNodo;
Model modelEsfera;
Model modelPared;
Model modelAntorcha;
Model modelGhost;

int mazeWidth = 5;
int mazeHeight = 5;
float mazeCellSize = 8.2f;
Maze maze(mazeWidth, mazeHeight, mazeCellSize);
Maze* maze_ptr = &maze;
float elapsedTime;

MainMenu mainMenu;

std::vector<Collectable> collectables = {
	Collectable(&modelEsfera, glm::vec3(2 * mazeCellSize, 2, 2 * mazeCellSize)),
	Collectable(&modelEsfera, glm::vec3(4 * mazeCellSize, 2, 0 * mazeCellSize)),
	Collectable(&modelEsfera, glm::vec3(0 * mazeCellSize, 2, 4 * mazeCellSize)),
	Collectable(&modelEsfera, glm::vec3(4 * mazeCellSize, 2, 4 * mazeCellSize))
};

GameSystem gameSystem = GameSystem(collectables);

GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/skybox1/1.png",
		"../Textures/skybox1/2.png",
		"../Textures/skybox1/3.png",
		"../Textures/skybox1/4.png",
		"../Textures/skybox1/5.png",
		"../Textures/skybox1/6.png" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;
int GhostStates = 0;

// Model matrix definitions
glm::mat4 modelMatrixGirl = glm::mat4(1.0f);
glm::mat4 modelMatrixGhost = glm::mat4(1.0f);
glm::vec3 targetOffset = glm::vec3(-0.35f, 1.5f, 0);

int animationIndex = 0;
int modelSelected = 0;
bool enableCountSelected = true;

double deltaTime;
double currTime, lastTime;

// Jump variables
bool isJump = false;
float GRAVITY = 1.81;
double tmv = 0;
double startTimeJump = 0;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	srand(static_cast <unsigned> (time(0)));

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation.vs", "../Shaders/multipleLights.fs");
	shaderTerrain.initialize("../Shaders/terrain.vs", "../Shaders/terrain.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	cylinderRay.init();
	cylinderRay.setShader(&shader);
	cylinderRay.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(100, 0, 100));


	//Model cell
	modelNodo.loadModel("../models/ProyectoFinal/NodoProy.obj");
	modelNodo.setShader(&shaderMulLighting);

	//Esfera
	modelEsfera.loadModel("../models/ProyectoFinal/modelEsfera.obj");
	modelEsfera.setShader(&shaderMulLighting);

	//Model pared
	modelPared.loadModel("../models/ProyectoFinal/ParedProy2.obj");
	modelPared.setShader(&shaderMulLighting);

	// Girl
	//girlModelAnimate.loadModel("../models/ProyectoFinal/Guard.fbx");
	girlModelAnimate.loadModel("../models/Practica2_obj/Girl.fbx");
	girlModelAnimate.setShader(&shaderMulLighting);

	modelAntorcha.loadModel("../models/ProyectoFinal/Antorcha.obj");
	modelAntorcha.setShader(&shaderMulLighting);


	modelGhost.loadModel("../models/ProyectoFinal/ghost.obj");
	modelGhost.setShader(&shaderMulLighting);

	camera->setPosition(glm::vec3(0.0, 0.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP* bitmap;
	unsigned char* data;

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP* bitmap = skyboxTexture.loadImage(true);
		unsigned char* data = skyboxTexture.convertToData(bitmap, imageWidth,
			imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
				GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}


	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Textures/Snow.tif");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBackground.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBackground.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainR("../Textures/Rock.tif");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainR.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainR.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainG("../Textures/grassFlowers.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainG.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainG.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainB("../Textures/Rock.tif");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainB.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainB.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Textures/blendMap2.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBlendMap.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBlendMap.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage(bitmap);
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Custom objects Delete
	cylinderRay.destroy();

	// Custom objects animate
	girlModelAnimate.destroy();
	modelNodo.destroy();
	modelPared.destroy();
	modelEsfera.destroy();
	modelAntorcha.destroy();
	modelGhost.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			//std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			//std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			//std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			//std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		gameSystem.EnterPress();
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		modelMatrixGirl = glm::rotate(modelMatrixGirl, (float)-(offsetX * deltaTime), glm::vec3(0, 1, 0));
	//camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		//modelMatrixGirl = glm::rotate(modelMatrixGirl,(float) (offsetY * deltaTime), glm::vec3(1, 0, 0));
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);
	offsetX = 0;
	offsetY = 0;

	// Seleccionar modelo
	if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		enableCountSelected = false;
		modelSelected++;
		if (modelSelected > 0)
			modelSelected = 0;
	}
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		enableCountSelected = true;

	bool keySpaceStatus = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if (!isJump && keySpaceStatus)
	{
		isJump = true;
		tmv = 0;
		startTimeJump = currTime;
	}

	if (modelSelected == 0)
	{
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			modelMatrixGirl = glm::rotate(modelMatrixGirl, 5.0f * (float) deltaTime, glm::vec3(0, 1, 0));
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			modelMatrixGirl = glm::rotate(modelMatrixGirl, -5.0f * (float) deltaTime, glm::vec3(0, 1, 0));
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			modelMatrixGirl = glm::translate(modelMatrixGirl, glm::vec3(0.0, 0.0, 7.5) * (float) deltaTime);
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			modelMatrixGirl = glm::translate(modelMatrixGirl, glm::vec3(0.0, 0.0, -7.5) * (float) deltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			girlModelAnimate.setAnimationIndex(0);
		}
		else
		{
			//girlModelAnimate.setAnimationIndex(4);
			girlModelAnimate.setAnimationIndex(1);
		}
	}
	else
	{
		//girlModelAnimate.setAnimationIndex(4);
		girlModelAnimate.setAnimationIndex(1);
	}

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	lastTime = TimeManager::Instance().GetTime();

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if (currTime - lastTime < 0.016666667) {
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(false);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / (float)screenHeight, 0.01f, 50.0f);

		if (modelSelected == 0)
		{
			axis = glm::axis(glm::quat_cast(modelMatrixGirl));
			angleTarget = glm::angle(glm::quat_cast(modelMatrixGirl));
			//angleTarget = glm::angle(glm::quat_cast(glm::mat4(1.0)));
			//target = modelMatrixGirl[3] + glm::vec4(-0.35, 1.5, 0, 1);
			target = modelMatrixGirl[3] + modelMatrixGirl[0] * targetOffset.x + modelMatrixGirl[1] * targetOffset.y;
		}

		if (std::isnan(angleTarget))
			angleTarget = 0.0;
		if (axis.y < 0)
			angleTarget = -angleTarget;
		if (modelSelected == 1)
			angleTarget -= glm::radians(90.0f);
		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();
		view = camera->getViewMatrix();

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
			glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
			glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false,
			glm::value_ptr(view));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.05)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(.01)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

		/*******************************************
		 * Propiedades Luz direccional Terrain
		 *******************************************/

		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.05)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.01)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		glm::vec3 spotPosition = glm::vec3(modelMatrixGirl[3]) + glm::vec3(0.0, 1.5, 0.2);
		glm::vec3 dirSpotlight = camera->getFront();

		shaderMulLighting.setInt("spotLightCount", 1);
		shaderTerrain.setInt("spotLightCount", 1);
		shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.1)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.4)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0)));
		shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(dirSpotlight));
		shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		shaderMulLighting.setFloat("spotLights[0].linear", 0.074);
		shaderMulLighting.setFloat("spotLights[0].quadratic", 0.03);
		shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(40.0f)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.1)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(1)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.0)));
		shaderTerrain.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		shaderTerrain.setVectorFloat3("spotLights[0].direction", glm::value_ptr(dirSpotlight));
		shaderTerrain.setFloat("spotLights[0].constant", 1.0);
		shaderTerrain.setFloat("spotLights[0].linear", 0.074);
		shaderTerrain.setFloat("spotLights[0].quadratic", 0.03);
		shaderTerrain.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		shaderTerrain.setFloat("spotLights[0].outerCutOff", cos(glm::radians(40.0f)));

		/*******************************************
		 * Propiedades PointLights
		 *******************************************/

		 /*******************************************
		  * Terrain Cesped
		  *******************************************/
		glm::mat4 modelCesped = glm::mat4(1.0);
		modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
		modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
		// Se activa la textura del background
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
		shaderTerrain.setInt("backgroundTexture", 0);
		// Se activa la textura de tierra
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
		shaderTerrain.setInt("rTexture", 1);
		// Se activa la textura de hierba
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
		shaderTerrain.setInt("gTexture", 2);
		// Se activa la textura del camino
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
		shaderTerrain.setInt("bTexture", 3);
		// Se activa la textura del blend map
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
		shaderTerrain.setInt("blendMapTexture", 4);
		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
		terrain.render();
		shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		/*******************************************
		 * Custom objects obj
		 *******************************************/
		 // Forze to enable the unit texture to 0 always ----------------- IMPORTANT
		glActiveTexture(GL_TEXTURE0);

		// Render the Ghost
		modelMatrixGhost[3][1] = terrain.getHeightTerrain(modelMatrixGhost[3][0], modelMatrixGhost[3][2]);
		glm::mat4 modelMatrixGhostBody = glm::mat4(modelMatrixGhost);
		modelMatrixGhostBody = glm::rotate(modelMatrixGhostBody, glm::radians(rotGhost), glm::vec3(0.0, 1.0, 0.0));
		modelMatrixGhostBody = glm::translate(modelMatrixGhostBody, glm::vec3(0.0, 2.0, Ghost));
		modelMatrixGhostBody = glm::scale(modelMatrixGhostBody, glm::vec3(0.5, 0.5, 0.5));
		modelGhost.render(modelMatrixGhostBody);

		switch (GhostStates) {
		case 0:
			rotGhost = 0;
			Ghost = Ghost + 0.2f;
			if (Ghost > 25) {
				GhostStates = 1;
				rotGhost = 180;
			}
			break;

		case 1:
			Ghost = Ghost - 0.2f;
			if (Ghost < 5) {
				GhostStates = 0;
			}
			break;

		}

		// Model Girl
		modelMatrixGirl[3][1] = terrain.getHeightTerrain(modelMatrixGirl[3][0], modelMatrixGirl[3][2]);
		glm::mat4 modelMatrixGirlBody = glm::mat4(modelMatrixGirl);
		modelMatrixGirlBody = glm::scale(modelMatrixGirlBody, glm::vec3(0.01, 0.01, 0.01));
		girlModelAnimate.render(modelMatrixGirlBody);

		/*************************
		* Ray in Girl direction
		**************************/
		glm::mat4 modelMatrixRayGirl = glm::mat4(modelMatrixGirl);
		modelMatrixRayGirl = glm::translate(modelMatrixRayGirl, glm::vec3(0.0, 1.0, 0.0));
		glm::vec3 rayDirectionGirl = glm::normalize(glm::vec3(modelMatrixRayGirl[2]));
		glm::vec3 oriGirl = glm::vec3(modelMatrixRayGirl[3]);
		glm::vec3 tarGirl = oriGirl + 10.0f * rayDirectionGirl;
		glm::vec3 tarmGirl = oriGirl + 5.0f * rayDirectionGirl;
		modelMatrixRayGirl[3] = glm::vec4(tarmGirl, 1.0f);
		modelMatrixRayGirl = glm::rotate(modelMatrixRayGirl, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
		modelMatrixRayGirl = glm::scale(modelMatrixRayGirl, glm::vec3(1.0, 10.0, 1.0));
		cylinderRay.render(modelMatrixRayGirl);

		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);

		/*******************************************
		 * Creacion de colliders
		 * IMPORTANT do this before interpolations
		 *******************************************/

		 // Colision de la camara, no se ve porque la cámara está dentro de la esfera
		AbstractModel::SBB cameraCollider;
		glm::mat4 modelMatrixColliderCamera = glm::mat4(1.0);
		modelMatrixColliderCamera[3] = glm::vec4(glm::vec3(modelMatrixGirl[3]) + glm::vec3(modelMatrixGirl[0]) * targetOffset.x + glm::vec3(modelMatrixGirl[1]) * targetOffset.y + camera->getFront() * (-distanceFromTarget + 1), 1);
		modelMatrixColliderCamera = glm::translate(modelMatrixColliderCamera, modelEsfera.getSbb().c);
		cameraCollider.c = glm::vec3(modelMatrixColliderCamera[3]);
		cameraCollider.ratio = modelEsfera.getSbb().ratio * 0.6;
		addOrUpdateColliders(collidersSBB, "camera", cameraCollider, modelMatrixColliderCamera);

		//Collider de Girl
		AbstractModel::OBB girlCollider;
		glm::mat4 modelMatrixColliderGirl = glm::mat4(modelMatrixGirl);
		//modelMatrixColliderGirl = glm::rotate(modelMatrixColliderGirl, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		girlCollider.u = glm::quat_cast(modelMatrixColliderGirl);
		modelMatrixColliderGirl = glm::scale(modelMatrixColliderGirl, glm::vec3(0.01));
		modelMatrixColliderGirl = glm::translate(modelMatrixColliderGirl, girlModelAnimate.getObb().c);
		girlCollider.c = glm::vec3(modelMatrixColliderGirl[3]);
		girlCollider.e = girlModelAnimate.getObb().e * glm::vec3(0.005, 0.015, 0.01) * glm::vec3(0.785, 0.785, 0.785);
		addOrUpdateColliders(collidersOBB, "girl", girlCollider, modelMatrixGirl);

		maze_ptr->DisplayMaze(modelNodo, modelPared, modelAntorcha, collidersOBB);
		gameSystem.UpdateCollectables(collectables, girlCollider);

		/*******************************************
		 * Render de colliders
		 *******************************************/
		 //for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
		 //		collidersOBB.begin(); it != collidersOBB.end(); it++) {
		 //	glm::mat4 matrixCollider = glm::mat4(1.0);
		 //	matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
		 //	matrixCollider = matrixCollider * glm::mat4(std::get<0>(it->second).u);
		 //	matrixCollider = glm::scale(matrixCollider, std::get<0>(it->second).e * 2.0f);
		 //	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
		 //	boxCollider.enableWireMode();
		 //	boxCollider.render(matrixCollider);
		 //}

		 //for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
		 //		collidersSBB.begin(); it != collidersSBB.end(); it++) {
		 //	glm::mat4 matrixCollider = glm::mat4(1.0);
		 //	matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
		 //	matrixCollider = glm::scale(matrixCollider, glm::vec3(std::get<0>(it->second).ratio * 2.0f));
		 //	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
		 //	sphereCollider.enableWireMode();
		 //	sphereCollider.render(matrixCollider);
		 //}

		 /*****************************************************
		 * Test prueba de colisiones
		 ******************************************************/
		 // Cajas vs cajas
		for (std::map<std::string,
			std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			collidersOBB.begin(); it != collidersOBB.end(); it++)
		{
			bool isCollision = false;
			for (std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersOBB.begin(); jt != collidersOBB.end() && !isCollision; jt++)
			{
				if (it != jt && testOBBOBB(std::get<0>(it->second), std::get<0>(jt->second)))
				{
					//std::cout << "Collision " << it->first << " with " << jt->first << std::endl;
					isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		// Esferas vs esferas
		for (std::map<std::string,
			std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++)
		{
			bool isCollision = false;
			for (std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersSBB.begin(); jt != collidersSBB.end() && !isCollision; jt++)
			{
				if (it != jt && testSphereSphereIntersection(std::get<0>(it->second), std::get<0>(jt->second)))
				{
					//std::cout << "Collision " << it->first << " with " << jt->first << std::endl;
					isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		// Esferas vs cajas
		for (std::map<std::string,
			std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++)
		{
			bool isCollision = false;
			for (std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersOBB.begin(); jt != collidersOBB.end() && !isCollision; jt++)
			{

				if (testSphereOBox(std::get<0>(it->second), std::get<0>(jt->second)))
				{
					//std::cout << "Collision " << it->first << " with " << jt->first << std::endl;
					isCollision = true;
					if (it->first == "camera" && jt->first == "girl")
					{
						isCollision = false;
					}
					addOrUpdateCollisionDetection(collisionDetection, jt->first, isCollision);
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		std::map<std::string, bool>::iterator colIt;
		for (colIt = collisionDetection.begin(); colIt != collisionDetection.end(); colIt++)
		{
			std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it = collidersOBB.find(colIt->first);
			std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt = collidersSBB.find(colIt->first);
			if (it != collidersOBB.end())
			{
				if (!colIt->second)
				{
					addOrUpdateColliders(collidersOBB, it->first);
				}
				else
				{
					if (it->first.compare("girl") == 0)
					{
						//	std::cout << "Collision " << it->first << " with " << jt->first << std::endl;
						modelMatrixGirl = std::get<1>(it->second);
					}
				}
			}
			if (jt != collidersSBB.end())
			{
				if (!colIt->second)
				{
					addOrUpdateColliders(collidersSBB, jt->first);
				}
				else
				{
					if (jt->first.compare("girl") == 0)
					{
						modelMatrixGirl = std::get<1>(jt->second);
					}
				}

				if (jt->first == "camera")
				{
					if (colIt->second)
					{
						if (distanceFromTargetOffset > -1.5)
						{
							distanceFromTargetOffset -= deltaTime * 5;
						}
						else
						{
							distanceFromTargetOffset = -1.5;
						}
					}
					else
					{
						if (distanceFromTargetOffset < 0)
						{
							distanceFromTargetOffset += deltaTime * 5;
						}
						else
						{
							distanceFromTargetOffset = 0;
						}
					}
					camera->setDistanceFromTarget(distanceFromTarget + distanceFromTargetOffset);
					//std::cout << distanceFromTarget + distanceFromTargetOffset << std::endl;
				}
			}
		}

		/********************************************
		* Prueba de colision Rayo
		*********************************************/
		for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++)
		{
			float tray;
			if (raySphereIntersect(oriGirl, tarGirl, rayDirectionGirl, std::get<0>(it->second), tray))
			{
				//std::cout << "Collision " << it->first << " with " << "Ray" << std::endl;
			}
		}

		for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			collidersOBB.begin(); it != collidersOBB.end(); it++)
		{
			if (testIntersectRayOBB(oriGirl, tarGirl, rayDirectionGirl, std::get<0>(it->second)))
			{
				//std::cout << "Colision " << it->first << " with " << "Ray" << std::endl;
			}
		}
		// Constantes de animaciones
		animationIndex = 1;

		/*******************************************
		 * State machines
		 *******************************************/

		glfwSwapBuffers(window);
	}
}

int main(int argc, char** argv) {
	init(800, 700, "Window GLFW", false);

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
		return -1;
	}

	FT_Face face;
	if (FT_New_Face(ft, "../fonts/ARIAL.ttf", 0, &face))
	{
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		return -1;
	}

	applicationLoop();
	destroy();
	return 1;
}
