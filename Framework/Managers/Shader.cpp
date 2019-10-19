#include"Shader.h"
#include"../Utils/Utils.h"

Managers::Shaders::Shaders()
	:m_numAttributes(0)
{
}

Managers::Shaders::~Shaders()
{
	glDeleteProgram(program);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Managers::Shaders::AddAttribute(const std::string& attributeName)
{
	glBindAttribLocation(program, m_numAttributes++, attributeName.c_str());
}

bool Managers::Shaders::Init(std::string fileVertexShader, std::string fileFragmentShader)
{
	vertexShader = LoadShader(GL_VERTEX_SHADER, fileVertexShader.c_str());

	if (vertexShader == 0) {
		return false;
	}

	fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fileFragmentShader.c_str());

	if (fragmentShader == 0)
	{
		glDeleteShader(vertexShader);
		return false;
	}


	LinkProgram();




	fileFragmentShader.erase(fileFragmentShader.find('.'), fileFragmentShader.size() - 1);
	fileFragmentShader.erase(0, fileFragmentShader.find_last_of('/') + 1);
	//SDL_Log("Init Shader %s successfully", fileFragmentShader.c_str());
	return true;
}
void Managers::Shaders::UseProgram()
{
	glUseProgram(this->program);
	for (int i = 0; i < m_numAttributes; i++) {
		glEnableVertexAttribArray(i);
	}
}

void Managers::Shaders::UnuseProgram()
{
	glUseProgram(0);
	for (int i = 0; i < m_numAttributes; i++) {
		glDisableVertexAttribArray(i);
	}
}

GLuint Managers::Shaders::LoadShader(GLenum type, const char* filename)
{
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader(type);

	if (shader == 0)
		return 0;
	char* v_data;
	Utils::GetInstance()->loadFile(v_data, filename);

	//glShaderSource(shader, 1, (const char **)&data, NULL);
	glShaderSource(shader, 1, (const char**)(&v_data), NULL);
	free(v_data);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint infoLen = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char* infoLog = new char[infoLen];


			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			//SDL_Log("Error compiling shader:\n%s\n", infoLog);

			delete[] infoLog;
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

void Managers::Shaders::setLocationVariables()
{


	//finding location of uniforms / attributes
	a_posL = glGetAttribLocation(program, "a_posL");
	a_norm = glGetAttribLocation(program, "a_norm");
	a_texcoord = glGetAttribLocation(program, "a_texcoord");

	u_ortho = glGetUniformLocation(program, "u_ortho");
	u_model = glGetUniformLocation(program, "u_model");
	u_view = glGetUniformLocation(program, "u_view");
	u_proj = glGetUniformLocation(program, "u_proj");
	u_camZ = glGetUniformLocation(program, "u_camZ");
	u_texSize = glGetUniformLocation(program, "u_texSize");
	u_spriteOffset = glGetUniformLocation(program, "u_spriteOffset");
	u_alpha = glGetUniformLocation(program, "u_alpha");
	u_texture0 = glGetUniformLocation(program, "u_texture0");
	u_shininess = glGetUniformLocation(program, "u_shininess");
	u_lightNum = glGetUniformLocation(program, "u_lightNum");
	for (int i = 0; i < 4; i++) {
		u_lights[i][0] = glGetUniformLocation(program, (std::string("u_lights[") + std::to_string(i) + std::string("].type")).c_str());
		u_lights[i][1] = glGetUniformLocation(program, (std::string("u_lights[") + std::to_string(i) + std::string("].diffuse")).c_str());
		u_lights[i][2] = glGetUniformLocation(program, (std::string("u_lights[") + std::to_string(i) + std::string("].pos")).c_str());
		u_lights[i][3] = glGetUniformLocation(program, (std::string("u_lights[") + std::to_string(i) + std::string("].constant")).c_str());
		u_lights[i][4] = glGetUniformLocation(program, (std::string("u_lights[") + std::to_string(i) + std::string("].linear")).c_str());
		u_lights[i][5] = glGetUniformLocation(program, (std::string("u_lights[") + std::to_string(i) + std::string("].quadratic")).c_str());
	}

	GLint maxAttribLen, numAttribs;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttribs);
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribLen);
	GLchar* attribName = (GLchar*)malloc(sizeof(GLchar) * maxAttribLen);
	for (int i = 0; i < numAttribs; i++) {
		GLint size;
		GLenum type;
		GLint location;
		glGetActiveAttrib(program, i, maxAttribLen, NULL, &size, &type, attribName);
		location = glGetAttribLocation(program, attribName);
		AddAttribute(attribName);
		//SDL_Log("%s %d", attribName, location);
	}
	free(attribName);



	GLint maxUniformLen, numUniforms;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLen);
	GLchar* uniformName = (GLchar*)malloc(sizeof(GLchar) * maxUniformLen);

	for (int i = 0; i < numUniforms; i++) {
		GLint size;
		GLenum type;
		GLint location;
		glGetActiveUniform(program, i, maxUniformLen, NULL, &size, &type, uniformName);
		location = glGetUniformLocation(program, uniformName);
		//SDL_Log("%s %d", uniformName, location);
	}
	free(uniformName);



}

void Managers::Shaders::LinkProgram()
{
	GLint linked;

	// Create the program object
	program = glCreateProgram();

	if (program == 0)
		return;

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	// Link the program
	glLinkProgram(program);

	// Check the link status
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint infoLen = 0;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1)
		{
			char* infoLog = new char[sizeof(char) * infoLen];


			glGetProgramInfoLog(program, infoLen, NULL, infoLog);
			Utils::GetInstance()->Log("SHADER","Error linking program: \n%s", infoLog);

			delete[] infoLog;
		}

		glDeleteProgram(program);
		return;
	}

	setLocationVariables();
}