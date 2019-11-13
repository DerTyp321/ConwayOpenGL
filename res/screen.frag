#version 330

#define SPHERE_RADIUS 0.4
#define SPHERE_HEIGHT 2
uniform sampler2D tex;
uniform vec2 size;
uniform float scale;
uniform vec2 mousePos;

in vec2 pass_texCoords;

out vec4 out_color;

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

bool rayIntersectsSphere(vec3 rayOrig, vec3 rayDir, vec3 spherePos, float sphereRadius) {
	vec3 l = spherePos - rayOrig;
    float adj = dot(l, rayDir);
    float d2 = dot(l, l) - (adj * adj);
	float radius2 = sphereRadius * sphereRadius;

    if (d2 > radius2) {
		return false;
	}
	float thc = sqrt(radius2 - d2);
    float t0 = adj - thc;
    float t1 = adj + thc;

    if (t0 < 0.0001 && t1 < 0.0001) {
        return false;
    }
	return true;
}

bool hasSphere(vec2 pos) {
	return texture(tex, (pos + vec2(0.5)) / size).r > 0.0;
}

vec4 calcLight(vec3 normal, vec3 pos, vec3 lightPos, vec3 albedo) {
	vec3 toLight = lightPos - pos;

	albedo = pow(albedo, vec3(2.2));

	float lightDist = length(toLight);
	vec3 lightDir = normalize(toLight);

	vec3 diffuse = vec3(0);

	vec3 specular = vec3(0);

	bool shadow = false;

	float t = 0;

	vec2 tileCoords = floor(pos.xy);

	vec3 curPos = vec3(0);
	while (curPos.z < SPHERE_HEIGHT + SPHERE_RADIUS) {
		curPos = pos + t * lightDir;

		if(hasSphere(tileCoords) && rayIntersectsSphere(pos, lightDir, vec3(floor(tileCoords), 0.0) + vec3(0.5, 0.5, SPHERE_HEIGHT), SPHERE_RADIUS)) {
			shadow = true;
			break;
		}

		vec2 dt = (tileCoords + vec2(lightDir.x > 0 ? 1 : 0, lightDir.y > 0 ? 1 : 0) - curPos.xy) / lightDir.xy;
		if(dt.x < dt.y) {
			t += dt.x;
			tileCoords += vec2(sign(lightDir.x), 0);
		} else if (dt.y < dt.x) {
			t += dt.y;
			tileCoords += vec2(0, sign(lightDir.y));
		} else {
			t += dt.x;
			tileCoords += vec2(sign(lightDir.x), sign(lightDir.y));
		}	
	}

	if(!shadow) {
		float intensity = 500000.0 * scale*scale;
		float attenuation = 1 / (lightDist * lightDist);

		diffuse = vec3(max(0, dot(normal, lightDir)) * intensity * attenuation) * albedo;

		specular = vec3(pow(max(0, dot(reflect(lightDir, normal), vec3(0, 0, -1))), 10)) * intensity * attenuation * 0.2;
	}

	vec3 ambient = vec3(0.02) * albedo;

	vec3 color = ambient + diffuse + specular;

	color = color / (color + 1.0);
	color = pow(color, vec3(1.0/2.2));

	return vec4(vec3(color), 1.0f);
}

void main(){

	vec2 inTilePos = fract(pass_texCoords * size);
	inTilePos -= vec2(0.5);
	inTilePos /= SPHERE_RADIUS;

	float age = texture(tex, pass_texCoords).r;
	float centerDist = sqrt(dot(inTilePos, inTilePos));

	vec3 lightPos = vec3(mousePos, 500.0 * scale);

	vec3 normal = vec3(0.0, 0.0, 1.0);
	vec3 pos = vec3(pass_texCoords * size, 0.0);
	vec3 albedo = vec3(0.3f, 0.3f, 0.3f);

	if(age >= 1.0f / 256.0f && centerDist < 1.0) {
		normal = vec3(inTilePos.x, inTilePos.y, sqrt(1-dot(inTilePos, inTilePos)));
		pos = vec3(pos.xy, normal.z * SPHERE_RADIUS + SPHERE_HEIGHT);
		//albedo = hsv2rgb(vec3(fract(sin(pos.x / 10.0) * cos(pos.y / 10.0)), 1.0, 1.0));
		albedo = hsv2rgb(vec3(age * 50.0 + 0.4, 1.0, 1.0));
	}

	out_color = calcLight(normal, pos, lightPos, albedo);
}
