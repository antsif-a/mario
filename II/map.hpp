#ifndef MAP_HPP
#define MAP_HPP

void clear_map();
void show_map(WINDOW * window);
bool is_pos_in_map(int x, int y);
void put_object_on_map(TObject &obj);

unsigned int get_map_width();
unsigned int get_map_height();

#endif
