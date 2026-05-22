#include <vector>
#include <string>
#include <format>
#include <unordered_map>
#include <print>
#include <cmath>
#include <cstring>

#include <ncurses.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <fcntl.h>
#include <unistd.h>

constexpr int map_width = 80;
constexpr int map_height = 25;

struct TObject {
    float x, y;
    float width, height;
    char cType;

    float vert_speed = 0;
    float horiz_speed = 0.2;

    bool is_fly = false;
};

TObject mario;
std::vector<TObject> bricks;
int level = 1;
int score;
int maxLvl;

std::vector<TObject> moving;

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

bool is_collision(TObject &o1, TObject &o2) {
    return (((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) &&
            ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height)));
}

void put_score_on_map() {
     std::string score_fmt = std::format("Score: {}", score);
     std::copy(score_fmt.begin(), score_fmt.end(), &Map[1][5]);
}

void create_level() {
    mario = {39, 10, 3, 3, '@'};
    score = 0;

    bricks.clear();
    moving.clear();
    if (level == 1) {
        bricks.push_back({20, 20, 40, 5, '#'});
            bricks.push_back({30, 10, 5, 3, '?'});
            bricks.push_back({50, 10, 5, 3, '?'});
        bricks.push_back({60, 15, 40, 10, '#'});
            bricks.push_back({60, 5, 5, 3, '-'});
            bricks.push_back({70, 5, 5, 3, '?'});
            bricks.push_back({75, 5, 5, 3, '-'});
            bricks.push_back({80, 5, 5, 3, '?'});
            bricks.push_back({85, 5, 10, 3, '-'});

        bricks.push_back({100, 20, 20, 5, '#'});
        bricks.push_back({120, 15, 10, 10, '#'});
        bricks.push_back({150, 20, 40, 5, '#'});
        bricks.push_back({210, 15, 10, 10, '+'});

        moving.push_back({25, 10, 3, 2, 'o'});
        moving.push_back({80, 10, 3, 2, 'o'});
    } else if (level == 2) {
        bricks.push_back({20, 20, 40, 5, '#'});
        bricks.push_back({60, 15, 10, 10, '#'});
        bricks.push_back({80, 20, 20, 5, '#'});
        bricks.push_back({120, 15, 10, 10, '#'});
        bricks.push_back({150, 20, 40, 5, '#'});
        bricks.push_back({210, 15, 10, 10, '+'});
        
        moving.push_back({25, 10, 3, 2, 'o'});
        moving.push_back({80, 10, 3, 2, 'o'});
        moving.push_back({65, 10, 3, 2, 'o'});
        moving.push_back({120, 10, 3, 2, 'o'});
        moving.push_back({160, 10, 3, 2, 'o'});
        moving.push_back({175, 10, 3, 2, 'o'});
    } else if (level == 3) {
        bricks.push_back({20, 20, 40, 5, '#'});
        bricks.push_back({80, 20,  15, 5, '#'});
        bricks.push_back({120, 15, 15, 10, '#'});
        bricks.push_back({160, 10, 15, 15, '+'});

        moving.push_back({25, 10, 3, 2, 'o'});
        moving.push_back({50, 10, 3, 2, 'o'});
        moving.push_back({80, 10, 3, 2, 'o'});
        moving.push_back({90, 10, 3, 2, 'o'});
        moving.push_back({120, 10, 3, 2, 'o'});
        moving.push_back({130, 10, 3, 2, 'o'});
    }

    maxLvl = 3;
}

void player_dead() {
    napms(500);
    create_level();
}

void delete_moving(int i) {
    moving.erase(moving.begin() + i);
}

void mario_collision() {
    for (size_t i = 0; i < moving.size(); ++i) {
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
    for (auto &brick : bricks) {
        if (is_collision(mario, brick)) {
            mario.x += dx;
            return;
        }
    }
    mario.x += dx;

    for (auto &brick : bricks)
        brick.x += dx;
    for (auto &movin : moving)
        movin.x += dx;
}

void vert_move_object(TObject * obj) {
    obj->is_fly = true;
    obj->vert_speed += 0.05;
    obj->y += obj->vert_speed;
    for (size_t i = 0; i < bricks.size(); ++i) {
        if (is_collision(*obj, bricks[i])) {
            if (obj->vert_speed >0)
                obj->is_fly = false;
            if ((bricks[i].cType == '?') && (obj->vert_speed < 0) && (obj == &mario)) {
                bricks[i].cType = '-';
                moving.push_back({bricks[i].x, bricks[i].y - 3, 3, 2, '$'});
                moving.back().vert_speed = -0.7;
            }

            obj->y -= obj->vert_speed;
            obj->vert_speed = 0;
            obj->is_fly = false;
            if (bricks[i].cType == '+') {
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

void horizon_move_object(TObject &obj) {
    obj.x += obj.horiz_speed;
    for (size_t i = 0; i < bricks.size(); ++i) {
        if (is_collision(obj, bricks[i])) {
            obj.x -= obj.horiz_speed;
            obj.horiz_speed = -obj.horiz_speed;
            return;
        }
    }

    if (obj.cType == 'o') {
        TObject tmp = obj;
        vert_move_object(&tmp);
        if (tmp.is_fly) {
            obj.x -= obj.horiz_speed;
            obj.horiz_speed = -obj.horiz_speed;
        }
    }
}

void put_object_on_map(TObject &obj) {
    int ix = std::round(obj.x);
    int iy = std::round(obj.y);
    int iw = std::round(obj.width);
    int ih = std::round(obj.height);
    for (int i = ix; i < (ix + iw); ++i)
        for (int j = iy; j < (iy + ih); ++j)
            if (is_pos_in_map(i, j))
                Map[j][i] = obj.cType;
}

std::unordered_map<int, int> keys = {};
enum KeyState {
    KEY_RELEASE,
    KEY_PRESS,
    KEY_REPEAT
};

void create_screen() {
    initscr();
    curs_set(0);
}

void delete_screen() {
    endwin();
}

int main() {
    int input_fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);
    if (input_fd < 0) {
        std::println(stderr, "open /dev/input/event2: {}", std::strerror(errno));
        return 1;
    }
    
    create_screen();
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
        for (auto &o : bricks)
            put_object_on_map(o);

        for (size_t i = 0; i < moving.size(); ++i) {
            vert_move_object(&moving[i]);
            horizon_move_object(moving[i]);
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

    delete_screen();
    return 0;
}


