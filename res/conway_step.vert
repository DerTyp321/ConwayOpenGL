#version 330

layout(location = 0)in vec2 position;

out vec2 pass_texCoords;

void main(){
	pass_texCoords = position;
	gl_Position = vec4(position * vec2(2.0f) - vec2(1.0f), 0.0f, 1.0f);
}