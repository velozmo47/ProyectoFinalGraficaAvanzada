#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>
#include <algorithm>

// OpenAL include
#include <AL/alut.h>
//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"
//Font Type render
#include "Headers/FontTypeRendering.h"
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


#include <ft2build.h>
#include FT_FREETYPE_H

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
#include "../Include/PlayerCharacter.h"
#include "../Include/GameSystem.h"
#include "../Include/Ghost.h"
#include "../Include/Extras.h"

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
float distanceFromTarget = 1.8;
float distanceFromTargetOffset;
//float Ghost;
float rotGhost = 0.0f;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Cylinder cylinderRay(10, 10, 0.05, 0.05);

// Guard
Model GuardModelAnimate;

//terreno
Terrain terrain(-1, -1, 150, 4, "../Textures/Heightmap.png");

Model modelNodo;
Model modelMoneda;
Model modelPared;
Model modelAntorcha;
Model modelGhost;
Model modelBarrel;
Model modelBoxes;
Model modelSkull;
Model modelSkull2;
Model modelShield2;

//Vector de fantasmas
std::vector<Ghost> ghosts;
PlayerCharacter player;

//Vector de cráneos
std::vector<Extras> skulls;

//Vector de cajas
std::vector<Extras> boxes;

//variables de laberinto
int mazeWidth = 5;
int mazeHeight = 5;
float mazeCellSize = 8.2f;
Maze maze(mazeWidth, mazeHeight, mazeCellSize);
Maze* maze_ptr = &maze;
float elapsedTime;

glm::vec3 positionsCol;

//variable de fin de juego
int endGame;

GameSystem gameSystem;

//Creación de texturas
GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint skyboxTextureID;

//Creación de modelo de texto
FontTypeRendering::FontTypeRendering *modelText;

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

// Model matrix definitions
glm::mat4 modelMatrixGuard = glm::mat4(1.0f);
glm::vec3 targetOffset = glm::vec3(-0.35f, 1.85f, 0);

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
glm::vec3 ghost1Pos;
glm::vec3 ghost2Pos;
// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;


/**********************
 * OpenAL config
 */

 // OpenAL Defines
//Numero de buffers necesarios
#define NUM_BUFFERS 4
#define NUM_SOURCES 6
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };

ALfloat sourceG1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat sourceG2Pos[] = { 2.0, 0.0, 0.0 };
// Source 0

ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1
ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };
// Source 2
ALfloat source2Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };
// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid* data;
int ch;
ALboolean loop;
//Vector de posiciones de fantasmas
std::vector<bool> sourcesPlay = { false, false, true,true, true,false};

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action,int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);

