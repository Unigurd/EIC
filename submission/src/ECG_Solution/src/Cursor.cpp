#include "Cursor.hpp"

Cursor::Cursor() { isPressed = false; } // Should poll to get the cursors position 
                                        // I don't remember what I meant by above comment

// Keeps track of where the cursor is and how long it has moved since last update
void Cursor::MoveTo(double x, double y) {
    deltaX = xpos - x;
    deltaY = ypos - y;
    xpos = x;
    ypos = y;
}

