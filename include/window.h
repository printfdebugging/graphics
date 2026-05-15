#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <cglm/struct.h>

struct Window {
        int         width;
        int         height;
        const char *title;
        const char *icon;
        GLFWwindow *window;
        vec4s       color;
};

struct Window *windowCreate(unsigned int width, unsigned int height, const char *title, vec4s color);

void windowSetClearColor(struct Window *window, vec4s color);
void windowProcessInput(struct Window *window);
void windowPollEvents(struct Window *window);
void windowClearColor(struct Window *window);
void windowSwapBuffers(struct Window *window);
void windowDestroy(struct Window *window);
bool windowClose(struct Window *window);
int  windowSetIcon(struct Window *window, const char *path);

#endif
