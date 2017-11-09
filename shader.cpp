/*
 * shader.cpp
 *
 *  Created on: 09.11.2017
 *      Author: kupperp1
 */
#include "shader.h"
#include "fileutil.h"
#include "iostream"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(std::string vertexPath, std::string fragmentPath) {
	GLint success;
	GLchar infoLog[256];

	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	std::string vsource = loadFile(vertexPath);
	const GLchar* vsourcec = vsource.c_str();
	int vlength = vsource.size();
	glShaderSource(vshader, 1, &vsourcec, &vlength);
	glCompileShader(vshader);
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderInfoLog(vshader, 256, nullptr, infoLog);
		std::cerr << "Error compiling vertex shader: " << infoLog << std::endl;
		glDeleteShader(vshader);
	}

	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fsource = loadFile(fragmentPath).c_str();
	const GLchar* fsourcec = fsource.c_str();
	int flength = fsource.size();
	glShaderSource(fshader, 1, &fsourcec, &flength);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE) {
		glGetShaderInfoLog(fshader, 256, nullptr, infoLog);
		std::cerr << "Error compiling fragment shader: " << infoLog
				<< std::endl;
		glDeleteShader(fshader);
	}

	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vshader);
	glAttachShader(shaderProg, fshader);
	glLinkProgram(shaderProg);
	glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		glGetProgramInfoLog(shaderProg, 256, nullptr, infoLog);
		std::cerr << "Error linking shader program: " << infoLog << std::endl;
		glDeleteProgram(shaderProg);
	}
	glDetachShader(shaderProg, vshader);
	glDetachShader(shaderProg, fshader);
	glDeleteShader(vshader);
	glDeleteShader(fshader);
}

Shader::~Shader() {
	glDeleteProgram(shaderProg);
}

void Shader::bind() {
	glUseProgram(shaderProg);
}

void Shader::unbind() {
	glUseProgram(0);
}

void Shader::addUniform(std::string name) {
	uniforms[name] = glGetUniformLocation(shaderProg, name.c_str());
	if (uniforms[name] == -1) {
		std::cerr << "Couldn't find uniform: " << name << std::endl;
	}
}

void Shader::setUniform(std::string name, int value) {
	bind();
	glUniform1i(uniforms[name], value);
}
void Shader::setUniform(std::string name, float value) {
	bind();
	glUniform1f(uniforms[name], value);
}
void Shader::setUniform(std::string name, glm::vec2 value) {
	bind();
	glUniform2fv(uniforms[name], 1, glm::value_ptr(value));
}
void Shader::setUniform(std::string name, float value1, float value2) {
	bind();
	glUniform2f(uniforms[name], value1, value2);
}
void Shader::setUniform(std::string name, glm::vec3 value) {
	bind();
	glUniform3fv(uniforms[name], 1, glm::value_ptr(value));
}
void Shader::setUniform(std::string name, float value1, float value2,
		float value3) {
	bind();
	glUniform3f(uniforms[name], value1, value2, value3);
}
void Shader::setUniform(std::string name, glm::vec4 value) {
	bind();
	glUniform4fv(uniforms[name], 1, glm::value_ptr(value));
}
void Shader::setUniform(std::string name, float value1, float value2,
		float value3, float value4) {
	bind();
	glUniform4f(uniforms[name], value1, value2, value3, value4);
}
void Shader::setUniform(std::string name, glm::mat4 value) {
	bind();
	glUniformMatrix4fv(uniforms[name], 1, GL_FALSE, glm::value_ptr(value));
}

