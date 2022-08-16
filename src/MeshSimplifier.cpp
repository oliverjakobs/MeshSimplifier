#include "MeshSimplifier.hpp"

#include <algorithm>

MeshSimplification::MeshSimplification(std::vector<glm::vec3> vertices, std::vector<GLuint> indices)
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

MeshSimplification::~MeshSimplification()
{
}

void MeshSimplification::simplfy(size_t targetFaces)
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
		vertices[newVertex] = removedEdge.middle;

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
		vertexNeighbors.erase(removedVertex);

		// update faces
		updateFaces(newVertex, removedVertex);

		// update edges
		for (auto& edge : edges)
		{
			if (edge.second == removedVertex)
			{
				edge.second = newVertex;
				setEdgeError(edge);
			}
			else if (edge.first == removedVertex)
			{
				edge.first = newVertex;
				setEdgeError(edge);
			}
		}

		// Reduce the faces count.
		currFaces = currFaces - 2;
		std::make_heap(edges.begin(), edges.end(), EdgeComperator());
	}
}

void MeshSimplification::createEdges()
{
	std::vector<Edge> allEdges;
	for (size_t i = 0; i < indices.size() - 3;)
	{
		uint32_t first = indices[i++];
		uint32_t second = indices[i++];
		uint32_t third = indices[i++];

		Edge edge;
		edge.first = first;
		edge.second = second;

		allEdges.push_back(edge);

		edge.first = second;
		edge.second = third;

		allEdges.push_back(edge);

		edge.first = third;
		edge.second = first;

		allEdges.push_back(edge);
	}

	for (size_t i = 0; i < allEdges.size(); i++)
	{
		bool found = false;
		for (size_t j = i + 1; j < allEdges.size() && !found; j++)
		{
			if (allEdges[i] == allEdges[j]) found = true;
		}

		if (!found) edges.push_back(allEdges[i]);
	}
}

void MeshSimplification::getVertexNeighbors()
{
	for (auto& e : edges)
	{
		vertexNeighbors.insert({ e.first, e.second });
		vertexNeighbors.insert({ e.second, e.first });
	}
}

bool MeshSimplification::isFace(uint32_t v1, uint32_t v2)
{
	auto range = vertexNeighbors.equal_range(v1);
	for (auto it = range.first; it != range.second; it++)
	{
		if (v2 == it->second) return true;
	}
	return false;
}

glm::mat4 MeshSimplification::getQuadricError(uint32_t vertex)
{
	glm::mat4 qMat(1.0f);
	auto range = vertexNeighbors.equal_range(vertex);

	// Find all the triangles of this vertex and add it's error to it.
	for (auto v1 = range.first; v1 != range.second; v1++)
	{
		for (auto v2 = v1; v2 != range.second; v2++)
		{
			if (v1->second != v2->second)
			{
				// Up until here we check if the 3 indexes creates a triangle.
				if (isFace(v1->second, v2->second))
				{
					// Calc cross prod.
					glm::vec3 n = glm::cross(vertices[v2->second]-vertices[vertex], vertices[v1->second]-vertices[vertex]);
					n = glm::normalize(n);

					glm::vec4 v_tag = glm::vec4(n, -(glm::dot(vertices[vertex], n)));
					for (int i = 0; i < 4; i++)
					{
						for (int j = 0; j < 4; j++)
						{
							qMat[i][j] += v_tag[i] * v_tag[j];
						}
					}
				}
			}
		}
	}
	return qMat;
}

void MeshSimplification::setEdgeError(Edge& edge)
{
	edge.qMat = errors[edge.first] + errors[edge.second];

	// calc new position in the middle
	glm::vec4 middle = glm::vec4((vertices[edge.first] + vertices[edge.second]) / 2.0f, 1.0);
	edge.middle = glm::vec3(middle);

	edge.error = glm::dot(middle, edge.qMat * middle);
}

void MeshSimplification::updateFaces(uint32_t firstIndex, uint32_t secondIndex)
{
	unsigned int first, second, third;
	for (size_t i = 0; i < indices.size() - 3; i += 3)
	{
		first = indices[i];		// First vertex of the current face.
		second = indices[i + 1];	// Second vertex of the current face.
		third = indices[i + 2];	// Third vertex of the current face.

		// If the current face contains the given edge indices.
		if ((firstIndex == first || firstIndex == second || firstIndex == third)
			&& (secondIndex == first || secondIndex == second || secondIndex == third))
		{
			for (int e = 0; e < edges.size(); )
			{
				unsigned int currEdgeFirst = edges[e].first;
				unsigned int currEdgeSecond = edges[e].second;

				// If the current edge is a part of the current face and is not connected to the firstEdgeInd 
				// then remove it.
				if ((currEdgeFirst != firstIndex && currEdgeSecond != firstIndex)
					&& (currEdgeFirst == first || currEdgeFirst == second || currEdgeFirst == third)
					&& (currEdgeSecond == first || currEdgeSecond == second || currEdgeSecond == third))
				{
					edges.erase(edges.begin() + e);
				}
				else
				{
					e++;
				}
			}

			indices.erase(indices.begin() + i);
			indices.erase(indices.begin() + i + 1);
			indices.erase(indices.begin() + i + 2);
		}
	}

	// Update m_OBJIndices that all indices that were of the removed index and set to the newly combined vertex.
	for (auto it = indices.begin(); it != indices.end(); ++it)
	{
		if (*it == secondIndex)
		{
			*it = firstIndex;
		}
	}
}

// @tbrief Prints vector of Edges.
void MeshSimplification::printEdges()
{
	printf("Edges: (%zd)\n", edges.size());
	for (auto& e : edges)
	{
		printf(" - (%d, %d) error=%f\n", e.first, e.second, e.error);
	}

}

void MeshSimplification::printNeighbors()
{
	printf("Neighbors:\n");

	for (size_t i = 0; i < vertices.size(); ++i)
	{
		printf(" - %zd: ", i);
		auto range = vertexNeighbors.equal_range(i);
		for (auto it = range.first; it != range.second; ++it)
		{
			printf("%d ", it->second);
		}
		printf("\n");
	}
}
