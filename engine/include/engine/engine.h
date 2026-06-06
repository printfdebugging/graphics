#ifndef ENGINE_STATE_H
#define ENGINE_STATE_H

#include "engine/core/defines.h"

struct window;

/** The `bridge` is a stateless interface for the engine subsystems
 * (abstractions) to send events/updates to the `client`. It's just
 * a bunch of function pointers which the client fills in and the
 * engine subsystems (like `struct window`) hold a reference to. The
 * lifetime of a bridge is managed by the client.
 *
 * When the term 'engine subsystem' is used, it just means a struct
 * and some functions to instanciate/use/destroy it. Most of this
 * code lives in the `engine/` directory. The purpose of separating
 * code into `engine/` and `projects/{name}` is to be able to share
 * the code between multiple projects. There does not exist any
 * `engine` object/instance/state at runtime.
 *
 * The `engine/` code doesn't assume what kind of application it
 * will be used for. Because if it did, then over time the engine
 * code will start accumulating assumptions for a certain type
 * of application which will make it brittle and thus hard to use
 * for other types of applications.
 *
 * All the state lives in a client data structure, say `game_state` or
 * `editor_state`. `struct bridge` is the first data member of such a
 * state struct. The engine subsystems on creation are passed a pointer
 * to this state instance which they store as `struct bridge *` and
 * when they want to pass some events to the client, they use this
 * bridge to call the appropriate callback.
 *
 * The first member of these callbacks i.e. `userdata` is actually this
 * bridge pointer from these subsystems which these callbacks convert to
 * the original client state type, thus getting access to the whole state.
 */
struct bridge {
        /** When the mouse moves over a window, this is the callback to
         * receive that event on the client side. x and y are the new
         * positions of the cursor on window with respect to it's top
         * left corner.
         */
        void (*mouse_move)(void *userdata, struct window *window, f64 x, f64 y);

        /** When the mouse wheel is scrolled, this is the callback to
         * receive that event on the client side. x and y being the offsets
         * along x and the y direction. Horizontal scrolling is done when
         * we scroll the wheel while holding the shift key.
         */
        void (*mouse_scroll)(void *userdata, struct window *window, f64 x, f64 y);

        /** When the window is resized, this is the callback to receive that
         * event on the client side. w and h being the new width and height
         * of the framebuffer.
         */
        void (*window_resize)(void *userdata, struct window *window, i32 w, i32 h);
};

#endif
