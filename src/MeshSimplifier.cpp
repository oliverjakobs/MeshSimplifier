#include "MeshSimplifier.hpp"

#include <algorithm>

MeshSimplifier::MeshSimplifier(std::vector<Vertex> vertices, std::vector<GLuint> indices)
    : vertices(vertices), indices(indices)
{
    createEdges();
    getVertexNeighbors();

    for (int i = 0; i < vertices.size(); i++)
        errors.push_back(getQuadricError(i));

    for (auto& edge : edges)
        setEdgeError(edge);

    std::make_heap(edges.begin(), edges.end(), EdgeComperator());
}

MeshSimplifier::~MeshSimplifier()
{
}

void MeshSimplifier::run(size_t targetFaces)
{
    size_t currFaces = indices.size() / 3;
    while (currFaces > targetFaces)
    {
        // remove the edge with the minimal error.
        std::pop_heap(edges.begin(), edges.end(), EdgeComperator());
        Edge removedEdge = edges.back();
        edges.pop_back();

        int newVertex = removedEdge.first;
        int removedVertex = removedEdge.second;

        // Update the error and position of the new vertex.
        errors[newVertex] = removedEdge.qMat;
        vertices[newVertex].position = removedEdge.middle;

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

            if (it->second != newVertex && !neighborToBothVertices)
                vertexNeighbors.insert({ it->second, newVertex });
        }

        // erase the removedVertex in the neighbors multimap.
        removeNeighbor(removedVertex);

        // update faces
        updateFaces(newVertex, removedVertex);

        // Update edges, so any edge that was connected to the removed edge has it's error recalculated 
        // and is only connected to the remaining vertex.
        for (auto& edge : edges)
        {
            if (edge.second == removedVertex)
                edge.second = newVertex;
            else if (edge.first == removedVertex)
                edge.first = newVertex;

            if (edge.first == newVertex || edge.second == newVertex)
                setEdgeError(edge);
        }
        
        // Reduce the faces count.
        currFaces = currFaces - 2;
        std::make_heap(edges.begin(), edges.end(), EdgeComperator());
    }
}

void MeshSimplifier::createEdges()
{
    std::vector<Edge> allEdges;
    for (size_t i = 0; i < indices.size() - 3;)
    {
        uint32_t first = indices[i++];
        uint32_t second = indices[i++];
        uint32_t third = indices[i++];

        allEdges.push_back({ first, second });
        allEdges.push_back({ second, third });
        allEdges.push_back({ third, first });
    }

    // remove duplicates
    for (size_t i = 0; i < allEdges.size(); ++i)
    {
        bool found = false;
        for (size_t j = i + 1; j < allEdges.size() && !found; ++j)
        {
            if (allEdges[i] == allEdges[j])
                found = true;
        }

        if (!found) edges.push_back(allEdges[i]);
    }
}

void MeshSimplifier::getVertexNeighbors()
{
    for (auto& e : edges)
    {
        vertexNeighbors.insert({ e.first, e.second });
        vertexNeighbors.insert({ e.second, e.first });
    }
}

void MeshSimplifier::removeNeighbor(uint32_t neighbor)
{
    vertexNeighbors.erase(neighbor);
    for (auto iter = vertexNeighbors.begin(); iter != vertexNeighbors.end();)
    {
        if (iter->second == neighbor)
            iter = vertexNeighbors.erase(iter);
        else
            ++iter;
    }
}

bool MeshSimplifier::isFace(uint32_t v1, uint32_t v2)
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
    auto range = vertexNeighbors.equal_range(vertex);

    // Find all the triangles of this vertex and add it's error to it.
    for (auto v1 = range.first; v1 != range.second; ++v1)
    {
        for (auto v2 = v1; v2 != range.second; ++v2)
        {
            if (v1->second != v2->second)
            {
                // Up until here we check if the 3 indexes creates a triangle.
                if (isFace(v1->second, v2->second))
                {
                    // Calc cross prod.
                    glm::vec3 p1 = vertices[v1->second].position - vertices[vertex].position;
                    glm::vec3 p2 = vertices[v2->second].position - vertices[vertex].position;
                    glm::vec3 n = glm::cross(p2, p1);
                    n = glm::normalize(n);

                    glm::vec4 v_tag = glm::vec4(n, -(glm::dot(vertices[vertex].position, n)));
                    for (int i = 0; i < 4; ++i)
                    {
                        for (int j = 0; j < 4; ++j)
                            qMat[i][j] += v_tag[i] * v_tag[j];
                    }
                }
            }
        }
    }
    return qMat;
}

void MeshSimplifier::setEdgeError(Edge& edge)
{
    edge.qMat = errors[edge.first] + errors[edge.second];

    // calc new position in the middle
    edge.middle = (vertices[edge.first].position + vertices[edge.second].position) / 2.0f;

    edge.error = glm::dot(glm::vec4(edge.middle, 1.0f), edge.qMat * glm::vec4(edge.middle, 1.0f));
}

void MeshSimplifier::updateFaces(uint32_t firstIndex, uint32_t secondIndex)
{
    for (size_t i = 0; i < indices.size(); )
    {
        uint32_t v0 = indices[i];		// First vertex of the current face.
        uint32_t v1 = indices[i + 1];	// Second vertex of the current face.
        uint32_t v2 = indices[i + 2];	// Third vertex of the current face.

        // If the current face contains the given edge indices.
        if ((firstIndex == v0 || firstIndex == v1 || firstIndex == v2)
            && (secondIndex == v0 || secondIndex == v1 || secondIndex == v2))
        {
            edges.erase(std::remove_if(edges.begin(), edges.end(), [firstIndex, v0, v1, v2](auto& edge)
                {
                    return (edge.first != firstIndex && edge.second != firstIndex)
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
        if (index == secondIndex)
            index = firstIndex;
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
    for (uint32_t i = 0; i < indices.size(); i += 3)
    {
        printf("[%d, %d, %d]\n", indices[i], indices[i + 1], indices[i + 2]);
    }
}
