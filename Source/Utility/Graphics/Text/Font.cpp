#include <skelpch.h>
#include <Resources/ResourceManager.h>
#include <Game/GameManager.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Font.h"

namespace Skel {
	Font::Font()
	{
		textShader = Resources.GetShader("UI");
		textShader->Bind();
		// Update shader
		textShader->SetUniform1i("u_Texture", 0);

		// Configure text VAO/VBO for texture quads
		glGenVertexArrays(1, &textVAO);
		glGenBuffers(1, &textVBO);
		glBindVertexArray(textVAO);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	Font::Font(const char* fontPath)
	{
		Font::Font();
		Load(fontPath);
	}

	Font::~Font()
	{
		//glDeleteProgram(textShader->Program);
	}

	void Font::Load(const char* fontPath)
	{
		// Check if Load was never called or fontPath is null
		if (fontPath == NULL)
		{
			std::cout << "ERROR::FREETYPE: Font Path is null" << std::endl;
			return;
		}
		// First clear the previously loaded Characters
		Characters.clear();
		// FreeType
		FT_Library ft;
		// All functions return a value different than 0 whenever an error occurred
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

		// Load font as face
		FT_Face face;
		if (FT_New_Face(ft, fontPath, 0, &face))
		{
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
			return;
		}

		// Set size to load glyphs as
		FT_Set_Pixel_Sizes(face, 0, 48);

		// Disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Load first 128 characters of ASCII set
		for (GLubyte c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// Generate texture
			GLuint texture;
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
			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Now store character for later use
			Character character = {
				texture,
				Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			Characters.insert(std::pair<GLchar, Character>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		// Destroy FreeType once we're finished
		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}
	// Computes the width and height of given string
	Vector2 Font::GetTextSize(std::string text, float scale)
	{
		Vector2 size(0.0f, 0.0f);

		// Iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];

			size.x += (ch.advance >> 6) * scale;
			size.y = glm::max(size.y, ch.size.y * scale);
		}
		return size;
	}

	void Font::RenderText(std::string text, Vector2 pos, float scale, Vector3& color)
	{
		//Matrix4x4 textProjection = Matrix4x4();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		textShader = Resources.GetShader("Text");
		textShader->Bind();
		Matrix4x4 model(1.0f);
		textShader->SetUniformMat4f("u_Model", model);
		textShader->SetUniformMat4f("u_Projection", Camera::GetOrthographicMatrix());

		textShader->SetUniform1i("u_UseTexture", 1);

		// Disable depth testing when rending text and re-enable when done.
		glDisable(GL_DEPTH_TEST);
		// Activate corresponding render state	
		textShader->SetUniform3f("u_Color", color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(textVAO);

		// Iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];

			GLfloat xpos = pos.x + ch.bearing.x * scale;
			GLfloat ypos = pos.y + (ch.size.y - ch.bearing.y) * scale + (35.0f - ch.size.y) * scale; //Uses hard-coded offset


			GLfloat w = ch.size.x * scale;
			GLfloat h = ch.size.y * scale;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				
				{ xpos + w, ypos,       1.0, 0.0 },
				{ xpos,     ypos,       0.0, 0.0 },
				{ xpos,     ypos + h,   0.0, 1.0 },
				
				{ xpos + w, ypos + h,   1.0, 1.0 },
				{ xpos + w, ypos,       1.0, 0.0 },
				{ xpos,     ypos + h,   0.0, 1.0 }
				
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.textureID);

			// Update shader
			textShader->SetUniform1i("u_Texture", 0);

			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, textVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			pos.x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glEnable(GL_DEPTH_TEST);
	}
}