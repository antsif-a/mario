#include <ncurses.h>

#include <window.hpp>

Window::Window(WINDOW * win) : win(win) {}

void Window::draw_box(chtype v, chtype h) {
    box(this->win, v, h);
}

void Window::refresh() {
    wrefresh(this->win);
}

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

Window Screen::create_window(int h, int w, int y, int x) {
    return Window(newwin(h, w, y, x));
}
