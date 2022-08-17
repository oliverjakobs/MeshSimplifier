#pragma once

#include <string>
#include <vector>

#include <Ignis/Ignis.h>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
};

struct HalfEdge
{
    uint32_t vert;
    uint32_t twin;
};

struct MeshData
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    MeshData(const std::string& filename);
    ~MeshData();
};

class Mesh
{
private:
    IgnisVertexArray vao;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices);
    ~Mesh();

    void reload(std::vector<Vertex> vertices, std::vector<GLuint> indices);

    void render();
};


