#include <GLFW/glfw3.h>
#include <cglm/struct.h>
#include <cglm/struct/affine.h>
#include <glad/glad.h>

#include <stdlib.h>
#include <string.h>

#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "shader.h"
#include "window.h"
#include "game.h"

void processInput(struct Window *window, float deltaTime);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

struct Mesh *createAxesMesh();
struct Shader *createAxesShader();
struct Model *createCubeModel();
struct Shader *createModelShader();

/* todo: deprecate this in favour of renderModel */
void drawAxes(struct Mesh *axesMesh, struct Shader *axesShader, mat4s model, mat4s view, mat4s projection);

int main() {
    struct GameData game = { NULL };
    // return gameRun();
    game.window = windowCreate(1550.0f, 700.0f, "floating", (vec4s) { 0.0f, 0.0f, 0.0f, 1.0 });
    if (!game.window) return EXIT_FAILURE;
    if (windowSetIcon(game.window, ASSETS_DIR "logo.png")) return EXIT_FAILURE;

    game.camera = cameraCreate();
    if (!game.camera) return EXIT_FAILURE;
    cameraAdjustDirection(game.camera);

    glfwSetCursorPosCallback(game.window->window, mouseCallback);
    glfwSetScrollCallback(game.window->window, scrollCallback);
    glfwSetWindowUserPointer(game.window->window, &game);

    struct Mesh *axesMesh = createAxesMesh();
    struct Shader *axesShader = createAxesShader();
    if (!axesMesh || !axesShader) return EXIT_FAILURE;

    struct Model *cube = createCubeModel();
    struct Shader *cubeShader = createModelShader();
    if (!cube || !cubeShader) return EXIT_FAILURE;

    while (!windowClose(game.window)) {
        float currentFrame = glfwGetTime();
        game.deltaTime = currentFrame - game.lastFrame;
        game.lastFrame = currentFrame;

        windowPollEvents(game.window);
        windowProcessInput(game.window);
        windowClearColor(game.window);
        processInput(game.window, game.deltaTime);

        /* model matrix for light source */
        vec3s lightpos = { 1.2f, 1.0f, 2.0f };
        vec3s lightscale = { 0.2f, 0.2f, 0.2f };

        mat4s model = { .raw = GLM_MAT4_IDENTITY_INIT };
        mat4s view = cameraGetViewMatrix(game.camera);
        mat4s projection = glms_perspective(glm_rad(game.camera->fov), (float) game.window->width / (float) game.window->height, 0.1f, 100.0f);

        model = glms_translate(model, lightpos);
        model = glms_scale(model, lightscale);

        /* without the render call below, why are axes not being drawn and instead cube's vertices are being drawn */
        drawAxes(axesMesh, axesShader, (mat4s) {}, view, projection);

        {
            /* render cube */
            cube->model = (mat4s) { .raw = GLM_MAT4_IDENTITY_INIT };
            cube->view = view;
            cube->projection = projection;
            glUseProgram(cubeShader->program);
            shaderSetUniform(cubeShader, "model", Matrix4fv, 1, GL_FALSE, &cube->model.col[0].raw[0]);
            shaderSetUniform(cubeShader, "view", Matrix4fv, 1, GL_FALSE, &cube->view.col[0].raw[0]);
            shaderSetUniform(cubeShader, "projection", Matrix4fv, 1, GL_FALSE, &cube->projection.col[0].raw[0]);
            shaderSetUniform(cubeShader, "object_color", 3fv, 1, (vec3s) { 1.0f, 0.5f, 0.31f }.raw);
            shaderSetUniform(cubeShader, "light_color", 3fv, 1, (vec3s) { 1.0f, 1.0f, 1.0f }.raw);
            for (int i = 0; i < cube->meshCount; ++i) {
                const struct Mesh *mesh = cube->mesh[i];
                glBindVertexArray(mesh->vao);
                if (mesh->indexCount) {
                    glDrawElements(mesh->drawMode, mesh->indexCount, mesh->indexType, NULL);
                } else {
                    glDrawArrays(mesh->drawMode, 0, mesh->vertexCount);
                }
            }
        }

        {
            /* render cube */
            cube->view = view;
            cube->projection = projection;
            cube->model = model;
            glUseProgram(cubeShader->program);
            shaderSetUniform(cubeShader, "model", Matrix4fv, 1, GL_FALSE, &cube->model.col[0].raw[0]);
            shaderSetUniform(cubeShader, "view", Matrix4fv, 1, GL_FALSE, &cube->view.col[0].raw[0]);
            shaderSetUniform(cubeShader, "projection", Matrix4fv, 1, GL_FALSE, &cube->projection.col[0].raw[0]);
            shaderSetUniform(cubeShader, "object_color", 3fv, 1, (vec3s) { 1.0f, 1.0f, 1.0f }.raw);
            shaderSetUniform(cubeShader, "light_color", 3fv, 1, (vec3s) { 1.0f, 1.0f, 1.0f }.raw);
            for (int i = 0; i < cube->meshCount; ++i) {
                const struct Mesh *mesh = cube->mesh[i];
                glBindVertexArray(mesh->vao);
                if (mesh->indexCount) {
                    glDrawElements(mesh->drawMode, mesh->indexCount, mesh->indexType, NULL);
                } else {
                    glDrawArrays(mesh->drawMode, 0, mesh->vertexCount);
                }
            }
        }

        windowSwapBuffers(game.window);
    }

    windowDestroy(game.window);
    cameraDestroy(game.camera);
    shaderDestroy(axesShader);
    meshDestroy(axesMesh);

    return 0;
}

