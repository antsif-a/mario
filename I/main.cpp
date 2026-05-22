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
    float horiz_speed;
};

TObject mario;
TObject *brick = nullptr; 
size_t   brick_len = 0;
int level = 1;
int score;
int maxLvl;

TObject *moving = nullptr;
size_t   moving_len = 0;

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
    obj->horiz_speed = 0.5;
}

bool is_collision(TObject o1, TObject o2) {
    return (((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) &&
            ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height)));
}

TObject * get_new_block() {
    brick_len++;
    if (brick == nullptr) {
        brick = new TObject[brick_len];
        return brick;
    }
    TObject * tmp = brick;
    brick = new TObject[brick_len];
    std::copy(tmp, tmp + brick_len, brick);
    return brick + brick_len - 1;
}

TObject * get_new_moving() {
    moving_len++;
    if (moving == nullptr) {
        moving = new TObject[moving_len];
        return moving;
    }
    TObject * tmp = moving;
    moving = new TObject[moving_len];
    std::copy(tmp, tmp + moving_len, moving);
    return moving + moving_len - 1;
}

void put_score_on_map() {
     char c[30];
     sprintf(c, "Score: %d", score);
     int len = strlen(c);
     for (int i = 0; i < len; ++i)
         Map[1][i+5] = c[i];
}

void create_level() {
    brick_len = 0;
    moving_len = 0;
    init_object(&mario, 39, 10, 3, 3, '@');
    score = 0;

    if (level == 1) {
        init_object(get_new_block(), 20, 20, 40, 5, '#');
            init_object(get_new_block(), 30, 10, 5, 3, '?');
            init_object(get_new_block(), 50, 10, 5, 3, '?');
        init_object(get_new_block(), 60, 15, 40, 10, '#');
            init_object(get_new_block(), 60, 5, 5, 3, '-');
            init_object(get_new_block(), 70, 5, 5, 3, '?');
            init_object(get_new_block(), 75, 5, 5, 3, '-');
            init_object(get_new_block(), 80, 5, 5, 3, '?');
            init_object(get_new_block(), 85, 5, 10, 3, '-');

        init_object(get_new_block(), 100, 20, 20, 5, '#');
        init_object(get_new_block(), 120, 15, 10, 10, '#');
        init_object(get_new_block(), 150, 20, 40, 5, '#');
        init_object(get_new_block(), 210, 15, 10, 10, '+');

        init_object(get_new_moving(), 25, 10, 3, 2, 'o');
        init_object(get_new_moving(), 80, 10, 3, 2, 'o');
    } else if (level == 2) {
        init_object(get_new_block(), 20, 20, 40, 5, '#');
        init_object(get_new_block(), 60, 15, 10, 10, '#');
        init_object(get_new_block(), 80, 20, 20, 5, '#');
        init_object(get_new_block(), 120, 15, 10, 10, '#');
        init_object(get_new_block(), 150, 20, 40, 5, '#');
        init_object(get_new_block(), 210, 15, 10, 10, '+');
        
        init_object(get_new_moving(), 25, 10, 3, 2, 'o');
        init_object(get_new_moving(), 80, 10, 3, 2, 'o');
        init_object(get_new_moving(), 65, 10, 3, 2, 'o');
        init_object(get_new_moving(), 120, 10, 3, 2, 'o');
        init_object(get_new_moving(), 160, 10, 3, 2, 'o');
        init_object(get_new_moving(), 175, 10, 3, 2, 'o');
    } else if (level == 3) {
        init_object(get_new_block(), 20, 20, 40, 5, '#');
        init_object(get_new_block(), 80, 20,  15, 5, '#');
        init_object(get_new_block(), 120, 15, 15, 10, '#');
        init_object(get_new_block(), 160, 10, 15, 15, '+');

        moving_len = 0;
        init_object(get_new_moving(), 25, 10, 3, 2, 'o');
        init_object(get_new_moving(), 50, 10, 3, 2, 'o');
        init_object(get_new_moving(), 80, 10, 3, 2, 'o');
        init_object(get_new_moving(), 90, 10, 3, 2, 'o');
        init_object(get_new_moving(), 120, 10, 3, 2, 'o');
        init_object(get_new_moving(), 130, 10, 3, 2, 'o');
    }

    maxLvl = 3;
}

void player_dead() {
    napms(500);
    create_level();
}

