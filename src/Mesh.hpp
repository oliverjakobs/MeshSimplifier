#pragma once

#include <string>
#include <vector>

#include <Ignis/Ignis.h>
#include <glm/glm.hpp>

struct Vertex
{
	float px, py, pz;
	float nx, ny, nz;
};

struct HalfEdge
{
    uint32_t vert;
    uint32_t twin;
};

struct MeshData
{
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;

    MeshData(const std::string& filename);
    ~MeshData();
};

class Mesh
{
private:
    IgnisVertexArray vao;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

public:
    Mesh(std::vector<glm::vec3> vertices, std::vector<GLuint> indices);
    ~Mesh();

    void reload(std::vector<Vertex> vertices, std::vector<GLuint> indices);

    void render();

    size_t getVertexCount() const { return vertices.size(); }
    size_t getFaceCount() const   { return indices.size() / 3; }
};


