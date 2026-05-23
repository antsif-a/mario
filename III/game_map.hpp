#ifndef GAME_MAP_HPP
#define GAME_MAP_HPP

#include <vector>

#include <window.hpp>
#include <object.hpp>

class GameMap {
    private:
        const int w, h;
        std::vector<std::vector<char>> m;

    public:
        GameMap(int w, int h);

        bool is_pos_in_map(int x, int y);
        void clear_map();
        void print_to_window(Window &window);
        void put_score_on_map(int score);
        void put_object_on_map(GameObject &obj);
        int get_width();
        int get_height();
};


#endif
