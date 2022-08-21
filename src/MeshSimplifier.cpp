#include "MeshSimplifier.hpp"

#include <algorithm>

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

    edges.clear();
    errors.clear();
    vertexNeighbors.clear();

    // create edges
    createEdges();

    // get all neighbors
    for (auto& e : edges)
    {
        vertexNeighbors.insert({ e.first, e.second });
        vertexNeighbors.insert({ e.second, e.first });
    }

    // calc the Quadric Error for every vertex
    for (int i = 0; i < vertices.size(); i++)
        errors.push_back(getQuadricError(i));

    // set the error for each edge
    for (auto& edge : edges)
        setEdgeError(edge);

    std::make_heap(edges.begin(), edges.end(), EdgeComperator());
}

void MeshSimplifier::run(size_t targetFaces)
{
    while (getFaceCount() > targetFaces)
    {
        // remove the edge with minimal error
        std::pop_heap(edges.begin(), edges.end(), EdgeComperator());
        Edge removedEdge = edges.back();
        edges.pop_back();

        int newVertex = removedEdge.first;
        int removedVertex = removedEdge.second;

        // set the error and position of the new vertex.
        errors[newVertex] = removedEdge.qMat;
        vertices[newVertex] = removedEdge.middle;

        // update data
        updateNeighbors(newVertex, removedVertex);
        updateIndices(newVertex, removedVertex);

        // update heap
        std::make_heap(edges.begin(), edges.end(), EdgeComperator());
    }
}

void MeshSimplifier::createEdges()
{
    for (size_t i = 0; i < indices.size() - 3;)
    {
        uint32_t v0 = indices[i++];
        uint32_t v1 = indices[i++];
        uint32_t v2 = indices[i++];

        edges.push_back({ v0, v1 });
        edges.push_back({ v1, v2 });
        edges.push_back({ v2, v0 });
    }
}

void MeshSimplifier::setEdgeError(Edge& edge)
{
    edge.qMat = errors[edge.first] + errors[edge.second];
    edge.middle = (vertices[edge.first] + vertices[edge.second]) / 2.0f;
    edge.error = glm::dot(glm::vec4(edge.middle, 1.0f), edge.qMat * glm::vec4(edge.middle, 1.0f));
}

bool MeshSimplifier::checkNeighbor(uint32_t v1, uint32_t v2)
{
    auto range = vertexNeighbors.equal_range(v1);
    for (auto it = range.first; it != range.second; it++)
    {
        if (v2 == it->second) return true;
    }
    return false;
}

glm::mat4 MeshSimplifier::getQuadricError(uint32_t vertex)
{
    glm::mat4 qMat(1.0f);

    // Find all the triangles of this vertex and add it's error to it.
    auto range = vertexNeighbors.equal_range(vertex);
    for (auto v1 = range.first; v1 != range.second; ++v1)
    {
        for (auto v2 = std::next(v1); v2 != range.second; ++v2)
        {
            // check if vertex, v1 and v2 form a face
            if (!checkNeighbor(v1->second, v2->second)) continue;

            // Calc cross prod.
            glm::vec3 p1 = vertices[v1->second] - vertices[vertex];
            glm::vec3 p2 = vertices[v2->second] - vertices[vertex];
            glm::vec3 n = glm::cross(p2, p1);
            n = glm::normalize(n);

            glm::vec4 v_tag = glm::vec4(n, -(glm::dot(vertices[vertex], n)));

            // calc error matrix
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    qMat[i][j] += v_tag[i] * v_tag[j];
        }
    }
    return qMat;
}

void MeshSimplifier::updateNeighbors(uint32_t newVertex, uint32_t removedVertex)
{
    // move all of removedVertex's neighbors to newVertex
    auto range = vertexNeighbors.equal_range(removedVertex);
    for (auto it = range.first; it != range.second; ++it)
    {
        // prevent duplicates
        bool neighborToBothVertices = false;

        auto range2 = vertexNeighbors.equal_range(it->second);
        for (auto it2 = range2.first; it2 != range2.second; ++it2)
        {
            if (it2->second == newVertex)
                neighborToBothVertices = true;
        }

        // insert new neighbor
        if (it->second != newVertex && !neighborToBothVertices)
            vertexNeighbors.insert({ it->second, newVertex });
    }

    // erase the removedVertex in the neighbors multimap
    vertexNeighbors.erase(removedVertex);
    for (auto it = vertexNeighbors.begin(); it != vertexNeighbors.end();)
    {
        if (it->second == removedVertex)
            it = vertexNeighbors.erase(it);
        else
            ++it;
    }
}

void MeshSimplifier::updateIndices(uint32_t newVertex, uint32_t removedVertex)
{
    // remove all invalid edges and indices
    for (size_t i = 0; i < indices.size(); )
    {
        uint32_t v0 = indices[i];       // First vertex of the current face.
        uint32_t v1 = indices[i + 1];   // Second vertex of the current face.
        uint32_t v2 = indices[i + 2];   // Third vertex of the current face.

        // If the current face contains the given edge indices.
        if ((newVertex == v0 || newVertex == v1 || newVertex == v2)
            && (removedVertex == v0 || removedVertex == v1 || removedVertex == v2))
        {
            edges.erase(std::remove_if(edges.begin(), edges.end(), [newVertex, v0, v1, v2](auto& edge)
                {
                    return (edge.first != newVertex && edge.second != newVertex)
                        && (edge.first == v0 || edge.first == v1 || edge.first == v2)
                        && (edge.second == v0 || edge.second == v1 || edge.second == v2);
                }), edges.end());

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
    for (auto it = edges.begin(); it != edges.end();)
    {
        if (it->first == it->second)
        {
            it = edges.erase(it);
            continue;
        }

        if (it->second == removedVertex)
            it->second = newVertex;
        else if (it->first == removedVertex)
            it->first = newVertex;

        if (it->first == newVertex || it->second == newVertex)
            setEdgeError(*it);

        ++it;
    }
}


void MeshSimplifier::printEdges()
{
    printf("Edges: (%zd)\n", edges.size());
    for (auto& e : edges)
    {
        printf(" - (%d, %d) error=%f\n", e.first, e.second, e.error);
    }
}

void MeshSimplifier::printNeighbors()
{
    printf("Neighbors:\n");
    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        auto range = vertexNeighbors.equal_range(i);
        if (range.first == range.second) continue;

        printf(" - %d: ", i);
        for (auto it = range.first; it != range.second; ++it)
        {
            printf("%d ", it->second);
        }
        printf("\n");
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
