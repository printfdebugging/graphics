
in vec3 position;
in vec3 color;
in vec2 uv;
in vec3 normal;

out vec4 out_color;

void main() {
	out_color = vec4(color, 0.5);
}
