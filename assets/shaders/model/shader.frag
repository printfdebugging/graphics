uniform sampler2D surface;
uniform vec3 object_color;
uniform vec3 light_color;

in vec3 color;
in vec2 uv;

out vec4 outColor;

void main() {
    outColor = vec4(object_color.rgb, 1.0) * vec4(light_color.rgb, 1.0);
}
