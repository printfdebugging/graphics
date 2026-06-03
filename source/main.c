#include <stdlib.h>

#include "cglm/struct.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "camera.h"
#include "game.h"
#include "primitive.h"
#include "model.h"
#include "renderer.h"
#include "shader.h"
#include "window.h"
#include "core/defines.h"

void process_input(struct window *window, f64 delta_time);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

struct primitive *create_axes_primitive();
struct shader *create_axes_shader();

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

        struct primitive *axes = create_axes_primitive();
        struct shader *axes_shader = create_axes_shader();
        if (!axes || !axes_shader)
                return EXIT_FAILURE;

        axes->shader = axes_shader;

        const char *engine_asset_path = ASSETS_DIR "models/CylinderEngine/glTF/2CylinderEngine.gltf";
        i8 status;

        struct model *engine = model_create();
        if ((status = model_load_from_file(engine, engine_asset_path)) != 0) {
                fprintf(stderr, "failed to load model: %s\n", engine_asset_path);
                return EXIT_FAILURE;
        }

        struct shader *engine_shader = shader_create();
        if ((status = shader_load_from_file(engine_shader, ASSETS_DIR "shaders/model/shader.vert", ASSETS_DIR "shaders/model/shader.frag")) != 0) {
                fprintf(stderr, "failed to load shader\n");
                return EXIT_FAILURE;
        }

        /* todo: we should have a shader_manager and create shaders based on shader options and reuse shaders */
        for (u64 i = 0; i < engine->primitive_count; ++i) {
                engine->primitives[i]->shader = engine_shader;
        }

        // return game_run();
        while (!window_close(game.window)) {
                f64 current_frame = glfwGetTime();
                game.delta_time = current_frame - game.last_frame;
                game.last_frame = current_frame;

                window_poll_events(game.window);
                window_process_input(game.window);
                window_clear_color(game.window);
                process_input(game.window, game.delta_time);

                mat4s model = { GLM_MAT4_IDENTITY_INIT };
                mat4s view = camera_get_view_matrix(game.camera);
                mat4s projection = glms_perspective(glm_rad(game.camera->fov), (float) game.window->width / (float) game.window->height, 0.1f, 100.0f);
                struct transform transform = { .model = model, .view = view, .projection = projection };

                /* todo: set trs on model */

                /* todo: continue from here, we transforming the transforms. also worth taking logs as you go, but i am on low energy, probably last day's coffee is coming out now */
                render_primitive(axes, transform);
                engine->transform = transform;
                render_model(engine);
                window_swap_buffers(game.window);
        }

        /* move to a separate cleanup function, or rather make the subsystems init/cleanup themselves */

        primitive_destroy(axes);
        shader_destroy(axes_shader);
        shader_destroy(engine_shader);
        model_destroy(engine);

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

struct primitive *create_axes_primitive() {
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

        struct primitive *axes = primitive_create();
        if (!axes)
                return NULL;

        primitive_load_vertices(axes, &vertices[0][0][0][0], (u32) count, 3 * sizeof(float));
        axes->draw_mode = GL_LINES;
        return axes;
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
