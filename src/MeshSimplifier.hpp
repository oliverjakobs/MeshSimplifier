#pragma once

#include "Mesh.hpp"

#include <map>

struct Edge
{
    uint32_t first;
    uint32_t second;

    float error;
    glm::mat4 qMat;
    glm::vec3 middle;
};

struct EdgeComperator
{
    bool operator()(const Edge& e1, const Edge& e2) const
    {
        return (e2.error < e1.error);
    }
};

class MeshSimplifier
{
private:
    std::vector<uint32_t> indices;

    std::vector<glm::vec3> vertices;
    std::vector<glm::mat4> errors;
    std::vector<std::vector<uint32_t>> neighbors;

    std::vector<Edge> edges;

public:
    MeshSimplifier(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);
    ~MeshSimplifier();

    void reload(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);

    void run(size_t targetFaces);
    
    std::vector<glm::vec3> getVertices() const { return vertices; }
    std::vector<uint32_t> getIndices() const { return indices; }

    size_t getVertexCount() const { return vertices.size(); }
    size_t getFaceCount() const { return indices.size() / 3; }

    void printEdges();
    void printFaces();

private:
    void setEdgeError(Edge& edge);
    glm::mat4 getQuadricError(uint32_t vertex);

    void updateIndices(uint32_t newVertex, uint32_t removedVertex);
};