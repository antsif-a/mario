#ifndef INPUT_HPP
#define INPUT_HPP

#include <unordered_map>
#include <filesystem>
#include <linux/input-event-codes.h>

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

class InputHandler {
    private:
        unsigned int input_fd;
        std::unordered_map<Key, KeyState> key_states;

    public:
        InputHandler(std::filesystem::path input_event_device_path);
        ~InputHandler();

        void poll_events();
        bool is_pressed(Key key);
        bool is_pressed_or_repeated(Key key);
};

#endif
