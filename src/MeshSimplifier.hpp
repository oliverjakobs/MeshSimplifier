#pragma once

#include "Mesh.hpp"

#include <map>

struct Edge
{
	unsigned int firstVertex;
	unsigned int secondVertex;
	float edgeError = 0;
	glm::mat4 edgeQ;
	glm::vec3 newPos;

	// Edge struct comperators

	bool operator==(const Edge& other) const
	{
		return (firstVertex == other.firstVertex && secondVertex == other.secondVertex)
			|| (secondVertex == other.firstVertex && firstVertex == other.secondVertex);
	}
	bool operator()(const Edge& first, const Edge& second) const
	{
		return first < second;
	}
	bool operator < (const Edge& other)const
	{
		if (firstVertex < other.firstVertex)
			return true;
		else
		{
			if (firstVertex == other.firstVertex)
				return (secondVertex < other.secondVertex);
			else
				return false;
		}
	}
};

// Edges comperator.
struct compEdgeErr
{
	bool operator()(const struct Edge& e1, const struct Edge& e2) const
	{
		if (e2.edgeError < e1.edgeError)
			return true;
		return false;
	}
};

class MeshSimplification
{
public:
	MeshSimplification(std::vector<glm::vec3> vertices, std::vector<GLuint> indices);
	void start(int targetFaces);
	std::vector<glm::vec3> getVertices();
	std::vector<GLuint> getIndices();
	~MeshSimplification();

private:
	std::vector<Edge> removeDups(std::vector<Edge> toRemove);
	void initEdgeVector();
	void initVertexNeighbor();
	void buildHeap();
	void calcFaces(int firstEdgeInd, int secondEdgeInd);
	void calcEdgeError(struct Edge& e);
	glm::mat4 calcVertexError(int vertexIndex);
	bool isTriangle(int second, int third);
	/*
	void printEdgeVector(vector<Edge> vec);
	void printFaces();
	void printNeighbors();
	*/

	std::vector<Edge> m_edgeVector;
	std::multimap<int, int> m_vertexNeighbor;
	std::vector<glm::mat4> m_errors;
	std::vector<GLuint> m_indices;
	std::vector<glm::vec3> m_vertices;
};