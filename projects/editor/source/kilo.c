#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "engine/core/defines.h"

#include "editor/editor.h"

/* defines */
#define CTRL_KEY(k)  ((k) & 0x1f)
#define ABUF_INIT    { NULL, 0 }
#define KILO_VERSION "0.0.1"

/* data */
struct abuf {
	char *b;
	int len;
};

void abuf_append(struct abuf *ab, const char *s, int len);
void abuf_free(struct abuf *ab);

struct editor_config {
	int cx, cy;
	int screenrows;
	int screencols;
	struct termios orig_termios;
};

struct editor_config e;

/* terminal */
void enable_raw_mode();
void disable_raw_mode();
void die(const char *s);

char editor_read_key();
void editor_process_keypress();
void editor_refresh_screen();
void editor_draw_rows(struct abuf *ab);
void editor_move_cursor(char key);

int get_window_size(int *rows, int *cols);
int get_cursor_position(int *rows, int *cols);

void init_editor();

status editor_initialize(struct editor_state *editor, int argc, char *argv[]) {
	status rc = status_success;
	enable_raw_mode();
	init_editor();
	return rc;
}

status editor_run(struct editor_state *editor) {
	status rc = status_success;
	while (true) {
		editor_refresh_screen();
		editor_process_keypress();
	}

	return rc;
}

status editor_shutdown(struct editor_state *editor) {
	status rc = status_success;
	return rc;
}

void enable_raw_mode() {
	if (tcgetattr(STDIN_FILENO, &e.orig_termios) == -1)
		die("tcgetattr");

	atexit(disable_raw_mode);

	struct termios raw = e.orig_termios;
	raw.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); /* read input byte by byte */
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
		die("tcsetattr");
}

void disable_raw_mode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &e.orig_termios) == -1)
		die("tcsetattr");
}

void die(const char *s) {
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}

char editor_read_key() {
	int nread;
	char c;

	while ((nread = read(STDIN_FILENO, &c, 1) != 1)) {
		if (nread == -1 && errno != EAGAIN)
			die("read");
	}

	return c;
}

void editor_process_keypress() {
	char c = editor_read_key();
	switch (c) {
		case CTRL_KEY('q'): {
			write(STDOUT_FILENO, "\x1b[2J", 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;
		}
		case 'h':
		case 'l':
		case 'j':
		case 'k': {
			editor_move_cursor(c);
			break;
		}
	}
}

void editor_refresh_screen() {
	struct abuf ab = ABUF_INIT;

	abuf_append(&ab, "\x1b[?25l", 6);
	abuf_append(&ab, "\x1b[H", 3);

	editor_draw_rows(&ab);

	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", e.cy + 1, e.cx + 1);
	abuf_append(&ab, buf, (i32) strlen(buf));
	abuf_append(&ab, "\x1b[?25h", 6);

	write(STDOUT_FILENO, ab.b, (u32) ab.len);
	abuf_free(&ab);
}

void editor_draw_rows(struct abuf *ab) {
	int y;
	for (y = 0; y < e.screenrows; ++y) {
		if (y == e.screenrows / 3) {
			char welcome[80];
			int welcomelen = snprintf(welcome, sizeof(welcome), "Kilo editor -- version %s", KILO_VERSION);
			if (welcomelen > e.screencols)
				welcomelen = e.screencols;

			int padding = (e.screencols - welcomelen) / 2;
			if (padding) {
				abuf_append(ab, "~", 1);
				padding--;
			}

			while (padding--) {
				abuf_append(ab, " ", 1);
			}
			abuf_append(ab, welcome, welcomelen);
		} else {
			abuf_append(ab, "~", 1);
		}
		abuf_append(ab, "\x1b[K", 3);
		if (y < e.screenrows - 1)
			abuf_append(ab, "\r\n", 2);
	}
}

int get_window_size(int *rows, int *cols) {
	struct winsize ws;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
		if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
			return -1;
		return get_cursor_position(rows, cols);
	} else {
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}

int get_cursor_position(int *rows, int *cols) {
	char buf[32];
	u32 i = 0;
	if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
		return -1;

	while (i < sizeof(buf) - 1) {
		if (read(STDIN_FILENO, &buf[i], 1) != 1)
			break;
		if (buf[i] == 'R')
			break;
		i++;
	}

	buf[i] = '\0';
	if (buf[0] != '\x1b' || buf[1] != '[')
		return -1;
	if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
		return -1;
	return 0;
}

void init_editor() {
	e.cx = 0;
	e.cy = 0;
	if (get_window_size(&e.screenrows, &e.screencols) == -1) {
		die("get_window_size");
	}
}

void abuf_append(struct abuf *ab, const char *s, int len) {
	char *new = realloc(ab->b, (u32) ab->len + (u32) len);
	if (new == NULL)
		return;

	memcpy(&new[ab->len], s, (u32) len);
	ab->b = new;
	ab->len += len;
}

void abuf_free(struct abuf *ab) {
	free(ab->b);
}

void editor_move_cursor(char key) {
	switch (key) {
		case 'h':
			e.cx--;
			break;
		case 'l':
			e.cx++;
			break;
		case 'j':
			e.cy++;
			break;
		case 'k':
			e.cy--;
			break;
	}
}
