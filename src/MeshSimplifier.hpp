#pragma once

#include "Mesh.hpp"

#include <map>

struct Edge
{
	uint32_t first;
	uint32_t second;

	float error = 0;
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
		if (e2.error < e1.error) return true;
		return false;
	}
};

class MeshSimplification
{
private:
	std::vector<GLuint> indices;
	std::vector<Edge> edges;

	std::vector<glm::vec3> vertices;
	std::vector<glm::mat4> errors;
	std::multimap<int, int> vertexNeighbors;

public:
	MeshSimplification(std::vector<glm::vec3> vertices, std::vector<GLuint> indices);
	~MeshSimplification();

	void simplfy(size_t targetFaces);
	
	std::vector<glm::vec3> getVertices() const { return vertices; }
	std::vector<GLuint> getIndices() const { return indices; }

	void printEdges();
	void printNeighbors();

private:
	void createEdges();
	void getVertexNeighbors();

	bool isFace(uint32_t v1, uint32_t v2);
	glm::mat4 getQuadricError(uint32_t vertex);

	void setEdgeError(Edge& edge);

	void updateFaces(uint32_t first, uint32_t second);
};