#include <bits/stdc++.h>
#include <ncurses.h>

using namespace std;

constexpr int map_width = 80;
constexpr int map_height = 25;

struct TObject {
    float x, y;
    float width, height;
    float vert_speed;
    bool is_fly;
    char cType;
};

TObject mario;
TObject *brick = nullptr; 
size_t   brick_len = 0;
int level = 1;

char Map[map_height][map_width + 1];

void clear_map() {
    for (int i = 0; i < map_width; ++i)
        Map[0][i] = ' ';
    Map[0][map_width] = '\0';
    for (int j = 0; j < map_height; ++j)
        sprintf(Map[j], "%s", Map[0]);
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

void init_object(TObject * obj, float x, float y, float w, float h, char inType) {
    set_object_pos(obj, x, y);
    obj->width = w;
    obj->height = h;
    obj->vert_speed = 0;
    obj->cType = inType;
}

bool is_collision(TObject o1, TObject o2) {
    return (((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) &&
            ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height)));
}

void create_level() {
    init_object(&mario, 39, 10, 3, 3, '@');

    if (brick != nullptr)
        delete[] brick;

    if (level == 1) {
        brick_len = 6;
        brick = new TObject[brick_len];
        init_object(brick+0, 20, 20, 40, 5, '#');
        init_object(brick+1, 60, 15, 10, 10, '#');
        init_object(brick+2, 80, 20, 20, 5, '#');
        init_object(brick+3, 120, 15, 10, 10, '#');
        init_object(brick+4, 150, 20, 40, 5, '#');
        init_object(brick+5, 210, 15, 10, 10, '+');
    } else if (level == 2) {
        brick_len = 4;
        brick = new TObject[brick_len];
        init_object(brick+0, 20, 20, 40, 5, '#');
        init_object(brick+1, 80, 20,  15, 5, '#');
        init_object(brick+2, 120, 15, 15, 10, '#');
        init_object(brick+3, 160, 10, 15, 15, '+');
    }
}

void horizon_move_map(float dx) {
    mario.x -= dx;
    for (size_t i = 0; i < brick_len; ++i)
        if (is_collision(mario, brick[i])) {
            mario.x += dx;
            return;
        }
    mario.x += dx;

    for (size_t i = 0; i < brick_len; ++i)
        brick[i].x += dx;
}

void vert_move_object(TObject * obj) {
    obj->is_fly = true;
    obj->vert_speed += 0.05;
    set_object_pos(obj, obj->x, obj->y + obj->vert_speed);
    for (size_t i = 0; i < brick_len; ++i) {
        if (is_collision(*obj, brick[i])) {
            obj->y = brick[i].y - obj->height;
            obj->vert_speed = 0;
            obj->is_fly = false;
            if (brick[i].cType == '+') {
                ++level;
                create_level();
                napms(1000);
            }
            break;
        }
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
                Map[j][i] = obj.cType;
}

int main() {
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, true);
    keypad(stdscr, true);

    create_level();

    char input;
    for (;;) {
        input = getch();
        if (input == ' ' && !mario.is_fly)
            mario.vert_speed = -1;
        if (input == 'a')
            horizon_move_map(1);
        if (input == 'd')
            horizon_move_map(-1);
        if (input == 'q')
            break;

        if (mario.y > map_height) create_level();
        vert_move_object(&mario);

        clear_map();

        put_object_on_map(mario);
        for (size_t i = 0; i < brick_len; ++i)
            put_object_on_map(brick[i]);
        show_map();
        refresh();
        napms(16);
    };

    endwin();
    return 0;
}
