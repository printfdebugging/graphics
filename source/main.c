#include <stdlib.h>

#include "cglm/struct.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "camera.h"
#include "game.h"
#include "mesh.h"
#include "shader.h"
#include "window.h"
#include "core/defines.h"

void process_input(struct window *window, f64 delta_time);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

struct mesh *create_axes_mesh();
struct shader *create_axes_shader();

/* todo: deprecate this in favour of renderModel */
void draw_axes(struct mesh *axes_mesh, struct shader *axes_shader, mat4s model, mat4s view, mat4s projection);

#define ensure_not_null(x)           \
        if ((x) == NULL) {           \
                return EXIT_FAILURE; \
        }

int main() {
        struct game_data game = { 0 };
        ensure_not_null(game.window = window_create((struct window) {
                WINDOW_DEFAULTS,
        }));

        ensure_not_null(game.camera = camera_create((struct camera) {
            CAMERA_DEFAULTS,
            .position = { { 0.0f, 4.0f, 9.0f } },
            .movement_speed = 5.0f,
            .pitch = -30.0f,
        }));

        game.light_position = (vec3s) { { 0.0f, 0.0f, 2.0f } };
        camera_adjust_direction(game.camera);

        /* these should be moved to the window layer, we would want to have many
         * callbacks over time, for different views/interactions. */
        glfwSetCursorPosCallback(game.window->window, mouse_callback);
        glfwSetScrollCallback(game.window->window, scroll_callback);
        glfwSetWindowUserPointer(game.window->window, &game);
        window_scale_to_monitor_dpi(game.window->window);

        struct mesh *axes_mesh = create_axes_mesh();
        struct shader *axes_shader = create_axes_shader();
        if (!axes_mesh || !axes_shader)
                return EXIT_FAILURE;

        // return game_run();
        while (!window_close(game.window)) {
                f64 current_frame = glfwGetTime();
                game.delta_time = current_frame - game.last_frame;
                game.last_frame = current_frame;

                window_poll_events(game.window);
                window_process_input(game.window);
                window_clear_color(game.window);
                process_input(game.window, game.delta_time);

                mat4s view = camera_get_view_matrix(game.camera);
                mat4s projection = glms_perspective(glm_rad(game.camera->fov), (float) game.window->width / (float) game.window->height, 0.1f, 100.0f);

                draw_axes(axes_mesh, axes_shader, (mat4s) {}, view, projection);
                window_swap_buffers(game.window);
        }

        /* move to a separate cleanup function, or rather make the subsystems init/cleanup themselves */

        mesh_destroy(axes_mesh);
        shader_destroy(axes_shader);

        camera_destroy(game.camera);
        window_destroy(game.window);

        return 0;
}

void input_move_camera(struct window *window, f64 delta_time) {
        struct game_data *data = glfwGetWindowUserPointer(window->window);
        if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
                camera_process_keyboard(data->camera, CAMERA_DIRECTION_FORWARD, delta_time);
        if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
                camera_process_keyboard(data->camera, CAMERA_DIRECTION_BACKWARD, delta_time);
        if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
                camera_process_keyboard(data->camera, CAMERA_DIRECTION_LEFT, delta_time);
        if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
                camera_process_keyboard(data->camera, CAMERA_DIRECTION_RIGHT, delta_time);
}

void input_move_point_light(struct window *window, f64 delta_time) {
        (void) delta_time;
        struct game_data *data = glfwGetWindowUserPointer(window->window);
        if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
                data->light_position.z -= 0.1f;
        if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
                data->light_position.z += 0.1f;
        if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
                data->light_position.x -= 0.1f;
        if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
                data->light_position.x += 0.1f;
}

void process_input(struct window *window, f64 delta_time) {
        // input_move_point_light(window, delta_time);
        input_move_camera(window, delta_time);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
        struct game_data *data = glfwGetWindowUserPointer(window);
        camera_process_mouse_movement(data->camera, (float) xpos, (float) ypos, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        (void) xoffset;
        struct game_data *data = glfwGetWindowUserPointer(window);
        camera_process_mouse_scroll(data->camera, (float) yoffset);
}

struct mesh *create_axes_mesh() {
        const i32 AXES = 2;
        const i32 LINES_PER_AXIS = 11;
        const i32 LINES_ON_EACH_SIDE = LINES_PER_AXIS / 2;
        const i32 POINTS_PER_LINE = 2;
        const i32 FLOATS_PER_POINT = 3;
        const i32 count = AXES * LINES_PER_AXIS * POINTS_PER_LINE;

        f32 vertices[AXES][LINES_PER_AXIS][POINTS_PER_LINE][FLOATS_PER_POINT];

        for (i32 z = -LINES_ON_EACH_SIDE; z <= LINES_ON_EACH_SIDE; ++z) {
                vertices[0][z + LINES_ON_EACH_SIDE][0][0] = (f32) -LINES_ON_EACH_SIDE;
                vertices[0][z + LINES_ON_EACH_SIDE][0][1] = 0.0f;
                vertices[0][z + LINES_ON_EACH_SIDE][0][2] = (f32) z;

                vertices[0][z + LINES_ON_EACH_SIDE][1][0] = (f32) LINES_ON_EACH_SIDE;
                vertices[0][z + LINES_ON_EACH_SIDE][1][1] = 0.0f;
                vertices[0][z + LINES_ON_EACH_SIDE][1][2] = (f32) z;
        }

        for (i32 x = -LINES_ON_EACH_SIDE; x <= LINES_ON_EACH_SIDE; ++x) {
                vertices[1][x + LINES_ON_EACH_SIDE][0][0] = (f32) x;
                vertices[1][x + LINES_ON_EACH_SIDE][0][1] = 0.0f;
                vertices[1][x + LINES_ON_EACH_SIDE][0][2] = (f32) -LINES_ON_EACH_SIDE;

                vertices[1][x + LINES_ON_EACH_SIDE][1][0] = (f32) x;
                vertices[1][x + LINES_ON_EACH_SIDE][1][1] = 0.0f;
                vertices[1][x + LINES_ON_EACH_SIDE][1][2] = (f32) LINES_ON_EACH_SIDE;
        }

        struct mesh *axes_mesh = mesh_create();
        if (!axes_mesh)
                return NULL;

        mesh_load_vertices(axes_mesh, &vertices[0][0][0][0], (u32) count, 3 * sizeof(float));
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
        (void) model;
        glUseProgram(axes_shader->program);
        shader_set_uniform(axes_shader, "view", Matrix4fv, 1, GL_FALSE, &view.col[0].raw[0]);
        shader_set_uniform(axes_shader, "projection", Matrix4fv, 1, GL_FALSE, &projection.col[0].raw[0]);

        glBindVertexArray(axes_mesh->vao);
        glDrawArrays(GL_LINES, 0, (i32) axes_mesh->vertex_count);
}
