#include <object.hpp>

bool TObject::collision(TObject &other) {
    return (((x + width) > other.x) && (x < (other.x + other.width)) &&
            ((y + height) > other.y) && (y < (other.y + other.height)));
}
