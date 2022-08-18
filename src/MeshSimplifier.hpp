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

    bool operator==(const Edge& other) const
    {
        return (first == other.first && second == other.second)
            || (second == other.first && first == other.second);
    }
};

struct EdgeComperator
{
    bool operator()(const struct Edge& e1, const struct Edge& e2) const
    {
        return (e2.error < e1.error);
    }
};

class MeshSimplifier
{
private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Edge> edges;

    std::vector<glm::mat4> errors;
    std::multimap<uint32_t, uint32_t> vertexNeighbors;

public:
    MeshSimplifier(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
    ~MeshSimplifier();

    void run(size_t targetFaces);
    
    std::vector<Vertex> getVertices() const { return vertices; }
    std::vector<uint32_t> getIndices() const { return indices; }

    size_t getVertexCount() const { return vertices.size(); }
    size_t getFaceCount() const { return indices.size() / 3; }

    void printEdges();
    void printNeighbors();
    void printFaces();

private:
    void createEdges();
    void setEdgeError(Edge& edge);

    bool isFace(uint32_t v1, uint32_t v2);
    glm::mat4 getQuadricError(uint32_t vertex);

    void updateNeighbors(uint32_t newVertex, uint32_t removedVertex);
    void updateFaces(uint32_t newVertex, uint32_t removedVertex);
};