#include "MeshSimplifier.hpp"

#include <algorithm>
#include <unordered_set>

MeshSimplifier::MeshSimplifier(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices)
{
    reload(vertices, indices);
}

MeshSimplifier::~MeshSimplifier()
{
}

void MeshSimplifier::reload(std::vector<glm::vec3> v, std::vector<uint32_t> i)
{
    vertices = v;
    indices = i;

    // calc the Quadric Error for every vertex
    errors.clear();
    for (int i = 0; i < vertices.size(); i++)
        errors.push_back(getQuadricError(i));

    // create all valid pairs
    std::unordered_set<VertexPair, VertexPairHash> pairSet;
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        uint32_t v0 = indices[i];
        uint32_t v1 = indices[i + 1];
        uint32_t v2 = indices[i + 2];

        pairSet.insert({ v0, v1 });
        pairSet.insert({ v1, v2 });
        pairSet.insert({ v2, v0 });
    }

    // convert pairs to a vector
    pairs = std::vector<VertexPair>(pairSet.begin(), pairSet.end());

    // set the error for each pair
    for (auto& p : pairs)
        setPairError(p);

    std::make_heap(pairs.begin(), pairs.end(), VertexPairComp());
}

void MeshSimplifier::run(size_t targetFaces)
{
    while (getFaceCount() > targetFaces)
    {
        // remove the edge with minimal error
        std::pop_heap(pairs.begin(), pairs.end(), VertexPairComp());
        VertexPair removedPair = pairs.back();
        pairs.pop_back();

        int newVertex = removedPair.first;
        int removedVertex = removedPair.second;

        // set the error and position of the new vertex.
        errors[newVertex] = removedPair.qMat;
        vertices[newVertex] = removedPair.middle;

        // update faces and pairs
        updateIndices(newVertex, removedVertex);

        // update heap
        std::make_heap(pairs.begin(), pairs.end(), VertexPairComp());
    }
}

void MeshSimplifier::setPairError(VertexPair& p)
{
    p.qMat = errors[p.first] + errors[p.second];
    p.middle = (vertices[p.first] + vertices[p.second]) / 2.0f;
    p.error = glm::dot(glm::vec4(p.middle, 1.0f), p.qMat * glm::vec4(p.middle, 1.0f));
}

glm::mat4 MeshSimplifier::getQuadricError(uint32_t vertex)
{
    glm::mat4 qMat(1.0f);

    // find all faces of 'vertex' and add it's error to qMat.
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        uint32_t v0 = indices[i];       // first vertex of the current face.
        uint32_t v1 = indices[i + 1];   // second vertex of the current face.
        uint32_t v2 = indices[i + 2];   // third vertex of the current face.

        // vertex is not part of this face
        if (vertex != v0 && vertex != v1 && vertex != v2) continue;

        // Calc cross prod.
        glm::vec3 p1 = vertices[v1] - vertices[v0];
        glm::vec3 p2 = vertices[v2] - vertices[v0];
        glm::vec3 n = glm::normalize(glm::cross(p2, p1));
        glm::vec4 v_tag = glm::vec4(n, -(glm::dot(vertices[vertex], n)));

        // calc error matrix
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                qMat[i][j] += v_tag[i] * v_tag[j];
    }

    return qMat;
}

void MeshSimplifier::updateIndices(uint32_t newVertex, uint32_t removedVertex)
{
    // remove all invalid pairs and and faces
    for (size_t i = 0; i < indices.size(); )
    {
        uint32_t v0 = indices[i];       // First vertex of the current face.
        uint32_t v1 = indices[i + 1];   // Second vertex of the current face.
        uint32_t v2 = indices[i + 2];   // Third vertex of the current face.

        // if the current face contains one of the given indices
        if ((newVertex == v0 || newVertex == v1 || newVertex == v2)
            && (removedVertex == v0 || removedVertex == v1 || removedVertex == v2))
        {
            // remove all pairs with indices in the current face excluding the index of the new vertex
            pairs.erase(std::remove_if(pairs.begin(), pairs.end(), [newVertex, v0, v1, v2](auto& p)
                {
                    return (p.first != newVertex && p.second != newVertex)
                        && (p.first == v0 || p.first == v1 || p.first == v2)
                        && (p.second == v0 || p.second == v1 || p.second == v2);
                }), pairs.end());

            // remove face
            indices.erase(indices.begin() + i);
            indices.erase(indices.begin() + i);
            indices.erase(indices.begin() + i);
        }
        else
        {
            i += 3;
        }
    }

    // Update indices that all indices that were of the removed index and set to the newly combined vertex.
    for (auto& index : indices)
    {
        if (index == removedVertex)
            index = newVertex;
    }

    // Update edges, so any edge that was connected to the removed edge has it's error recalculated 
    // and is only connected to the remaining vertex.
    for (auto it = pairs.begin(); it != pairs.end();)
    {
        if (it->second == removedVertex)
            it->second = newVertex;
        else if (it->first == removedVertex)
            it->first = newVertex;

        // recalculate edge error
        if (it->first == newVertex || it->second == newVertex)
            setPairError(*it);

        ++it;
    }
}


void MeshSimplifier::printPairs()
{
    printf("Pairs: (%zd)\n", pairs.size());
    for (auto& e : pairs)
    {
        printf(" - (%d, %d) error=%f\n", e.first, e.second, e.error);
    }
}

void MeshSimplifier::printFaces()
{
    printf("Faces: (%zd)\n", indices.size() / 3);
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        printf("[%d, %d, %d]\n", indices[i], indices[i + 1], indices[i + 2]);
    }
}
