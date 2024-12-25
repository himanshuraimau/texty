#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>

struct termios orig_termios; // a struct that stores the original terminal attributes

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // set the terminal attributes to the original terminal attributes
}

void enableRawMode()
{
    struct termios raw;     // a struct that stores the terminal attributes
    atexit(disableRawMode); // at exit, disable the raw mode

    raw = orig_termios; // copy the original terminal attributes to raw

    tcgetattr(STDIN_FILENO, &raw); // get the terminal attributes
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // turn off the echo and canonical mode

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // set the terminal attributes in the terminal
}

int main()
{
    enableRawMode(); // enable the raw mode
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
    {
        if (iscntrl(c))
        {
            printf("%d\n", c);
        }
        else
        {
            printf("%d ('%c')\n", c, c);
        }
    }; // read the input from the terminal and print it until the input is 'q'
    return 0;
}