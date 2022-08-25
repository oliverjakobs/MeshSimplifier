#pragma once

#include "Mesh.hpp"

#include <map>

struct VertexPair
{
    uint32_t first;
    uint32_t second;

    float cost;
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
        return (p2.cost < p1.cost);
    }
};

class MeshSimplifier
{
private:
    std::vector<uint32_t> indices;

    // vertex data
    std::vector<glm::vec3> vertices;
    std::vector<glm::mat4> errors;

    // min heap of all valid pairs
    std::vector<VertexPair> pairs;

public:
    MeshSimplifier(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);
    ~MeshSimplifier();

    // prepare the algorithm (calculate errors and create pairs)
    void setup(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices);

    // run the algorithm until the face count is less than or equal to targetFaces
    void run(size_t targetFaces);
    
    std::vector<glm::vec3> getVertices() const { return vertices; }
    std::vector<uint32_t> getIndices() const { return indices; }

    size_t getVertexCount() const { return vertices.size(); }
    size_t getFaceCount() const { return indices.size() / 3; }

    // debug print functions
    void printPairs();
    void printFaces();

private:
    // create all valid pairs
    void createValidPairs();

    // set the cost, qMat and middle for the edge
    void setPairCost(VertexPair& edge);

    // calculate the quadric error matrix for the given vertex
    glm::mat4 getQuadricError(uint32_t vertex);

    // remove removedVertex (or replace it with newVertex) and repair the mesh afterwards
    void removeVertex(uint32_t newVertex, uint32_t removedVertex);
};