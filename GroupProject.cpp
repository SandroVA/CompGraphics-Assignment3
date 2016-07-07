#include "GroupProject.h"
using namespace std;

//Gets userinput between 0 and the number entered
void getUserInput(int inputs)
{
	bool correctInput = false;
	//If the input is incorrect it will keep asking the user for input
	while (!correctInput)
	{
		cin >> userSelection;
		if (cin.fail() || userSelection > inputs || userSelection < 1) {
			cin.clear();
			cin.ignore(256, '\n');
			userSelection = 0;
			cout << "Error, please enter an integer between the correct values." << endl;
		}
		else
		{
			correctInput = true;
		}
	}
}

//Checks to see if the object you are currently moving hits any other objects bounding box.
bool doesCurrentObjectCollide(const glm::mat4 &currentMatrix, glm::vec3 currentObjectCenter)
{
	glm::vec3 currentObjectSize = bounding_box_sizes[bounding_box_sizes.size() - 1];
	glm::vec3 currentObjectDimensions = glm::abs(glm::vec3(currentMatrix * glm::vec4(currentObjectSize.x, currentObjectSize.y, currentObjectSize.z, 1.0)));
	glm::vec3 otherObjectLocation;
	glm::vec3 otherObjectDimensions;

	//Loop through all the objects
	for (int i = 0; i < bounding_box_sizes.size() - 1; i++)
	{
		otherObjectLocation = glm::vec3(glm::vec4(bounding_box_centers[i].x, bounding_box_centers[i].y, bounding_box_centers[i].z, 1.0));
		otherObjectDimensions = glm::abs(glm::vec3(bounding_box_matrix[i] * glm::vec4(bounding_box_sizes[i].x, bounding_box_sizes[i].y, bounding_box_sizes[i].z, 1.0)));
		
		float distancex = glm::distance(currentObjectCenter.x, otherObjectLocation.x);
		float distancez = glm::distance(currentObjectCenter.z, otherObjectLocation.z);
		if (((currentObjectDimensions.x + otherObjectDimensions.x) > distancex &&
			 (currentObjectDimensions.z + otherObjectDimensions.z) > distancez))
		{
			return true;
		}
	}

	return false;
}

//Calculates the bounding box size for an object
glm::vec3 calculateBoundingBox(std::vector<glm::vec3> vertices)
{
	GLfloat min_x, max_x, min_y, max_y, min_z, max_z;
	min_x = max_x = vertices[0].x;
	min_y = max_y = vertices[0].y;
	min_z = max_z = vertices[0].z;

	for (int i = 0; i < (int)vertices.size(); i++) {
		if (vertices[i].x < min_x) min_x = vertices[i].x;
		if (vertices[i].x > max_x) max_x = vertices[i].x;
		if (vertices[i].y < min_y) min_y = vertices[i].y;
		if (vertices[i].y > max_y) max_y = vertices[i].y;
		if (vertices[i].z < min_z) min_z = vertices[i].z;
		if (vertices[i].z > max_z) max_z = vertices[i].z;
	}
	glm::vec3 size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
	glm::vec3 center = glm::vec3((min_x + max_x) / 2, (min_y + max_y) / 2, (min_z + max_z) / 2);
	
	bounding_box_sizes.push_back(size);
	return center;
}

//Calculates the rooms bounding box
void calculateRoomBoundingBox()
{
	glm::vec3 wallCenter;

	glm::mat4 wall_matrix;
	glm::vec3 wall_scale = glm::vec3(ROOM_WIDTH + 2, ROOM_HEIGHT + 2, 3);
	wall_matrix = glm::scale(wall_matrix, wall_scale);
	wallCenter = calculateBoundingBox(vertices);
	bounding_box_centers.push_back(glm::vec3(0.0f, 0.0f, 0.0f)+(wallCenter*wall_scale));
	bounding_box_matrix.push_back(wall_matrix);

	wall_matrix = glm::scale(empty_matrix, wall_scale);
	wallCenter = calculateBoundingBox(vertices);
	bounding_box_centers.push_back(glm::vec3(0.0f, 0.0f, ROOM_WIDTH) + (wallCenter*wall_scale));
	bounding_box_matrix.push_back(wall_matrix);

	wall_scale = glm::vec3(3, ROOM_HEIGHT + 2, ROOM_WIDTH + 2);
	wall_matrix = glm::scale(empty_matrix, wall_scale);
	wallCenter = calculateBoundingBox(vertices);
	bounding_box_centers.push_back(glm::vec3(0.0f, 0.0f, 0.0f) + (wallCenter*wall_scale));
	bounding_box_matrix.push_back(wall_matrix);

	wall_matrix = glm::scale(empty_matrix, wall_scale);
	wallCenter = calculateBoundingBox(vertices);
	bounding_box_centers.push_back(glm::vec3(ROOM_WIDTH, 0.0f, 0.0f) + (wallCenter*wall_scale));
	bounding_box_matrix.push_back(wall_matrix);
}

