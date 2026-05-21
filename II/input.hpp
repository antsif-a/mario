#ifndef INPUT_HPP
#define INPUT_HPP

#include <unordered_map>
#include <linux/input.h>
#include <linux/input-event-codes.h>

using namespace std;

enum struct Key {
    A = KEY_A,
    D = KEY_D,
    Q = KEY_Q,
    Space = KEY_SPACE
};

enum struct KeyState {
    Release,
    Press,
    Repeat
};

void handle_keyboard_states(int fd, unordered_map<Key, KeyState> &keys);

#endif
