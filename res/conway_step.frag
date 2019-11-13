#version 330

uniform sampler2D previous;
uniform vec2 size;

in vec2 pass_texCoords;

vec2 delta;

layout (location = 0)out vec4 out_color;

int sampleValue(int x, int y){
	return int(texture(previous, pass_texCoords + (vec2(x, y) * delta)).r * 256.0);
}

int sample(int x, int y){
	return min(int(texture(previous, pass_texCoords + (vec2(x, y) * delta)).r * 256.0), 1);
}

void main(){
	delta = vec2(1.0f) / size;
	int neighbours = 
		sample(-1, -1) +
		sample( 0, -1) +
		sample( 1, -1) +
		sample(-1,  0) +
		sample( 1,  0) +
		sample(-1,  1) +
		sample( 0,  1) +
		sample( 1,  1);
				
	int color = sampleValue(0, 0);
	if(color > 0){
		if(neighbours >= 2 && neighbours <= 3){
			out_color = vec4((float(neighbours) + 1.1) / 256.0, 0.0, 0.0, 1.0);
		}else{
			out_color = vec4(0.0, 0.0, 0.0, 1.0);
		}
	}else{
		if(neighbours == 3){
			out_color = vec4(1.1 / 256.0, 0.0, 0.0, 1.0);
		}else{
			out_color = vec4(0.0, 0.0, 0.0, 1.0);
		}
	}		
}
