#version 330

uniform sampler2D tex;
uniform vec2 size;
uniform float scale;

in vec2 pass_texCoords;

out vec4 out_color;

void main(){
	vec2 inTile = fract(pass_texCoords * size);
	inTile -= vec2(0.5);
	float color = texture(tex, pass_texCoords).r;
	float dist = inTile.x * inTile.x + inTile.y * inTile.y;
	//abs(inTile).x > 0.4 || abs(inTile).y > 0.4
	if(color >= 1.0f / 256.0f){
		vec4 color1 = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		vec4 color2 = vec4(1.0 - pow(clamp(color * 5.0f, 0.0, 1.0), 0.3f), 0.0f, 0.0f, 1.0f);
		out_color = mix(color2, color1, smoothstep(0.2 - scale, 0.2 + scale, dist));
	}else{
		out_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

}
