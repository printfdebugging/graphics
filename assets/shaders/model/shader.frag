uniform sampler2D surface;

in vec3 color;
in vec2 uv;

out vec4 outColor;

void main() {
    outColor = texture(surface, uv);
}
