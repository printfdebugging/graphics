uniform mat4 view;
uniform mat4 projection;

in vec3 position;

out vec3 color;

void main() {
    gl_Position = projection * view * vec4(position.xyz, 1.0);
    color = vec3(0.5, 0.5, 0.5);
}
