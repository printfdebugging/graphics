#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stb/stb_image.h>

#include "game.h"
#include "window.h"
#include "logger.h"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"
#include <dwmapi.h>
#endif

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
static bool __mswIsDarkMode() {
    HINSTANCE uxthemelib = LoadLibraryExW(L"uxtheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!uxthemelib) {
        fprintf(stderr, "failed to open uxtheme.dll\n");
        return true; /* default to dark mode */
    }

    bool use_dark_mode = GetProcAddress(uxthemelib, MAKEINTRESOURCEA(132))();
    FreeLibrary(uxthemelib);
    return use_dark_mode;
}
#endif

static void __windowFrameBufferResizeCallback(GLFWwindow *window, int width, int height) {
    struct GameData *data = glfwGetWindowUserPointer(window);
    data->window->width = width;
    data->window->height = height;

    glViewport(0, 0, width, height);
}

struct Window *windowCreate(unsigned int width, unsigned int height, const char *title, vec4s color) {
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

    /* use a dark titlebar on windows in dark mode. */
#ifdef _WIN32
    HWND hwnd = glfwGetWin32Window(window);
    DWORD value = __mswIsDarkMode();
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
#endif

    glfwSetFramebufferSizeCallback(window, __windowFrameBufferResizeCallback);
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

    struct Window *win = malloc(sizeof(struct Window));
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
    win->icon = NULL;
    return win;
}

void windowSetClearColor(struct Window *window, vec4s color) {
    window->color = color;
}

void windowProcessInput(struct Window *window) {
    if (glfwGetKey(window->window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
        glfwSetWindowShouldClose(window->window, GLFW_TRUE);
}

void windowPollEvents(struct Window *window) {
    (void) window;
    glfwPollEvents();
}

void windowClearColor(struct Window *window) {
    (void) window;
    glClearColor(window->color.r, window->color.g, window->color.b, window->color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void windowSwapBuffers(struct Window *window) {
    glfwSwapBuffers(window->window);
}

void windowDestroy(struct Window *window) {
    glfwDestroyWindow(window->window);
    glfwTerminate();
    free(window);
}

bool windowClose(struct Window *window) {
    if (!window->window)
        return GL_TRUE;
    return glfwWindowShouldClose(window->window);
}

int windowSetIcon(struct Window *window, const char *path) {
    GLFWimage image;
    int image_channel_count;
    image.pixels = stbi_load(path, &image.width, &image.height, &image_channel_count, 0);
    if (!image.pixels) {
        fprintf(stderr, "failed to read window icon: %s", path);
        return 1;
    }

    glfwSetWindowIcon(window->window, 1, &image);
    window->icon = path;

    stbi_image_free(image.pixels);
    return 0;
}
