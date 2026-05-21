#ifndef MAP_HPP
#define MAP_HPP

#include <vector>

#include <screen.hpp>
#include <object.hpp>

class Map {
    private:
        std::vector<std::vector<char>> m;
        const int height, width;

        bool is_pos_in_map(int x, int y);

    public:
        Map(int h, int w);
        void clear();
        void put_object(TObject &obj);
        void print_to_window(Window &window);
        int get_width();
        int get_height();
};

#endif
