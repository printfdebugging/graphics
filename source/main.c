#include <GLFW/glfw3.h>
#include <cglm/struct.h>
#include <cglm/struct/affine.h>
#include <glad/glad.h>

#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "renderer.h"
#include "shader.h"
#include "window.h"

#include <stdlib.h>
#include <string.h>

struct camera *camera;

void process_input(struct window *window, float delta_time);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

struct mesh *create_axes_mesh();
struct shader *create_axes_shader();

/* todo: deprecate this in favour of render_model */
void draw_axes(struct mesh *axes_mesh, struct shader *axes_shader, mat4s model, mat4s view, mat4s projection);

int main() {
    const float window_width = 1550.0f;
    const float window_height = 700.0f;
    const char *window_title = "OpenGL";
    const vec4s window_color = { 0.24, 0.24, 0.24, 1.0 };
    struct window *window;

    window = window_create(window_width, window_height, window_title, window_color);
    if (!window) return EXIT_FAILURE;
    if (window_set_icon(window, ASSETS_DIR "logo.png")) return EXIT_FAILURE;

    camera = camera_create();
    if (!camera) return EXIT_FAILURE;
    camera_adjust_direction(camera);

    glfwSetCursorPosCallback(window->window, mouse_callback);
    glfwSetScrollCallback(window->window, scroll_callback);

    struct mesh *axes_mesh = create_axes_mesh();
    struct shader *axes_shader = create_axes_shader();

    /* todo: why does loading this after axes_mesh replace
     * the mesh from the axes_mesh? i mean why is cube drawn
     * instead of the axes mesh? */
    const char *model_path = ASSETS_DIR "models/DamagedHelmet/glTF/DamagedHelmet.gltf";
    struct model *model = model_create();
    if (model_load_from_file(model, model_path)) {
        model_destroy(model);
        return EXIT_FAILURE;
    }

    struct shader *cube_shader = shader_create();
    if (!cube_shader)
        return EXIT_FAILURE;
    if (shader_load_from_file(cube_shader, ASSETS_DIR "shaders/model/shader.vert", ASSETS_DIR "shaders/model/shader.frag")) {
        shader_destroy(cube_shader);
        return EXIT_FAILURE;
    }

    float last_frame = 0.0f;
    float delta_time = 0.0f;
    while (!window_close(window)) {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        window_poll_events(window);
        window_process_input(window);
        window_clear_color(window);
        process_input(window, delta_time);

        mat4s view = camera_get_view_matrix(camera);
        mat4s projection = glms_perspective(glm_rad(camera->fov), window_width / window_height, 0.1f, 100.0f);

        /* without the render call below, why are axes not being drawn and instead cube's vertices are being drawn */
        draw_axes(axes_mesh, axes_shader, (mat4s) {}, view, projection);

        model->view = view;
        model->projection = projection;
        render_model(model, cube_shader);
        window_swap_buffers(window);
    }

    window_destroy(window);
    camera_destroy(camera);
    shader_destroy(axes_shader);
    mesh_destroy(axes_mesh);

    return 0;
}

void process_input(struct window *window, float delta_time) {
    if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
        camera_process_keyboard(camera, CAMERA_DIRECTION_FORWARD, delta_time);
    if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
        camera_process_keyboard(camera, CAMERA_DIRECTION_BACKWARD, delta_time);
    if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
        camera_process_keyboard(camera, CAMERA_DIRECTION_LEFT, delta_time);
    if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
        camera_process_keyboard(camera, CAMERA_DIRECTION_RIGHT, delta_time);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    camera_process_mouse_movement(camera, (float) xpos, (float) ypos, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera_process_mouse_scroll(camera, (float) yoffset);
}

struct mesh *create_axes_mesh() {
    const int AXES = 2;
    const int LINES_PER_AXIS = 501;
    const int LINES_ON_EACH_SIDE = LINES_PER_AXIS / 2;
    const int POINTS_PER_LINE = 2;
    const int FLOATS_PER_POINT = 3;
    const int count = AXES * LINES_PER_AXIS * POINTS_PER_LINE;

    float vertices[AXES][LINES_PER_AXIS][POINTS_PER_LINE][FLOATS_PER_POINT];

    for (int z = -LINES_ON_EACH_SIDE; z <= LINES_ON_EACH_SIDE; ++z) {
        vertices[0][z + LINES_ON_EACH_SIDE][0][0] = (float) -LINES_ON_EACH_SIDE;
        vertices[0][z + LINES_ON_EACH_SIDE][0][1] = 0.0f;
        vertices[0][z + LINES_ON_EACH_SIDE][0][2] = (float) z;

        vertices[0][z + LINES_ON_EACH_SIDE][1][0] = (float) LINES_ON_EACH_SIDE;
        vertices[0][z + LINES_ON_EACH_SIDE][1][1] = 0.0f;
        vertices[0][z + LINES_ON_EACH_SIDE][1][2] = (float) z;
    }

    for (int x = -LINES_ON_EACH_SIDE; x <= LINES_ON_EACH_SIDE; ++x) {
        vertices[1][x + LINES_ON_EACH_SIDE][0][0] = (float) x;
        vertices[1][x + LINES_ON_EACH_SIDE][0][1] = 0.0f;
        vertices[1][x + LINES_ON_EACH_SIDE][0][2] = (float) -LINES_ON_EACH_SIDE;

        vertices[1][x + LINES_ON_EACH_SIDE][1][0] = (float) x;
        vertices[1][x + LINES_ON_EACH_SIDE][1][1] = 0.0f;
        vertices[1][x + LINES_ON_EACH_SIDE][1][2] = (float) LINES_ON_EACH_SIDE;
    }

    struct mesh *axes_mesh = mesh_create();
    if (!axes_mesh) return NULL;

    mesh_load_vertices(axes_mesh, &vertices[0][0][0][0], count, 3 * sizeof(float));
    return axes_mesh;
}

struct shader *create_axes_shader() {
    struct shader *axes_shader = shader_create();
    if (!axes_shader)
        return NULL;
    if (shader_load_from_file(axes_shader, ASSETS_DIR "shaders/lines/shader.vert", ASSETS_DIR "shaders/lines/shader.frag")) {
        shader_destroy(axes_shader);
        return NULL;
    }
    return axes_shader;
}

void draw_axes(struct mesh *axes_mesh, struct shader *axes_shader, mat4s model, mat4s view, mat4s projection) {
    glUseProgram(axes_shader->program);
    shader_set_uniform(axes_shader, "view", Matrix4fv, 1, GL_FALSE, &view.col[0].raw[0]);
    shader_set_uniform(axes_shader, "projection", Matrix4fv, 1, GL_FALSE, &projection.col[0].raw[0]);

    glBindVertexArray(axes_mesh->vao);
    glDrawArrays(GL_LINES, 0, axes_mesh->vertex_count);
}
