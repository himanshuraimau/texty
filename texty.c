/*** includes ***/
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <errno.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f) // a macro that sets the control key

/*** data ***/
struct editorConfig {
  struct termios orig_termios;
};
struct editorConfig E; // a global struct that stores the terminal attributes

/*** terminal ***/
void die(const char *s)
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s); // print the error message
    exit(1);   // exit the program
}

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr"); // set the terminal attributes to the original terminal attributes
}

void enableRawMode()
{
     if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
        die("tcgetattr");
    atexit(disableRawMode); // at exit, disable the raw mode
    // a struct that stores the terminal attributes

   struct termios raw = E.orig_termios;// copy the original terminal attributes to raw

    tcgetattr(STDIN_FILENO, &raw);                            // get the terminal attributes
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // turn off the input flags
    raw.c_iflag &= ~(IXON | ICRNL);                           // turn off the input flags
    raw.c_oflag &= ~(OPOST);                                  // turn off the output flags
    raw.c_cflag |= (CS8);                                     // set the character size to 8 bits
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);          // turn off the echo and canonical mode

    raw.c_cc[VMIN] = 0;  // set the minimum number of bytes of input needed before read() can return
    raw.c_cc[VTIME] = 1; // set the maximum amount of time to wait before read() returns

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr"); // set the terminal attributes in the terminal
}

char editorReadKey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }
    return c;
}

int getWindowSize(int *rows, int *cols) {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    return -1;
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/*** output ***/

void editorDrawRows()
{
    int y;
    for (y = 0; y < 24; y++)
    {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}

void editorRefreshScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}

/*** input ***/
void editorProcessKeypress()
{
    char c = editorReadKey();
    switch (c)
    {
    case CTRL_KEY('q'):
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;
    }
}

/*** init ***/
int main()
{
    enableRawMode(); // enable the raw mode
    char c;
    while (1)
    {
        editorRefreshScreen();
        editorProcessKeypress();
    }; // read the input from the terminal and print it until the input is 'q'
    return 0;
}