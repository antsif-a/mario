#include <bits/stdc++.h>
#include <ncurses.h>

using namespace std;

constexpr int map_width = 80;
constexpr int map_height = 25;

struct TObject {
    float x, y;
    float width, height;
    float vert_speed;
};

TObject mario;
TObject brick[1];

char Map[map_height][map_width + 1];

void clear_map() {
    for (int i = 0; i < map_width; ++i)
        Map[0][i] = '.';
    Map[0][map_width] = '\0';
    for (int j = 0; j < map_height; ++j)
        sprintf(Map[j], Map[0]);
}

void show_map() {
    Map[map_height - 1][map_width - 1] = '\0';
    for (int j = 0; j < map_height; ++j)
        mvprintw(j, 0, "%s", Map[j]);
}

void set_object_pos(TObject * obj, float x, float y) {
    obj->x = x;
    obj->y = y;
}

void init_object(TObject * obj, float x, float y, float w, float h) {
    set_object_pos(obj, x, y);
    obj->width = w;
    obj->height = h;
    obj->vert_speed = 0;
}

bool is_collision(TObject o1, TObject o2) {
    return ((o1.x + o1.width > o2.x) && (o1.x < o2.x + o2.width) &&
            (o1.y + o1.height > o2.y) && (o1.y < o2.y + o2.height));
}

void vert_move_object(TObject * obj, float dt = 0) {
    obj->vert_speed += 0.05;
    set_object_pos(obj, obj->x, obj->y + obj->vert_speed);
    if (is_collision(*obj, brick[0])) {
        obj->y = brick[0].y - obj->height;
        obj->vert_speed = 0;
    }
}

bool is_pos_in_map(int x, int y) {
    return ((x >= 0) && (x < map_width) && (y >= 0) && (y < map_height));
}

void put_object_on_map(TObject obj) {
    int ix = round(obj.x);
    int iy = round(obj.y);
    int iw = round(obj.width);
    int ih = round(obj.height);
    for (int i = ix; i < (ix + iw); ++i)
        for (int j = iy; j < (iy + ih); ++j)
            if (is_pos_in_map(i, j))
                Map[j][i] = '@';
}

int main() {
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, true);

    init_object(&mario, 39, 10, 3, 3);
    init_object(brick, 20, 20, 40, 5);

    for (;;) {
        if (getch() == ' ')
            mario.vert_speed = -0.9;
        vert_move_object(&mario);

        clear_map();
        put_object_on_map(mario);
        put_object_on_map(brick[0]);
        show_map();
        refresh();
        napms(16);
    };

    endwin();
    return 0;
}
