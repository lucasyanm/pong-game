#include "../include/input.h"

void resetInput(Input& input) {
    for (int i = 0; i < KeyboardButton::BUTTONS_COUNT; i++) {
        input.buttons[i].changed = false;
    }
}