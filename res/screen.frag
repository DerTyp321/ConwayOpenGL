#version 330

uniform sampler2D tex;
uniform vec2 size;

in vec2 pass_texCoords;

out vec4 out_color;

void main(){
	vec2 inTile = fract(pass_texCoords * size);
	inTile -= vec2(0.5);
	float color = texture(tex, pass_texCoords).r;
	float dist = inTile.x * inTile.x + inTile.y * inTile.y;
	//abs(inTile).x > 0.4 || abs(inTile).y > 0.4
	if(color >= 1.0f / 256.0f){
		if(dist > 0.20){
			out_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		}else{
			out_color = vec4(1.0 - pow(clamp(color * 5.0f, 0.0, 1.0), 0.3f), 0.0f, 0.0f, 1.0f);
		}

	}else{
		out_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

}
