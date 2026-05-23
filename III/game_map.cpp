#include <algorithm>
#include <string>
#include <format>
#include <cmath>

#include <game_map.hpp>

GameMap::GameMap(int w, int h) : w(w), h(h) {
    m.resize(h);
    for (auto &v : m)
        v.resize(w + 1);
}

bool GameMap::is_pos_in_map(int x, int y) {
    return ((x >= 0) && (x < w) && (y >= 0) && (y < h));
}

void GameMap::clear_map() {
    for (int j = 0; j < h; ++j) {
        std::fill(m[j].begin(), m[j].end(), ' ');
        m[j][w] = '\0'; // Ensure null terminator
    }
}

void GameMap::print_to_window(Window &window) {
    m[h - 1][w - 1] = '\0';
    for (int j = 0; j < h; ++j)
        window.print(j, 0, "%s", m[j].data());
}

void GameMap::put_score_on_map(int score) {
     std::string score_fmt = std::format("Score: {}", score);
     std::copy(score_fmt.begin(), score_fmt.end(), &m[2][5]);
}

void GameMap::put_object_on_map(GameObject &obj) {
    int ix = std::round(obj.x);
    int iy = std::round(obj.y);
    int iw = std::round(obj.width);
    int ih = std::round(obj.height);
    for (int i = ix; i < (ix + iw); ++i)
        for (int j = iy; j < (iy + ih); ++j)
            if (is_pos_in_map(i, j))
                m[j][i] = obj.get_render_char();
}

int GameMap::get_width() {
    return w;
}

int GameMap::get_height() {
    return h;
}


