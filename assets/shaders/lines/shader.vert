uniform mat4 view;
uniform mat4 projection;

in vec3 in_position;

out vec3 color;

bool is_xaxis() {
    return in_position.y == 0.0f &&
           in_position.z == 0.0f;
}

bool is_yaxis() {
    return in_position.x == 0.0f &&
           in_position.z == 0.0f;
}

bool is_zaxis() {
    return in_position.x == 0.0f &&
           in_position.y == 0.0f;
}

bool is_grid_edge() {
    return int(in_position.x) % 10 == 0 &&
           int(in_position.y) % 10 == 0 &&
           int(in_position.z) % 10 == 0;
}

void main() {
    gl_Position = projection * view * vec4(in_position.xyz, 1.0);
    if (is_xaxis()) {
        color = vec3(0.69f, 0.22f, 0.29f);
    } else if (is_yaxis()) {
        color = vec3(0.49f, 0.75f, 0.07f);
    } else if (is_zaxis()) {
        color = vec3(0.20f, 0.43f, 0.67f);
    } else {
        if (is_grid_edge())
            color = vec3(0.39f, 0.39f, 0.39f);
        else
            color = vec3(0.29f, 0.29f, 0.29f);
    }
}
