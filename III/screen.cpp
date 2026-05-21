#include <ncurses.h>

#include <screen.hpp>

Screen::Screen() {
    initscr();
    curs_set(0);
}

Screen::~Screen() {
    endwin();
}

int Screen::get_width() {
    return getmaxx(stdscr);
}

int Screen::get_height() {
    return getmaxy(stdscr);
}

Window::Window(int h, int w, int y, int x) {
    this->win = newwin(h, w, y, x);
}

Window::~Window() {
    delwin(this->win);
}

void Window::refresh() {
    wrefresh(this->win);
}

void Window::draw_box(chtype v, chtype h) {
    box(this->win, v, h);
}
