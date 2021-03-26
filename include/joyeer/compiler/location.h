#ifndef __joyeer_compiler_location_h__
#define __joyeer_compiler_location_h__

// Location in the source file
struct Location {
    int offset;

    Location(int offset): offset(offset) {}

};

#endif
