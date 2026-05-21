#ifndef OBJECT_HPP
#define OBJECT_HPP

struct TObject {
    float x;
    float y;
    float width;
    float height;
    char cType;
    float vert_speed = 0;
    bool is_fly = 0;

    bool collision(TObject &other);
};

#endif
