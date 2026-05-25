uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

/*
 * note:
 * - each shader has these in_ variables which represent various vbos
 * - we bind these variables to positions in shader.c instead of doing that here
 */
in vec3 in_position;
in vec2 in_uv;

out vec3 color;
out vec2 uv;

void main() {
        gl_Position = projection * view * model * vec4(in_position.xyz, 1.0);
        color = vec3(0.5, 0.5, 0.5);
        uv = in_uv;
}
