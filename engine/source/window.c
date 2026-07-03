#include <stdio.h>
#include <stdlib.h>

#include "GLFW/glfw3.h"
#include "cglm/struct.h"
#include "glad/glad.h"
#include "stb_image.h"

#include "engine/engine.h"
#include "engine/window.h"
#include "engine/core/defines.h"

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <dwmapi.h>
#include "GLFW/glfw3native.h"
#endif

#ifdef _WIN32
static bool
__msw_is_dark_mode() {
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

static void __window_frame_buffer_resize_callback(GLFWwindow *window, i32 width, i32 height) {
	struct window *_window = glfwGetWindowUserPointer(window);
	_window->bridge->window_resize(_window->bridge, _window, width, height);
	/* maybe this should be moved to the client side code? */
	fprintf(stderr, "setting width, height: %i, %i\n", width, height);
	glViewport(0, 0, width, height);
}

static void __window_mouse_scroll_callback(GLFWwindow *window, f64 x, f64 y) {
	struct window *_window = glfwGetWindowUserPointer(window);
	_window->bridge->mouse_scroll(_window->bridge, _window, x, y);
}

static void __window_mouse_move_callback(GLFWwindow *window, f64 x, f64 y) {
	struct window *_window = glfwGetWindowUserPointer(window);
	_window->bridge->mouse_move(_window->bridge, _window, x, y);
}

status window_init(struct window *window, struct window opts) {
	status rc = status_success;
	if (opts.window) {
		rc = status_failure;
		goto cleanup;
	}

	if (!opts.bridge) {
		fprintf(stderr, "opts.bridge is required to create a window");
		rc = status_failure;
		goto cleanup;
	}

	if (!glfwInit()) {
		fprintf(stderr, "failed to initialize glfw");
		rc = status_failure;
		goto cleanup;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

	opts.window = glfwCreateWindow(opts.width, opts.height, opts.title, NULL, NULL);
	if (!opts.window) {
		fprintf(stderr, "failed to create glfw winodw\n");
		rc = status_failure;
		goto glfw_cleanup;
	}

	glfwMakeContextCurrent(opts.window);
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		fprintf(stderr, "failed to initialize glad\n");
		rc = status_failure;
		goto glfw_cleanup;
	}

	/* use a dark titlebar on windows in dark mode. */
#ifdef _WIN32
	HWND hwnd = glfwGetWin32Window(opts.window);
	DWORD value = __msw_is_dark_mode();
	DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
#endif

	if (!window_set_icon(&opts, opts.icon)) {
		fprintf(stderr, "failed to set window icon\n");
		rc = status_failure;
		goto glfw_cleanup;
	}

	glfwSetInputMode(opts.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2);

	// TODO: this is explained in the blending section
	// TODO: enable them i know what they do
	// https://learnopengl.com/Advanced-OpenGL/Blending.
	// glEnable(GL_CULL_FACE);
	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* todo: clean this up a bit, it's confusing/complicated */
	glfwSetWindowUserPointer(opts.window, window);
	glfwSetCursorPosCallback(opts.window, __window_mouse_move_callback);
	glfwSetScrollCallback(opts.window, __window_mouse_scroll_callback);
	glfwSetFramebufferSizeCallback(opts.window, __window_frame_buffer_resize_callback);

	*window = opts;
cleanup:
	return rc;

glfw_cleanup:
	glfwTerminate();
	return rc;
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

status window_close(struct window *window) {
	if (!window->window)
		return status_success;
	return (status) glfwWindowShouldClose(window->window);
}

status window_set_icon(struct window *window, const char *path) {
	GLFWimage image;
	int image_channel_count;
	image.pixels = stbi_load(path, &image.width, &image.height, &image_channel_count, 0);
	if (!image.pixels) {
		fprintf(stderr, "failed to read window icon: %s", path);
		return status_failure;
	}

	glfwSetWindowIcon(window->window, 1, &image);
	window->icon = path;

	stbi_image_free(image.pixels);
	return status_success;
}

status window_set_cursor_icon(struct window *window, const char *path, i32 size) {
	GLFWimage image;
	int image_channel_count;
	image.pixels = stbi_load(path, &image.width, &image.height, &image_channel_count, 0);
	if (!image.pixels) {
		fprintf(stderr, "failed to read window icon: %s", path);
		return status_failure;
	}

	GLFWcursor *cursor = glfwCreateCursor(&image, size, size);
	glfwSetCursor(window->window, cursor);

	stbi_image_free(image.pixels);
	return status_success;
}

void window_get_size(struct window *window, i32 *width, i32 *height) {
	glfwGetWindowSize(window->window, width, height);
	fprintf(stderr, "WindowSize: { .width = %i, .height = %i };\n", *width, *height);
}

void window_get_content_scale(struct window *window, f32 *xscale, f32 *yscale) {
	glfwGetWindowContentScale(window->window, xscale, yscale);
	fprintf(stderr, "WindowScale: { .xscale = %f, .yscale = %f };\n", *xscale, *yscale);
}

void window_get_framebuffer_size(struct window *window, i32 *width, i32 *height) {
	glfwGetFramebufferSize(window->window, width, height);
	fprintf(stderr, "FramebufferSize: { .width = %i, .height = %i };\n", *width, *height);
}

/* todo: use window to get the monitor it is on */
status window_get_monitor_dpi(struct window *window, i32 *dpi) {
	(void) window;
	GLFWmonitor *monitor = NULL;
	const GLFWvidmode *videomode = NULL;

	if (!(monitor = glfwGetPrimaryMonitor()) ||
	    !(videomode = glfwGetVideoMode(monitor))) {
		fprintf(stderr, "unable to get the GLFWmonitor or GLFWvidmode\n");
		return status_failure;
	}

	i32 width_mm, height_mm;
	glfwGetMonitorPhysicalSize(monitor, &width_mm, &height_mm);

	f32 width_in = (f32) width_mm / INCH;
	f32 pixels = (f32) videomode->width;
	*dpi = (i32) (pixels / width_in);

	fprintf(stderr, "monitor_size: { .width = %i, .height = %i, .dpi = %i };\n", width_mm, height_mm, *dpi);
	return status_success;
}

void window_get_monitor_scale(struct window *window, f32 *xscale, f32 *yscale) {
	GLFWmonitor *monitor = glfwGetWindowMonitor(window->window);
	glfwGetMonitorContentScale(monitor, xscale, yscale);
	fprintf(stderr, "MonitorScale: { .xscale = %f, .yscale = %f };\n", *xscale, *yscale);
}