void applicationLoop();


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
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_fog.vs", "../Shaders/multipleLights_fog.fs");
	shaderTerrain.initialize("../Shaders/terrain_fog.vs", "../Shaders/terrain_fog.fs");


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


	//Model Soporte
	modelNodo.loadModel("../models/ProyectoFinal/NodoProy.obj");
	modelNodo.setShader(&shaderMulLighting);

	//Moneda
	modelMoneda.loadModel("../models/ProyectoFinal/Moneda/Moneda.obj");
	modelMoneda.setShader(&shaderMulLighting);

	//Model pared
	modelPared.loadModel("../models/ProyectoFinal/ParedProy2.obj");
	modelPared.setShader(&shaderMulLighting);

	// Guard
	GuardModelAnimate.loadModel("../models/ProyectoFinal/Guard.fbx");
	GuardModelAnimate.setShader(&shaderMulLighting);

	//Antorcha
	modelAntorcha.loadModel("../models/ProyectoFinal/Antorcha.obj");
	modelAntorcha.setShader(&shaderMulLighting);

	//model Fantasma
	modelGhost.loadModel("../models/ProyectoFinal/ghost.obj");
	modelGhost.setShader(&shaderMulLighting);

	//Model barril
	modelBarrel.loadModel("../models/ProyectoFinal/Barril2.obj");
	modelBarrel.setShader(&shaderMulLighting);

	//Modelo Cajas
	modelBoxes.loadModel("../models/ProyectoFinal/Box.obj");
	modelBoxes.setShader(&shaderMulLighting);

	//Modelo craneos
	modelSkull.loadModel("../models/ProyectoFinal/Skull.obj");
	modelSkull.setShader(&shaderMulLighting);

	//Modelo craneos 2
	modelSkull2.loadModel("../models/ProyectoFinal/Skull2.obj");
	modelSkull2.setShader(&shaderMulLighting);



	modelShield2.loadModel("../models/ProyectoFinal/Shield2.fbx");
	modelShield2.setShader(&shaderMulLighting);

	//Posición de la camara
	camera->setPosition(glm::vec3(0.0, 0.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(0.8);

	modelMatrixGuard[3] = glm::vec4(mazeCellSize * floor(mazeWidth / 2), 0, mazeCellSize * floor(mazeHeight / 2), 1);
	//Vector de craneos

	skulls = std::vector<Extras>{
		Extras(&modelSkull, &terrain, glm::vec3((2 * mazeCellSize) - 0.8, 0, 1 * mazeCellSize - 2.8)),
		Extras(&modelSkull2, &terrain, glm::vec3((2 * mazeCellSize), 0, 4 * mazeCellSize + 2.4)),
		Extras(&modelSkull2, &terrain, glm::vec3((4 * mazeCellSize) + 1.2, 4, 0 * mazeCellSize + 3.4)),
		Extras(&modelSkull, &terrain, glm::vec3((0 * mazeCellSize), 0, 2 * mazeCellSize + 3)),
		Extras(&modelSkull, &terrain, glm::vec3((1 * mazeCellSize) + 0.4, 0, 3 * mazeCellSize - 1.4)),
		Extras(&modelSkull2, &terrain, glm::vec3((0 * mazeCellSize) - 3.2, 0, 0 * mazeCellSize - 2.4)),
		Extras(&modelShield2, &terrain, glm::vec3((3 * mazeCellSize) - 2.2, 0.1, 3 * mazeCellSize - 2)),
		Extras(&modelShield2, &terrain, glm::vec3((4 * mazeCellSize) + 1.5, 0.1, 4 * mazeCellSize - 3.5)),

	};


	//Vector de cajas y barriles
	boxes = std::vector<Extras>{
		Extras(&modelBoxes,&terrain, glm::vec3((1 * mazeCellSize) + 3,0, 0 * mazeCellSize - 3)),
		Extras(&modelBoxes,&terrain, glm::vec3((0 * mazeCellSize) - 3.2, 0 , mazeCellSize - 2)),
		Extras(&modelBoxes,&terrain, glm::vec3((2 * mazeCellSize) - 3,0, 2 * mazeCellSize - 2)),
		Extras(&modelBoxes,&terrain, glm::vec3(((mazeWidth - 1) * mazeCellSize - 3.5), 2, 3 * mazeCellSize - 3)),
		//Barriles
		Extras(&modelBarrel,&terrain, glm::vec3((1 * mazeCellSize) - 3,0, 0 * mazeCellSize + 3)),
		Extras(&modelBarrel,&terrain, glm::vec3((3 * mazeCellSize) - 2,0, 1 * mazeCellSize - 3)),
		Extras(&modelBarrel,&terrain, glm::vec3((2 * mazeCellSize) - 2,0, 4 * mazeCellSize + 3)),
		Extras(&modelBarrel,&terrain, glm::vec3((2 * mazeCellSize) + 2,0, 1 * mazeCellSize - 2.8)),
		Extras(&modelBarrel,&terrain, glm::vec3((4 * mazeCellSize) - 3,0, 2 * mazeCellSize - 3)),
		Extras(&modelBarrel,&terrain, glm::vec3((1 * mazeCellSize) + 1,0, 3 * mazeCellSize + 3)),
		Extras(&modelBarrel,&terrain, glm::vec3((3 * mazeCellSize) + 2,0, 4 * mazeCellSize - 2.5))
	};

	//Vector de fantasmas
	ghosts = std::vector<Ghost>{
		Ghost(&modelGhost, &terrain, glm::vec3(0 * mazeCellSize, 2, 0 * mazeCellSize)),
		Ghost(&modelGhost, &terrain, glm::vec3((mazeWidth - 1) * mazeCellSize, 2, (mazeHeight - 1) * mazeCellSize))
	};

	//Creación de guardia
	player = PlayerCharacter(&GuardModelAnimate);

	//Vector de monedas
	gameSystem = GameSystem(std::vector<Collectable> {
		Collectable(&modelMoneda, glm::vec3(0 * mazeCellSize, 2, 0 * mazeCellSize)),
			Collectable(&modelMoneda, glm::vec3((mazeWidth - 1)* mazeCellSize, 2, 0 * mazeCellSize)),
			Collectable(&modelMoneda, glm::vec3(0 * mazeCellSize, 2, (mazeHeight - 1)* mazeCellSize)),
			Collectable(&modelMoneda, glm::vec3((mazeWidth - 1)* mazeCellSize, 2, (mazeHeight - 1)* mazeCellSize))
		},
		&player
	);

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


	/*******************************************
	 * OpenAL init
	 *******************************************/
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Config source 0
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/Auch.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/Coins.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/Fondo.wav");
	buffer[3] = alutCreateBufferFromFile("../sounds/Ghost.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR) {
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}


	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	//Source sonido choque con fantasma
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 4.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_FALSE);
	alSourcef(source[0], AL_MAX_DISTANCE, 1);

	//Source sonido choque con fantasma
	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 3.0f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[0]);
	alSourcei(source[1], AL_LOOPING, AL_FALSE);
	alSourcef(source[1], AL_MAX_DISTANCE, 1);

	//Source sonido fantasma
	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 0.1f);
	alSourcefv(source[2], AL_POSITION, source2Pos);
	alSourcefv(source[2], AL_VELOCITY, source2Vel);
	alSourcei(source[2], AL_BUFFER, buffer[3]);
	alSourcei(source[2], AL_LOOPING, AL_TRUE);
	alSourcef(source[2], AL_MAX_DISTANCE, 10000);

	//Source sonido fantasma
	alSourcef(source[3], AL_PITCH, 1.0f);
	alSourcef(source[3], AL_GAIN, 0.1f);
	alSourcefv(source[3], AL_POSITION, source2Pos);
	alSourcefv(source[3], AL_VELOCITY, source2Vel);
	alSourcei(source[3], AL_BUFFER, buffer[3]);
	alSourcei(source[3], AL_LOOPING, AL_TRUE);
	alSourcef(source[3], AL_MAX_DISTANCE, 10000);

	//Sonido fondo
	alSourcef(source[4], AL_PITCH, 1.0f);
	alSourcef(source[4], AL_GAIN, 3.0f);
	alSourcefv(source[4], AL_POSITION, source2Pos);
	alSourcefv(source[4], AL_VELOCITY, source2Vel);
	alSourcei(source[4], AL_BUFFER, buffer[2]);
	alSourcei(source[4], AL_LOOPING, AL_TRUE);
	alSourcef(source[4], AL_MAX_DISTANCE, 10);

	//Source sonido recolección de moneda
	alSourcef(source[5], AL_PITCH, 1.0f);
	alSourcef(source[5], AL_GAIN, 3.0f);
	alSourcefv(source[5], AL_POSITION, source1Pos);
	alSourcefv(source[5], AL_VELOCITY, source1Vel);
	alSourcei(source[5], AL_BUFFER, buffer[1]);
	alSourcei(source[5], AL_LOOPING, AL_FALSE);
	alSourcef(source[5], AL_MAX_DISTANCE, 1);
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
	GuardModelAnimate.destroy();
	modelNodo.destroy();
	modelPared.destroy();
	modelMoneda.destroy();
	modelAntorcha.destroy();
	modelGhost.destroy();
	modelBarrel.destroy();
	modelBoxes.destroy();
	modelSkull.destroy();
	modelSkull2.destroy();
	modelShield2.destroy();

	//borrado de modelo de texto
	modelText->~FontTypeRendering();

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

	//Delete sounds
	alDeleteSources(NUM_SOURCES,source);
	alDeleteBuffers(NUM_BUFFERS, buffer);
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
	if (distanceFromTarget - yoffset >= 0.8 && distanceFromTarget - yoffset <= 6.0)
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
	bool joystickanim = FALSE;

	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	//Checar existencia de joystick
	if (gameSystem.currentState == 1 && glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE) {
		int axesCount, buttonCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1,
			&buttonCount);

			if (buttons[7] == GLFW_PRESS) {
				return false;
			}
		//*****************************************************************
		//control de joystick caminando
		if (fabs(axes[1]) > 0.3 && fabs(axes[1]) > 0 && buttons[5] == GLFW_RELEASE) {
			modelMatrixGuard = glm::translate(modelMatrixGuard, glm::vec3(0, 0, +axes[1] * 5.5) * (float)deltaTime);
			player.ChangeAnimationIndex(0);
			//GuardModelAnimate.setAnimationIndex(0);
			joystickanim = TRUE;
		}
		if (fabs(axes[1]) < 0.3 && fabs(axes[1]) > 0 && fabs(axes[0]) > 0 && fabs(axes[0]) < 0.3 && buttons[5] == GLFW_RELEASE) {
			modelMatrixGuard = glm::rotate(modelMatrixGuard, glm::radians(-axes[0] * 5.5f), glm::vec3(0, 1, 0) * (float)deltaTime);
			modelMatrixGuard = glm::translate(modelMatrixGuard, glm::vec3(+axes[1] * 5.5, 0, +axes[1] * 5.5) * (float)deltaTime);
			player.ChangeAnimationIndex(0);
			//GuardModelAnimate.setAnimationIndex(0);
			joystickanim = TRUE;
		}

		if (fabs(axes[0]) > 0.3 && fabs(axes[0]) > 0 && buttons[5] == GLFW_RELEASE) {
			modelMatrixGuard = glm::rotate(modelMatrixGuard, glm::radians(-axes[0] * 5.5f), glm::vec3(0, 1, 0) * (float)deltaTime);
			player.ChangeAnimationIndex(0);
			//GuardModelAnimate.setAnimationIndex(0);
			joystickanim = TRUE;
		}
		//*****************************************************************
		//control de joystick corriendo
		if (fabs(axes[1]) > 0.3 && fabs(axes[1]) > 0 && buttons[5] == GLFW_PRESS) {
			modelMatrixGuard = glm::translate(modelMatrixGuard, glm::vec3(0, 0, +axes[1] * 9.5) * (float)deltaTime);
			player.ChangeAnimationIndex(1);
			//GuardModelAnimate.setAnimationIndex(1);
			joystickanim = TRUE;
		}
		if (fabs(axes[1]) < 0.3 && fabs(axes[1]) > 0 && fabs(axes[0]) < 0.3 && fabs(axes[0]) > 0 && buttons[5] == GLFW_PRESS) {
			modelMatrixGuard = glm::rotate(modelMatrixGuard, glm::radians(-axes[0] * 5.5f), glm::vec3(0, 1, 0) * (float)deltaTime);
			modelMatrixGuard = glm::translate(modelMatrixGuard, glm::vec3(+axes[1] * 9.5, 0, +axes[1] * 5.5) * (float)deltaTime);
			player.ChangeAnimationIndex(1);
			//GuardModelAnimate.setAnimationIndex(1);
			joystickanim = TRUE;
		}

		if (fabs(axes[0]) > 0.3 && fabs(axes[0]) > 0 && buttons[5] == GLFW_PRESS) {
			modelMatrixGuard = glm::rotate(modelMatrixGuard, glm::radians(-axes[0] * 5.5f), glm::vec3(0, 1, 0) * (float)deltaTime);
			player.ChangeAnimationIndex(1);
			//GuardModelAnimate.setAnimationIndex(1);
			joystickanim = TRUE;
		}

		//Para inicio de juego
		if (buttons[4] == GLFW_PRESS) {
			gameSystem.EnterPress();
		}
		//si no se mueve se pone en falso la animación
		if (fabs(axes[0]) == 0 && fabs(axes[1]) == 0) {
			joystickanim = FALSE;
		}

		//**************************************************************
		//Rotación de camara en y botones X y B
		if (buttons[0] == GLFW_PRESS) {

			camera->mouseMoveCamera(0.0, offsetY + 2, deltaTime);
		}
		if (buttons[3] == GLFW_PRESS){

			camera->mouseMoveCamera(0.0, offsetY - 2, deltaTime);
		}

		//**************************************************************
