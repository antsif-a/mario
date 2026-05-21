#include <unordered_map>
#include <unistd.h>

#include <input.hpp>

using namespace std;

void handle_keyboard_states(int fd, unordered_map<Key, KeyState> &keys) {
    input_event ev = {};
    while (read(fd, &ev, sizeof ev) != -1) {
        if (ev.type == 1)
            keys[static_cast<Key>(ev.code)] = static_cast<KeyState>(ev.value);
    }
}
