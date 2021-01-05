#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/type_precision.hpp>

namespace BasicVertices {

	namespace Cube {

		enum Faces {
			Left,
			Right,
			Bottom,
			Top,
			Back,
			Front
		};

		const static glm::u8vec3 TopVertices[4] = {
			glm::u8vec3(0, 1, 1),
			glm::u8vec3(1, 1, 1),
			glm::u8vec3(1, 1, 0),
			glm::u8vec3(0, 1, 0),
		};

		const static glm::u8vec3 BottomVertices[4] = {
			glm::u8vec3(0, 0, 1),
			glm::u8vec3(1, 0, 1),
			glm::u8vec3(1, 0, 0),
			glm::u8vec3(0, 0, 0),
		};

		const static glm::u8vec3 LeftVertices[4] = {
			glm::u8vec3(0, 0, 0),
			glm::u8vec3(0, 0, 1),
			glm::u8vec3(0, 1, 1),
			glm::u8vec3(0, 1, 0),
		};

		const static glm::u8vec3 RightVertices[4] = {
			glm::u8vec3(1, 0, 0),
			glm::u8vec3(1, 0, 1),
			glm::u8vec3(1, 1, 1),
			glm::u8vec3(1, 1, 0),
		};

		const static glm::u8vec3 FrontVertices[4] = {
			glm::u8vec3(0, 0, 1),
			glm::u8vec3(1, 0, 1),
			glm::u8vec3(1, 1, 1),
			glm::u8vec3(0, 1, 1),
		};

		const static glm::u8vec3 BackVertices[4] = {
			glm::u8vec3(0, 0, 0),
			glm::u8vec3(1, 0, 0),
			glm::u8vec3(1, 1, 0),
			glm::u8vec3(0, 1, 0),
		};

		inline const glm::u8vec3* GetFace(int id)
		{
			switch (id)
			{
			case Faces::Left: return LeftVertices;
			case Faces::Right: return RightVertices;
			case Faces::Bottom: return BottomVertices;
			case Faces::Top: return TopVertices;
			case Faces::Back: return BackVertices;
			case Faces::Front: return FrontVertices;
			}

			return nullptr;
		}

		const static uint16_t Indices[6] = {
			0, 1, 2,
			2, 3, 0
		};

		const static glm::vec3 FullCube[36] = {
			glm::vec3(-1.0f, 1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(1.0f, 1.0f, -1.0f),
			glm::vec3(-1.0f, 1.0f, -1.0f),

			glm::vec3(-1.0f, -1.0f, 1.0f),
			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, 1.0f, -1.0f),
			glm::vec3(-1.0f, 1.0f, -1.0f),
			glm::vec3(-1.0f, 1.0f, 1.0f),
			glm::vec3(-1.0f, -1.0f, 1.0f),

			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),

			glm::vec3(-1.0f, -1.0f, 1.0f),
			glm::vec3(-1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, -1.0f, 1.0f),
			glm::vec3(-1.0f, -1.0f, 1.0f),

			glm::vec3(-1.0f, 1.0f, -1.0f),
			glm::vec3(1.0f, 1.0f, -1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(-1.0f, 1.0f, 1.0f),
			glm::vec3(-1.0f, 1.0f, -1.0f),

			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f, 1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f, 1.0f),
			glm::vec3(1.0f, -1.0f, 1.0)
		};
	};

    namespace Quad {
        const static glm::vec3 Face[4] = {
			glm::vec3(-0.5f, -0.5f, 0),
			glm::vec3(0.5f, -0.5f, 0),
			glm::vec3(0.5f, 0.5f, 0),
			glm::vec3(-0.5f, 0.5f, 0),
		};

        const static uint16_t Indices[6] = {
			0, 1, 2,
			2, 3, 0
		};

        const static glm::vec2 TextureCoordinates[4] = {
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
        };

        inline void Construct(glm::vec3 positions[4], float width, float height, float z = 0.0f, bool center = true)
        {
            if (center)
            {
                positions[0] = glm::vec3(-width / 2, -height / 2, z);
                positions[1] = glm::vec3( width / 2, -height / 2, z);
                positions[2] = glm::vec3( width / 2,  height / 2, z);
                positions[3] = glm::vec3(-width / 2,  height / 2, z);
            } else
            {
                positions[0] = glm::vec3(0.0f,  0.0f,   z);
                positions[1] = glm::vec3(width, 0.0f,   z);
                positions[2] = glm::vec3(width, height, z);
                positions[3] = glm::vec3(0.0f,  height, z);
            }

        }
    }
};