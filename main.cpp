/*
 * main.cpp
 *
 *  Created on: 03.11.2017
 *      Author: kupperp1
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shader.h"


//int WIDTH = 1920;
//int HEIGHT = 1080;
int WIDTH = 1024;
int HEIGHT = 720;

double upsGoal = 16.0f;
bool playing = true;
bool dragging = false;

float mousePosX;
float mousePosY;

float dragStartX;
float dragStartY;

float offsetX = 10.0f;
float offsetY = 10.0f;

float scale = 1.0f;


void glfwErrorCallback(int error, const char* description)
{
    std::cerr << "Error: " << description << std::endl;
}

void glfwFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	WIDTH = width;
	HEIGHT = height;
}

void checkGlError() {
        GLenum err (glGetError());

        while(err!=GL_NO_ERROR) {
                std::string error;

                switch(err) {
                        case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
                        case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
                        case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
                        case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
                        case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
                }
                std::cerr << "OpenGL error: " << error << std::endl;
                err=glGetError();
        }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (action == GLFW_PRESS){
    	switch(key){
    	case GLFW_KEY_ESCAPE:
        	glfwSetWindowShouldClose(window, true);
    		break;
    	case GLFW_KEY_PERIOD:
        	if(upsGoal < 65536.0)upsGoal *= 2.0;
    		break;
    	case GLFW_KEY_COMMA:
    		if(upsGoal > 1)upsGoal /= 2.0;
    		break;
    	case GLFW_KEY_SPACE:
    		playing = !playing;
    		break;
    	}
    }else{

    }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	float factor = pow(1.1f, yoffset);
	scale *= factor;
	float distX =  mousePosX - offsetX;
	float distY = mousePosY - offsetY;
	offsetX -= factor * distX - distX;
	offsetY -= factor * distY - distY;
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos){
	mousePosX = xpos;
	mousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
    	dragging = true;
    	dragStartX = mousePosX;
    	dragStartY = mousePosY;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
    	offsetX = offsetX + (mousePosX - dragStartX);
    	offsetY = offsetY + (mousePosY - dragStartY);
    	dragging = false;
    }
}

int main(int argc, const char* argv[]){

	std::string imagePath;

	for(int i = 0; i < argc - 1; i++){
		if(strcmp(argv[i], "-img") == 0 || strcmp(argv[i], "-i") == 0){
			imagePath = std::string(argv[i+1]);
		}
		if(strcmp(argv[i], "-ups") == 0 || strcmp(argv[i], "-u") == 0){
			std::cout << argv[i+1];
			upsGoal = std::stof(argv[i+1]);
		}
	}

	if (!glfwInit())
	{
	    std::cerr << "GLFW init failed!" << std::endl;
	    return 1;
	}
	glfwSetErrorCallback(glfwErrorCallback);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Conways Game of Life", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Creating window failed!" << std::endl;
	    return 1;
	}

	glfwMakeContextCurrent(window);
	GLenum error = glewInit();
	if(error != GLEW_OK){
		std::cerr << "Failed to init glew!" << std::endl;
	    return 1;
	}

	glfwSetFramebufferSizeCallback(window, glfwFramebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSwapInterval(1);


	Shader screenShader("res/screen.vert", "res/screen.frag");

	screenShader.bind();

	screenShader.addUniform("tex");
	screenShader.setUniform("tex", 0);

	screenShader.addUniform("transform");
	screenShader.addUniform("size");


	Shader conwayShader("res/conway_step.vert", "res/conway_step.frag");

	conwayShader.bind();
	conwayShader.addUniform("previous");
	conwayShader.setUniform("previous", 0);

	conwayShader.addUniform("size");

	int width, height, channels;

	unsigned char* image = stbi_load(imagePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	GLuint buffers[2];
	glGenTextures(2, buffers);
	glBindTexture(GL_TEXTURE_2D, buffers[0]);
	if(channels == 4){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	}else if(channels == 3){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}else{
		std::cerr << "unsupported image format" << std::endl;
		return 1;
	}

	GLenum edgeMode;
	edgeMode = GL_CLAMP_TO_BORDER;
	//edgeMode = GL_REPEAT;

	float border[] = {0.0f, 0.0f, 0.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);

	glBindTexture(GL_TEXTURE_2D, buffers[1]);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edgeMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edgeMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);




	float screenQuad[] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			0.0f, 1.0f
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuad), screenQuad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (int*)0);
	glBindVertexArray(0);

	checkGlError();

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glDisable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	int drawBuffer = 1;

	int frames = 0;
	int updates = 0;

	double lastUpdate = glfwGetTime();
	double lastSecond = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vao);

		double now = glfwGetTime();

		conwayShader.bind();
		conwayShader.setUniform("size", (float)width, (float)height);
		glViewport(0, 0, width, height);
		while(now - lastUpdate >= 1.0 / upsGoal){
			lastUpdate += 1.0 / upsGoal;
			if(playing){
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, buffers[1-drawBuffer]);
				glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffers[drawBuffer], 0);

				glDrawArrays(GL_TRIANGLES, 0, 6);
				drawBuffer = 1 - drawBuffer;
			}

			updates++;
		}
		if(now - lastSecond >= 1.0){
			std::stringstream title;
			title << "Conways Game of Life - " << frames << "FPS, " << updates << "UPS";
			glfwSetWindowTitle(window, title.str().c_str());
			frames = 0;
			updates = 0;
			lastSecond += 1.0;
		}
		screenShader.bind();
		screenShader.setUniform("size", (float)width, (float)height);

		float scaledWidth = (float)width * scale;
		float scaledHeight = (float)height * scale;
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(dragging ? offsetX + (mousePosX - dragStartX): offsetX, dragging ? offsetY + (mousePosY - dragStartY) : offsetY, 0.0f));
		model = glm::scale(model, glm::vec3(scaledWidth, scaledHeight, 1.0f));
		glm::mat4 projection = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
		glm::mat4 transform = projection * model;
		screenShader.setUniform("transform", transform);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, buffers[drawBuffer]);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WIDTH, HEIGHT);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);

		frames++;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
