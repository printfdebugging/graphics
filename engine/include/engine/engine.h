#ifndef ENGINE_STATE_H
#define ENGINE_STATE_H

#include "engine/window.h"

/* bare minimum state the engine needs from the application.
 * we can go take two routes from here:
 * - the engine_init and engine_deinit route
 *      - much more complicated
 *      - for this case, the engine manages it's own state
 *      - engine_state grows a lot and engine would have many subsystems
 *      - the application just consumes the api and the engine manages the lifetimes internally
 * - the engine as a part of the application route
 *      - much simpler
 *      - the engine has no state of it's own
 *      - engine_state only contains the minimal attributes various subsystems need from the application
 *      - the application manages everything, engine_state is essentially a container for passing data around
 *      - the application would have it's own state and engine_state would be the shared portion
 */

/* todo: improve the design */
/* todo: this will just be callbacks, no winodw * here. */
/* todo:  but each emitter of events will have a pointer to engine_interface or engine_bridge
 * (not state) so that it can pass events there... */
struct engine_state {
        /* this assumes that the engine/application has only one
         * window. it's fine for now, but later we would want to change
         * to something which provides us much more flexibility, because
         * here we need to now have the window wrapped in engine_state, or
         * else if we create a temporary engine_state and pass that to the
         * windowing subsystem, it will store pointers to it which will become
         * invalid after some time. */
        struct window *window;

        // add function pointers here and that alone would be enough.
};

// struct engine_state *engine_init(struct engine_state);
// struct engine_state *engine_shutdown(struct engine_state);

#endif
