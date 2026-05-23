#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <ncurses.h>

class Window {
    WINDOW * win;
    public:
        Window(WINDOW * win);

        template <typename ...Args>
        void print(int y, int x, const char * fmt, Args ...args) {
            mvwprintw(this->win, y, x, fmt, args...);
        }

        void draw_box(chtype v = ACS_VLINE, chtype h = ACS_HLINE);
        void refresh();
};

class Screen {
    public:
        Screen();
        ~Screen();
        int get_width();
        int get_height();
        Window create_window(int h, int w, int y, int x);
};

#endif
