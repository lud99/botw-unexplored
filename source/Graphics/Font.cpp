#include "Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>

#include "BasicVertices.h"

int Font::Load(const std::string& filepath)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("Freetype: Could not init FreeType Library");
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, filepath.c_str(), 0, &face))
    {
        printf("Freetype: Failed to load font");  
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);  

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("Freetype: Failed to load Glyph");
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture, 
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        m_Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Shader
    std::string vertexShaderSource = R"text(
        #version 330 core

        layout (location = 0) in vec3 vertex;
        layout (location = 2) in vec2 textureCoords; // <vec2 pos, vec2 tex>

        out vec2 TexCoords;

        uniform mat4 u_ProjectionMatrix = mat4(1.0);
        uniform mat4 u_ViewMatrix = mat4(1.0);
        uniform mat4 u_ModelMatrix = mat4(1.0);

        uniform vec2 u_Size = vec2(1.0, 1.0);

        void main()
        {
            gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(vertex.xy * u_Size, 0.0, 1.0);
            TexCoords = textureCoords;
        } 
    )text";

    std::string fragmentShaderSource = R"text(
        #version 330 core

        in vec2 TexCoords;
        out vec4 color;

        uniform sampler2D text;
        uniform vec3 textColor;

        void main()
        {    
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = vec4(textColor, 1.0) * sampled;
        } 
    )text";

    m_Shader = ShaderLoader::CreateShaderFromSource(vertexShaderSource, fragmentShaderSource);

    glm::vec3 vertexPositions[4] = {
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.0f)
    };

    uint16_t indices[6] = {
        0, 1, 2, 0, 2, 3
    };

    glm::vec2 textureCoords[4] = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f)
    };

    for (int i = 0; i < 4; i++)
    {
        TextureVertex vertex;
        vertex.position = vertexPositions[i];
        vertex.textureCoord = textureCoords[i];
        m_Mesh.AddVertex(vertex);
    }

    for (int i = 0; i < 6; i++)
        m_Mesh.AddIndex(indices[i]);

    m_Mesh.Update();

    m_Initialized = true;

    printf("Loaded font '%s'\n", filepath.c_str());

    return 1;
}

glm::vec2 Font::RenderText(const std::string& text, glm::vec2 position, float scale, glm::vec3 color, int align)
{
    m_Shader.Bind();

    m_Shader.SetUniform("textColor", color);
    m_Shader.SetUniform("u_ProjectionMatrix", *m_ProjectionMatrix);
    m_Shader.SetUniform("u_ViewMatrix", *m_ViewMatrix);

    glActiveTexture(GL_TEXTURE0);

    glm::vec2 startPos(position.x, position.y);
    glm::vec2 textSize = glm::vec2(0.0f, 0.0f);

    glm::vec2* characterPositions = new glm::vec2[text.length()](); 

    for (unsigned int i = 0; i < text.length(); i++)
    {
        Character ch = m_Characters[text[i]];

        characterPositions[i] = glm::vec2(position.x + ch.Bearing.x * scale, position.y - (ch.Size.y - ch.Bearing.y) * scale);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        position.x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    glm::vec2 endPosition(position.x, position.y);
    textSize.x = endPosition.x - startPos.x;

    // iterate through all characters
    for (unsigned int i = 0; i < text.length(); i++)
    {
        Character ch = m_Characters[text[i]];

        glm::vec2 pos = characterPositions[i];

        if (align == ALIGN_CENTER)
            pos.x -= textSize.x / 2;
        else if (align == ALIGN_RIGHT)
            pos.x -= textSize.x;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        if (h > textSize.y)
            textSize.y = h;

        glm::mat4 modelMatrix(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(pos, 0.0f));

        m_Shader.SetUniform("u_ModelMatrix", modelMatrix);
        m_Shader.SetUniform("u_Size", glm::vec2(w, h));

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        m_Mesh.Render();
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    m_Shader.Unbind();

    delete[] characterPositions;

    return textSize;
}


void Font::RenderText(Text& text, glm::vec2 position, float scale, glm::vec3 color)
{
    m_Shader.Bind();

    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(position, 0.0f));

    m_Shader.SetUniform("u_ModelMatrix", modelMatrix);

    m_Shader.SetUniform("textColor", color);
    m_Shader.SetUniform("u_ProjectionMatrix", *m_ProjectionMatrix);
    m_Shader.SetUniform("u_ViewMatrix", *m_ViewMatrix);
    m_Shader.SetUniform("u_ModelMatrix", modelMatrix);

    glActiveTexture(GL_TEXTURE0);

    text.m_Mesh.Render();

    glBindTexture(GL_TEXTURE_2D, 0);

    m_Shader.Unbind();
}

void Text::Create(const std::string& text)
{
    float xPosition = 0.0f;

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = m_Font->m_Characters[*c];

        float x = xPosition + ch.Bearing.x;
        float y = (ch.Size.y - ch.Bearing.y);

        float w = ch.Size.x;
        float h = ch.Size.y;

        // update VBO for each character
        glm::vec3 vertexPositions[4] = {
            glm::vec3(x,     y + h, 0.0f),
            glm::vec3(x,     y,     0.0f),
            glm::vec3(x + w, y,     0.0f),
            glm::vec3(x + w, y + h, 0.0f)
        };

        glm::vec2 textureCoords[4] = {
            glm::vec2(0.0f, 0.0f),
            glm::vec2(0.0f, 1.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 0.0f)
        };

        uint16_t indices[6] = {
            0, 1, 2, 0, 2, 3
        };

        /*
        uint16_t indices2[6] = {
            4, 5, 6, 4, 6, 7
        };
        */

        for (int i = 0; i < 4; i++)
        {
            TextureVertex vertex;
            vertex.position = vertexPositions[i];
            vertex.textureCoord = textureCoords[i];
            m_Mesh.AddVertex(vertex);
        }

        for (int i = 0; i < 6; i++)
            m_Mesh.AddIndex(indices[i] + m_Mesh.GetVertices().size() - 4); // Index into the correct quad 

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        xPosition += (ch.Advance >> 6); // bitshift by 6 to get value in pixels (2^6 = 64)
    }

    m_Mesh.Update();

    /*
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    */
}