//Rotación de camara en y botones Y y A
		if (buttons[2] == GLFW_PRESS){
			if (distanceFromTarget >= 0.8)
			distanceFromTarget -= 0.2;
			camera->setDistanceFromTarget(distanceFromTarget);
		}
		if (buttons[1] == GLFW_PRESS) {
			if(distanceFromTarget<=6.0)
			distanceFromTarget += 0.2;
			camera->setDistanceFromTarget(distanceFromTarget);
		}

	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		gameSystem.EnterPress();
	}

	if (gameSystem.currentState == 1)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			modelMatrixGuard = glm::rotate(modelMatrixGuard, (float)-(offsetX * deltaTime), glm::vec3(0, 1, 0));
		//camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			//modelMatrixGuard = glm::rotate(modelMatrixGuard,(float) (offsetY * deltaTime), glm::vec3(1, 0, 0));
			camera->mouseMoveCamera(0.0, offsetY, deltaTime);
		}
		offsetX = 0;
		offsetY = 0;

		//***********************************************************************************************
		//Control teclas de teclado
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			modelMatrixGuard = glm::rotate(modelMatrixGuard, 5.0f * (float) deltaTime, glm::vec3(0, 1, 0));
			player.ChangeAnimationIndex(0);
			//GuardModelAnimate.setAnimationIndex(0);
			joystickanim = TRUE;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			modelMatrixGuard = glm::rotate(modelMatrixGuard, -5.0f * (float) deltaTime, glm::vec3(0, 1, 0));
			player.ChangeAnimationIndex(0);
			//GuardModelAnimate.setAnimationIndex(0);
			joystickanim = TRUE;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			modelMatrixGuard = glm::translate(modelMatrixGuard, glm::vec3(0.0, 0.0, 5.5) * (float)deltaTime);
			player.ChangeAnimationIndex(0);
			//GuardModelAnimate.setAnimationIndex(0);
			joystickanim = TRUE;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		{
			modelMatrixGuard = glm::translate(modelMatrixGuard, glm::vec3(0.0, 0.0, -5.5) * (float)deltaTime);
			player.ChangeAnimationIndex(0);
			//GuardModelAnimate.setAnimationIndex(0);
			joystickanim = TRUE;
		}


		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			modelMatrixGuard = glm::translate(modelMatrixGuard, glm::vec3(0.0, 0.0, 9.5) * (float)deltaTime);
			player.ChangeAnimationIndex(1);
			//GuardModelAnimate.setAnimationIndex(1);
			joystickanim = TRUE;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		{
			modelMatrixGuard = glm::translate(modelMatrixGuard, glm::vec3(0.0, 0.0, -9.5) * (float)deltaTime);
			player.ChangeAnimationIndex(1);
			//GuardModelAnimate.setAnimationIndex(1);
			joystickanim = TRUE;


		}

	}
	if (joystickanim == FALSE) {
		player.ChangeAnimationIndex(4);
		//GuardModelAnimate.setAnimationIndex(4);
	}
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	glm::vec3 auxPosGhosts;

	//Creacion de objeto texto
	modelText = new FontTypeRendering::FontTypeRendering(screenWidth, screenHeight);
	modelText->Initialize();

	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	lastTime = TimeManager::Instance().GetTime();

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if (currTime - lastTime < 0.016666667) {
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / (float)screenHeight, 0.01f, 50.0f);

		axis = glm::axis(glm::quat_cast(modelMatrixGuard));
		angleTarget = glm::angle(glm::quat_cast(modelMatrixGuard));
		target = modelMatrixGuard[3] + modelMatrixGuard[0] * targetOffset.x + modelMatrixGuard[1] * targetOffset.y;

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

		//estado del juego
		if (gameSystem.currentState == 0)
		{
			//Mensaje si se tiene el control conectado
			if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE) {
				//std::cout << "Esta presente el joystick" << std::endl;
				modelText->render("Bienvenido al laberinto", -0.5, 0.4, 55, 1.0, 1.0, 1.0, 1.0);
				modelText->render("Para moverte utiliza el joystick izquierdo", -0.8, 0.2, 40, 1.0, 1.0, 1.0, 1.0);
				modelText->render("Para girar camara hacia arriba o abajo", -0.5, 0.0, 40, 1.0, 0.0, 0.0, 1.0);
				modelText->render("Y->Mover camara arriba", -0.5, -0.1, 40, 1.0, 0.0, 0.0, 1.0);
				modelText->render("A->Mover camara abajo", -0.5, -0.2, 40, 1.0, 0.0, 0.0, 1.0);
				modelText->render("X->Acercar camara", -0.5, -0.3, 40, 1.0, 0.0, 0.0, 1.0);
				modelText->render("B->Alejar camara", -0.5, -0.4, 40, 1.0, 0.0, 0.0, 1.0);
				modelText->render("RB + movimiento joystick-> correr", -0.8, -0.55, 40, 0.0, 1.0, 0.0, 1.0);
				modelText->render("Cuidado con los fantasmas!!", -0.8, -0.65, 40, 1.0, 1.0, 1.0, 1.0);
				modelText->render("Presiona LB para comenzar", -0.8, -0.75, 55, 1.0, 1.0, 1.0, 1.0);

			}
			else {
				//modelText->render("Bienvenido al laberinto",         x,      y size, R , G  , B  , Alpha);
				//Renderizado de texto de inicio
				modelText->render("Bienvenido al laberinto", -0.5, 0.6, 55, 1.0, 1.0, 1.0, 1.0);
				modelText->render("para moverte utiliza las teclas", -0.8, 0.2, 40, 1.0, 1.0, 1.0, 1.0);
				modelText->render("W->avanzar hacia adelante", -0.4, 0.0, 40, 1.0, 0.0, 0.0, 1.0);
				modelText->render("S->avanzar hacia atras", -0.4, -0.1, 40, 1.0, 0.0, 0.0, 1.0);
				modelText->render("A->avanzar hacia la izquierda", -0.4, -0.2, 40, 1.0, 0.0, 0.0, 1.0);
				modelText->render("D->avanzar hacia la derecha", -0.4, -0.3, 40, 1.0, 0.0, 0.0, 1.0);
				modelText->render("W o S +Shift->correr", -0.4, -0.4, 40, 1.0, 0.0, 0.0, 1.0);
				modelText->render("Encuentra todas las monedas", -0.8, -0.55, 40, 0.0, 1.0, 0.0, 1.0);
				modelText->render("Cuidado con los fantasmas!!", -0.8, -0.65, 40, 1.0, 1.0, 1.0, 1.0);
				modelText->render("Presiona Enter para comenzar", -0.8, -0.85, 55, 1.0, 1.0, 1.0, 1.0);
			}
			//std::cout << "Press ENTER to start" << std::endl;
		}
		else if (gameSystem.currentState == 1 || gameSystem.currentState == 2)
		{
			if (gameSystem.lives != 0)
			{
				//Cantidad de monedas y vidas que se tienen
				int monedas;
				monedas = gameSystem.collectables.size() - 4;
				monedas = -monedas;
				string vidasG(std::to_string(gameSystem.lives));
				string total(std::to_string(monedas));
				modelText->render("monedas " + total, -0.8, -0.85, 55, 1.0, 1.0, 1.0, 1.0);
				modelText->render("vidas " + vidasG, 0.7, 0.6, 40, 1.0, 1.0, 1.0, 1.0);


				//neblina configuración
				shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.01, 0.01, 0.01)));
				shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));

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

				if (gameSystem.currentState == 1) {
					glm::vec3 spotPosition = gameSystem.collectables.back().posicion + glm::vec3(0, 2, 0);

					//shaderMulLighting.setInt("spotLightCount", 1);
					//shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.1)));
					//shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(1)));
					//shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3)));
					//shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
					//shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
					//shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
					//shaderMulLighting.setFloat("spotLights[0].linear", 0.074);
					//shaderMulLighting.setFloat("spotLights[0].quadratic", 0.03);
					//shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
					//shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(40.0f)));

					//shaderTerrain.setInt("spotLightCount", 1);
					//shaderTerrain.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.1)));
					//shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(1)));
					//shaderTerrain.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3)));
					//shaderTerrain.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
					//shaderTerrain.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
					//shaderTerrain.setFloat("spotLights[0].constant", 1.0);
					//shaderTerrain.setFloat("spotLights[0].linear", 0.074);
					//shaderTerrain.setFloat("spotLights[0].quadratic", 0.03);
					//shaderTerrain.setFloat("spotLights[0].cutOff", cos(glm::radians(30.0f)));
					//shaderTerrain.setFloat("spotLights[0].outerCutOff", cos(glm::radians(40.0f)));
				}

				/*******************************************
				 * Propiedades PointLights
				 *******************************************/
				glm::vec3 pointLightPosition = glm::vec3(modelMatrixGuard[3]) + glm::vec3(0.0, 1.5, 0.0) + camera->getFront() * 1.5f;

				shaderMulLighting.setInt("pointLightCount", 1);
				shaderTerrain.setInt("pointLightCount", 1);
				for (int i = 0; i <= 1; i++) {
					shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
					shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02) * 2.0f));
					shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
					shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(pointLightPosition));
					shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
					shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
					shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.01);
					shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
					shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02) * 2.0f));
					shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
					shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(pointLightPosition));
					shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
					shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
					shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.02);
				}

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

				// Model Guard
				modelMatrixGuard[3][1] = terrain.getHeightTerrain(modelMatrixGuard[3][0], modelMatrixGuard[3][2]);
				glm::mat4 modelMatrixGuardBody = glm::mat4(modelMatrixGuard);
				modelMatrixGuardBody = glm::scale(modelMatrixGuardBody, glm::vec3(0.01, 0.01, 0.01));
				player.Render(modelMatrixGuardBody, deltaTime);

				/*************************
				* Ray in Girl direction
				*************************
				glm::mat4 modelMatrixRayGirl = glm::mat4(modelMatrixGuard);
				modelMatrixRayGirl = glm::translate(modelMatrixRayGirl, glm::vec3(0.0, 1.0, 0.0));
				glm::vec3 rayDirectionGirl = glm::normalize(glm::vec3(modelMatrixRayGirl[2]));
				glm::vec3 oriGirl = glm::vec3(modelMatrixRayGirl[3]);
				glm::vec3 tarGirl = oriGirl + 10.0f * rayDirectionGirl;
				glm::vec3 tarmGirl = oriGirl + 5.0f * rayDirectionGirl;
				modelMatrixRayGirl[3] = glm::vec4(tarmGirl, 1.0f);
				modelMatrixRayGirl = glm::rotate(modelMatrixRayGirl, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
				modelMatrixRayGirl = glm::scale(modelMatrixRayGirl, glm::vec3(1.0, 10.0, 1.0));
				cylinderRay.render(modelMatrixRayGirl);
				*/

				/*******************************************
				 * Creacion de colliders
				 * IMPORTANT do this before interpolations
				 *******************************************/

				 // Colision de la camara, no se ve porque la cámara está dentro de la esfera
				AbstractModel::SBB cameraCollider;
				glm::mat4 modelMatrixColliderCamera = glm::mat4(1.0);
				modelMatrixColliderCamera[3] = glm::vec4(glm::vec3(modelMatrixGuard[3]) + glm::vec3(modelMatrixGuard[0]) * targetOffset.x + glm::vec3(modelMatrixGuard[1]) * targetOffset.y + camera->getFront() * (-distanceFromTarget + 1), 1);
				modelMatrixColliderCamera = glm::translate(modelMatrixColliderCamera, modelMoneda.getSbb().c);
				cameraCollider.c = glm::vec3(modelMatrixColliderCamera[3]);
				cameraCollider.ratio = modelMoneda.getSbb().ratio * 0.6;
				addOrUpdateColliders(collidersSBB, "camera", cameraCollider, modelMatrixColliderCamera);

				//Collider de Guard
				glm::mat4 modelMatrixColliderGuard = glm::mat4(modelMatrixGuard);
				player.PlayerCollider().u = glm::quat_cast(modelMatrixColliderGuard);
				modelMatrixColliderGuard = glm::scale(modelMatrixColliderGuard, glm::vec3(0.01));
				modelMatrixColliderGuard = glm::translate(modelMatrixColliderGuard, GuardModelAnimate.getObb().c);
				player.PlayerCollider().c = glm::vec3(modelMatrixColliderGuard[3]);
				player.PlayerCollider().e = GuardModelAnimate.getObb().e * glm::vec3(0.005, 0.015, 0.01) * glm::vec3(0.785, 0.785, 0.785);
				addOrUpdateColliders(collidersOBB, "guard", player.PlayerCollider(), modelMatrixGuard);

				for (int i = 0; i < ghosts.size(); i++)
				{
					sourcesPlay[i] = ghosts[i].UpdateGhost(&maze, deltaTime, &gameSystem);

				}

				for (int i = 0; i < skulls.size(); i++)
				{
					skulls[i].RenderBarrel();
				}


				for (int i = 0; i < boxes.size(); i++)
				{
					boxes[i].RenderBarrel();
					boxes[i].UpdateCollider("Cajas " + std::to_string(i), collidersOBB);
				}

				//localización de fantasmas para sonido
				ghost1Pos = ghosts[0].CurrentPosition();
				ghost2Pos = ghosts[1].CurrentPosition();
				sourceG1Pos[0] = ghost1Pos.x;
				sourceG1Pos[1] = ghost1Pos.y;
				sourceG1Pos[2] = ghost1Pos.z;


				sourceG2Pos[0] = ghost2Pos.x;
				sourceG2Pos[1] = ghost2Pos.y;
				sourceG2Pos[2] = ghost2Pos.z;

				alSourcefv(source[2], AL_POSITION, sourceG1Pos);

				alSourcefv(source[3], AL_POSITION, sourceG2Pos);


				maze_ptr->DisplayMaze(modelNodo, modelPared, modelAntorcha, collidersOBB);
				gameSystem.UpdateGameSystem(&player, modelText);
				sourcesPlay[5]= gameSystem.collectedCoin;
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

				 //glm::mat4 matrixCollider = glm::mat4(1.0);
				 //matrixCollider = glm::translate(matrixCollider, ghost.Collider().c);
				 //matrixCollider = matrixCollider * glm::mat4(ghost.Collider().u);
				 //matrixCollider = glm::scale(matrixCollider, ghost.Collider().e * 2.0f);
				 //boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
				 //boxCollider.enableWireMode();
				 //boxCollider.render(matrixCollider);

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
							if (it->first == "camera" && jt->first == "guard")
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
							if (it->first.compare("guard") == 0)
							{
								//	std::cout << "Collision " << it->first << " with " << jt->first << std::endl;
								modelMatrixGuard = std::get<1>(it->second);
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
							if (jt->first.compare("guard") == 0)
							{
								modelMatrixGuard = std::get<1>(jt->second);
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
				********************************************
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
				}*/

				// Constantes de animaciones
				animationIndex = 1;
			}

			if (gameSystem.lives == 0) {
				gameSystem.currentState = 2;
				modelText->render("Te mataron los fantasmas =(", -0.6, 0, 55, 1.0, 0.0, 0.0, 1.0);
				modelText->render("Enter para Intentalo de nuevo", -0.3, -0.2, 45, 1.0, 0.0, 0.0, 1.0);
			}
		}
		else if (gameSystem.currentState == 3)
		{
			// Creacion de laberinto
			maze = Maze (mazeWidth, mazeHeight, mazeCellSize);

			// Reseteo de guardia
			modelMatrixGuard = glm::mat4(1.0f);
			modelMatrixGuard[3] = glm::vec4(mazeCellSize * floor(mazeWidth / 2), 0, mazeCellSize * floor(mazeHeight / 2), 1);

			ghosts.clear();
			// Creación de fantasmas
			ghosts = std::vector<Ghost>{
				Ghost(&modelGhost, &terrain, glm::vec3(0 * mazeCellSize, 2, 0 * mazeCellSize)),
				Ghost(&modelGhost, &terrain, glm::vec3((mazeWidth - 1) * mazeCellSize, 2, (mazeHeight - 1) * mazeCellSize))
			};

			//Creación de guardia
			player = PlayerCharacter(&GuardModelAnimate);

			//Vector de monedas
			gameSystem = GameSystem(
				std::vector<Collectable> {
					Collectable(&modelMoneda, glm::vec3(0 * mazeCellSize, 2, 0 * mazeCellSize)),
					Collectable(&modelMoneda, glm::vec3((mazeWidth - 1)* mazeCellSize, 2, 0 * mazeCellSize)),
					Collectable(&modelMoneda, glm::vec3(0 * mazeCellSize, 2, (mazeHeight - 1)* mazeCellSize)),
					Collectable(&modelMoneda, glm::vec3((mazeWidth - 1)* mazeCellSize, 2, (mazeHeight - 1)* mazeCellSize))
				},
				& player
			);

			collidersOBB.clear();
			collidersSBB.clear();
		}

		glfwSwapBuffers(window);
		// Listener for the First person camera
		listenerPos[0] = camera->getPosition().x;
		listenerPos[1] = camera->getPosition().y;
		listenerPos[2] = camera->getPosition().z;
		alListenerfv(AL_POSITION, listenerPos);
		listenerOri[0] = camera->getFront().x;
		listenerOri[1] = camera->getFront().y;
		listenerOri[2] = camera->getFront().z;
		listenerOri[3] = camera->getUp().x;
		listenerOri[4] = camera->getUp().y;
		listenerOri[5] = camera->getUp().z;
		alListenerfv(AL_ORIENTATION, listenerOri);

		for (int i = 0; i < ghosts.size(); i++) {
			
		}

		for (unsigned int i = 0; i < sourcesPlay.size(); i++) {
			if (sourcesPlay[i]) {
				sourcesPlay[i] = false;
				alSourcePlay(source[i]);
			}
		}
	}
}

//Función main
int main(int argc, char** argv) {
	init(1250, 1100, "Maze", false);
	applicationLoop();
	destroy();
	return 1;
}


