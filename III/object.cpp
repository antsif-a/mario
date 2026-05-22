#include <object.hpp>

GameObject::GameObject(float x, float y, float w, float h, char type):
    x(x), y(y), width(w), height(h), type(type) {}

bool GameObject::collides_with(GameObject &other) {
    return (((x + width) > other.x) && (x < (other.x + other.width)) &&
            ((y + height) > other.y) && (y < (other.y + other.height)));
}

DynamicGameObject::DynamicGameObject(float x, float y, float w, float h, char type):
    GameObject(x, y, w, h, type) {}

Player::Player(float x, float y, float w, float h):
    DynamicGameObject(x, y, w, h, GameObject::Player) {
        this->is_fly = false; // Fix: Player starts on ground
        this->vert_speed = 0;
        this->horiz_speed = 0;
    }


