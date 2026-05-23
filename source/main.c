#include "GLFW/glfw3.h"
#include "cglm/struct.h"
#include "cglm/struct/affine.h"
#include "glad/glad.h"

#include <stdlib.h>
#include <string.h>

#include "camera.h"
#include "game.h"
#include "material.h"
#include "mesh.h"
#include "model.h"
#include "shader.h"
#include "texture.h"
#include "window.h"

void process_input(struct window *window, float delta_time);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

struct mesh *create_axes_mesh();
struct shader *create_axes_shader();
struct model *create_cube_model();
struct shader *create_cube_shader();
struct shader *create_light_shader();

/* todo: deprecate this in favour of renderModel */
void draw_axes(struct mesh *axes_mesh, struct shader *axes_shader, mat4s model,
               mat4s view, mat4s projection);

int
main()
{
   struct game_data game = {NULL};
   // return game_run();
   game.window = window_create(1550.0f, 700.0f, "floating",
                               (vec4s) {0.0f, 0.0f, 0.0f, 1.0});
   if (!game.window)
      return EXIT_FAILURE;
   if (window_set_icon(game.window, ASSETS_DIR "logo.png"))
      return EXIT_FAILURE;

   game.camera = camera_create();
   if (!game.camera)
      return EXIT_FAILURE;
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

   struct model *cube = create_cube_model();
   struct shader *cube_shader = create_cube_shader();
   struct shader *light_shader = create_light_shader();
   if (!cube || !cube_shader || !light_shader)
      return EXIT_FAILURE;

   struct texture *material_diffuse_map =
      texture_create_from_file(ASSETS_DIR "textures/diffuse.png");
   if (!material_diffuse_map)
      return EXIT_FAILURE;

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, material_diffuse_map->texture);
   material_diffuse_map->texture_index = 0;

   while (!window_close(game.window)) {
      float currentFrame = glfwGetTime();
      game.delta_time = currentFrame - game.last_frame;
      game.last_frame = currentFrame;

      window_poll_events(game.window);
      window_process_input(game.window);
      window_clear_color(game.window);
      process_input(game.window, game.delta_time);

      /* model matrix for light source */
      vec3s lightpos = {0.0f, 0.0f, 2.0f};
      vec3s lightscale = {0.2f, 0.2f, 0.2f};

      mat4s view = camera_get_view_matrix(game.camera);
      mat4s projection = glms_perspective(
         glm_rad(game.camera->fov),
         (float) game.window->width / (float) game.window->height, 0.1f,
         100.0f);

      /* without the render call below, why are axes not being drawn and instead
       * cube's vertices are being drawn */
      draw_axes(axes_mesh, axes_shader, (mat4s) {}, view, projection);

      {
         /* render model*/
         cube->model = (mat4s) {.raw = GLM_MAT4_IDENTITY_INIT};
         cube->model = glms_scale(cube->model, (vec3s) {1.0f, 1.0f, 1.0f});
         cube->view = view;
         cube->projection = projection;
         glUseProgram(cube_shader->program);
         shader_set_uniform(cube_shader, "model", Matrix4fv, 1, GL_FALSE,
                            &cube->model.col[0].raw[0]);
         shader_set_uniform(cube_shader, "view", Matrix4fv, 1, GL_FALSE,
                            &cube->view.col[0].raw[0]);
         shader_set_uniform(cube_shader, "projection", Matrix4fv, 1, GL_FALSE,
                            &cube->projection.col[0].raw[0]);
         shader_set_uniform(cube_shader, "light_color", 3fv, 1,
                            (vec3s) {1.0f, 1.0f, 1.0f}.raw);
         shader_set_uniform(cube_shader, "light_position", 3fv, 1,
                            lightpos.raw);
         shader_set_uniform(cube_shader, "camera_position", 3fv, 1,
                            game.camera->position.raw);
         enum material_type mat = GOLD;
         shader_set_uniform(cube_shader, "material_specular", 3fv, 1,
                            MATERIALS[mat].specular.raw);
         shader_set_uniform(cube_shader, "material_shininess", 1f,
                            MATERIALS[mat].shininess * 128.0f);
         shader_set_uniform(cube_shader, "material_diffuse_map", 1i,
                            material_diffuse_map->texture_index);
         for (int i = 0; i < cube->mesh_count; ++i) {
            const struct mesh *mesh = cube->mesh[i];
            glBindVertexArray(mesh->vao);
            if (mesh->index_count) {
               glDrawElements(mesh->draw_mode, mesh->index_count,
                              mesh->index_type, NULL);
            } else {
               glDrawArrays(mesh->draw_mode, 0, mesh->vertex_count);
            }
         }
      }

      {
         mat4s model_matrix_light = {.raw = GLM_MAT4_IDENTITY_INIT};
         model_matrix_light = glms_translate(model_matrix_light, lightpos);
         model_matrix_light = glms_scale(model_matrix_light, lightscale);

         /* render cube */
         cube->view = view;
         cube->projection = projection;
         cube->model = model_matrix_light;
         glUseProgram(light_shader->program);
         shader_set_uniform(light_shader, "model", Matrix4fv, 1, GL_FALSE,
                            &cube->model.col[0].raw[0]);
         shader_set_uniform(light_shader, "view", Matrix4fv, 1, GL_FALSE,
                            &cube->view.col[0].raw[0]);
         shader_set_uniform(light_shader, "projection", Matrix4fv, 1, GL_FALSE,
                            &cube->projection.col[0].raw[0]);
         for (int i = 0; i < cube->mesh_count; ++i) {
            const struct mesh *mesh = cube->mesh[i];
            glBindVertexArray(mesh->vao);
            if (mesh->index_count) {
               glDrawElements(mesh->draw_mode, mesh->index_count,
                              mesh->index_type, NULL);
            } else {
               glDrawArrays(mesh->draw_mode, 0, mesh->vertex_count);
            }
         }
      }

      window_swap_buffers(game.window);
   }

   window_destroy(game.window);
   camera_destroy(game.camera);
   shader_destroy(axes_shader);
   mesh_destroy(axes_mesh);

   return 0;
}

