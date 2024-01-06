#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#define DELTA_SIZE 4

typedef uint32_t coord_t;

struct Coordinate {
    coord_t x;
    coord_t y;
};

/*
 * Used to make steps in different directions.
 *
 * UP, RIGHT, DOWN, LEFT
 */
struct Coordinate delta[DELTA_SIZE] = {
        {0, 1},
        {1, 0},
        {0, -1},
        {-1, 0}
};

int32_t commands_count(unsigned degree);

/*
 * Method saves command to the result string of commands
 */
void write_command(char* commands, int32_t command_index, char command) {
    commands[command_index] = command;
}

/*
 * Methods saves - command
 */
void write_left(char* commands, int32_t command_index) {
    write_command(commands, command_index, '-');
}

/*
 * Methods saves F command
 */
void write_forward(char* commands, int32_t command_index) {
    write_command(commands, command_index, 'F');
}

/*
 * Methods saves + command
 */
void write_right(char* commands, int32_t command_index) {
    write_command(commands, command_index, '+');
}

/*
 * Methods copy commands from [from] to [to] in result string of commands
 *
 * @param degree is used to get the count of symbols to copy
 */
void copy_commands(unsigned degree, char* commands, const int32_t from, const int32_t to) {
    const int32_t n = commands_count(degree);
    for (int i = 0; i < n; i++) {
        commands[to + i] = commands[from + i];
    }
}

/*
 * Function calculates from which position the functions l(i) and r(i) begin
 *
 * Start of l and r depends on parity of degree
 * l(i) starts with r(i - 1)
 * r(i) starts with l(i - 1)
 * So, if degree is even, then l(1) starts before r(1), otherwise r(1) before l(1).
 * Analogously for other l(i) and r(i)
 */
void init_functions_starts(const int32_t degree, int32_t* l_commands_start, int32_t* r_commands_start) {
    bool l_is_first = degree % 2;
    for (int i = 1; i <= degree; i++) {
        const int first_start = degree - i;
        const int second_start = first_start + commands_count(i) + 2;
        if (l_is_first) {
            l_commands_start[i] = first_start;
            r_commands_start[i] = second_start;
        } else {
            l_commands_start[i] = second_start;
            r_commands_start[i] = first_start;
        }
        l_is_first ^= 1;
    }
}

/*
 * Method calculates L function −RF+LFL+FR−
 *
 * It uses previous calculated L(degree - 1) and R(degree - 1)
 */
void calc_l(unsigned degree, char* commands, int32_t* l_commands_start, int32_t* r_commands_start) {
    const int pref_commands_count = commands_count(degree - 1);

    //−RF+LFL+FR−
    int command_index = l_commands_start[degree];
    write_left(commands, command_index++); //write -

    //Copies the first written r(degree - 1) to the commands string
    if (command_index != r_commands_start[degree - 1]) copy_commands(degree - 1, commands, r_commands_start[degree - 1], command_index);
    command_index += pref_commands_count;

    write_forward(commands, command_index++); //write F
    write_right(commands, command_index++); //write +

    //Copies the first written l(degree - 1) to the commands string
    if (command_index != l_commands_start[degree - 1]) copy_commands(degree - 1, commands, l_commands_start[degree - 1], command_index);
    command_index += pref_commands_count;

    write_forward(commands, command_index++); //write F

    //Copies the first written l(degree - 1) to the commands string
    copy_commands(degree - 1, commands, l_commands_start[degree - 1], command_index);
    command_index += pref_commands_count;

    write_right(commands, command_index++); //write +
    write_forward(commands, command_index++); //write F

    //Copies the first written r(degree - 1) to the commands string
    copy_commands(degree - 1, commands, r_commands_start[degree - 1], command_index);
    command_index += pref_commands_count;

    write_left(commands, command_index); //write -
}

/*
 * Method calculates R function +LF−RFR−FL+
 *
 * It uses previous calculated L(degree - 1) and R(degree - 1)
 */
void calc_r(unsigned degree, char* commands, int32_t* l_commands_start, int32_t* r_commands_start) {
    const int pref_commands_count = commands_count(degree - 1);

    //+LF−RFR−FL+
    int command_index = r_commands_start[degree];
    write_right(commands, command_index++); //write +

    //Copies the first written l(degree - 1) to the commands string
    if (command_index != l_commands_start[degree - 1])  copy_commands(degree - 1, commands, l_commands_start[degree - 1], command_index);
    command_index += pref_commands_count;

    write_forward(commands, command_index++); //write F
    write_left(commands, command_index++); //write -

    //Copies the first written r(degree - 1) to the commands string
    if (command_index != r_commands_start[degree - 1]) copy_commands(degree - 1, commands, r_commands_start[degree - 1], command_index);
    command_index += pref_commands_count;

    write_forward(commands, command_index++); //write F

    //Copies the first written r(degree - 1) to the commands string
    copy_commands(degree - 1, commands, r_commands_start[degree - 1], command_index);
    command_index += pref_commands_count;

    write_left(commands, command_index++); //write -
    write_forward(commands, command_index++); //write F

    //Copies the first written l(degree - 1) to the commands string
    copy_commands(degree - 1, commands, l_commands_start[degree - 1], command_index);
    command_index += pref_commands_count;

    write_right(commands, command_index); //write +
}

/*
 * Method calculates all l(i) and r(i) for all i <= degree - 2
 *
 * It also calculates
 */
