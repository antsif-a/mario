#include <bits/stdc++.h>
#include <ncurses.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <fcntl.h>

using namespace std;

constexpr int map_width = 80;
constexpr int map_height = 30;

struct TObject {
    float x, y;
    float width, height;
    float vert_speed;
    bool is_fly;
    char cType;
};

TObject mario;
vector<TObject> bricks;
int level = 1;

char Map[map_height][map_width + 1];

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


void init_object(TObject &obj, float x, float y, float w, float h, char inType) {
    obj.x = x;
    obj.y = y;
    obj.width = w;
    obj.height = h;
    obj.vert_speed = 0;
    obj.cType = inType;
}

bool is_collision(TObject &o1, TObject &o2) {
    return (((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) &&
            ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height)));
}

void create_level() {
    init_object(mario, (map_width - 4) / 2 - (map_width / 6), 10, 4, 3, '@');

    if (level == 1) {
        bricks.clear();
        bricks.reserve(6);
        init_object(bricks.emplace_back(), 20, 20, 40, 5, '#');
        init_object(bricks.emplace_back(), 60, 15, 10, 10, '#');
        init_object(bricks.emplace_back(), 80, 20, 20, 5, '#');
        init_object(bricks.emplace_back(), 120, 15, 10, 10, '#');
        init_object(bricks.emplace_back(), 150, 20, 40, 5, '#');
        init_object(bricks.emplace_back(), 210, 15, 10, 10, '+');
    } else if (level == 2) {
        bricks.clear();
        bricks.reserve(4);
        init_object(bricks.emplace_back(), 20, 20, 40, 5, '#');
        init_object(bricks.emplace_back(), 80, 20,  15, 5, '#');
        init_object(bricks.emplace_back(), 120, 15, 15, 10, '#');
        init_object(bricks.emplace_back(), 160, 10, 15, 15, '+');
    }
}

void horizon_move_map(float dx) {
    mario.x -= dx;
    for (TObject &brick : bricks)
        if (is_collision(mario, brick)) {
            mario.x += dx;
            return;
        }
    mario.x += dx;

    for (TObject &brick : bricks)
        brick.x += dx;
}

void vert_move_object(TObject &obj) {
    obj.is_fly = true;
    obj.vert_speed += 0.05;
    obj.y += obj.vert_speed;
    for (TObject &brick : bricks) {
        if (is_collision(obj, brick)) {
            obj.y = brick.y - obj.height;
            obj.vert_speed = 0;
            obj.is_fly = false;
            if (brick.cType == '+') {
                ++level;
                if (level > 2)
                    level = 1;
                create_level();
            }
            break;
        }
    }
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

/* --- input --- */

enum struct Key {
    A = KEY_A,
    D = KEY_D,
    Q = KEY_Q,
    Space = KEY_SPACE
};

enum struct KeyState {
    Release,
    Press,
    Repeat
};

void handle_keyboard_states(int fd, unordered_map<Key, KeyState> &keys) {
    input_event ev = {};
    while (read(fd, &ev, sizeof ev) != -1) {
        if (ev.type == 1)
            keys[static_cast<Key>(ev.code)] = static_cast<KeyState>(ev.value);
    }
}
/* --- */

bool should_quit = false;

void update(unordered_map<Key, KeyState> &keys) {
    if (keys[Key::Space] == KeyState::Press && !mario.is_fly)
        mario.vert_speed = -1;

    if (keys[Key::A] == KeyState::Press || keys[Key::A] == KeyState::Repeat)
        horizon_move_map(1);

    if (keys[Key::D] == KeyState::Press || keys[Key::D] == KeyState::Repeat)
        horizon_move_map(-1);

    if (keys[Key::Q] == KeyState::Press)
        should_quit = true;

    if (mario.y > map_height)
        create_level();

    vert_move_object(mario);
}

void render(WINDOW * window) {
    clear_map();
    put_object_on_map(mario);
    for (TObject &brick : bricks)
        put_object_on_map(brick);
    show_map(window);
    box(window, ACS_VLINE, ACS_HLINE);
    wrefresh(window);
}

int main() {
    int input_fd = open("/dev/input/event2", O_RDONLY | O_NONBLOCK);
    if (input_fd < 0) {
        println(stderr, "open /dev/input/event2: {}", strerror(errno));
        return 1;
    }

    unordered_map<Key, KeyState> keys = {};

    initscr();
    curs_set(0);
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    WINDOW * game_window = newwin(map_height, map_width, (rows - map_height) / 2, (cols - map_width) / 2);
    
    create_level();
    while (!should_quit) {
        handle_keyboard_states(input_fd, keys);
        update(keys);
        render(game_window);
        napms(1000 / 60);
    };

    delwin(game_window);
    endwin();
    return 0;
}

