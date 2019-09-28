#include "Mesh.h"
#include <vector>
#include <stdio.h>
#include <fstream>
#include <Logging.h>

Mesh::Mesh(Vertex* vertices, size_t numVerts, uint32_t* indices, size_t numIndices)
{
	myIndexCount = numIndices;
	myVertexCount = numVerts;
	// Create and bind our vertex array
	glCreateVertexArrays(1, &myVao);
	glBindVertexArray(myVao);
	// Create 2 buffers, 1 for vertices and the other for indices
	glCreateBuffers(2, myBuffers);
	// Bind and buffer our vertex data
	glBindBuffer(GL_ARRAY_BUFFER, myBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	// Bind and buffer our index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBuffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	// Get a null vertex to get member offsets from
	Vertex* vert = nullptr;

	// Enable vertex attribute 0
	glEnableVertexAttribArray(0);
	// Our first attribute is 3 floats, the distance between
	// them is the size of our vertex, and they will map to the position in our vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), &(vert->Position));
	// Enable vertex attribute 1
	glEnableVertexAttribArray(1);
	// Our second attribute is 4 floats, the distance between
	// them is the size of our vertex, and they will map to the color in our vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex), &(vert->Color));
	// Unbind our VAO
	glBindVertexArray(0);
}

Mesh::Mesh(const char* objFilePath)
{
	//temporary data locations
	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> tempVerts, tempNormals;
	std::vector<glm::vec2> tempUvs;

	//opening the file
	FILE* file = fopen(objFilePath, "r");

	//making sure the file opened
	if(!file)
	{
		throw std::runtime_error("Unable to open file");
	}
	
	//running through the first word of each line to check for v, vt, and vn
	while(true)
	{
		//first word, assumed to be less than 128 characters
		char first[128];
		
		//scanning until End Of File
		unsigned int i =  fscanf(file, "%s", first);
		if (i == EOF)
			break;
		else
		{
			//comparing what was read to "v" to see if we should read vertex data
			if(strcmp (first, "v") == 0)
			{
				//reading vertex data into a vector that we store in the temporary container we made earlier
				glm::vec3 vertex;
				unsigned int dataMatch = fscanf(file, "%f %f %f \n", &vertex.x, &vertex.y, &vertex.z);
				tempVerts.push_back(vertex);

				//quick error check
				if (dataMatch != 9)
					throw std::runtime_error("Unable to read vertex data");
			}

			//comparing what was read to "vt" to see if we should read uv data
			if(strcmp (first, "vt") == 0)
			{
				//reading uv data into a vector that we store in the temporary container we made earlier
				glm::vec2 uv;
				unsigned int dataMatch = fscanf(file, "%f %f\n", &uv.x, &uv.y);
				tempUvs.push_back(uv);
				
				//quick error check
				if (dataMatch != 2)
					throw std::runtime_error("Unable to read uv data");
			}

			//comparing what was read to "vn" to see if we should read normal data
			if(strcmp (first, "vn") == 0)
			{
				//reading normal data into a vector that we store in the temporary container we made earlier
				glm::vec3 normal;
				unsigned int dataMatch = fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				tempNormals.push_back(normal);
				
				//quick error check
				if (dataMatch != 3)
					throw std::runtime_error("Unable to read normal data");
			}

			//comparing what was read to "f" to see if we should read face data
			if (strcmp(first, "f") == 0)
			{
				//read in the data that makes each vertex and group them into tri's
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				//vI/vtI/vnI
				int dataMatch = fscanf(file, "%d %d %d  %d %d %d  %d %d %d\n",  &vertexIndex[0], &uvIndex[0], &normalIndex[0], 
																&vertexIndex[1], &uvIndex[1], &normalIndex[1], 
																&vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				//quick error check
				if(dataMatch != 9)
					throw std::runtime_error("Unable to read face data, check to make sure faces are triangulated");
				
				for(int i = 0; i < 3 ; i++)
				{
					vertexIndices.push_back(vertexIndex[i]);
					uvIndices.push_back(uvIndex[i]);
					normalIndices.push_back(normalIndex[i]);
				}
				
			}
		}
	}
	
	
	// Create and bind our vertex array
	glCreateVertexArrays(1, &myVao);
	glBindVertexArray(myVao);
	// Create 2 buffers, 1 for vertices and the other for indices
	glCreateBuffers(2, myBuffers);
	// Bind and buffer our vertex data
	glBindBuffer(GL_ARRAY_BUFFER, myBuffers[0]);
	//glBufferData(GL_ARRAY_BUFFER, numVerts * sizeof(Vertex), vertices, GL_STATIC_DRAW);
	// Bind and buffer our index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBuffers[1]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	// Get a null vertex to get member offsets from
	Vertex* vert = nullptr;

	// Enable vertex attribute 0
	glEnableVertexAttribArray(0);
	// Our first attribute is 3 floats, the distance between
	// them is the size of our vertex, and they will map to the position in our vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), &(vert->Position));
	// Enable vertex attribute 1
	glEnableVertexAttribArray(1);
	// Our second attribute is 4 floats, the distance between
	// them is the size of our vertex, and they will map to the color in our vertices
	glVertexAttribPointer(1, 4, GL_FLOAT, false, sizeof(Vertex), &(vert->Color));
	// Unbind our VAO
	glBindVertexArray(0);
}

Mesh::~Mesh() {
	// Clean up our buffers
	glDeleteBuffers(2, myBuffers);
	// Clean up our VAO
	glDeleteVertexArrays(1, &myVao);
}

void Mesh::Draw() {
	// Bind the mesh
	glBindVertexArray(myVao);
	// Draw all of our vertices as triangles, our indexes are unsigned ints (uint32_t)
	glDrawElements(GL_TRIANGLES, myIndexCount, GL_UNSIGNED_INT, nullptr);
}
