#include "Font.h"

#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>

#include "../Log.h"
#include "BasicVertices.h"

size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

int Font::Load(const std::string& filepath)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        Log("Freetype: Could not init FreeType Library");
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, filepath.c_str(), 0, &face))
    {
        Log("Freetype: Failed to load font");  
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);  

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
    for (unsigned char c = 0; c < 128; c++)
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            Log("Freetype: Failed to load Glyph");
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
            (unsigned int)face->glyph->advance.x
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
        uniform vec3 textColor = vec3(1.0, 1.0, 1.0);

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

    // Set settings for the optimized mesh
    int maxCountOfEachChar = 300; // Might need to be adjusted
    m_CharMesh.m_UseDynamicBuffer = true;
    m_CharMesh.m_DynamicBufferSize = sizeof(TextureVertex) * 4 * maxCountOfEachChar;
    m_CharMesh.CreateEmptyBuffer();

    m_Initialized = true;

    Log("Loaded font", filepath.c_str());

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

void Font::BeginBatch()
{
    m_CharsToRender.clear();
}

glm::vec2 Font::AddTextToBatch(const std::string& text, glm::vec2 position, float scale, glm::vec3 color, int align, float maxWidth)
{
    glm::vec2 itPosition(position);
    glm::vec2 startPosition(position);
    glm::vec2 textSize;

    // Split the string into words
    std::vector<std::string> words;
    split(text, words, ' ');

    std::vector<glm::vec2> charPositions;
    for (unsigned int w = 0; w < words.size(); w++)
    {
        std::string& word = words[w];

        glm::vec2 itPositionWord = itPosition;

        // Calculate the position of the last character in the word to be able to do word wrap
        for (unsigned int c = 0; c < word.length() + 1; c++)
        {
            // Because the spaces have been removed, we need to iterate an extra time to add it
            Character ch = m_Characters[word[c]];
            if (c == word.length())
                ch = m_Characters[' '];
        
            itPositionWord.x += (ch.Advance >> 6) * scale;
        }

        float wordRightX = itPositionWord.x + m_Characters[word.back()].Bearing.x * scale;

        for (unsigned int c = 0; c < word.length() + 1; c++)
        {
            // Because the spaces have been removed, we need to iterate an extra time to add it
            Character ch = m_Characters[word[c]];
            if (c == word.length())
                ch = m_Characters[' '];

            // Do word wrap
            if (maxWidth > 0.0f)
            {
                // If over the width and a word wrap hasn't happened to this line yet
                if (wordRightX > startPosition.x + maxWidth && itPositionWord.y == itPosition.y)
                {
                    itPosition.x = startPosition.x;
                    itPosition.y -= 50.0f * scale; 
                }
            }

            // Calculate the position of each character
            glm::vec2 position(itPosition.x + ch.Bearing.x * scale, itPosition.y - (ch.Size.y - ch.Bearing.y) * scale);

            charPositions.push_back(position);

            // The text height should be equal to the highest character;
            float h = ch.Size.y * scale;
            if (h > textSize.y)
                textSize.y = h;

            // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            itPosition.x += (ch.Advance >> 6) * scale;
        }
    }
    textSize = glm::abs(itPosition - startPosition);

    // Add the characters to the map
    for (unsigned int i = 0; i < text.length(); i++)
    {
        char c = text[i];

        // Calculate the position of each character
        glm::vec2 charPosition = charPositions[i];

        if (align == ALIGN_CENTER)
            charPosition.x -= textSize.x / 2;
        else if (align == ALIGN_RIGHT)
            charPosition.x -= textSize.x;

        // Add the text to the vector of char
        m_CharsToRender[c].emplace_back(c, charPosition, scale, color, align);
    }

    return textSize;
}
void Font::RenderBatch()
{
    glm::vec2 textureCoords[4] = {
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f)
    };

    m_Shader.Bind();

    m_Shader.SetUniform("u_ProjectionMatrix", *m_ProjectionMatrix);
    m_Shader.SetUniform("u_ViewMatrix", *m_ViewMatrix);

    for (auto &entry : m_CharsToRender)
    {
        std::vector<Text>& texts = entry.second;

        Character ch = m_Characters[entry.first];

        m_CharMesh.Clear();

        // Iterate through all texts
        for (unsigned int i = 0; i < texts.size(); i++)
        {
            Text text = texts[i];

            float w = ch.Size.x * text.scale;
            float h = ch.Size.y * text.scale;

            // Construct all the verticies
            glm::vec3 vertexPositions[4];
            BasicVertices::Quad::ConstructFromBottomleft(vertexPositions, text.position, w, h);

            for (int i = 0; i < 4; i++)
            {
                TextureVertex vertex;
                vertex.position = vertexPositions[i];

                vertex.textureCoord = textureCoords[i];
                m_CharMesh.AddVertex(vertex);
            }

            for (int i = 0; i < 6; i++)
                m_CharMesh.AddIndex(BasicVertices::Quad::Indices[i] + m_CharMesh.GetVertices().size() - 4);
        }

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Set to standard values. Will create weird rendering otherwise
        m_Shader.SetUniform("u_ModelMatrix", glm::mat4(1.0f));
        m_Shader.SetUniform("u_Size", glm::vec2(1.0f, 1.0f));
        m_Shader.SetUniform("textColor", texts[0].color);

        m_CharMesh.Update();
        m_CharMesh.Render();
    }

    m_CharsToRender.clear();

    m_Shader.Unbind();
}

Font::~Font()
{
    m_Shader.Delete();

    // Delete character textures
    for (unsigned char c = 0; c < 128; c++)
    {
        glDeleteTextures(1, &m_Characters[c].TextureID);
    }   
}