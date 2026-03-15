#include "glad/glad.h"

#include "window.h"
#include "logger.h"

#include <stdlib.h>
#include <string.h>

static void __window_frame_buffer_resize_callback(GLFWwindow *window, int width, int height) {
    (void) window;
    glViewport(0, 0, width, height);
}

struct window *window_create(unsigned int width, unsigned int height, const char *title, vec4s color) {
    if (!glfwInit()) {
        fprintf(stderr, "failed to initialize glfw");
        return NULL;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow((int) width, (int) height, title, NULL, NULL);
    if (!window) {
        fprintf(stderr, "failed to create glfw winodw\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "failed to initialize glad\n");
        glfwTerminate();
        return NULL;
    }

    glfwSetFramebufferSizeCallback(window, __window_frame_buffer_resize_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glLineWidth(2);

    // TODO: this is explained in the blending section
    // TODO: enable them i know what they do
    // https://learnopengl.com/Advanced-OpenGL/Blending.
    // glEnable(GL_CULL_FACE);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    struct window *win = malloc(sizeof(struct window));
    if (!win) {
        fprintf(stderr, "failed to initialize glad\n");
        glfwTerminate();
        return NULL;
    }

    win->width = width;
    win->height = height;
    win->title = title;
    win->color = color;
    win->window = window;
    return win;
}

void window_set_clear_color(struct window *window, vec4s color) {
    window->color = color;
}

void window_process_input(struct window *window) {
    if (glfwGetKey(window->window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
        glfwSetWindowShouldClose(window->window, GLFW_TRUE);
}

void window_poll_events(struct window *window) {
    (void) window;
    glfwPollEvents();
}

void window_clear_color(struct window *window) {
    (void) window;
    glClearColor(window->color.r, window->color.g, window->color.b, window->color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void window_swap_buffers(struct window *window) {
    glfwSwapBuffers(window->window);
}

void window_destroy(struct window *window) {
    glfwDestroyWindow(window->window);
    glfwTerminate();
    free(window);
}

bool window_close(struct window *window) {
    if (!window->window)
        return GL_TRUE;
    return glfwWindowShouldClose(window->window);
}
