#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>

#include <vector>
#include <mutex>

#include <cstdint>

#include <glm/vec3.hpp>

#include "Vertex.h"
#include "Texture2D.h"

template <typename VertexT>
class Mesh
{
public:
	Mesh();
	Mesh(std::vector<VertexT>& vertices);

	void CreateVao();

	void Update();
	void UpdateVertices(const std::vector<VertexT>&);
	void SetVertexAttributes();

	void BindVao();
	void BindBuffer(unsigned int buffer);
	void UnbindVao();
	void UnbindBuffer();

	void Clear();

	void Render();
	void Render(int verticesCount);

	void AddVertex(VertexT vertex);
	void SetVertices(std::vector<VertexT> vertices);

	void AddIndex(uint16_t index);
	void SetIndices(std::vector<uint16_t> indices);

	std::vector<VertexT>& GetVertices();
	std::vector<uint16_t>& GetIndices();

	const unsigned int GetVao();

	~Mesh();

public:
	Texture2D* m_Texture = nullptr;

protected:
	std::vector<VertexT> m_Vertices;
	std::vector<uint16_t> m_Indices;

	unsigned int m_Vao = 0;
	unsigned int m_Vbo = 0;
	unsigned int m_Ebo = 0;

private:
	std::mutex m_VerticesMutex;
	std::mutex m_IndicesMutex;
};

template <typename VertexT>
Mesh<VertexT>::Mesh() {}


template <typename VertexT>
Mesh<VertexT>::Mesh(std::vector<VertexT>& vertices)
{
	m_Vertices = vertices;

	CreateVao();
}

template <typename VertexT>
void Mesh<VertexT>::CreateVao()
{
	// Generate vertex array object
	glGenVertexArrays(1, &m_Vao);
}

template <typename VertexT>
void Mesh<VertexT>::Update()
{
	UpdateVertices(m_Vertices);
}

template <typename VertexT>
void Mesh<VertexT>::UpdateVertices(const std::vector<VertexT>& vertices)
{
	// Don't update the vertices if there are none
	if (vertices.empty())
		return;

	m_Vertices = vertices;

	// Generate a vertex array object if none has been created before
	if (m_Vao == 0)
		CreateVao();

	BindVao();

	// Generate a vertex buffer if none has been created before
	if (m_Vbo == 0)
		glGenBuffers(1, &m_Vbo);

	// Generate element array object if none has been created before
	if (m_Ebo == 0)
		glGenBuffers(1, &m_Ebo);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexT), &vertices[0], GL_STATIC_DRAW);

	if (!m_Indices.empty())
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(uint16_t), &m_Indices[0], GL_STATIC_DRAW);
	}

	SetVertexAttributes();

	UnbindBuffer();
	UnbindVao();
}

template <typename VertexT>
void Mesh<VertexT>::SetVertexAttributes()
{
	// Packed data
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(VertexT) /* Number of total bytes */, 0);

	// Texture coordinates
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2 /* x, y */, GL_FLOAT, GL_FALSE, sizeof(Vertex) /* Number of total bytes */, (const void*)offsetof(Vertex, textureCoordinates));
}

template <>
inline void Mesh<BasicVertex>::SetVertexAttributes()
{
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3 /* x, y, z */, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), 0 /* Offset */);
}

template <>
inline void Mesh<ColorVertex>::SetVertexAttributes()
{
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3 /* x, y, z */, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, position));

    // Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4 /* r, g, b, a */, GL_FLOAT, GL_FALSE, sizeof(ColorVertex), (void*)offsetof(ColorVertex, color));
}

template <>
inline void Mesh<TextureVertex>::SetVertexAttributes()
{
	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3 /* x, y, z */, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)offsetof(TextureVertex, position));

    // Color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4 /* r, g, b, a */, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)offsetof(TextureVertex, color));

    // Texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2 /* x, y */, GL_FLOAT, GL_FALSE, sizeof(TextureVertex), (void*)offsetof(TextureVertex, textureCoord));
}

template <typename VertexT>
void Mesh<VertexT>::BindVao() { glBindVertexArray(m_Vao); }

template <typename VertexT>
void Mesh<VertexT>::UnbindVao() { glBindVertexArray(0); }

template <typename VertexT>
void Mesh<VertexT>::BindBuffer(unsigned int buffer) { glBindBuffer(GL_ARRAY_BUFFER, buffer); }

template <typename VertexT>
void Mesh<VertexT>::UnbindBuffer() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

template <typename VertexT>
void Mesh<VertexT>::Clear()
{
	m_Vertices.clear();
	m_Indices.clear();
}

template <typename VertexT>
void Mesh<VertexT>::Render()
{
	//if (!m_Vao || !m_Vbo) return;

	BindVao();
	if (m_Texture) m_Texture->Bind();

	// Try to use the indices, but if they aren't set use the vertices count instead
	if (!m_Indices.empty())
		glDrawElements(GL_TRIANGLES, (GLsizei)m_Indices.size(), GL_UNSIGNED_SHORT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_Vertices.size());

	if (m_Texture) m_Texture->Unbind();
	UnbindVao();
}

template <typename VertexT>
void Mesh<VertexT>::Render(int verticesCount)
{
	BindVao();

	if (m_Texture) m_Texture->Bind();

	glDrawArrays(GL_TRIANGLES, 0, verticesCount);

	if (m_Texture) m_Texture->Unbind();

	UnbindVao();
}

template <typename VertexT>
std::vector<VertexT>& Mesh<VertexT>::GetVertices() { return m_Vertices; }
template <typename VertexT>
std::vector<uint16_t>& Mesh<VertexT>::GetIndices() { return m_Indices; }

template <typename VertexT>
void Mesh<VertexT>::AddVertex(VertexT vertex)
{
	m_VerticesMutex.lock();

	m_Vertices.push_back(vertex);

	m_VerticesMutex.unlock();
}
template <typename VertexT>
void Mesh<VertexT>::SetVertices(std::vector<VertexT> vertices) { m_Vertices = vertices; };

template <typename VertexT>
void Mesh<VertexT>::AddIndex(uint16_t index)
{
	m_IndicesMutex.lock();

	m_Indices.push_back(index);

	m_IndicesMutex.unlock();
};

template <typename VertexT>
void Mesh<VertexT>::SetIndices(std::vector<uint16_t> indices) { m_Indices = indices; };

template <typename VertexT>
const unsigned int Mesh<VertexT>::GetVao() { return m_Vao; }

template <typename VertexT>
Mesh<VertexT>::~Mesh()
{
	glDeleteVertexArrays(1, &m_Vao);

	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ebo);
}

#endif