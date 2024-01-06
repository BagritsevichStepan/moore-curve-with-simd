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
struct Coordinate delta_recursive[DELTA_SIZE] = {
        {0, 1},
        {1, 0},
        {0, -1},
        {-1, 0}
};

void l_recursive(unsigned degree, coord_t* x, coord_t* y, struct Coordinate* cur_point, int32_t* direction, int32_t* point_index);

void r_recursive(unsigned degree, coord_t* x, coord_t* y, struct Coordinate* cur_point, int32_t* direction, int32_t* point_index);


/*
 * Adds the cur_point to the results x and y
 */
void add_point_recursive(coord_t* x, coord_t* y, struct Coordinate* cur_point, int32_t* point_index) {
    x[*point_index] = cur_point->x;
    y[*point_index] = cur_point->y;
    *point_index += 1;
}

/*
 * Method implements F constant
 *
 * It takes a step in line with the current direction
 */
void go_forward_recursive(coord_t* x, coord_t* y, struct Coordinate* cur_point, const int32_t* direction, int32_t* point_index) {
    cur_point->x += delta_recursive[*direction].x;
    cur_point->y += delta_recursive[*direction].y;
    add_point_recursive(x, y, cur_point, point_index);
}

/*
 * Method implements turn to right (+ constant).
 *
 * It increases the direction by 1.
 */
void turn_right_recursive(int32_t* direction) {
    *direction = (*direction + 1) % DELTA_SIZE;
}

/*
 * Method implements turn to left (- constant).
 *
 * It decreases the direction by -1.
 */
void turn_left_recursive(int32_t* direction) {
    *direction = (*direction + DELTA_SIZE - 1) % DELTA_SIZE;
}

/*
 * Method returns start point x for current degree
 */
coord_t get_start_coord_recursive(unsigned degree) {
    return (1 << (degree - 1)) - 1;
}

/*
 * Method calculates l function −RF+LFL+FR−
 *
 * It uses l_recursive(degree - 1) and r_recursive(degree - 1) methods for current degree.
 */
void l_recursive(unsigned degree, coord_t* x, coord_t* y, struct Coordinate* cur_point, int32_t* direction, int32_t* point_index) {
    if (degree <= 0) {
        return;
    }

    //−RF+LFL+FR−
    turn_left_recursive(direction);
    r_recursive(degree - 1, x, y, cur_point, direction, point_index);
    go_forward_recursive(x, y, cur_point, direction, point_index);
    turn_right_recursive(direction);
    l_recursive(degree - 1, x, y, cur_point, direction, point_index);
    go_forward_recursive(x, y, cur_point, direction, point_index);
    l_recursive(degree - 1, x, y, cur_point, direction, point_index);
    turn_right_recursive(direction);
    go_forward_recursive(x, y, cur_point, direction, point_index);
    r_recursive(degree - 1, x, y, cur_point, direction, point_index);
    turn_left_recursive(direction);
}

/*
 * Method calculates r function +LF−RFR−FL+
 *
 * It uses l_recursive(degree - 1) and r_recursive(degree - 1) methods for current degree.
 */
void r_recursive(unsigned degree, coord_t* x, coord_t* y, struct Coordinate* cur_point, int32_t* direction, int32_t* point_index) {
    if (degree <= 0) {
        return;
    }

    //+LF−RFR−FL+
    turn_right_recursive(direction);
    l_recursive(degree - 1, x, y, cur_point, direction, point_index);
    go_forward_recursive(x, y, cur_point, direction, point_index);
    turn_left_recursive(direction);
    r_recursive(degree - 1, x, y, cur_point, direction, point_index);
    go_forward_recursive(x, y, cur_point, direction, point_index);
    r_recursive(degree - 1, x, y, cur_point, direction, point_index);
    turn_left_recursive(direction);
    go_forward_recursive(x, y, cur_point, direction, point_index);
    l_recursive(degree - 1, x, y, cur_point, direction, point_index);
    turn_right_recursive(direction);
}


/*
 * Method calculates axiom LFL+F+LFL
 *
 * It uses l_recursive method.
 */
void axiom_recursive(unsigned degree, coord_t* x, coord_t* y, struct Coordinate* cur_point, int32_t* direction, int32_t* point_index) {
    l_recursive(degree - 1, x, y, cur_point, direction, point_index);
    go_forward_recursive(x, y, cur_point, direction, point_index);
    l_recursive(degree - 1, x, y, cur_point, direction, point_index);
    turn_right_recursive(direction);
    go_forward_recursive(x, y, cur_point, direction, point_index);
    turn_right_recursive(direction);
    l_recursive(degree - 1, x, y, cur_point, direction, point_index);
    go_forward_recursive(x, y, cur_point, direction, point_index);
    l_recursive(degree - 1, x, y, cur_point, direction, point_index);
}


/*
 * Method finds points coordinates of the moore curve using recursion.
 *
 * When degree <= 0 function will print an error.
 */
void moore_recursive(unsigned degree, coord_t* x, coord_t* y) {
    if (degree <= 0) {
        fprintf(stderr, "Moore curve degree must be between 1 and 15");
        return;
    }

    struct Coordinate cur_point = {get_start_coord_recursive(degree), 0};
    int32_t point_index = 0;
    int32_t direction = 0;

    add_point_recursive(x, y, &cur_point, &point_index);
    axiom_recursive(degree, x, y, &cur_point, &direction, &point_index);
}

