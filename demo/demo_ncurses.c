//
// Created by v on 16-10-19.
//

#include <ncurses.h>

void start_1() {
    int ch;

    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();

    printw("Type any character to see it in bold \n");
    ch = getch();

    if (ch == KEY_F(1))
        printw("F1 Key Pressed");
    else {
        printw("The pressed key is ");
        attron(A_BOLD);
        printw("%c", ch);
        attroff(A_BOLD);
    }

    refresh();
    getch();
    endwin();
}

void start_0() {
    initscr();
    printw("Hello arges");
    refresh();
    getch();
    endwin();
}

int main() {
    start_1();

    return 0;
}