#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/type_precision.hpp>

#include <cstdint>

struct Vertex
{
	int data;
};

struct PackedVertex
{
	unsigned int data;
};

struct BasicVertex
{
    BasicVertex(glm::vec3 pos) : position(pos) {};
    BasicVertex(glm::vec2 pos) : position(glm::vec3(pos, 0.0f)) {};

	glm::vec3 position = glm::vec3(0);
};

struct ColorVertex
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct TextureVertex
{
    TextureVertex() {};
    TextureVertex(glm::vec2 pos, glm::vec2 texCoord) : position(glm::vec3(pos, 0.0f)), textureCoord(texCoord) {};

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec2 textureCoord = glm::vec2(0.0f, 0.0f);
};

struct PackedVertexData
{
	virtual unsigned int Pack() = 0;

	PackedVertex CreatePackedVertex();
};


struct BlockVertex : public PackedVertexData
{
	glm::u8vec3 position;
	uint8_t index;
	uint8_t texture;
	uint8_t lightLevel;

	unsigned int Pack() override;
	BlockVertex Unpack(unsigned int packedData);
};