//Draws all the table objects in the world
void drawTable ()
{
	glProgramUniform4fv(shaderProgram, colorLoc, 1, no_color);
	glUniform1i(useTextureLoc, true);
	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0); //tell our uniform texture sampler to sample texture unit 0
	glBindTexture(GL_TEXTURE_2D, table_texture);

	for (int i = 0; i < numberOfTablesToSpawn; i++)
	{
		glm::mat4 table_matrix;
		table_matrix = glm::translate(table_matrix, table_translation [i]);
		table_matrix = glm::rotate(table_matrix, table_rotation[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		table_matrix = glm::scale(table_matrix, glm::vec3(2.5f, 1.75f, 2.5f)); //table was too small and needed to be scaled up

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(table_matrix));

		glBindVertexArray(Table_VAO);

		glDrawArrays(GL_TRIANGLES, 0, table_vertices.size());

		glBindVertexArray(0);
	}
}

//Draws all the toy objects in the world
void drawToy()
{
	glProgramUniform4fv(shaderProgram, colorLoc, 1, no_color);
	glUniform1i(useTextureLoc, true);
	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0); //tell our uniform texture sampler to sample texture unit 0
	glBindTexture(GL_TEXTURE_2D, toy_texture);

	for (int i = 0; i < numberOfToysToSpawn; i++)
	{
		glm::mat4 toy_matrix;
		toy_matrix = glm::translate(toy_matrix, toy_translation[i]);
		toy_matrix = glm::rotate(toy_matrix, toy_rotation[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
		toy_matrix = glm::scale(toy_matrix, glm::vec3(1.75f, 1.75f, 1.75f)); //toy was too small and needed to be scaled up

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(toy_matrix));

		glBindVertexArray(Toy_VAO);

		glDrawArrays(GL_TRIANGLES, 0, toy_vertices.size());

		glBindVertexArray(0);
	}
}

//Draws all the chair objects in the world
void drawChair()
{
	glProgramUniform4fv(shaderProgram, colorLoc, 1, no_color);
	glUniform1i(useTextureLoc, true);
	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 0); //tell our uniform texture sampler to sample texture unit 0
	glBindTexture(GL_TEXTURE_2D, chair_texture);

	for (int i = 0; i < numberOfChairsToSpawn; i++)
	{
		glm::mat4 chair_matrix;
		chair_matrix = glm::translate(chair_matrix, chair_translation[i]);
		chair_matrix = glm::rotate(chair_matrix, chair_rotation[i].y, glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(chair_matrix));

		glBindVertexArray(Chair_VAO);

		glDrawArrays(GL_TRIANGLES, 0, chair_vertices.size());

		glBindVertexArray(0);
	}
}