void calc_functions(unsigned degree, char* commands, int32_t* l_commands_start, int32_t* r_commands_start) {
    init_functions_starts((int32_t)degree, l_commands_start, r_commands_start); // initialize functions starts

    for (int i = 1; i < degree; i++) {
        calc_l(i, commands, l_commands_start, r_commands_start);
        calc_r(i, commands, l_commands_start, r_commands_start);
    }
    if (degree >= 1) {
        calc_l(degree, commands, l_commands_start, r_commands_start); // calculates l(degree - 1)
    }
}

/*
 * Methods calculates axiom LFL+F+LFL
 *
 * It uses the calculated L(degree - 1) from [calc_functions]
 */
void calc_axiom(unsigned degree, char* commands, int32_t* l_commands_start, int32_t* r_commands_start) {
    calc_functions(degree - 1, commands, l_commands_start, r_commands_start);

    //LFL+F+LFL
    const int functions_commands_count = commands_count(degree - 1);

    int command_index = functions_commands_count; //The first L is already written in the commands string
    write_forward(commands, command_index++); //write F

    //Copies the first L to the commands string
    copy_commands(degree - 1, commands, l_commands_start[degree - 1], command_index);
    command_index += functions_commands_count;

    write_right(commands, command_index++); //write +
    write_forward(commands, command_index++); //write F
    write_right(commands, command_index++); //write +

    //Copies the first L to the commands string
    copy_commands(degree - 1, commands, l_commands_start[degree - 1], command_index);
    command_index += functions_commands_count;

    write_forward(commands, command_index++); //write F
    //Copies the first L to the commands string
    copy_commands(degree - 1, commands, l_commands_start[degree - 1], command_index);
}


/*
 * Adds the cur_point to the results x and y
 */
void add_point(coord_t* x, coord_t* y, struct Coordinate* cur_point, int32_t* point_index) {
    x[*point_index] = cur_point->x;
    y[*point_index] = cur_point->y;
    *point_index += 1;
}

/*
 * Method implements F constant
 *
 * It takes a step in line with the current direction
 */
void go_forward(coord_t* x, coord_t* y, struct Coordinate* cur_point, const int32_t* direction, int32_t* point_index) {
    cur_point->x += delta[*direction].x;
    cur_point->y += delta[*direction].y;
    add_point(x, y, cur_point, point_index);
}

/*
 * Method implements turn to right (+ constant).
 *
 * It increases the direction by 1.
 */
void turn_right(int32_t* direction) {
    *direction = (*direction + 1) % DELTA_SIZE;
}

/*
 * Method implements turn to left (- constant).
 *
 * It decreases the direction by -1.
 */
void turn_left(int32_t* direction) {
    *direction = (*direction + DELTA_SIZE - 1) % DELTA_SIZE;
}

/*
 * Method returns start point x for current degree
 */
coord_t get_start_coord(unsigned degree) {
    return (1 << (degree - 1)) - 1;
}

/*
 * Method process all commands +, - and F from the current string of commands
 *
 * It saves the points to the x and y
 */
void process_commands(unsigned degree, const int32_t n, const char* commands, coord_t* x, coord_t* y) {
    struct Coordinate cur_point = {get_start_coord(degree), 0};
    int32_t point_index = 0;
    int32_t direction = 0;

    add_point(x, y, &cur_point, &point_index);
    for (int32_t i = 0; i < n; i++) {
        switch (commands[i]) {
            case '+': turn_right(&direction); break;
            case '-': turn_left(&direction); break;
            case 'F': go_forward(x, y, &cur_point, &direction, &point_index); break;
        }
    }
}

static bool malloc_failed = false;

/*
 * Used in rahmenprogramm.c to print error if allocation in this file fails
 */
bool malloc_is_failed() {
    return malloc_failed;
}

/*
 * Method finds points coordinates of the moore curve using gray code method.
 *
 * When degree <= 0 function will print an error.
 */
void moore(unsigned degree, coord_t* x, coord_t* y) {
    malloc_failed = false;
    if (degree <= 0 || degree > 15) {
        fprintf(stderr, "Moore curve degree must be between 1 and 15");
        return;
    }

    // Allocates memory for l_commands_start() function
    int32_t* l_commands_start = (int32_t*) malloc(sizeof(int32_t) * (degree + 1));
    if (l_commands_start == NULL) {
        malloc_failed = true;
        return;
    }

    // Allocates memory for r_commands_start() function
    int32_t* r_commands_start = (int32_t*) malloc(sizeof(int32_t) * (degree + 1));
    if (r_commands_start == NULL) {
        malloc_failed = true;
        free(l_commands_start);
        return;
    }

    // Allocates memory for the string of commands +, - and F
    const int32_t commands_size = (4 * commands_count(degree - 1) + 5);
    char* commands = (char*) malloc(sizeof(char) * commands_size);
    if (commands == NULL) {
        malloc_failed = true;
        free(l_commands_start);
        free(r_commands_start);
        return;
    }

    calc_axiom(degree, commands, l_commands_start, r_commands_start); // initialize commands
    process_commands(degree, commands_size, commands, x, y); // read/process all commands and save coordinates

    free(l_commands_start);
    free(r_commands_start);
    free(commands);
}


/*
 * Calculates the commands count for the current degree
 */
int32_t commands_count(unsigned degree) {
    if (degree <= 0) return 0;
    return ((1 << (2 * degree)) - 1) / 3 * 7;
}