#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
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
enum editor_key {
	KEY_ARROW_LEFT = 1000,
	KEY_ARROW_RIGHT,
	KEY_ARROW_DOWN,
	KEY_ARROW_UP,
	KEY_DEL,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,
	KEY_HOME,
	KEY_END,
};

struct abuf {
	char *b;
	int len;
};

void abuf_append(struct abuf *ab, const char *s, int len);
void abuf_free(struct abuf *ab);

typedef struct erow {
	int size;
	char *chars;
} erow;

struct editor_config {
	int cx, cy;
	int rowoff;
	int coloff;
	int screenrows;
	int screencols;
	int numrows;
	erow *row;
	struct termios orig_termios;
};

struct editor_config e;

/* terminal */
void enable_raw_mode();
void disable_raw_mode();
void die(const char *s);

void editor_init();
int editor_read_key();
void editor_process_keypress();
void editor_refresh_screen();
void editor_draw_rows(struct abuf *ab);
void editor_move_cursor(int key);
void editor_open(char *filename);
void editor_row_append(char *s, size_t len);
void editor_scroll();

int get_window_size(int *rows, int *cols);
int get_cursor_position(int *rows, int *cols);

status editor_initialize(struct editor_state *editor, int argc, char *argv[]) {
	status rc = status_success;
	enable_raw_mode();
	editor_init();
	if (argc >= 2)
		editor_open(argv[1]);
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

int editor_read_key() {
	int nread;
	char c;

	while ((nread = read(STDIN_FILENO, &c, 1) != 1)) {
		if (nread == -1 && errno != EAGAIN)
			die("read");
	}

	if (c == '\x1b') {
		char seq[3];
		if (read(STDIN_FILENO, &seq[0], 1) != 1)
			return '\x1b';
		if (read(STDIN_FILENO, &seq[1], 1) != 1)
			return '\x1b';
		if (seq[0] == '[') {
			if (seq[1] >= '0' && seq[1] <= '9') {
				if (read(STDIN_FILENO, &seq[2], 1) != 1)
					return '\x1b';
				if (seq[2] == '~') {
					switch (seq[1]) {
						case '1':
							return KEY_HOME;
						case '3':
							return KEY_DEL;
						case '4':
							return KEY_END;
						case '5':
							return KEY_PAGE_UP;
						case '6':
							return KEY_PAGE_DOWN;
						case '7':
							return KEY_HOME;
						case '8':
							return KEY_END;
					}
				}
			} else {
				switch (seq[1]) {
					case 'A':
						return KEY_ARROW_UP;
					case 'B':
						return KEY_ARROW_DOWN;
					case 'C':
						return KEY_ARROW_RIGHT;
					case 'D':
						return KEY_ARROW_LEFT;
					case 'H':
						return KEY_HOME;
					case 'F':
						return KEY_END;
				}
			}
		} else if (seq[0] == 'O') {
			switch (seq[1]) {
				case 'H':
					return KEY_HOME;
				case 'F':
					return KEY_END;
			}
		}

		return '\x1b';
	} else {
		return c;
	}
}

void editor_process_keypress() {
	int c = editor_read_key();
	switch (c) {
		case CTRL_KEY('q'): {
			write(STDOUT_FILENO, "\x1b[2J", 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
		} break;
		case KEY_HOME: {
			e.cx = 0;
		} break;
		case KEY_END: {
			e.cx = e.screencols - 1;
		} break;
		case KEY_PAGE_UP:
		case KEY_PAGE_DOWN: {
			int times = e.screenrows;
			while (times--)
				editor_move_cursor(c == KEY_PAGE_UP ? KEY_ARROW_UP : KEY_ARROW_DOWN);
		} break;
		case KEY_ARROW_LEFT:
		case KEY_ARROW_RIGHT:
		case KEY_ARROW_DOWN:
		case KEY_ARROW_UP: {
			editor_move_cursor(c);
		} break;
	}
}

void editor_refresh_screen() {
	editor_scroll();
	struct abuf ab = ABUF_INIT;

	abuf_append(&ab, "\x1b[?25l", 6);
	abuf_append(&ab, "\x1b[H", 3);

	editor_draw_rows(&ab);

	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (e.cy - e.rowoff) + 1, (e.cx - e.coloff) + 1);
	abuf_append(&ab, buf, (i32) strlen(buf));
	abuf_append(&ab, "\x1b[?25h", 6);

	write(STDOUT_FILENO, ab.b, (u32) ab.len);
	abuf_free(&ab);
}

void editor_draw_rows(struct abuf *ab) {
	int y;
	for (y = 0; y < e.screenrows; ++y) {
		int filerow = y + e.rowoff;
		if (filerow >= e.numrows) {
			if (e.numrows == 0 && y == e.screenrows / 3) {
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
		} else {
			int len = e.row[filerow].size - e.coloff;
			if (len < 0)
				len = 0;
			if (len > e.screencols)
				len = e.screencols;
			abuf_append(ab, &e.row[filerow].chars[e.coloff], len);
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

void editor_init() {
	e.cx = 0;
	e.cy = 0;
	e.rowoff = 0;
	e.coloff = 0;
	e.numrows = 0;
	e.row = NULL;

	if (get_window_size(&e.screenrows, &e.screencols) == -1)
		die("get_window_size");
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

void editor_move_cursor(int key) {
	switch (key) {
		case KEY_ARROW_LEFT:
			if (e.cx != 0)
				e.cx--;
			break;
		case KEY_ARROW_RIGHT:
			e.cx++;
			break;
		case KEY_ARROW_DOWN:
			if (e.cy < e.numrows)
				e.cy++;
			break;
		case KEY_ARROW_UP:
			if (e.cy != 0)
				e.cy--;
			break;
	}
}

void editor_open(char *filename) {
	FILE *fp = fopen(filename, "r");
	if (!fp)
		die("fopen");

	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;

	while ((linelen = getline(&line, &linecap, fp)) != -1) {
		while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
			linelen--;
		editor_row_append(line, (size_t) linelen);
	}
	free(line);
	fclose(fp);
}

void editor_row_append(char *s, size_t len) {
	e.row = realloc(e.row, sizeof(erow) * ((u32) e.numrows + 1));
	int at = e.numrows;
	e.row[at].size = (int) len;
	e.row[at].chars = malloc((u32) len + 1);
	memcpy(e.row[at].chars, s, (u32) len);
	e.row[at].chars[len] = '\0';
	e.numrows++;
}

/** An important thing to note here is that the cy is the location
 * of the cursor in the file & e.rowoff is the line number of the top
 * line visible in the window. so the math checks out that way. */
void editor_scroll() {
	if (e.cy < e.rowoff)
		e.rowoff = e.cy;
	if (e.cy >= e.rowoff + e.screenrows)
		e.rowoff = e.cy - e.screenrows + 1;
	if (e.cx < e.coloff)
		e.coloff = e.cx;
	if (e.cx >= e.coloff + e.screencols)
		e.coloff = e.cx - e.screencols + 1;
}
