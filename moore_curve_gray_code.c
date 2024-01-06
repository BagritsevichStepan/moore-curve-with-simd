#include <stdio.h>
#include <stdint.h>

typedef uint32_t coord_t;

/*
 * transforming coordinates in the Hilbert's curve of degree (n - 1) to obtain coordinates in the Moore's curve of degree n
 * applies transformations to the coordinates in the the Hilbert's curve based on the location in one of the 4 quarters of space
*/
void transform_to_moore(coord_t* x_ptr, coord_t* y_ptr, int quadrant, int moore_n) {
    coord_t cur_x = (*x_ptr);
    coord_t cur_y = (*y_ptr);
    coord_t prev_y = cur_y;
    size_t k = (1 << (moore_n - 1));

    switch (quadrant) {
        case 0:
            cur_y = cur_x;
            cur_x = (k - 1) - prev_y;
            break;
        case 1:
            cur_y = cur_x + k;
            cur_x = (k - 1) - prev_y;
            break;
        case 2:
            cur_y = (k - 1) - cur_x + k;
            cur_x = prev_y + k;
            break;
        case 3:
            cur_y = (k - 1) - cur_x;
            cur_x = prev_y + k;
            break;
    }

    (*x_ptr) = cur_x;
    (*y_ptr) = cur_y;
}

/*
 * obtaining the initial values of (x, y) coordinates for 1 point;
 * the coordinates are in binary representation utilizing an array of integers;
 * using the Gray code value corresponding to the index of the vertex in the Hilbert's curve of degree (n - 1)
 */
void init_coordinates(coord_t* xs, coord_t* ys, size_t gray_number, int n) {
    size_t temp_orig_number = gray_number;
    temp_orig_number >>= 1;
    (*xs) = 0;
    (*ys) = 0;
    for (int i = 0; i < n; i++) {
        if ((temp_orig_number % 2) == 1)
            (*xs) += (1 << i);
        temp_orig_number >>= 2;
    }
    temp_orig_number = gray_number;
    for (int i = 0; i < n; i++) {
        if ((temp_orig_number % 2) == 1)
            (*ys) += (1 << i);
        temp_orig_number >>= 2;
    }
}


/* applying the transformations to the (x, y) coordinates initially corresponding to the Gray code value;
 resultingly, the values of coordinates correspond to those in the Hilbert curve of degree (n - 1)*/
void transform_to_hilbert (coord_t* xs, coord_t* ys, int n) {
    // iterating through the bits of the coordinates' values where the index 0 corresponds to the lowest bit
    coord_t temp_xs = (*xs);
    coord_t temp_ys = (*ys);
    temp_ys >>= 1;
    temp_xs >>= 1;
    for (int i = 1; i < n; i++) {
        if ((temp_ys % 2) == 1) {
            // inverting the bits [i - 1, i - 2, ..., 0] in x, where n - 1 cooresponds to the index of the lowest bit
            /*for (int j = i + 1; j < n; j++) {
                if (xs[j] == 1) xs[j] = 0;
                else xs[j] = 1;
            }*/
            coord_t mask = (1 << i) - 1;    // ...0...1[at index i - 1]...1
            (*xs) = (*xs) ^ mask;
        }
        else {
            // exchanging the bits [i - 1, i - 2, ..., 0] between x and y, where n - 1 cooresponds to the index of the lowest bit
            /*for (int j = i + 1; j < n; j++) {
                int temp = xs[j];
                xs[j] = ys[j];
                ys[j] = temp;
            }*/
            coord_t mask = (1 << i) - 1;
            coord_t lower_xs = mask & (*xs);    // obtaining the bits [i - 1, i - 2, ..., 0] in x
            coord_t lower_ys = mask & (*ys);    // obtaining the bits [i - 1, i - 2, ..., 0] in y
            coord_t inverted_mask = ~mask;
            (*xs) = (*xs) & inverted_mask;      // setting the bits [i - 1, i - 2, ..., 0] in x to 0
            (*ys) = (*ys) & inverted_mask;      // setting the bits [i - 1, i - 2, ..., 0] in y to 0
            (*xs) = (*xs) | lower_ys;           // setting the bits [i - 1, i - 2, ..., 0] in x to their values in y
            (*ys) = (*ys) | lower_xs;           // setting the bits [i - 1, i - 2, ..., 0] in y to their values in x
        }
        temp_ys >>= 1;

        if ((temp_xs % 2) == 1) {
            // inverting the bits [i - 1, i - 2, ..., 0] in x, where n - 1 cooresponds to the index of the lowest bit
            /*for (int j = i + 1; j < n; j++) {
                if (xs[j] == 1) xs[j] = 0;
                else xs[j] = 1;
            }*/
            coord_t mask = (1 << i) - 1;    // ...0...1[at index i - 1]...1
            (*xs) = (*xs) ^ mask;
        }
        temp_xs >>= 1;
    }
}


void get_coordinates(coord_t* x_ptr, coord_t* y_ptr, size_t vertex_number, int moore_n) {
    size_t mask = 3 << (2 * (moore_n - 1));
    mask = ~mask;
    // calculating the correspondent index in the Hilbert's curve of degree (n - 1)
    int orig_number = (vertex_number & mask);

    // calculating the Gray code for the correspondent index in the Hilbert's curve of degree (n - 1)
    size_t gray_number = orig_number ^ (orig_number >> 1);
    int n = moore_n - 1;
    coord_t xs = 0;
    coord_t ys = 0;

    init_coordinates(&xs, &ys, gray_number, n);
    transform_to_hilbert(&xs, &ys, n); 

    // obtaining two most significant bits of the index of the vertex;
    // those correspond to one of the 4 quarters of space:
    //              00 --- lower left
    //              01 --- upper left
    //              10 --- upper right
    //              11 --- lower right
    mask = 3 << (2 * (moore_n - 1));
    int quadrant = (vertex_number & mask) >> (2 * (moore_n - 1));
    transform_to_moore(&xs, &ys, quadrant, moore_n);

    (*x_ptr) = xs;
    (*y_ptr) = ys;
}


/*
 * Method finds points coordinates of the moore curve using gray code method.
 *
 * When degree <= 0 function will print an error.
 */
void moore_gray_code(unsigned degree, coord_t* x, coord_t* y) {
    if (degree <= 0) {
        fprintf(stderr, "Moore curve degree must be between 1 and 15");
        return;
    }


    size_t num_points = (1 << degree) * (1 << degree);
    size_t arr_size = 0;
    for (size_t index = 0; index < num_points; index++) {
        coord_t cur_x = 0;
        coord_t cur_y = 0;
        get_coordinates(&cur_x, &cur_y, index, degree);
        //cur_y = (k2 - 1) - cur_y;

        *(x + arr_size) = cur_x;
        *(y + arr_size) = cur_y;
        arr_size += 1;
    }
}