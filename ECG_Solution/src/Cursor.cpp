#include "Cursor.hpp"

Cursor::Cursor() { isPressed = false; } // Should poll to get the cursors position

void Cursor::MoveTo(double x, double y) {
    deltaX = xpos - x;
    deltaY = ypos - y;
    xpos = x;
    ypos = y;
}