void delete_moving(int i) {
    moving_len--;
    moving[i] = moving[moving_len];
    TObject * tmp = moving;
    moving = new TObject[moving_len];
    std::copy(tmp, tmp + moving_len, moving);
    delete[] tmp;
}

void mario_collision() {
    for (size_t i = 0; i < moving_len; ++i) {
        if (is_collision(mario, moving[i])) {
            if (moving[i].cType == 'o') {
                if (mario.is_fly && mario.vert_speed > 0 && mario.y + mario.height < moving[i].y + moving[i].height * 0.5) {
                    delete_moving(i);
                    score += 50;
                    i--;
                    continue;
                } else {
                    player_dead();
                }
            }
            if (moving[i].cType == '$') {
                delete_moving(i);
                score += 100;
                i--;
                continue;
            }
        }
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
    for (size_t i = 0; i < moving_len; ++i)
        moving[i].x += dx;
}

void vert_move_object(TObject * obj) {
    obj->is_fly = true;
    obj->vert_speed += 0.05;
    set_object_pos(obj, obj->x, obj->y + obj->vert_speed);
    for (size_t i = 0; i < brick_len; ++i) {
        if (is_collision(*obj, brick[i])) {
            if (obj->vert_speed >0)
                obj->is_fly = false;
            if ((brick[i].cType == '?') && (obj->vert_speed < 0) && (obj == &mario)) {
                brick[i].cType = '-';
                init_object(get_new_moving(), brick[i].x, brick[i].y - 3, 3, 2, '$');
                moving[moving_len - 1].vert_speed = -0.7;
            }

            obj->y -= obj->vert_speed;
            obj->vert_speed = 0;
            obj->is_fly = false;
            if (brick[i].cType == '+') {
                ++level;
                if (level > maxLvl)
                    level = 1;
                napms(500);
                create_level();
            }
            break;
        }
    }
}

bool is_pos_in_map(int x, int y) {
    return ((x >= 0) && (x < map_width) && (y >= 0) && (y < map_height));
}

void horizon_move_object(TObject *obj) {
    obj->x += obj->horiz_speed;
    for (size_t i = 0; i < brick_len; ++i) {
        if (is_collision(*obj, brick[i])) {
            obj->x -= obj->horiz_speed;
            obj->horiz_speed = -obj->horiz_speed;
            return;
        }
    }

    if (obj->cType == 'o') {
        TObject tmp = *obj;
        vert_move_object(&tmp);
        if (tmp.is_fly) {
            obj->x -= obj->horiz_speed;
            obj->horiz_speed = -obj->horiz_speed;
        }
    }
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

#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <fcntl.h>

unordered_map<int, int> keys = {};
enum KeyState {
    KEY_RELEASE,
    KEY_PRESS,
    KEY_REPEAT
};

int main() {
    int input_fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);
    if (input_fd < 0) {
        println(stderr, "open /dev/input/event2: {}", strerror(errno));
        return 1;
    }
    
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, true);
    keypad(stdscr, true);

    create_level();

    input_event ev;
    for (;;) {
        while (read(input_fd, &ev, sizeof ev) != -1) {
            if (ev.type != 1)
                continue;
            if (ev.code == KEY_SPACE && ev.type == 1 && !mario.is_fly)
                mario.vert_speed = -1;
            keys[ev.code] = ev.value;
        }

        if (keys[KEY_SPACE] == KEY_PRESS && !mario.is_fly)
            mario.vert_speed = -1;
        if (keys[KEY_A] == KEY_PRESS || keys[KEY_A] == KEY_REPEAT)
            horizon_move_map(1);
        if (keys[KEY_D] == KEY_PRESS || keys[KEY_D] == KEY_REPEAT)
            horizon_move_map(-1);
        if (keys[KEY_Q] == KEY_PRESS)
            break;

        if (mario.y > map_height)
            player_dead();
        vert_move_object(&mario);
        mario_collision();

        clear_map();

        put_object_on_map(mario);
        for (size_t i = 0; i < brick_len; ++i)
            put_object_on_map(brick[i]);
        for (size_t i = 0; i < moving_len; ++i) {
            vert_move_object(&moving[i]);
            horizon_move_object(&moving[i]);
            if (moving[i].y > map_height) {
                delete_moving(i);
                i--;
                continue;
            }
            put_object_on_map(moving[i]);
        }
        put_score_on_map();
        show_map();
        refresh();
        napms(16);
    };

    endwin();
    return 0;
}


