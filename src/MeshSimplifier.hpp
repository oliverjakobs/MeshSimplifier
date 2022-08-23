#pragma once

#include "Mesh.hpp"

#include <map>

struct VertexPair
{
    uint32_t first;
    uint32_t second;

    float error;
    glm::mat4 qMat;
    glm::vec3 middle;

    bool operator==(const VertexPair& other) const
    {
        return (first == other.first && second == other.second)
            || (second == other.first && first == other.second);
    }
};

struct VertexPairHash
{
    size_t operator()(const VertexPair& p) const
    {
        size_t h1 = std::hash<uint32_t>{}(std::min(p.first, p.second));
        size_t h2 = std::hash<uint32_t>{}(std::max(p.first, p.second));
        return h1 ^ (h2 << 1);
    }
};

struct VertexPairComp
{
    bool operator()(const VertexPair& p1, const VertexPair& p2) const
    {
        return (p2.error < p1.error);
    }
};

class MeshSimplifier
{
private:
    std::vector<uint32_t> indices;

    std::vector<glm::vec3> vertices;
    std::vector<glm::mat4> errors;

    std::vector<VertexPair> pairs;

public:
    MeshSimplifier(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);
    ~MeshSimplifier();

    void reload(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);

    void run(size_t targetFaces);
    
    std::vector<glm::vec3> getVertices() const { return vertices; }
    std::vector<uint32_t> getIndices() const { return indices; }

    size_t getVertexCount() const { return vertices.size(); }
    size_t getFaceCount() const { return indices.size() / 3; }

    void printPairs();
    void printFaces();

private:
    void setPairError(VertexPair& edge);
    glm::mat4 getQuadricError(uint32_t vertex);

    void updateIndices(uint32_t newVertex, uint32_t removedVertex);
};