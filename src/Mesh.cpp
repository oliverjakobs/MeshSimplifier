#include "Mesh.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

Mesh::Mesh(std::vector<glm::vec3> positions, std::vector<GLuint> indices)
    : indices(indices)
{
    ignisGenerateVertexArray(&vao);

    for (auto p : positions)
    {
        vertices.push_back({ p.x,p.y,p.z });
    }

    // calc and apply face normals
    for (size_t i = 0; i < indices.size() - 3; i += 3)
    {
        glm::vec3 p0 = positions[indices[i + 0]];
        glm::vec3 p1 = positions[indices[i + 1]];
        glm::vec3 p2 = positions[indices[i + 2]];

        glm::vec3 normal = glm::cross(p1 - p0, p2 - p0);

        if (glm::length(normal) != 0.0f)
            normal = glm::normalize(normal);

        vertices[indices[i + 0]].nx = normal.x;
        vertices[indices[i + 0]].ny = normal.y;
        vertices[indices[i + 0]].nz = normal.z;

        vertices[indices[i + 1]].nx = normal.x;
        vertices[indices[i + 1]].ny = normal.y;
        vertices[indices[i + 1]].nz = normal.z;

        vertices[indices[i + 2]].nx = normal.x;
        vertices[indices[i + 2]].ny = normal.y;
        vertices[indices[i + 2]].nz = normal.z;
    }

    // create vertex buffer
    IgnisBufferElement layout[] =
    {
        { GL_FLOAT, 3, GL_FALSE },
        { GL_FLOAT, 3, GL_FALSE },
    };

    ignisAddArrayBufferLayout(&vao, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW, 0, layout, 2);

    // create element buffer
    ignisLoadElementBuffer(&vao, indices.data(), indices.size(), GL_STATIC_DRAW);
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

MeshData::MeshData(const std::string& filename)
{
    std::ifstream stream;
    stream.open(filename);

    std::string line;
    while (getline(stream, line))
    {
        std::vector<std::string> c = splitString(line, ' ');

        if (c.size() == 0) continue;

        if (c[0].compare("v") == 0)
        {
            vertices.push_back(glm::vec3(std::stof(c[1]), std::stof(c[2]), std::stof(c[3])));
        }
        else if (c[0].compare("f") == 0)
        {
            size_t numVertices = c.size() - 1;
            std::vector<GLuint> faceIndices;

            for (size_t i = 0; i < numVertices; ++i)
            {
                GLuint index = std::stoi(splitString(c[i + 1], '/')[0]) - 1;
                faceIndices.push_back(index);
            }

            GLuint index0 = faceIndices[0];
            GLuint index1 = faceIndices[1];

            for (size_t i = 2; i < numVertices; ++i) {
                GLuint index2 = faceIndices[i];

                indices.push_back(index0);
                indices.push_back(index1);
                indices.push_back(index2);

                index1 = index2;
            }
        }
    }

    printf("Loaded OBJ model \"%s\" ", filename.c_str());
    printf("with %zd vertices and %zd faces.\n", vertices.size(), indices.size() / 3);
}

MeshData::~MeshData()
{
}