void
process_input(struct window *window, float delta_time)
{
   struct game_data *data = glfwGetWindowUserPointer(window->window);
   if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
      camera_process_keyboard(data->camera, CAMERA_DIRECTION_FORWARD,
                              delta_time);
   if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
      camera_process_keyboard(data->camera, CAMERA_DIRECTION_BACKWARD,
                              delta_time);
   if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
      camera_process_keyboard(data->camera, CAMERA_DIRECTION_LEFT, delta_time);
   if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
      camera_process_keyboard(data->camera, CAMERA_DIRECTION_RIGHT, delta_time);
}

void
mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
   struct game_data *data = glfwGetWindowUserPointer(window);
   camera_process_mouse_movement(
      data->camera, (float) xpos, (float) ypos,
      glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}

void
scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
   struct game_data *data = glfwGetWindowUserPointer(window);
   camera_process_mouse_scroll(data->camera, (float) yoffset);
}

struct mesh *
create_axes_mesh()
{
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
   if (!axes_mesh)
      return NULL;

   mesh_load_vertices(axes_mesh, &vertices[0][0][0][0], count,
                      3 * sizeof(float));
   return axes_mesh;
}

struct shader *
create_axes_shader()
{
   struct shader *axes_shader = shader_create();
   if (!axes_shader)
      return NULL;
   if (shader_load_from_file(axes_shader,
                             ASSETS_DIR "shaders/lines/shader.vert",
                             ASSETS_DIR "shaders/lines/shader.frag")) {
      shader_destroy(axes_shader);
      return NULL;
   }
   return axes_shader;
}

void
draw_axes(struct mesh *axes_mesh, struct shader *axes_shader, mat4s model,
          mat4s view, mat4s projection)
{
   glUseProgram(axes_shader->program);
   shader_set_uniform(axes_shader, "view", Matrix4fv, 1, GL_FALSE,
                      &view.col[0].raw[0]);
   shader_set_uniform(axes_shader, "projection", Matrix4fv, 1, GL_FALSE,
                      &projection.col[0].raw[0]);

   glBindVertexArray(axes_mesh->vao);
   glDrawArrays(GL_LINES, 0, axes_mesh->vertex_count);
}

struct model *
create_cube_model()
{
   /* clang-format off */
   float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
      -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f, 0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
      -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
      0.5f,  0.5f,  0.5f, 0.5f,  0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f,
      -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
      -0.5f,  0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f,  0.5f, 0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
   };
   float normals[] = {
      0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, -1.0f,
      0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
      -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
      1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f,
      0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f,
      0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,  0.0f,
   };
   float uv[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f
   };

   /* clang-format on */
   struct model *model = model_create();
   if (!model)
      return NULL;

   /* todo: test this malloc as well, or more like break these steps in the
    * model loader itself */
   model->mesh = malloc(sizeof(struct model *));
   model->mesh_count = 1;

   *model->mesh = mesh_create();
   model->mesh[0]->draw_mode = GL_TRIANGLES;
   mesh_load_vertices(*model->mesh, vertices, 36, 3 * sizeof(float));
   mesh_load_normals(*model->mesh, normals, 36, 3 * sizeof(float));
   mesh_load_uv(*model->mesh, uv, 24, 2 * sizeof(float));
   return model;
}

struct shader *
create_cube_shader()
{
   struct shader *shader = shader_create();
   if (!shader)
      return NULL;
   if (shader_load_from_file(shader, ASSETS_DIR "shaders/model/shader.vert",
                             ASSETS_DIR "shaders/model/shader.frag")) {
      shader_destroy(shader);
      return NULL;
   }
   return shader;
}

struct shader *
create_light_shader()
{
   struct shader *shader = shader_create();
   if (!shader)
      return NULL;
   if (shader_load_from_file(shader, ASSETS_DIR "shaders/light/shader.vert",
                             ASSETS_DIR "shaders/light/shader.frag")) {
      shader_destroy(shader);
      return NULL;
   }
   return shader;
}
