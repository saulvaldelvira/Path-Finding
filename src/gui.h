#ifndef GUI_H
#define GUI_H

#include <stdbool.h>

extern bool animate_search;

int gui_init(void);
void gui_loop(void);
void gui_shutdown(void);

void take_step();

#endif // GUI_H
