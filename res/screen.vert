#version 330

layout(location = 0)in vec2 position;

uniform mat4 transform;

out vec2 pass_texCoords;

void main(){
	pass_texCoords = position;
	gl_Position = (transform * vec4(position, 0.5f, 1.0f));
}