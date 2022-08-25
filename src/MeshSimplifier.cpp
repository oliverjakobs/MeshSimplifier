#include "MeshSimplifier.hpp"

#include <algorithm>
#include <unordered_set>

MeshSimplifier::MeshSimplifier(std::vector<glm::vec3> vertices, std::vector<uint32_t> indices)
{
    setup(vertices, indices);
}

MeshSimplifier::~MeshSimplifier() { }

void MeshSimplifier::setup(std::vector<glm::vec3> v, std::vector<uint32_t> i)
{
    vertices = v;
    indices = i;

    // calc the Quadric Error for every vertex
    errors.clear();
    for (int i = 0; i < vertices.size(); i++)
        errors.push_back(getQuadricError(i));

    // create all valid pairs
    createValidPairs();

    // set the error for each pair
    for (auto& p : pairs)
        setPairCost(p);

    std::make_heap(pairs.begin(), pairs.end(), VertexPairComp());
}

void MeshSimplifier::run(size_t targetFaces)
{
    while (getFaceCount() > targetFaces)
    {
        // get and remove the edge with minimal error
        std::pop_heap(pairs.begin(), pairs.end(), VertexPairComp());
        VertexPair removedPair = pairs.back();
        pairs.pop_back();

        int newVertex = removedPair.first;
        int removedVertex = removedPair.second;

        // set the error and position of the new vertex.
        errors[newVertex] = removedPair.qMat;
        vertices[newVertex] = removedPair.middle;

        // replace removedVertex with newVertex
        removeVertex(newVertex, removedVertex);

        // update heap
        std::make_heap(pairs.begin(), pairs.end(), VertexPairComp());
    }
}

void MeshSimplifier::createValidPairs()
{
    // insert all valid pairs into set to prevent duplicates
    std::unordered_set<VertexPair, VertexPairHash> pairSet;
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        uint32_t v0 = indices[i];
        uint32_t v1 = indices[i + 1];
        uint32_t v2 = indices[i + 2];

        // every edge in the mesh is a valid pair
        pairSet.insert({ v0, v1 });
        pairSet.insert({ v1, v2 });
        pairSet.insert({ v2, v0 });
    }

    // convert set to a vector
    pairs = std::vector<VertexPair>(pairSet.begin(), pairSet.end());
}

void MeshSimplifier::setPairCost(VertexPair& p)
{
    p.qMat = errors[p.first] + errors[p.second];
    p.middle = (vertices[p.first] + vertices[p.second]) / 2.0f;
    p.cost = glm::dot(glm::vec4(p.middle, 1.0f), p.qMat * glm::vec4(p.middle, 1.0f));
}

glm::mat4 MeshSimplifier::getQuadricError(uint32_t vertex)
{
    glm::mat4 qMat(1.0f);

    // find all faces of 'vertex' and add it's error to qMat.
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        uint32_t v0 = indices[i];       // first vertex of the current face
        uint32_t v1 = indices[i + 1];   // second vertex of the current face
        uint32_t v2 = indices[i + 2];   // third vertex of the current face

        // check if vertex is part of the current face or not
        if (vertex != v0 && vertex != v1 && vertex != v2) continue;

        glm::vec3 p1 = vertices[v1] - vertices[v0];
        glm::vec3 p2 = vertices[v2] - vertices[v0];
        glm::vec3 n = glm::normalize(glm::cross(p2, p1));
        glm::vec4 v_tag = glm::vec4(n, -(glm::dot(vertices[vertex], n)));

        // add error to matrix
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                qMat[i][j] += v_tag[i] * v_tag[j];
    }

    return qMat;
}

void MeshSimplifier::removeVertex(uint32_t newVertex, uint32_t removedVertex)
{
    // remove all invalid pairs and and faces
    for (size_t i = 0; i < indices.size(); )
    {
        uint32_t v0 = indices[i];       // first vertex of the current face
        uint32_t v1 = indices[i + 1];   // second vertex of the current face
        uint32_t v2 = indices[i + 2];   // third vertex of the current face

        // check if the current face contains one of the given indices
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

    // change all occurances of removedVertex to newVertex
    for (auto& index : indices)
    {
        if (index == removedVertex)
            index = newVertex;
    }

    for (auto it = pairs.begin(); it != pairs.end(); ++it)
    {
        // if the pair cointains the removedVertex change it to newVertex
        if (it->second == removedVertex)
            it->second = newVertex;
        else if (it->first == removedVertex)
            it->first = newVertex;

        // recalculate pair cost
        if (it->first == newVertex || it->second == newVertex)
            setPairCost(*it);
    }
}


void MeshSimplifier::printPairs()
{
    printf("Pairs: (%zd)\n", pairs.size());
    for (auto& e : pairs)
        printf(" - (%d, %d) error=%f\n", e.first, e.second, e.cost);
}

void MeshSimplifier::printFaces()
{
    printf("Faces: (%zd)\n", indices.size() / 3);
    for (size_t i = 0; i < indices.size(); i += 3)
        printf("[%d, %d, %d]\n", indices[i], indices[i + 1], indices[i + 2]);
}
