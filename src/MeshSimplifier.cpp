#include "MeshSimplifier.hpp"

#include <algorithm>

MeshSimplification::MeshSimplification(std::vector<glm::vec3> vertices, std::vector<GLuint> indices)
	: m_vertices(vertices), m_indices(indices)
{
	// Init edgeVector.
	initEdgeVector();

	// Init neighbors multimap.
	initVertexNeighbor();

	for (int i = 0; i < m_vertices.size(); i++)
	{
		m_errors.push_back(calcVertexError(i));
	}
	for (int i = 0; i < m_edgeVector.size(); i++)
	{
		calcEdgeError(m_edgeVector[i]);
	}

	buildHeap();
}

MeshSimplification::~MeshSimplification()
{
}


/**
* @tbrief Check if the 2 given vertices are neigbhors, they are both connected to the same vertex so combine to a triangle.
*
* @param second The index of the possible triangle.
* @param third The index of the possible triangle.
*/
bool MeshSimplification::isTriangle(int second, int third)
{
	//second & third already in first
	auto result = m_vertexNeighbor.equal_range(second);
	for (auto it = result.first; it != result.second; it++)
	{
		if (third == it->second)
		{
			return true;
		}
	}
	return false;
}

/**
* @tbrief Given the indices of the edge being removed, remove the faces that are invalid now.
*
* @param firstEdgeInd The first index of the edge being removed.
* @param secondEdgeInd The second index of the edge being removed.
*/
void MeshSimplification::calcFaces(int firstEdgeInd, int secondEdgeInd)
{
	unsigned int first, second, third;
	for (size_t i = 0; i < m_indices.size() - 3; i += 3)
	{
		first = m_indices[i];		// First vertex of the current face.
		second = m_indices[i + 1];	// Second vertex of the current face.
		third = m_indices[i + 2];	// Third vertex of the current face.

		// If the current face contains the given edge indices.
		if ((firstEdgeInd == first || firstEdgeInd == second || firstEdgeInd == third)
			&& (secondEdgeInd == first || secondEdgeInd == second || secondEdgeInd == third))
		{
			for (int e = 0; e < m_edgeVector.size(); )
			{
				unsigned int currEdgeFirst = m_edgeVector[e].firstVertex;
				unsigned int currEdgeSecond = m_edgeVector[e].secondVertex;

				// If the current edge is a part of the current face and is not connected to the firstEdgeInd 
				// then remove it.
				if ((currEdgeFirst != firstEdgeInd && currEdgeSecond != firstEdgeInd)
					&& (currEdgeFirst == first || currEdgeFirst == second || currEdgeFirst == third)
					&& (currEdgeSecond == first || currEdgeSecond == second || currEdgeSecond == third))
				{
					m_edgeVector.erase(m_edgeVector.begin() + e);
				}
				else
				{
					e++;
				}
			}

			m_indices.erase(m_indices.begin() + i);
			m_indices.erase(m_indices.begin() + i + 1);
			m_indices.erase(m_indices.begin() + i + 2);
		}
	}

	// Update m_OBJIndices that all indices that were of the removed index and set to the newly combined vertex.
	for (auto it = m_indices.begin(); it != m_indices.end(); ++it)
	{
		if (*it == secondEdgeInd)
		{
			*it = firstEdgeInd;
		}
	}
}

// @tbrief Helper function to remove duplicated edges from the EdgeVector.
std::vector<Edge> MeshSimplification::removeDups(std::vector<Edge> toRemove)
{
	unsigned int i;
	unsigned int j;
	std::vector<Edge> noDup;
	bool found = false;
	for (i = 0; i < toRemove.size(); i++)
	{
		found = false;
		for (j = i + 1; j < toRemove.size() && !found; j++)
		{
			if (toRemove[i] == toRemove[j])
				found = true;
		}
		if (!found)
			noDup.push_back(toRemove[i]);
	}
	return noDup;
}

// @tbrief Initialize EdgeVector without duplicate Edges.
void MeshSimplification::initEdgeVector()
{
	int counter = 0;
	unsigned int first;
	unsigned int second;
	unsigned int third;
	for (auto it = m_indices.begin(); it != m_indices.end(); it++)
	{
		if (counter == 0)
			first = *it;
		if (counter == 1)
			second = *it;
		if (counter == 2)
		{
			third = *it;
			counter = 0;
			struct Edge edge1;
			struct Edge edge2;
			struct Edge edge3;

			edge1.firstVertex = first;
			edge1.secondVertex = second;

			if (first > second)
			{
				edge1.firstVertex = second;
				edge1.secondVertex = first;
			}

			edge2.firstVertex = first;
			edge2.secondVertex = third;

			if (first > third)
			{
				edge1.firstVertex = third;
				edge1.secondVertex = first;
			}
			edge3.firstVertex = second;
			edge3.secondVertex = third;

			if (second > third)
			{
				edge1.firstVertex = third;
				edge1.secondVertex = second;
			}
			m_edgeVector.push_back(edge1);
			m_edgeVector.push_back(edge2);
			m_edgeVector.push_back(edge3);
			continue;
		}
		counter++;
	}
	m_edgeVector = removeDups(m_edgeVector);
}

