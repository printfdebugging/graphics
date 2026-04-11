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

/* todo: also try out clay & cimgui and see how it does things. */
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuclear.h"
#include "nuclear_glfw_opengl3.h"

#define MAX_VERTEX_BUFFER  512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

struct camera *camera;

struct nk_context *ctx;
struct nk_colorf bg = {
    .r = 0.10f,
    .g = 0.18f,
    .b = 0.24f,
    .a = 1.0f,
};

void process_input(struct window *window, float delta_time);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

struct mesh *create_axes_mesh();
struct shader *create_axes_shader();
struct model *create_cube_model();
struct shader *create_model_shader();

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
    if (!axes_mesh || !axes_shader) return EXIT_FAILURE;

    struct model *cube = create_cube_model();
    struct shader *cube_shader = create_model_shader();
    if (!cube || !cube_shader) return EXIT_FAILURE;

    ctx = nk_glfw3_init(window->window, NK_GLFW3_INSTALL_CALLBACKS);
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    nk_glfw3_font_stash_end();

    float last_frame = 0.0f;
    float delta_time = 0.0f;
    while (!window_close(window)) {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        nk_glfw3_new_frame();

        if (nk_begin(ctx, "State", nk_rect(50, 50, 230, 250), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
            enum { EASY,
                   HARD };
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        window_poll_events(window);
        window_process_input(window);
        window_clear_color(window);
        process_input(window, delta_time);

        mat4s view = camera_get_view_matrix(camera);
        mat4s projection = glms_perspective(glm_rad(camera->fov), window_width / window_height, 0.1f, 100.0f);

        /* without the render call below, why are axes not being drawn and instead cube's vertices are being drawn */
        draw_axes(axes_mesh, axes_shader, (mat4s) {}, view, projection);

        cube->view = view;
        cube->projection = projection;
        render_model(cube, cube_shader);
        nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        window_swap_buffers(window);
    }

    window_destroy(window);
    camera_destroy(camera);
    shader_destroy(axes_shader);
    mesh_destroy(axes_mesh);
    nk_glfw3_shutdown();

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

struct model *create_cube_model() {
    /* clang-format off */
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, 
        0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, 
        -0.5f,  0.5f, -0.5f, 0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, 
    };
    /* clang-format on */
    struct model *model = model_create();
    if (!model) return NULL;

    /* todo: test this malloc as well, or more like break these steps in the model loader itself */
    model->mesh = malloc(sizeof(struct model *));
    model->mesh_count = 1;

    *model->mesh = mesh_create();
    model->mesh[0]->draw_mode = GL_TRIANGLES;
    mesh_load_vertices(*model->mesh, vertices, 36, 3 * sizeof(float));
    return model;
}

struct shader *create_model_shader() {
    struct shader *shader = shader_create();
    if (!shader)
        return NULL;
    if (shader_load_from_file(shader, ASSETS_DIR "shaders/model/shader.vert", ASSETS_DIR "shaders/model/shader.frag")) {
        shader_destroy(shader);
        return NULL;
    }
    return shader;
}
