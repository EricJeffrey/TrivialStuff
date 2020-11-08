#if !defined(TERMINAL_CTL_H)
#define TERMINAL_CTL_H

#include <cstdlib>
#include <termios.h>
#include <unistd.h>

class TerminalCtl {
public:
    static termios ttyOrigin;
    // return -1 on error
    static int ttySetRaw(int fd);
    static void ttyReset();
    static int clearScreen();
};

inline int TerminalCtl::ttySetRaw(int fd) {
    /* [tlpi](https://www.man7.org/tlpi/code/online/dist/tty/tty_functions.c.html#ttySetRaw) */

    struct termios t;
    if (tcgetattr(fd, &t) == -1)
        return -1;

    ttyOrigin = t;

    t.c_lflag &= ~(ICANON | ECHONL | ISIG | IEXTEN | ECHO);
    /* Noncanonical mode, disable signals, extended
       input processing, and echoing */

    t.c_iflag &= ~(BRKINT | ICRNL | IGNBRK | IGNCR | INLCR | INPCK | ISTRIP | IXON | PARMRK);
    /* Disable special handling of CR, NL, and BREAK.
       No 8th-bit stripping or parity error handling.
       Disable START/STOP output flow control. */

    t.c_cflag &= ~(CSIZE | PARENB);
    t.c_cflag &= ~(CS8);

    t.c_oflag &= ~OPOST; /* Disable all output processing */

    t.c_cc[VMIN] = 1;  /* Character-at-a-time input */
    t.c_cc[VTIME] = 0; /* with blocking */
    if (tcsetattr(fd, TCSAFLUSH, &t) == -1)
        return -1;
    return 0;
}

inline void TerminalCtl::ttyReset() { tcsetattr(STDIN_FILENO, TCSANOW, &(ttyOrigin)); }

// return -1 on error
inline int TerminalCtl::clearScreen() {
    fprintf(stderr, "\033[2J\033[1;1H");
    char buf[] = "\033[2J\033[1;1H";
    // write may be interrupted
    const ssize_t totalNum = sizeof(buf) - 1;
    ssize_t curNum = 0;
    while (curNum < totalNum) {
        ssize_t tmpNum = write(STDOUT_FILENO, buf + curNum, totalNum - curNum);
        if (tmpNum == -1 || tmpNum == 0)
            return -1;
        else
            curNum += tmpNum;
    }
    return 0;
}
#endif // TERMINAL_CTL_H