//Draws the room
void drawRoom()
{
	glUniform1i(glGetUniformLocation(shaderProgram, "texture"), 0); //tell our uniform texture sampler to sample texture unit 0

	//DRAW FLOOR --------------------------
	if (changeFloorColor)
	{
		glUniform1i(useTextureLoc, false);
		glUniform4f(colorLoc, floorColor[0], floorColor[1], floorColor[2], floorColor[3]);
	}
	else
	{
		glUniform1i(useTextureLoc, true);
		glProgramUniform4fv(shaderProgram, colorLoc, 1, no_color);
	}
	glBindTexture(GL_TEXTURE_2D, floor_texture);
	glm::mat4 floor_matrix;
	glm::vec3 floor_scale = glm::vec3(ROOM_WIDTH, 2, ROOM_WIDTH);
	floor_matrix = glm::scale(floor_matrix, floor_scale);

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(floor_matrix));

	glBindVertexArray(VAO);

	if (camera_translation.y > 0.0f)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	//DRAW ROOF --------------------------
	if (changeCeilingColor)
	{
		glUniform1i(useTextureLoc, false);
		glUniform4f(colorLoc, ceilingColor[0], ceilingColor[1], ceilingColor[2], ceilingColor[3]);
	}
	else
	{
		glUniform1i(useTextureLoc, true);
		glProgramUniform4fv(shaderProgram, colorLoc, 1, no_color);
	}
	glBindTexture(GL_TEXTURE_2D, ceiling_texture);
	glm::mat4 roof_matrix;
	roof_matrix = glm::translate(roof_matrix, glm::vec3(0.0f, ROOM_HEIGHT, 0.0f));
	glm::vec3 roof_scale = glm::vec3(ROOM_WIDTH, 2, ROOM_WIDTH);
	roof_matrix = glm::scale(roof_matrix, roof_scale);

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(roof_matrix));

	if (camera_translation.y < 499.0f)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	//DRAW WALL --------------------------
	if (changeWallColor)
	{
		glUniform1i(useTextureLoc, false);
		glUniform4f(colorLoc, wallColor[0], wallColor[1], wallColor[2], wallColor[3]);
	}
	else
	{
		glUniform1i(useTextureLoc, true);
		glProgramUniform4fv(shaderProgram, colorLoc, 1, no_color);
	}
	glBindTexture(GL_TEXTURE_2D, wall_texture);
	glm::mat4 wall_matrix;
	glm::vec3 wall_scale = glm::vec3(ROOM_WIDTH+2, ROOM_HEIGHT+2, 3);
	wall_matrix = glm::scale(wall_matrix, wall_scale);

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(wall_matrix));

	if (camera_translation.z > 2.0f)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	//DRAW WALL --------------------------
	wall_matrix = glm::translate(empty_matrix, glm::vec3(0.0f, 0.0f, ROOM_WIDTH));
	wall_matrix = glm::scale(wall_matrix, wall_scale);

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(wall_matrix));

	if (camera_translation.z < 1998.0f)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	//DRAW WALL --------------------------
	wall_scale = glm::vec3(3, ROOM_HEIGHT + 2, ROOM_WIDTH + 2);
	wall_matrix = glm::scale(empty_matrix, wall_scale);

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(wall_matrix));

	if (camera_translation.x > 0.0f)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	
	//DRAW WALL --------------------------
	wall_matrix = glm::translate(empty_matrix, glm::vec3(ROOM_WIDTH, 0.0f, 0.0f));
	wall_matrix = glm::scale(wall_matrix, wall_scale);

	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(wall_matrix));

	if (camera_translation.x < 1998.0f)
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);
}

void mouseCameraControls(GLFWwindow* window) {
	double x;
	double y;
	glfwGetCursorPos(window, &x, &y);
	if (firstClick)
	{
		lastx = (float) x;
		lasty = (float) y;
		firstClick = false;
	}
	float diffx = (float) x - lastx;
	float diffy = lasty - (float) y;
	lastx = (float) x;
	lasty = (float) y;

	//Holding left click lets you rotate in 360 degrees
	if (left_button_down && (diffx != 0 || diffy != 0))
	{
		roty += 0.75f * diffy;
		rotx += 0.75f * diffx;
		if (roty > 89.0f)
			roty = 89.0f;
		if (roty < -89.0f)
			roty = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(rotx)) * cos(glm::radians(roty));
		front.y = sin(glm::radians(roty));
		front.z = sin(glm::radians(rotx)) * cos(glm::radians(roty));
		camera_rotation = glm::normalize(front);
	}
	//Holding rightclick allows you to move LEFT/RIGHT/UP/DOWN
	if (right_button_down && (diffx != 0 || diffy != 0))
	{
		tx += 1.0f * diffx;
		ty += 1.0f * diffy;
		camera_translation.z = camera_translation.z + (diffx * camera_rotation.x);
		camera_translation.x = camera_translation.x + (-diffx * camera_rotation.z);
		camera_translation.y = ty;
	}
}

