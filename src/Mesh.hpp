#pragma once

#include <string>
#include <vector>

#include "VertexStructs.h"

#include <Ignis/VertexArray.h>

class Mesh
{
private:
    IgnisVertexArray vao;

    std::vector<Vertex> vertices;
    std::vector<Triangle> faces;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<Triangle> faces);
    ~Mesh();

    void render();

    static Mesh* loadObj(const std::string& filename);
};


