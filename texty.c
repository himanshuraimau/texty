/*** includes ***/
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f) // a macro that sets the control key

/*** data ***/
struct termios orig_termios; // a struct that stores the original terminal attributes

/*** terminal ***/
void die(const char *s)
{
    perror(s); // print the error message
    exit(1);   // exit the program
}

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr"); // set the terminal attributes to the original terminal attributes
}

void enableRawMode()
{
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode); // at exit, disable the raw mode
    // a struct that stores the terminal attributes

    struct termios raw = orig_termios; // copy the original terminal attributes to raw

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

/*** output ***/
void editorRefreshScreen() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
}

/*** input ***/
void editorProcessKeypress()
{
    char c = editorReadKey();
    switch (c)
    {
    case CTRL_KEY('q'):
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
    {   editorRefreshScreen();
        editorProcessKeypress();
    }; // read the input from the terminal and print it until the input is 'q'
    return 0;
}