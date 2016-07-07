#pragma once
#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include <..\COMP371_Lab6\objloader.hpp> 
#include "..\soil\SOIL.h"

// Window dimensions
const GLuint WINDOW_WIDTH = 800, WINDOW_HEIGHT = 800;
const float ROOM_WIDTH = 2000.0f, ROOM_HEIGHT = 500.0f;

//Current window
GLFWwindow* window;

//All the Shader Locs
GLuint projectionLoc;
GLuint viewMatrixLoc;
GLuint transformLoc;
GLuint colorLoc;
GLuint useTextureLoc;
GLuint viewPosLoc;

//VAOS, VBOS, Textures & ShaderProgram
GLuint VAO, vertices_VBO, normals_VBO, UVs_VBO, Chair_VAO, Chair_vertices_VBO, Chair_normals_VBO, Chair_UVs_VBO, Table_VAO, Table_vertices_VBO, Table_normals_VBO, Table_UVs_VBO, Toy_VAO, Toy_vertices_VBO, Toy_normals_VBO, Toy_UVs_VBO;
GLuint floor_texture, ceiling_texture, wall_texture, chair_texture, table_texture, toy_texture;
GLuint shaderProgram;

//All Normals, UVs and Vertices 
std::vector<glm::vec3> vertices, chair_vertices, table_vertices, toy_vertices;
std::vector<glm::vec3> normals, chair_normals, table_normals, toy_normals;
std::vector<glm::vec2> UVs, chair_UVs, table_UVs, toy_UVs;

//Middle location of room
glm::vec3 middle_of_room = glm::vec3(1000, 1, 1000);

//Projection Matrix and Empty Matrix
glm::mat4 projection_matrix;
glm::mat4 view_matrix;
glm::mat4 empty_matrix;

//Rotation and Translation for the current object
glm::vec3 object_translation = middle_of_room;
glm::vec3 object_rotation = glm::vec3(0.0f, 0.0f, 0.0f);

//Camera Location, Rotation and Controls
glm::vec3 camera_translation = glm::vec3(12.0f, 486.2f, 1941.6f);
glm::vec3 camera_rotation = glm::vec3(0.617670774f, -0.386711001f, -0.684790134f);
bool left_button_down = false;
bool right_button_down = false;
bool firstClick = true;
float zoom = 0;
float previousZoom = 0;
float roty = -28.8f;
float rotx = -44.1f;
float tx = camera_translation.x;
float ty = camera_translation.y;
float lastx = 0;
float lasty = 0;

//Bounding Box information
std::vector<glm::vec3> bounding_box_sizes;
std::vector<glm::vec3> bounding_box_centers;
std::vector<glm::mat4> bounding_box_matrix;
glm::vec3 objectCenter;

//User Selection
int userSelection = 0;
bool changeFloorColor = false;
bool changeWallColor = false;
bool changeCeilingColor = false;
bool inMenu = false;
GLfloat floorColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat wallColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat ceilingColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat no_color[] = { 0.0f, 0.0f, 0.0f, 0.0f };

//Chairs
int numberOfChairsToSpawn = 0;
std::vector<glm::vec3> chair_translation;
std::vector<glm::vec3> chair_rotation;

//Tables
int numberOfTablesToSpawn = 0;
std::vector<glm::vec3> table_translation;
std::vector<glm::vec3> table_rotation;

//Toys
int numberOfToysToSpawn = 0;
std::vector<glm::vec3> toy_translation;
std::vector<glm::vec3> toy_rotation;

//Latest Object to modify
int currentObject = -1;
int currentObjectArrayIndex = -1;