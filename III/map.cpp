#include <algorithm>
#include <vector>
#include <cmath>
#include <cstring>

#include <ncurses.h>

#include <object.hpp>
#include <screen.hpp>
#include <map.hpp>

Map::Map(int h, int w) : height(h), width(w) {
    m.resize(h);
    for (std::vector<char> &v : m)
        v.resize(w + 1);
}

void Map::clear() {
    for (int i = 0; i < width; ++i)
        m[0][i] = ' ';
    m[0][width] = '\0';
    for (int j = 0; j < height; ++j)
        std::copy(m[0].begin(), m[0].end(), m[j].data());
}

void Map::print_to_window(Window &window) {
    m[height - 1][width - 1] = '\0';
    for (int j = 0; j < height; ++j)
        window.print(j, 0, "%s", m[j].data());
}

bool Map::is_pos_in_map(int x, int y) {
    return ((x >= 0) && (x < width) && (y >= 0) && (y < height));
}

void Map::put_object(TObject &obj) {
    int ix = round(obj.x);
    int iy = round(obj.y);
    int iw = round(obj.width);
    int ih = round(obj.height);
    for (int i = ix; i < (ix + iw); ++i)
        for (int j = iy; j < (iy + ih); ++j)
            if (is_pos_in_map(i, j))
                m[j][i] = obj.cType;
}

int Map::get_width() {
    return width;
}

int Map::get_height() {
    return height;
}
