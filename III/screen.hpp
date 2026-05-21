#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <ncurses.h>

class Screen {
public:
    Screen();
    ~Screen();

    int get_width();
    int get_height();
};

class Window {
    WINDOW * win;

public:
    Window(int h, int w, int y, int x);
    ~Window();

    template <typename ...Args>
    void print(int y, int x, const char * fmt, Args ...args) {
        mvwprintw(this->win, y, x, fmt, args...);
    }

    void refresh();
    void draw_box(chtype v, chtype h);
};

#endif
