#pragma once

#include <string>
#include <vector>

#include <Ignis/Ignis.h>
#include <glm/glm.hpp>

struct HalfEdge
{
    uint32_t vert;
    uint32_t twin;
};

struct MeshData
{
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;

    MeshData(const std::string& filename);
    ~MeshData();
};

class Mesh
{
private:
    IgnisVertexArray vao;

public:
    Mesh(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);
    ~Mesh();

    void reload(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);

    void render();
};


