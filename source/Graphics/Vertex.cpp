#include "Vertex.h"

PackedVertex PackedVertexData::CreatePackedVertex()
{
	PackedVertex vertex;
	vertex.data = Pack();

	return vertex;
}

unsigned int BlockVertex::Pack()
{
	unsigned int packedData = 0;

	packedData = (packedData << 5) | position.x;
	packedData = (packedData << 9) | position.y;
	packedData = (packedData << 5) | position.z;

	packedData = (packedData << 2) | index;
	packedData = (packedData << 6) | texture;
	packedData = (packedData << 4) | lightLevel;

	return packedData;
}

BlockVertex BlockVertex::Unpack(unsigned int packedData)
{
	BlockVertex vertex;

	vertex.position.x = (packedData >> 26)&((1 << 5) - 1);
	vertex.position.y = (packedData >> 17)&((1 << 9) - 1);
	vertex.position.z = (packedData >> 12)&((1 << 5) - 1);

	vertex.index = (packedData >> 10)&((1 << 2) - 1);
	vertex.texture = (packedData >> 4)&((1 << 6) - 1);
	vertex.lightLevel = (packedData >> 0)&((1 << 4) - 1);

	return vertex;
}