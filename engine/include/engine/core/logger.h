#ifndef LOGGER_H
#define LOGGER_H

/* a very basic logger for the engine, just logs out important things like errors etc */
/* note: this doesn't record/replay the events, that's something which the event system will do, but the logger can output the  data to a file to be shared and replayed */

void log_something();

#endif