// @tbrief Initialize m_vertexNeighbor multimap.
void  MeshSimplification::initVertexNeighbor()
{
	for (size_t i = 0; i < m_vertices.size(); i++)
	{
		for (size_t j = 0; j < m_edgeVector.size(); j++)
		{
			if (i == m_edgeVector[j].firstVertex)
				m_vertexNeighbor.insert(std::pair<int, int>(i, m_edgeVector[j].secondVertex));
			if (i == m_edgeVector[j].secondVertex)
				m_vertexNeighbor.insert(std::pair<int, int>(i, m_edgeVector[j].firstVertex));
		}
	}
}

// @tbrief calculates the Quadric Error of a vertex
glm::mat4 MeshSimplification::calcVertexError(int vertexIndex)
{
	glm::mat4 qMat(1.0f);
	auto result = m_vertexNeighbor.equal_range(vertexIndex);
	// Find all the triangles of this vertex and add it's error to it.
	for (auto it = result.first; it != result.second; it++)
	{
		for (auto it2 = it; it2 != result.second; it2++)
		{
			if (it2->second != it->second)
			{
				// Up until here we check if the 3 indexes creates a triangle.
				if (isTriangle(it->second, it2->second))
				{
					// Calc cross prod.
					glm::vec3 n = glm::cross(m_vertices[it2->second] - m_vertices[vertexIndex], m_vertices[it->second] - m_vertices[vertexIndex]);
					n = glm::normalize(n);

					glm::vec4 v_tag = glm::vec4(n, -(dot(m_vertices[vertexIndex], n)));
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

void MeshSimplification::buildHeap()
{
	std::make_heap(m_edgeVector.begin(), m_edgeVector.end(), compEdgeErr());
}

// @tbrief Calculate the given error of this edge.
void MeshSimplification::calcEdgeError(struct Edge& e)
{
	e.edgeQ = m_errors[e.firstVertex] + m_errors[e.secondVertex];

	// calc new position in the middle
	glm::vec4 midVec = glm::vec4((m_vertices[e.firstVertex] + m_vertices[e.secondVertex]) / 2.0f, 1.0);
	e.newPos = glm::vec3(midVec);

	// temp is a row vector
	glm::vec4 temp = e.edgeQ * midVec;
	e.edgeError = glm::dot(midVec, temp);
}

//  @tbrief The simplification algorithm.
void MeshSimplification::start(int targetFaces)
{
	int currFaces = m_indices.size() / 3;

	while (currFaces > targetFaces)
	{
		// Get the edge to remove.
		std::pop_heap(m_edgeVector.begin(), m_edgeVector.end(), compEdgeErr());

		Edge removedEdge = m_edgeVector.back();
		int newVertex = removedEdge.firstVertex;
		int removedVertex = removedEdge.secondVertex;

		// Update the errors and position of the newly set vertex.
		m_errors[newVertex] = removedEdge.edgeQ;
		m_vertices[newVertex] = removedEdge.newPos;

		// Get all the neighbors of the vertex we're removing (secondVertexInd).
		// Insert into every one of them the combined vertex (secondVertexInd) unless it already had it.
		// Notice we don't remove the removed vertex from the neighbors since it's not valid to erase while 
		// iterating inside an iterator without creating a copy of the original multimap and this doesn't 
		// effect correctness.
		bool neighborToBothVertices = false;
		auto ret = m_vertexNeighbor.equal_range(removedVertex);
		for (auto it = ret.first; it != ret.second; ++it)
		{
			neighborToBothVertices = false;
			auto neighbor = m_vertexNeighbor.equal_range(it->second);

			for (auto it2 = neighbor.first; it2 != neighbor.second; ++it2)
			{
				// This neighbor contained both firstVertexInd and secondVertexInd, so after the loop don't 
				// add to it the firstVertexInd.
				if (it2->second == newVertex)
				{
					neighborToBothVertices = true;
				}
			}

			if (it->second != newVertex && !neighborToBothVertices)
			{
				m_vertexNeighbor.insert(std::pair<int, int>(it->second, newVertex));
			}
		}

		// Removes the edge with the minimal error.
		m_edgeVector.pop_back();

		// Reduce the faces count.
		currFaces = currFaces - 2;

		// Removing the deleted vertex in the neighbors multimap.
		m_vertexNeighbor.erase(removedVertex);

		// Calculate new faces now without the removed edge.
		calcFaces(newVertex, removedVertex);

		// Updating m_edgeVector, so any edge that was connected to the removed edge has it's error 
		// recalculated and is only connected to the remaining vertex.
		for (int i = 0; i < m_edgeVector.size(); i++)
		{
			if (m_edgeVector[i].secondVertex == removedVertex)
			{
				m_edgeVector[i].secondVertex = newVertex;
			}

			else if (m_edgeVector[i].firstVertex == removedVertex)
			{
				m_edgeVector[i].firstVertex = newVertex;
			}

			if (m_edgeVector[i].firstVertex == newVertex || m_edgeVector[i].secondVertex == newVertex)
			{
				calcEdgeError(m_edgeVector[i]);
			}
		}

		// Rebuild the heap with the updated edge errors.
		buildHeap();
	}
}

std::vector<glm::vec3> MeshSimplification::getVertices()
{
	return m_vertices;
}

std::vector<GLuint> MeshSimplification::getIndices()
{
	return m_indices;
}