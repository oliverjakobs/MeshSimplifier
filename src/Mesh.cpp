#include "Mesh.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Triangle> faces)
	: vertices(vertices), faces(faces)
{
	ignisGenerateVertexArray(&vao);
	
	IgnisBufferElement layout[] =
	{
		{ GL_FLOAT, 3, GL_FALSE },
		{ GL_FLOAT, 3, GL_FALSE },
		{ GL_FLOAT, 2, GL_FALSE },
	};

	ignisAddArrayBufferLayout(&vao, vertices.size() * 8 * sizeof(float), vertices.data(), GL_STATIC_DRAW, 0, layout, 3);

	size_t element_count = faces.size() * 3;
	GLuint* elements = new GLuint[element_count];
	std::vector<Triangle>::iterator elemIt = faces.begin();

	int ibIndex = 0;
	while (elemIt != faces.end()) {
		Triangle f = (*elemIt);

		elements[ibIndex++] = f.i[0];
		elements[ibIndex++] = f.i[1];
		elements[ibIndex++] = f.i[2];
		elemIt++;
	}

	ignisLoadElementBuffer(&vao, elements, element_count, GL_STATIC_DRAW);

	delete[] elements;
}

Mesh::~Mesh()
{
	ignisDeleteVertexArray(&vao);
}

void Mesh::render()
{
	ignisBindVertexArray(&vao);
	glDrawElements(GL_TRIANGLES, vao.element_count, GL_UNSIGNED_INT, 0);
}

static std::vector<std::string> splitString(std::string& str, char delimiter)
{
	std::vector<std::string> words;
	std::string word;
	std::stringstream stream(str);

	while (getline(stream, word, delimiter))
		words.push_back(word);

	return words;
}

Mesh* Mesh::loadObj(const std::string& filename)
{
	std::ifstream stream;
	stream.open(filename);

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	std::vector<Vertex> vertices;
	std::vector<Triangle> faces;

	std::map<std::string, GLuint> vertexHashMap;

	std::string line;
	while (getline(stream, line))
	{
		std::vector<std::string> c = splitString(line, ' ');

		if (c.size() == 0) continue;

		if (c[0].compare("v") == 0)
		{
			positions.push_back(glm::vec3((float)atof(c[1].c_str()), (float)atof(c[2].c_str()), (float)atof(c[3].c_str())));
		}
		else if (c[0].compare("vn") == 0)
		{
			normals.push_back(glm::vec3((float)atof(c[1].c_str()), (float)atof(c[2].c_str()), (float)atof(c[3].c_str())));
		}
		else if (c[0].compare("vt") == 0)
		{
			texCoords.push_back(glm::vec2((float)atof(c[1].c_str()), (float)atof(c[2].c_str())));
		}
		else if (c[0].compare("f") == 0)
		{
			size_t numVertices = c.size() - 1;
			std::vector<GLuint> indices;

			for (size_t i = 0; i < numVertices; ++i)
			{
				GLuint index;
				std::string vs = c[i + 1];

				// check if the vertex already exists
				std::map<std::string, GLuint>::iterator it = vertexHashMap.find(vs);
				if (it == vertexHashMap.end()) // it's a new vertex
				{
					std::vector<std::string> indices = splitString(vs, '/');
					int pIndex = atoi(indices[0].c_str()) - 1;
					int tIndex = atoi(indices[1].c_str()) - 1;
					int nIndex = atoi(indices[2].c_str()) - 1;

					Vertex vertex = {
						positions[pIndex].x, positions[pIndex].y, positions[pIndex].z,
						normals[nIndex].x,   normals[nIndex].y,   normals[nIndex].z,
						texCoords[tIndex].x, texCoords[tIndex].y
					};

					index = vertices.size();
					vertices.push_back(vertex);
					vertexHashMap[vs] = index;
				}
				else
				{
					index = vertexHashMap[vs];
				}

				indices.push_back(index);
			}

			GLuint index0 = indices[0];
			GLuint index1 = indices[1];

			for (size_t i = 2; i < numVertices; ++i) {
				GLuint index2 = indices[i];
				faces.push_back({ index0, index1, index2 });
				index1 = index2;
			}
		}
	}

	std::cout << "Found " << positions.size() << " vertex positions\n";
	std::cout << "Found " << texCoords.size() << " texture coordinates\n";
	std::cout << "Found " << normals.size() << " normal vectors\n";
	std::cout << "Created " << vertices.size() << " unique vertices\n";
	std::cout << "Created " << faces.size() << " triangular faces\n";

    return new Mesh(vertices, faces);
}