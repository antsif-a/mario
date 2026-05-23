#include <vector>
#include <cmath>

#include <ncurses.h>

#include <object.hpp>
#include <input.hpp>
#include <window.hpp>
#include <game_map.hpp>

const int init_mario_x = 39;
const int init_mario_y = 10;
Player mario = Player(init_mario_x, init_mario_y, 3, 3);
std::vector<GameObject> bricks;
std::vector<DynamicGameObject> moving;

int current_level;
const int max_level = 3;

int score;

void add_moving(float x, float y, float w, float h, char type) {
    DynamicGameObject obj(x, y, w, h, type);
    obj.horiz_speed = 0.5f;
    moving.push_back(obj);
}

void add_brick(float x, float y, float w, float h, char type) {
    GameObject obj(x, y, w, h, type);
    bricks.push_back(obj);
}

void create_level(int level) {
    score = 0;
    mario.x = init_mario_x;
    mario.y = init_mario_y;

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
}

void next_level() {
    current_level++;
    if (current_level > max_level)
        current_level = 1;
    create_level(current_level);
}

void restart_level_on_death() {
    create_level(current_level);
}

void delete_moving(int i) {
    moving.erase(moving.begin() + i);
}

template <typename T>
T * find_collision_object(std::vector<T> &arr, GameObject &obj) {
    for (auto &target : arr)
        if (obj.collides_with(target))
            return &target;
    return nullptr;
}

template <typename T>
int find_collision_object_index(std::vector<T> &arr, GameObject &obj) {
    for (size_t i = 0; i < arr.size(); ++i)
        if (arr[i].collides_with(obj))
            return i;
    return -1;
}

void horizon_move_map(float dx) {
    mario.x -= dx;
    if (find_collision_object(bricks, mario) != nullptr) {
        mario.x += dx;
        return;
    }
    
    mario.x += dx;

    for (auto &brick : bricks)
        brick.x += dx;

    for (auto &movin : moving)
        movin.x += dx;
}

void handle_player_collision(Player &player) {
    int i = find_collision_object_index(moving, player);
    if (i < 0)
        return;

    GameObject &collision_obj = moving[i];

    switch (collision_obj.type) {
        case GameObject::Enemy:
            if (player.is_fly && player.vert_speed > 0 && player.y + player.height < moving[i].y + moving[i].height * 0.5) {
                delete_moving(i);
                score += 50;
            } else {
                napms(500);
                restart_level_on_death();
                break;
            }

        case GameObject::Money:
            delete_moving(i);
            score += 100;
            break;
    }
}

void handle_vertical_collision(DynamicGameObject &obj) {
    GameObject * collision_obj = find_collision_object(bricks, obj);
    if (collision_obj != nullptr) {
        obj.y -= obj.vert_speed;
        if (obj.vert_speed > 0)
            obj.is_fly = false;

        switch (collision_obj->type) {
            case GameObject::MoneyBlock:
                if (obj.vert_speed >= 0 || obj.type != GameObject::Player)
                    break;
                collision_obj->type = GameObject::MoneyBlockEmpty;
                moving.push_back({collision_obj->x, collision_obj->y - 3, 3, 2, '$'});
                moving.back().vert_speed = -0.7;
                break;
            case GameObject::Final:
                napms(500);
                next_level();
                break;
        }
        obj.vert_speed = 0;
    }
}

void vert_move_object(DynamicGameObject &obj) {
    obj.is_fly = true;
    obj.vert_speed += 0.05;
    obj.y += obj.vert_speed;
    handle_vertical_collision(obj);
}

void handle_horizontal_collision(DynamicGameObject &obj) {
    GameObject * collision_obj = find_collision_object(bricks, obj);
    if (collision_obj != nullptr) {
        obj.x -= obj.horiz_speed;
        obj.horiz_speed = -obj.horiz_speed;
        return;
    }

    if (obj.type == GameObject::Enemy) {
        DynamicGameObject tmp = obj;
        vert_move_object(tmp);
        if (tmp.is_fly) {
            obj.x -= obj.horiz_speed;
            obj.horiz_speed = -obj.horiz_speed;
        }
    }
}

void horizon_move_object(DynamicGameObject &obj) {
    obj.x += obj.horiz_speed;
    handle_horizontal_collision(obj);
}

bool update(InputHandler &input, int map_height) {
    input.poll_events();

    if (input.is_pressed(Key::Q))
        return false;

    if (input.is_pressed(Key::Space) && !mario.is_fly)
        mario.vert_speed = -1;

    if (input.is_pressed_or_repeated(Key::A))
        horizon_move_map(1);

    if (input.is_pressed_or_repeated(Key::D))
        horizon_move_map(-1);

    if (mario.y > map_height) {
        napms(500);
        restart_level_on_death();
    }

    vert_move_object(mario);
    handle_player_collision(mario);
    for (size_t i = 0; i < moving.size(); ++i) {
        vert_move_object(moving[i]);
        horizon_move_object(moving[i]);
        if (moving[i].y > map_height) {
            delete_moving(i);
            i--;
            continue;
        }
    }

    return true;
}

void render(GameMap &game_map, Window &window) {
    game_map.clear_map();
    game_map.put_object_on_map(mario);
    for (auto &brick : bricks)
        game_map.put_object_on_map(brick);
    for (auto &obj   : moving)
        game_map.put_object_on_map(obj);
    game_map.put_score_on_map(score);
    game_map.print_to_window(window);
    window.draw_box();
    window.refresh();
}

int main() {
    InputHandler input = InputHandler("/dev/input/event2");
    GameMap game_map = GameMap(120, 40);

    Screen screen;
    int window_h = game_map.get_height();
    int window_w = game_map.get_width();
    int start_y = (screen.get_height() - window_h) / 1.5;
    int start_x = (screen.get_width()  - window_w) / 2;
    Window window = screen.create_window(window_h, window_w, start_y, start_x);

    current_level = 1;
    create_level(current_level);

    while (update(input, game_map.get_height())) {
        render(game_map, window);
        napms(16);
    };

    return 0;
}


