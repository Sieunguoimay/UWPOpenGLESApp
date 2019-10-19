#pragma once
#include<string>
#include<GLES3/gl3.h>
namespace Managers {

class Shaders
{
	GLuint LoadShader(GLenum type, const char* filename);
	//std::map<std::string, GLint>m_variableLocations;
	int m_numAttributes;
	void setLocationVariables();
public:
	GLuint program, vertexShader, fragmentShader;
	char fileVS[260];
	char fileFS[260];
	bool Init(std::string fileVertexShader, std::string fileFragmentShader);
	void LinkProgram();


	void UseProgram();
	void UnuseProgram();

	Shaders();
	~Shaders();
	void AddAttribute(const std::string& attributeName);

	GLint a_posL;
	GLint a_norm;
	GLint a_texcoord;

	GLint u_ortho;
	GLint u_model;
	GLint u_view;
	GLint u_proj;
	GLint u_camZ;
	GLint u_texSize;
	GLint u_spriteOffset;
	GLint u_alpha;
	GLint u_texture0;
	GLint u_shininess;
	GLint u_lightNum;
	GLint u_lights[4][6];

};
}
