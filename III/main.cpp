#include <bits/stdc++.h>
#include <ncurses.h>

#include <linux/input.h>
#include <linux/input-event-codes.h>
#include <fcntl.h>

#include <input.hpp>
#include <object.hpp>
#include <map.hpp>
#include <screen.hpp>

using namespace std;
namespace fs = std::filesystem;

/* --- level --- */
struct Level {
    TObject mario;
    vector<TObject> bricks;
};

const int last_level = 2;
int current_level = 1;

Map game_map = Map(30, 80);

void restart_level(Level &level, int level_number) {
    level.mario = {float((game_map.get_width() - 4) / 2 - (game_map.get_width() / 6)), 10, 4, 3, '@'};
    level.bricks.clear();
    if (level_number == 1) {
        level.bricks.push_back({20, 20, 40, 5, '#'});
        level.bricks.push_back({60, 15, 10, 10, '#'});
        level.bricks.push_back({80, 20, 20, 5, '#'});
        level.bricks.push_back({120, 15, 10, 10, '#'});
        level.bricks.push_back({150, 20, 40, 5, '#'});
        level.bricks.push_back({210, 15, 10, 10, '+'});
    } else if (level_number == 2) {
        level.bricks.push_back({20, 20, 40, 5, '#'});
        level.bricks.push_back({80, 20,  15, 5, '#'});
        level.bricks.push_back({120, 15, 15, 10, '#'});
        level.bricks.push_back({160, 10, 15, 15, '+'});
    }
}

void horizon_move_map(Level &level, float dx) {
    level.mario.x -= dx;
    for (TObject &brick : level.bricks)
        if (level.mario.collision(brick)) {
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
        if (obj.collision(brick)) {
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
void update(Level &level, InputHandler &input) {
    if (input.is_pressed(Key::Q)) {
        should_quit = true;
        return;
    }

    if (input.is_pressed(Key::Space) && !level.mario.is_fly)
        level.mario.vert_speed = -1;

    if (input.is_pressed_or_repeated(Key::A))
        horizon_move_map(level, 1);

    if (input.is_pressed_or_repeated(Key::D))
        horizon_move_map(level, -1);

    if (level.mario.y > game_map.get_height())
        restart_level(level, current_level);

    vert_move_object(level, level.mario); 
}

void render(Level &level, Window &window) {
    game_map.clear();
    game_map.put_object(level.mario);
    for (TObject &brick : level.bricks)
        game_map.put_object(brick);
    game_map.print_to_window(window);
    window.draw_box(ACS_VLINE, ACS_HLINE);
    window.refresh();
}

const fs::path input_event_device_path = "/dev/input/event2";

int main() {
    if (!fs::exists(input_event_device_path))
        println(stderr, "no such file: {}", input_event_device_path.c_str());

    InputHandler input = InputHandler(input_event_device_path);
    Screen screen;
    Window window = Window(
        game_map.get_height(),
        game_map.get_width(),
        (screen.get_height() - game_map.get_height()) / 2,
        (screen.get_width()  - game_map.get_width())  / 2
    );

    Level level = {};
    restart_level(level, current_level = 1);
    while (!should_quit) {
        input.poll_events();
        update(level, input);
        render(level, window);
        napms(1000 / 60);
    };

    return 0;
}

