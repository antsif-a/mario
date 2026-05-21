#ifndef OBJECT_HPP
#define OBJECT_HPP

struct TObject {
    float x, y;
    float width, height;
    float vert_speed;
    bool is_fly;
    char cType;
};

void init_object(TObject &obj, float x, float y, float w, float h, char inType);
bool is_collision(TObject &o1, TObject &o2);

#endif
