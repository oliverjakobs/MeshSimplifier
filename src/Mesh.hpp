#pragma once

#include <string>
#include <vector>

#include <Ignis/Ignis.h>
#include <glm/glm.hpp>

struct Vertex
{
	float px;
	float py;
	float pz;
	float nx;
	float ny;
	float nz;
	float u;
	float v;
};

struct Triangle
{
	GLuint i[3];
};

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


