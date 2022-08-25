#pragma once

#include <string>
#include <vector>

#include <Ignis/Ignis.h>
#include <glm/glm.hpp>

// struct to load an .obj file into two vectors
struct MeshData
{
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;

    MeshData(const std::string& filename);
    ~MeshData();
};

// class for creating an vertex array for a mesh and rendering it
class Mesh
{
private:
    IgnisVertexArray vao;

public:
    Mesh(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);
    ~Mesh();

    // write new data into the buffers (can resize them)
    void recreate(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);

    // render the mesh
    void render();
};


