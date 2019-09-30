#define _CRT_SECURE_NO_WARNINGS
#include "Mesh.h"
#include <vector>
#include <stdio.h>
#include <fstream>
#include <Logging.h>

#define BUFFER_OFFSET(i) ((char*)0 + (i)) //cheers emilian


struct Face
{
	Face()
	{
		for(int i = 0 ; i < 3 ; i++)
		{
			vertices[i] = 0;
			normals[i]  = 0;
			uvs[i]		= 0;
		}
	}

	Face(unsigned int v1, unsigned int v2, unsigned int v3, 
		unsigned int u1, unsigned int u2, unsigned int u3,
		unsigned int n1, unsigned int n2, unsigned int n3)
	{
		vertices[0] = v1;
		 normals[0]	= n1;
		     uvs[0] = u1;

		vertices[1] = v2;
		 normals[1] = n2;
		     uvs[1] = u2;

		vertices[2] = v3;
		 normals[2] = n3;
		     uvs[2] = u3;
	}
	
	unsigned int vertices[3];
	unsigned int normals[3];
	unsigned int uvs[3];
};



Mesh::Mesh(Vertex* vertices, size_t numVerts, uint32_t* faceIndices, size_t numIndices)
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(uint32_t), faceIndices, GL_STATIC_DRAW);
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

Mesh::Mesh(const char* filePath)
{
	loadObj(filePath);
}

Mesh::~Mesh() {
	// Clean up our buffers
	glDeleteBuffers(2, myBuffers);
	// Clean up our VAO
	glDeleteVertexArrays(1, &myVao);
}

bool Mesh::loadObj(const std::string& objPath)
{
	std::ifstream input;
	input.open(objPath);

	if(!input)
	{
		//some kind of error handling
		return false;
	}

	char inputString[128];

	//unique data
	std::vector<glm::vec3> vertexData;
	std::vector<glm::vec3> normalData;
	std::vector<glm::vec2> uvData;
	//index data / face data
	std::vector<Face> faceData;
	//OpenGl ready data
	std::vector<float> unPackedVertexData;
	std::vector<float> unPackedNormalData;
	std::vector<float> unPackedTextureData;

	while(!input.eof())
	{
		input.getline(inputString, 128);
		
		//Comments
		if (std::strstr(inputString,"#") != nullptr)
		{
			continue;
		}
		
		/////////////////////Normal Data////////////////////////
		else if (std::strstr(inputString, "vn") != nullptr)
		{
			glm::vec3 temp;
			
			//using sscanf returns the number of matches in the line to the format we provided
			unsigned int matches = std::sscanf(inputString, "vn %f %f %f", &temp.x, &temp.y, &temp.z);

			//use matches to make sure our data is all loaded properly
			if (matches != 3)
				throw std::runtime_error("can't load normal data");

			//push back our loaded data
			normalData.push_back(temp);
		}
		
		/////////////////////Texture Data////////////////////////
		else if (std::strstr(inputString, "vt") != nullptr)
		{
			glm::vec2 temp;
			
			//using sscanf returns the number of matches in the line to the format we provided
			unsigned int matches = std::sscanf(inputString, "vt %f %f", &temp.x, &temp.y);

			//use matches to make sure our data is all loaded properly
			if(matches != 2)
				throw std::runtime_error("can't load texture data");

			//push back our loaded data
			uvData.push_back(temp);
		}
		
		/////////////////////Vertex Data////////////////////////
		else if(std::strstr(inputString, "v") != nullptr)
		{
			glm::vec3 temp;

			//using sscanf returns the number of matches in the line to the format we provided
			unsigned int matches = std::sscanf(inputString, "v %f %f %f", &temp.x, &temp.y, &temp.z);
			//use matches to make sure our data is all loaded properly
			if (matches != 3)
				throw std::runtime_error("can't load texture data");
			
			//push back our loaded data
			vertexData.push_back(temp);
		}
		
		///////////////////////Face Data/////////////////////////
		else if (std::strstr(inputString, "f") != nullptr)
		{
			Face temp;

			//using sscanf returns the number of matches in the line to the format we provided
			unsigned int matches = std::sscanf(inputString, "f %u/%u/%u %u/%u/%u %u/%u/%u", 
				&temp.vertices[0], &temp.uvs[0], &temp.normals[0], 
				&temp.vertices[1], &temp.uvs[1], &temp.normals[1],
				&temp.vertices[2], &temp.uvs[2], &temp.normals[2]);

			//use matches to make sure our data is all loaded properly
			if (matches != 9)
				throw std::runtime_error("can't load face data");

			//push back our loaded data
			faceData.push_back(temp);
		}

	}
		input.close();

	//unpacking data
	for(int i = 0 ; i <faceData.size(); i++)
	{
		for(int j = 0 ; j < 3 ; j++)
		{
			unPackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].x);
			unPackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].y);
			unPackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].z);

			unPackedTextureData.push_back(uvData[faceData[i].uvs[j] - 1].x);
			unPackedTextureData.push_back(uvData[faceData[i].uvs[j] - 1].y);

			unPackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].x);
			unPackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].y);
			unPackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].z);
			
		}
	}

	numFaces = faceData.size();
	numVertices = numFaces * 3;

	//Send data to openGl
	glGenVertexArrays(1, &myVao);
	
	glGenBuffers(1, &vertexVBO);
	glGenBuffers(1, &textureVBO);
	glGenBuffers(1, &normalVBO);

	glBindVertexArray(myVao);

	glEnableVertexAttribArray(0); //Vertex -> Uvs-> Normals all stored
	glEnableVertexAttribArray(1); //Vertex -> Uvs-> Normals all stored
	glEnableVertexAttribArray(2); //Vertex -> Uvs-> Normals all stored

	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* unPackedVertexData.size(), &unPackedVertexData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, false, sizeof(float) * 3, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* unPackedTextureData.size(), &unPackedTextureData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, false, sizeof(float) * 2, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* unPackedNormalData.size(), &unPackedNormalData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, false, sizeof(float) * 3, BUFFER_OFFSET(0));

	//cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	vertexData.clear();
	uvData.clear();
	normalData.clear();
	faceData.clear();
	unPackedVertexData.clear();
	unPackedTextureData.clear();
	unPackedNormalData.clear();

	return true;
}

void Mesh::unloadObj()
{
	glDeleteBuffers(1, &normalVBO);
	glDeleteBuffers(1, &textureVBO);
	glDeleteBuffers(1, &vertexVBO);

	glDeleteVertexArrays(1, &myVao);

	normalVBO = 0;
	textureVBO = 0;
	vertexVBO = 0;

	myVao = 0;

	numFaces = 0;
	numVertices = 0;
}

unsigned int Mesh::getNumFaces() const
{
	return numFaces;
}

unsigned int Mesh::getNumVertices() const
{
	return numVertices;
}

void Mesh::Draw() {
	// Bind the mesh
	glBindVertexArray(myVao);
	// Draw all of our vertices as triangles, our indexes are unsigned ints (uint32_t)
	//glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glDrawElements(GL_TRIANGLES, myIndexCount, GL_UNSIGNED_INT, nullptr);
}
