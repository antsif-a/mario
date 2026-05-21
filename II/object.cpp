#include <object.hpp>

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