void processInput(struct Window *window, float delta_time) {
    struct GameData *data = glfwGetWindowUserPointer(window->window);
    if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
        cameraProcessKeyboard(data->camera, CAMERA_DIRECTION_FORWARD, delta_time);
    if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
        cameraProcessKeyboard(data->camera, CAMERA_DIRECTION_BACKWARD, delta_time);
    if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
        cameraProcessKeyboard(data->camera, CAMERA_DIRECTION_LEFT, delta_time);
    if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
        cameraProcessKeyboard(data->camera, CAMERA_DIRECTION_RIGHT, delta_time);
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
    struct GameData *data = glfwGetWindowUserPointer(window);
    cameraProcessMouseMovement(data->camera, (float) xpos, (float) ypos, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    struct GameData *data = glfwGetWindowUserPointer(window);
    cameraProcessMouseScroll(data->camera, (float) yoffset);
}

struct Mesh *createAxesMesh() {
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

    struct Mesh *axes_mesh = meshCreate();
    if (!axes_mesh) return NULL;

    meshLoadVertices(axes_mesh, &vertices[0][0][0][0], count, 3 * sizeof(float));
    return axes_mesh;
}

struct Shader *createAxesShader() {
    struct Shader *axes_shader = shaderCreate();
    if (!axes_shader)
        return NULL;
    if (shaderLoadFromFile(axes_shader, ASSETS_DIR "shaders/lines/shader.vert", ASSETS_DIR "shaders/lines/shader.frag")) {
        shaderDestroy(axes_shader);
        return NULL;
    }
    return axes_shader;
}

void drawAxes(struct Mesh *axesMesh, struct Shader *axesShader, mat4s model, mat4s view, mat4s projection) {
    glUseProgram(axesShader->program);
    shaderSetUniform(axesShader, "view", Matrix4fv, 1, GL_FALSE, &view.col[0].raw[0]);
    shaderSetUniform(axesShader, "projection", Matrix4fv, 1, GL_FALSE, &projection.col[0].raw[0]);

    glBindVertexArray(axesMesh->vao);
    glDrawArrays(GL_LINES, 0, axesMesh->vertexCount);
}

struct Model *createCubeModel() {
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
    struct Model *model = modelCreate();
    if (!model) return NULL;

    /* todo: test this malloc as well, or more like break these steps in the model loader itself */
    model->mesh = malloc(sizeof(struct Model *));
    model->meshCount = 1;

    *model->mesh = meshCreate();
    model->mesh[0]->drawMode = GL_TRIANGLES;
    meshLoadVertices(*model->mesh, vertices, 36, 3 * sizeof(float));
    return model;
}

struct Shader *createModelShader() {
    struct Shader *shader = shaderCreate();
    if (!shader)
        return NULL;
    if (shaderLoadFromFile(shader, ASSETS_DIR "shaders/model/shader.vert", ASSETS_DIR "shaders/model/shader.frag")) {
        shaderDestroy(shader);
        return NULL;
    }
    return shader;
}