void menu()
{
	bool exit = false;
	while (!exit) //While you have not selected exit....
	{
		inMenu = true;
		cout << "What would you like to do?" << endl;
		cout << "1) Change room color" << endl;
		cout << "2) Change room Texture" << endl;
		cout << "3) Spawn Object" << endl;
		cout << "4) Exit" << endl;
		getUserInput(4);
		if (userSelection == 1) //Change room Color ------------------------------------------
		{
			cout << "1) Change Floor Color" << endl;
			cout << "2) Change Wall Color" << endl;
			cout << "3) Change Ceiling Color" << endl;
			getUserInput(3);
			int surfaceToChange = userSelection;
			if (userSelection == 1) //Change Floor Color
			{
				changeFloorColor = true;
			}
			else if (userSelection == 2) //Change Wall Color
			{
				changeWallColor = true;
			}
			else if (userSelection == 3) //Change Ceiling Color
			{
				changeCeilingColor = true;
			}
			cout << "What color would you like?" << endl;
			cout << "1) Red" << endl;
			cout << "2) Green" << endl;
			cout << "3) Blue" << endl;
			cout << "4) None" << endl;
			getUserInput(4);
			if (userSelection == 1) //RED
			{
				if (surfaceToChange == 1) //Change Floor Color
				{
					floorColor[0] = 1.0f;
					floorColor[1] = 0.0f;
					floorColor[2] = 0.0f;
				}
				else if (surfaceToChange == 2) //Change Wall Color
				{
					wallColor[0] = 1.0f;
					wallColor[1] = 0.0f;
					wallColor[2] = 0.0f;
				}
				else if (surfaceToChange == 3) //Change Ceiling Color
				{
					ceilingColor[0] = 1.0f;
					ceilingColor[1] = 0.0f;
					ceilingColor[2] = 0.0f;
				}
			}
			else if (userSelection == 2) //BLUE
			{
				if (surfaceToChange == 1) //Change Floor Color
				{
					floorColor[0] = 0.0f;
					floorColor[1] = 1.0f;
					floorColor[2] = 0.0f;
				}
				else if (surfaceToChange == 2) //Change Wall Color
				{
					wallColor[0] = 0.0f;
					wallColor[1] = 1.0f;
					wallColor[2] = 0.0f;
				}
				else if (surfaceToChange == 3) //Change Ceiling Color
				{
					ceilingColor[0] = 0.0f;
					ceilingColor[1] = 1.0f;
					ceilingColor[2] = 0.0f;
				}
			}
			else if (userSelection == 3) //GREEN
			{
				if (surfaceToChange == 1) //Change Floor Color
				{
					floorColor[0] = 0.0f;
					floorColor[1] = 0.0f;
					floorColor[2] = 1.0f;
				}
				else if (surfaceToChange == 2) //Change Wall Color
				{
					wallColor[0] = 0.0f;
					wallColor[1] = 0.0f;
					wallColor[2] = 1.0f;
				}
				else if (surfaceToChange == 3) //Change Ceiling Color
				{
					ceilingColor[0] = 0.0f;
					ceilingColor[1] = 0.0f;
					ceilingColor[2] = 1.0f;
				}
			}
			else if (userSelection == 4) //NONE
			{
				if (surfaceToChange == 1) //Change Floor Color
				{
					changeFloorColor = false;
				}
				else if (surfaceToChange == 2) //Change Wall Color
				{
					changeWallColor = false;
				}
				else if (surfaceToChange == 3) //Change Ceiling Color
				{
					changeCeilingColor = false;
				}
			}
		}
		else if (userSelection == 2) //Change Room Texture ------------------------------------------
		{
			cout << "Which texture would you like to change?" << endl;
			cout << "1) Change Floor Texture" << endl;
			cout << "2) Change Wall Texture" << endl;
			cout << "3) Change Ceiling Texture" << endl;
			getUserInput(3);
			int toBeChanged = userSelection;

			cout << "What texture would you like? (Please enter a number between 1 and 3)" << endl;
			getUserInput(3);

			int texture_width, texture_height;
			if (toBeChanged == 1) { //Change Floor Texture
				glActiveTexture(GL_TEXTURE0); //select texture unit 0
				glBindTexture(GL_TEXTURE_2D, floor_texture); //bind this texture to the currently bound texture unit
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				string temp = "textures/floor" + std::to_string(userSelection) + ".jpg";
				const char* textureLocation = temp.c_str();
				unsigned char* image = SOIL_load_image(textureLocation, &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				SOIL_free_image_data(image); //free resources
			}
			else if (toBeChanged == 2) { //Change Wall Texture
				glActiveTexture(GL_TEXTURE0); //select texture unit 0
				glBindTexture(GL_TEXTURE_2D, wall_texture); //bind this texture to the currently bound texture unit
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				string temp = "textures/wall" + std::to_string(userSelection) + ".jpg";
				const char* textureLocation = temp.c_str();
				unsigned char* image = SOIL_load_image(textureLocation, &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				SOIL_free_image_data(image); //free resources
			}
			else if (toBeChanged == 3) { //Change Ceiling Texture
				glActiveTexture(GL_TEXTURE0); //select texture unit 0
				glBindTexture(GL_TEXTURE_2D, ceiling_texture); //bind this texture to the currently bound texture unit
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				string temp = "textures/ceiling" + std::to_string(userSelection) + ".jpg";
				const char* textureLocation = temp.c_str();
				unsigned char* image = SOIL_load_image(textureLocation, &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
				SOIL_free_image_data(image); //free resources
			}
		}
		else if (userSelection == 3) //Spawn Object ------------------------------------------
		{
			cout << "Which object would you like to spawn?" << endl;
			cout << "1) Spawn Chair" << endl;
			cout << "2) Spawn Table" << endl;
			cout << "3) Spawn Toy" << endl;
			getUserInput(3);

			if (currentObject == 1) //Calculate the final rotation and translation of the previously spawned chair object
			{
				glm::mat4 chair_matrix;
				chair_matrix = glm::rotate(chair_matrix, object_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				bounding_box_matrix.push_back(chair_matrix);
				bounding_box_centers.push_back(object_translation + objectCenter);
			}
			else if (currentObject == 2) //Calculate the final rotation and translation of the previously spawned table object
			{
				glm::mat4 table_matrix;
				table_matrix = glm::rotate(table_matrix, object_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				table_matrix = glm::scale(table_matrix, glm::vec3(2.5f, 1.75f, 2.5f));
				bounding_box_matrix.push_back(table_matrix);
				bounding_box_centers.push_back(object_translation + (objectCenter*glm::vec3(0.25f, 0.25f, 0.15f)));
			}
			else if (currentObject == 3) //Calculate the final rotation and translation of the previously spawned toy object
			{
				glm::mat4 toy_matrix;
				toy_matrix = glm::rotate(toy_matrix, object_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				toy_matrix = glm::scale(toy_matrix, glm::vec3(1.75f, 1.75f, 1.75f));
				bounding_box_matrix.push_back(toy_matrix);
				bounding_box_centers.push_back(object_translation + (objectCenter*glm::vec3(1.75f, 1.75f, 1.75f)));
			}
			int previousObject = currentObject;
			currentObject = userSelection;
			object_translation = middle_of_room;
			object_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
			int count = 0;
			if (userSelection == 1) //Spawn Chair
			{
				objectCenter = calculateBoundingBox(chair_vertices);
				glm::mat4 chair_matrix;
				chair_matrix = glm::rotate(chair_matrix, object_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				//If theres already an object in the middle of the room try to spawn this object in a random location.
				while (doesCurrentObjectCollide(chair_matrix, objectCenter + object_translation) && count < 10000)
				{
					object_translation.x = rand() % 2000;
					object_translation.z = rand() % 2000;
					count++;
				}
				if (count < 10000) //If the spawning of the object succeeds...
				{
					numberOfChairsToSpawn++;
					chair_translation.push_back(object_translation);
					chair_rotation.push_back(object_rotation);
					currentObjectArrayIndex = chair_translation.size() - 1;
				}
				else //If the spawning of the object fails (after attempting to spawn it in 10000 different locations
				{
					//Reset all changes to the object we just made.
					cout << "ERROR: Could not find a suitable place to spawn the object." << endl;
					object_translation = chair_translation[chair_translation.size() - 1];
					object_rotation = chair_rotation[chair_rotation.size() - 1];
					bounding_box_matrix.pop_back();
					bounding_box_centers.pop_back();
					bounding_box_sizes.pop_back();
					currentObject = previousObject;
				}
			}
			else if (userSelection == 2) //Spawn table
			{
				objectCenter = calculateBoundingBox(table_vertices);
				glm::mat4 table_matrix;
				table_matrix = glm::rotate(table_matrix, object_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				table_matrix = glm::scale(table_matrix, glm::vec3(2.5f, 1.75f, 2.5f));
				//If theres already an object in the middle of the room try to spawn this object in a random location.
				while (doesCurrentObjectCollide(table_matrix, objectCenter + object_translation) && count < 10000)
				{
					object_translation.x = rand() % 2000;
					object_translation.z = rand() % 2000;
					count++;
				}
				if (count < 10000) //If the spawning of the object succeeds...
				{
					numberOfTablesToSpawn++;
					table_translation.push_back(object_translation);
					table_rotation.push_back(object_rotation);
					currentObjectArrayIndex = table_translation.size() - 1;
				}
				else //If the spawning of the object fails (after attempting to spawn it in 10000 different locations
				{
					//Reset all changes to the object we just made.
					object_translation = table_translation[table_translation.size() - 1];
					object_rotation = table_rotation[table_rotation.size() - 1];
					cout << "ERROR: Could not find a suitable place to spawn the object." << endl;
					bounding_box_matrix.pop_back();
					bounding_box_centers.pop_back();
					bounding_box_sizes.pop_back();
					currentObject = previousObject;
				}
			}
			else if (userSelection == 3) //Spawn toy
			{
				objectCenter = calculateBoundingBox(toy_vertices);
				glm::mat4 toy_matrix;
				toy_matrix = glm::rotate(toy_matrix, object_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				toy_matrix = glm::scale(toy_matrix, glm::vec3(1.75f, 1.75f, 1.75f));
				//If theres already an object in the middle of the room try to spawn this object in a random location.
				while (doesCurrentObjectCollide(toy_matrix, objectCenter + object_translation) && count < 10000)
				{
					object_translation.x = rand() % 2000;
					object_translation.z = rand() % 2000;
					count++;
				}
				if (count < 10000) //If the spawning of the object succeeds...
				{
					numberOfToysToSpawn++;
					toy_translation.push_back(object_translation);
					toy_rotation.push_back(object_rotation);
					currentObjectArrayIndex = toy_translation.size() - 1;
				}
				else //If the spawning of the object fails (after attempting to spawn it in 10000 different locations
				{
					//Reset all changes to the object we just made.
					object_translation = toy_translation[toy_translation.size() - 1];
					object_rotation = toy_rotation[toy_rotation.size() - 1];
					cout << "ERROR: Could not find a suitable place to spawn the object." << endl;
					bounding_box_matrix.pop_back();
					bounding_box_centers.pop_back();
					bounding_box_sizes.pop_back();
					currentObject = previousObject;
				}
			}
		}
		else if (userSelection == 4) // Exit ----------------------------------------------
		{
			exit = true;
		}

		//Redraw everything --------------------------------------------
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view_matrix = glm::lookAt(camera_translation, camera_translation + camera_rotation, glm::vec3(0.0, 1.0, 0.0));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		drawRoom();
		drawChair();
		drawTable();
		drawToy();

		// Swap the screen buffers
		glfwSwapBuffers(window);
		mouseCameraControls(window);
	}
	inMenu = false;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	// Update the Projection matrix after a window resize event
	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 5000.0f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	glm::vec3 previousObjectTranslation = object_translation;
	glm::vec3 previousObjectRotation = object_rotation;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W) //Translate on Z
		object_translation.z -= 10.0f;

	if (key == GLFW_KEY_A) //Translate on X
		object_translation.x -= 10.0f;
	
	if (key == GLFW_KEY_S) //Translate on Z
		object_translation.z += 10.0f;

	if (key == GLFW_KEY_D) //Translate on X
		object_translation.x += 10.0f;

	if (key == GLFW_KEY_Q) //Rotate on Y
		object_rotation.y -= 0.12f;
	
	if (key == GLFW_KEY_E) //Rotate on Y
		object_rotation.y += 0.12f;

	if (currentObjectArrayIndex >= 0)
	{
		if (key == GLFW_KEY_W || GLFW_KEY_A || GLFW_KEY_S || GLFW_KEY_D || key == GLFW_KEY_Q || GLFW_KEY_E)
		{
			if (currentObject == 1) //Chair Object
			{
				glm::mat4 chair_matrix;
				chair_matrix = glm::rotate(chair_matrix, object_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				//If the new object location and translation does not collide with anything then move said object
				if (!doesCurrentObjectCollide(chair_matrix, objectCenter + object_translation))
				{
					chair_translation[currentObjectArrayIndex] = object_translation;
					chair_rotation[currentObjectArrayIndex] = object_rotation;
				}
				else
				{
					object_translation = previousObjectTranslation;
					object_rotation = previousObjectRotation;
				}
			}
			else if (currentObject == 2) //Table Object
			{
				glm::mat4 table_matrix;
				table_matrix = glm::rotate(table_matrix, object_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				table_matrix = glm::scale(table_matrix, glm::vec3(2.5f, 1.75f, 2.5f));
				//If the new object location and translation does not collide with anything then move said object
				if (!doesCurrentObjectCollide(table_matrix, objectCenter + object_translation))
				{
					table_translation[currentObjectArrayIndex] = object_translation;
					table_rotation[currentObjectArrayIndex] = object_rotation;
				}
				else
				{
					object_translation = previousObjectTranslation;
					object_rotation = previousObjectRotation;
				}
			}
			else if (currentObject == 3) //Toy Object
			{
				glm::mat4 toy_matrix;
				toy_matrix = glm::rotate(toy_matrix, object_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
				toy_matrix = glm::scale(toy_matrix, glm::vec3(1.75f, 1.75f, 1.75f));
				//If the new object location and translation does not collide with anything then move said object
				if (!doesCurrentObjectCollide(toy_matrix, objectCenter + object_translation))
				{
					toy_translation[currentObjectArrayIndex] = object_translation;
					toy_rotation[currentObjectArrayIndex] = object_rotation;
				}
				else
				{
					object_translation = previousObjectTranslation;
					object_rotation = previousObjectRotation;
				}
			}
		}
	}

	//Pressing the HOME key will open the menu.
	if (key == GLFW_KEY_HOME && action == GLFW_PRESS)
	{
		if(!inMenu)
			menu();
	}
		
}

//Scrolling forward/backward will move the camera forward/backward.
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	zoom += 20*((float)yoffset - previousZoom);
	previousZoom = (float) yoffset;
	camera_translation.x = camera_translation.x + zoom * camera_rotation.x;
	camera_translation.y = ty = camera_translation.y + zoom * camera_rotation.y;
	camera_translation.z = camera_translation.z + zoom * camera_rotation.z;
}

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			left_button_down = true;
		else if (GLFW_RELEASE == action)
			left_button_down = false;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			right_button_down = true;
		else if (GLFW_RELEASE == action)
			right_button_down = false;
	}
}

int main()
{
	std::cout << "--------------------------------------------------------------------------------";
	std::cout << "Welcome to the final group project by: Anna Rogozin and Sandro Victoria-Arena." << std::endl;
	std::cout << "--------------------------------------------------------------------------------";
	std::cout << "Use left mouse click to look around," << std::endl;
	std::cout << "right mouse click to move up/down left/right in the direction you are looking," << std::endl;
	std::cout << "and scroll to move forward/backward in the direction you are looking." << std::endl << std::endl;
	std::cout << "To enter the settings menu press the 'Home' button." << std::endl;
	std::cout << "To move the currently spawned object use the WASD keys and" << std::endl;
	std::cout << "to rotate the object use the Q and E keys." << std::endl << std::endl;
	std::cout << "To close the program click on the red X or press the escape key." << std::endl;
	std::cout << "--------------------------------------------------------------------------------" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glEnable(GL_MULTISAMPLE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Group Project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 5000.0f);

	// Build and compile our shader program
	// Vertex shader

	// Read the Vertex Shader code from the file
	string vertex_shader_path = "vertex.shader";
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, ios::in);

	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_path = "fragment.shader";
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	loadOBJ("objs/cube.obj", vertices, normals, UVs);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertices_VBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &UVs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, UVs_VBO);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Chair Obj------------------------------------------------
	loadOBJ("objs/chair.obj", chair_vertices, chair_normals, chair_UVs);
	glGenVertexArrays(1, &Chair_VAO);
	glBindVertexArray(Chair_VAO);
	glGenBuffers(1, &Chair_vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Chair_vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, chair_vertices.size() * sizeof(glm::vec3), &chair_vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &Chair_normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Chair_normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, chair_normals.size() * sizeof(glm::vec3), &chair_normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &Chair_UVs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Chair_UVs_VBO);
	glBufferData(GL_ARRAY_BUFFER, chair_UVs.size() * sizeof(glm::vec2), &chair_UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Table Obj------------------------------------------------
	loadOBJ("objs/table.obj", table_vertices, table_normals, table_UVs);
	glGenVertexArrays(1, &Table_VAO);
	glBindVertexArray(Table_VAO);
	glGenBuffers(1, &Table_vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Table_vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, table_vertices.size() * sizeof(glm::vec3), &table_vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &Table_normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Table_normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, table_normals.size() * sizeof(glm::vec3), &table_normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &Table_UVs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Table_UVs_VBO);
	glBufferData(GL_ARRAY_BUFFER, table_UVs.size() * sizeof(glm::vec2), &table_UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Toy Obj------------------------------------------------
	loadOBJ("objs/toy.obj", toy_vertices, toy_normals, toy_UVs);
	glGenVertexArrays(1, &Toy_VAO);
	glBindVertexArray(Toy_VAO);
	glGenBuffers(1, &Toy_vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Toy_vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, toy_vertices.size() * sizeof(glm::vec3), &toy_vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &Toy_normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Toy_normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, toy_normals.size() * sizeof(glm::vec3), &toy_normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &Toy_UVs_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, Toy_UVs_VBO);
	glBufferData(GL_ARRAY_BUFFER, toy_UVs.size() * sizeof(glm::vec2), &toy_UVs.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");
	viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");
	colorLoc = glGetUniformLocation(shaderProgram, "selectedColor");
	useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");
	viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

	glClearColor(0.5f, 0.75f, 0.93f, 1.0f);

	glActiveTexture(GL_TEXTURE0); //select texture unit 0

	//Floor Texture ------------------------------------
	glGenTextures(1, &floor_texture);
	glBindTexture(GL_TEXTURE_2D, floor_texture); //bind this texture to the currently bound texture unit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int texture_width, texture_height;
	unsigned char* image = SOIL_load_image("textures/floor1.jpg", &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image); //free resources

	//Ceiling Texture ------------------------------------
	glGenTextures(1, &ceiling_texture);
	glBindTexture(GL_TEXTURE_2D, ceiling_texture); //bind this texture to the currently bound texture unit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("textures/ceiling1.jpg", &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image); //free resources

	//Wall Texture ------------------------------------
	glGenTextures(1, &wall_texture);
	glBindTexture(GL_TEXTURE_2D, wall_texture); //bind this texture to the currently bound texture unit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("textures/wall1.jpg", &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image); //free resources

	//Chair Texture ------------------------------------
	glGenTextures(1, &chair_texture);
	glBindTexture(GL_TEXTURE_2D, chair_texture); //bind this texture to the currently bound texture unit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("textures/chair.jpg", &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image); //free resources

	//Table Texture ------------------------------------
	glGenTextures(1, &table_texture);
	glBindTexture(GL_TEXTURE_2D, table_texture); //bind this texture to the currently bound texture unit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("textures/table.png", &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image); //free resources

	//Toy Texture ------------------------------------
	glGenTextures(1, &toy_texture);
	glBindTexture(GL_TEXTURE_2D, toy_texture); //bind this texture to the currently bound texture unit
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = SOIL_load_image("textures/toy.png", &texture_width, &texture_height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image); //free resources


	calculateRoomBoundingBox();
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Current Cameras location and translation
		glm::mat4 view_matrix;
		view_matrix = glm::lookAt(camera_translation, camera_translation+camera_rotation, glm::vec3(0.0, 1.0, 0.0));

		//MVP
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		glUniformMatrix4fv(viewPosLoc, 1, GL_FALSE, glm::value_ptr(camera_translation));

		//Draw room and all the objects
		drawRoom();
		drawChair();
		drawTable();
		drawToy();

		// Swap the screen buffers
		glfwSwapBuffers(window);
		mouseCameraControls(window);
	}

	glfwTerminate();
	return 0;
}
