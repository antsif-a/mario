#include <ncurses.h>
#include <string.h>
#include <cmath>

#include <object.hpp>

static const int map_width = 80;
static const int map_height = 30;

static char Map[map_height][map_width + 1];

void clear_map() {
    for (int i = 0; i < map_width; ++i)
        Map[0][i] = ' ';
    Map[0][map_width] = '\0';
    for (int j = 0; j < map_height; ++j)
        sprintf(Map[j], "%s", Map[0]);
}

void show_map(WINDOW * window) {
    Map[map_height - 1][map_width - 1] = '\0';
    for (int j = 0; j < map_height; ++j)
        mvwprintw(window, j, 0, "%s", Map[j]);
}

bool is_pos_in_map(int x, int y) {
    return ((x >= 0) && (x < map_width) && (y >= 0) && (y < map_height));
}

void put_object_on_map(TObject &obj) {
    int ix = round(obj.x);
    int iy = round(obj.y);
    int iw = round(obj.width);
    int ih = round(obj.height);
    for (int i = ix; i < (ix + iw); ++i)
        for (int j = iy; j < (iy + ih); ++j)
            if (is_pos_in_map(i, j))
                Map[j][i] = obj.cType;
}

unsigned int get_map_width() {
    return map_width;
}

unsigned int get_map_height() {
    return map_height;
}
