#ifndef SHADER
#define SHADER

#include <GL/glew.h>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Shader{
public:
	Shader(std::string vertexPath, std::string fragmentPath);
	~Shader();

	void bind();
	void unbind();
	void addUniform(std::string name);
	void setUniform(std::string name, int value);
	void setUniform(std::string name, float value);
	void setUniform(std::string name, glm::vec2 value);
	void setUniform(std::string name, float value1, float value2);
	void setUniform(std::string name, glm::vec3 value);
	void setUniform(std::string name, float value1, float value2, float value3);
	void setUniform(std::string name, glm::vec4 value);
	void setUniform(std::string name, float value1, float value2, float value3, float value4);
	void setUniform(std::string name, glm::mat4 value);
private:
	GLint shaderProg;
	std::map<std::string, GLuint> uniforms;
};

#endif
