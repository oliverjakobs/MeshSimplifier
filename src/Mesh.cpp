#include "Mesh.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices)
{
    ignisGenerateVertexArray(&vao);

    // create vertex buffer
    IgnisBufferElement layout[] = { { GL_FLOAT, 3, GL_FALSE } };
    ignisAddArrayBufferLayout(&vao, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW, 0, layout, 1);

    // create element buffer
    ignisLoadElementBuffer(&vao, indices.data(), indices.size(), GL_DYNAMIC_DRAW);
    reload(indices);
}

Mesh::~Mesh()
{
    ignisDeleteVertexArray(&vao);
}

void Mesh::reload(std::vector<uint32_t> indices)
{
    ignisBufferSubData(&vao.element_buffer, 0, indices.size() * sizeof(uint32_t), indices.data());
    vao.element_count = (GLsizei)indices.size();
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
            std::vector<uint32_t> faceIndices;

            for (size_t i = 0; i < numVertices; ++i)
            {
                uint32_t index = std::stoi(splitString(c[i + 1], '/')[0]) - 1;
                faceIndices.push_back(index);
            }

            uint32_t index0 = faceIndices[0];
            uint32_t index1 = faceIndices[1];

            for (size_t i = 2; i < numVertices; ++i) {
                uint32_t index2 = faceIndices[i];

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
