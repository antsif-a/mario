#include <bits/stdc++.h>
#include <ncurses.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <fcntl.h>

#include <input.hpp>
#include <object.hpp>
#include <map.hpp>

using namespace std;

/* --- level --- */
struct Level {
    TObject mario;
    vector<TObject> bricks;
};

const int last_level = 2;
int current_level = 1;

void restart_level(Level &level, int level_number) {
    init_object(level.mario, (get_map_width() - 4) / 2 - (get_map_width() / 6), 10, 4, 3, '@');
    if (level_number == 1) {
        level.bricks.clear();
        init_object(level.bricks.emplace_back(), 20, 20, 40, 5, '#');
        init_object(level.bricks.emplace_back(), 60, 15, 10, 10, '#');
        init_object(level.bricks.emplace_back(), 80, 20, 20, 5, '#');
        init_object(level.bricks.emplace_back(), 120, 15, 10, 10, '#');
        init_object(level.bricks.emplace_back(), 150, 20, 40, 5, '#');
        init_object(level.bricks.emplace_back(), 210, 15, 10, 10, '+');
    } else if (level_number == 2) {
        level.bricks.clear();
        init_object(level.bricks.emplace_back(), 20, 20, 40, 5, '#');
        init_object(level.bricks.emplace_back(), 80, 20,  15, 5, '#');
        init_object(level.bricks.emplace_back(), 120, 15, 15, 10, '#');
        init_object(level.bricks.emplace_back(), 160, 10, 15, 15, '+');
    }
}

void horizon_move_map(Level &level, float dx) {
    level.mario.x -= dx;
    for (TObject &brick : level.bricks)
        if (is_collision(level.mario, brick)) {
            level.mario.x += dx;
            return;
        }
    level.mario.x += dx;

    for (TObject &brick : level.bricks)
        brick.x += dx;
}

void next_level(Level &level) {
    if (current_level == last_level) {
        current_level = 1;
        restart_level(level, 1);
    } else {
        current_level++;
        restart_level(level, current_level);
    }
}

void vert_move_object(Level &level, TObject &obj) {
    obj.is_fly = true;
    obj.vert_speed += 0.05;
    obj.y += obj.vert_speed;
    for (TObject &brick : level.bricks) {
        if (is_collision(obj, brick)) {
            obj.y = brick.y - obj.height;
            obj.vert_speed = 0;
            obj.is_fly = false;
            if (brick.cType == '+')
                next_level(level);
            break;
        }
    }
}
/* --- */

bool should_quit = false;
void update(Level &level, unordered_map<Key, KeyState> &keys) {
    if (keys[Key::Space] == KeyState::Press && !level.mario.is_fly)
        level.mario.vert_speed = -1;

    if (keys[Key::A] == KeyState::Press || keys[Key::A] == KeyState::Repeat)
        horizon_move_map(level, 1);

    if (keys[Key::D] == KeyState::Press || keys[Key::D] == KeyState::Repeat)
        horizon_move_map(level, -1);

    if (keys[Key::Q] == KeyState::Press)
        should_quit = true;

    if (level.mario.y > get_map_height())
        restart_level(level, current_level);

    vert_move_object(level, level.mario); 
}

void render(Level &level, WINDOW * window) {
    clear_map();
    put_object_on_map(level.mario);
    for (TObject &brick : level.bricks)
        put_object_on_map(brick);
    show_map(window);
    box(window, ACS_VLINE, ACS_HLINE);
    wrefresh(window);
}

WINDOW * create_game_window() {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    return newwin(get_map_height(), get_map_width(), (rows - get_map_height()) / 2, (cols - get_map_width()) / 2);
}

void delete_window(WINDOW * window) {
    delwin(window);
}

void create_screen() {
    initscr();
    curs_set(0);
}

void delete_screen() {
    endwin();
}

int main() {
    unordered_map<Key, KeyState> keys = {};
    int input_fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);
    if (input_fd < 0) {
        println(stderr, "open /dev/input/event2: {}", strerror(errno));
        return 1;
    }

    create_screen();
    auto * window = create_game_window();

    Level level = {};
    restart_level(level, current_level = 1);
    while (!should_quit) {
        handle_keyboard_states(input_fd, keys);
        update(level, keys);
        render(level, window);
        napms(1000 / 60);
    };

    delete_window(window);
    delete_screen();

    return 0;
}

