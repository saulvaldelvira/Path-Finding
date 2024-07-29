/**
 * Main program
 * Author: Saúl Valdelvira (2023)
 */
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <stdio.h>
#include "gui.h"
#include "path_finding.h"
#include "args.h"


int main(int argc, char *argv[]){
        args_parse(argc, argv);
	if (path_finding_init() != 1){
		fprintf(stderr, "Error at path_finding_init\n");
		return 1;
	}
        if (gui_init() != EXIT_SUCCESS)
                return EXIT_FAILURE;

        gui_loop();

        gui_shutdown();
	path_finding_free();
	return EXIT_SUCCESS;
}


