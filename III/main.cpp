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

#include <object.hpp>
#include <input.hpp>

const int map_width = 80;
const int map_height = 25;

const int mario_x = 39;
const int mario_y = 10;
Player mario = Player(mario_x, mario_y, 3, 3);
std::vector<GameObject> bricks;
int level = 1;
int score;
int maxLvl;

std::vector<DynamicGameObject> moving;

char Map[map_height][map_width + 1];

bool is_pos_in_map(int x, int y) {
    return ((x >= 0) && (x < map_width) && (y >= 0) && (y < map_height));
}

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

void put_score_on_map() {
     std::string score_fmt = std::format("Score: {}", score);
     std::copy(score_fmt.begin(), score_fmt.end(), &Map[1][5]);
}

void add_moving(float x, float y, float w, float h, char type) {
    DynamicGameObject obj(x, y, w, h, type);
    obj.horiz_speed = 0.5f;
    moving.push_back(obj);
}

void add_brick(float x, float y, float w, float h, char type) {
    GameObject obj(x, y, w, h, type);
    bricks.push_back(obj);
}

void create_level() {
    score = 0;
    mario.x = mario_x;
    mario.y = mario_y;

    bricks.clear();
    moving.clear();
    if (level == 1) {
        add_brick(20, 20, 40, 5, '#');
            add_brick(30, 10, 5, 3, '?');
            add_brick(50, 10, 5, 3, '?');
        add_brick(60, 15, 40, 10, '#');
            add_brick(60, 5, 5, 3, '-');
            add_brick(70, 5, 5, 3, '?');
            add_brick(75, 5, 5, 3, '-');
            add_brick(80, 5, 5, 3, '?');
            add_brick(85, 5, 10, 3, '-');

        add_brick(100, 20, 20, 5, '#');
        add_brick(120, 15, 10, 10, '#');
        add_brick(150, 20, 40, 5, '#');
        add_brick(210, 15, 10, 10, '+');

        add_moving(25, 10, 3, 2, 'o');
        add_moving(80, 10, 3, 2, 'o');
    } else if (level == 2) {
        add_brick(20, 20, 40, 5, '#');
        add_brick(60, 15, 10, 10, '#');
        add_brick(80, 20, 20, 5, '#');
        add_brick(120, 15, 10, 10, '#');
        add_brick(150, 20, 40, 5, '#');
        add_brick(210, 15, 10, 10, '+');
        
        add_moving(25, 10, 3, 2, 'o');
        add_moving(80, 10, 3, 2, 'o');
        add_moving(65, 10, 3, 2, 'o');
        add_moving(120, 10, 3, 2, 'o');
        add_moving(160, 10, 3, 2, 'o');
        add_moving(175, 10, 3, 2, 'o');
    } else if (level == 3) {
        add_brick(20, 20, 40, 5, '#');
        add_brick(80, 20,  15, 5, '#');
        add_brick(120, 15, 15, 10, '#');
        add_brick(160, 10, 15, 15, '+');

        add_moving(25, 10, 3, 2, 'o');
        add_moving(50, 10, 3, 2, 'o');
        add_moving(80, 10, 3, 2, 'o');
        add_moving(90, 10, 3, 2, 'o');
        add_moving(120, 10, 3, 2, 'o');
        add_moving(130, 10, 3, 2, 'o');
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
        if (mario.collides_with(moving[i])) {
            if (moving[i].type == 'o') {
                if (mario.is_fly && mario.vert_speed > 0 && mario.y + mario.height < moving[i].y + moving[i].height * 0.5) {
                    delete_moving(i);
                    score += 50;
                    i--;
                    continue;
                } else {
                    player_dead();
                }
            }
            if (moving[i].type == '$') {
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
        if (mario.collides_with(brick)) {
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

void vert_move_object(DynamicGameObject * obj) {
    obj->is_fly = true;
    obj->vert_speed += 0.05;
    obj->y += obj->vert_speed;
    for (size_t i = 0; i < bricks.size(); ++i) {
        if (obj->collides_with(bricks[i])) {
            if (obj->vert_speed > 0)
                obj->is_fly = false;

            if ((bricks[i].type == '?') && (obj->vert_speed < 0) && (obj == &mario)) {
                bricks[i].type = '-';
                moving.push_back({bricks[i].x, bricks[i].y - 3, 3, 2, '$'});
                moving.back().vert_speed = -0.7;
            }

            obj->y -= obj->vert_speed;
            obj->vert_speed = 0;
            obj->is_fly = false;
            if (bricks[i].type == '+') {
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

void horizon_move_object(DynamicGameObject &obj) {
    obj.x += obj.horiz_speed;
    for (size_t i = 0; i < bricks.size(); ++i) {
        if (obj.collides_with(bricks[i])) {
            obj.x -= obj.horiz_speed;
            obj.horiz_speed = -obj.horiz_speed;
            return;
        }
    }

    if (obj.type == 'o') {
        DynamicGameObject tmp = obj;
        vert_move_object(&tmp);
        if (tmp.is_fly) {
            obj.x -= obj.horiz_speed;
            obj.horiz_speed = -obj.horiz_speed;
        }
    }
}

void put_object_on_map(GameObject &obj) {
    int ix = std::round(obj.x);
    int iy = std::round(obj.y);
    int iw = std::round(obj.width);
    int ih = std::round(obj.height);
    for (int i = ix; i < (ix + iw); ++i)
        for (int j = iy; j < (iy + ih); ++j)
            if (is_pos_in_map(i, j))
                Map[j][i] = obj.get_render_char();
}

void create_screen() {
    initscr();
    curs_set(0);
}

void delete_screen() {
    endwin();
}

int main() {
    InputHandler input = InputHandler("/dev/input/event2");
    create_screen();
    create_level();

    for (;;) {
        input.poll_events();
        
        if (input.is_pressed(Key::Q))
            break;

        if (input.is_pressed(Key::Space) && !mario.is_fly)
            mario.vert_speed = -1;

        if (input.is_pressed_or_repeated(Key::A))
            horizon_move_map(1);

        if (input.is_pressed_or_repeated(Key::D))
            horizon_move_map(-1);

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


