#include <unordered_map>
#include <exception>

#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include <input.hpp>

InputHandler::InputHandler(std::filesystem::path input_event_device_path) {
    this->input_fd = open(input_event_device_path.c_str(), O_RDONLY | O_NONBLOCK);
}

InputHandler::~InputHandler() {
    close(this->input_fd);
}

void InputHandler::poll_events() {
    input_event ev = {};
    while (read(this->input_fd, &ev, sizeof ev) != -1) {
        if (ev.type == 1)
            this->key_states[static_cast<Key>(ev.code)] = static_cast<KeyState>(ev.value);
    }
}

bool InputHandler::is_pressed(Key key) {
    return this->key_states[key] == KeyState::Press;
}


bool InputHandler::is_pressed_or_repeated(Key key) {
    return this->key_states[key] == KeyState::Press
        || this->key_states[key] == KeyState::Repeat;
}
