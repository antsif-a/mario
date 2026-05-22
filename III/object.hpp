#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <utility>

class GameObject {
    public:
        enum Type : char {
            Player = '@',
            Brick  = '#',
            Money  = '$',
            Enemy  = 'o'
        };

        float x;
        float y;
        float width;
        float height;
        
        char type;
    public:
        GameObject(float x, float y, float w, float h, char type);

        bool collides_with(GameObject &);

        char get_render_char() {
            return type; 
        }
};

class DynamicGameObject : public GameObject {
    public:
        float vert_speed = 0.0;
        float horiz_speed = 0.2;
        bool  is_fly = true;
    public:
        DynamicGameObject(float x, float y, float w, float h, char type);
};

class Player : public DynamicGameObject {
    public:
        Player(float x, float y, float w, float h);
};

#endif
