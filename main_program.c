#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>

#define SVG_FILE_NAME "svg_result.svg"

// Errors output
#define MISSING_ARGUMENTS "None of the arguments are specified. Use --help to get information about possible arguments"
#define UNKNOWN_ARGUMENT "Specified argument is not supported"

typedef uint32_t coord_t;


int error(const char* error);

int missing_argument_error(const char *argument);

int invalid_moore_curve_degree();

int invalid_solution_type(int32_t max_supported_solution_number);

int invalid_number_of_benchmarking_cycles();

int invalid_average_benchmark();

int failed_malloc();

int failed_to_open_file(const char *file_name);

void print_help_message();


void moore(unsigned degree, coord_t* x, coord_t* y);

bool malloc_is_failed();

void moore_gray_code(unsigned degree, coord_t* x, coord_t* y);

void moore_recursive(unsigned degree, coord_t* x, coord_t* y);


int number_or_default(int len, char* strings[], size_t* index, int default_value) {
    if (*index < len && isdigit(strings[*index][0])) {
        int number = atoi(strings[*index]);
        *index += 1;
        return number;
    }
    return default_value;
}

bool expect_word(const char *expected, const char* actual, size_t* index) {
    if (strcmp(actual, expected) == 0) {
        *index += 1;
        return true;
    }
    return false;
}

double calc_moore_curve_points(unsigned degree, coord_t* x, coord_t* y, int solution_type, bool with_benchmarking) {
    struct timespec start;
    struct timespec end;

    if (with_benchmarking) clock_gettime(CLOCK_MONOTONIC , &start);

    switch (solution_type) {
        case 0: moore(degree, x, y); break;
        case 1: moore_gray_code(degree, x, y); break;
        case 2: moore_recursive(degree, x, y); break;
    }

    if (with_benchmarking && !malloc_is_failed()) {
        clock_gettime(CLOCK_MONOTONIC , &end);
        double time = end.tv_sec - start.tv_sec + 1e-9 * (end.tv_nsec - start.tv_nsec);
        printf("Time: %f\n", time);
        return time;
    }
    return 0.0;
}

void print_moore_curve_points(FILE *fptr, const int32_t points_number, coord_t* x, coord_t* y) {
    for (size_t i = 0; i < points_number; i++) {
        fprintf(fptr, "%d, %d\n", x[i], y[i]);
    }
}

void print_to_svg(FILE *fptr, unsigned degree, const int32_t points_number, coord_t* x, coord_t* y) {
    coord_t max_x = 0;
    coord_t max_y = 0;

    for (size_t i = 0; i < points_number; i++) {
        coord_t new_x = x[i] * 100;
        if (max_x < new_x) {
            max_x = new_x;
        }

        coord_t new_y = y[i] * 100;
        if (max_y < new_y) {
            max_y = new_y;
        } 
    }

    fprintf(fptr, "<?xml version=\"1.0\" standalone=\"no\"?>\n");
    fprintf(fptr, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" version=\"1.1\" baseProfile=\"full\">\n", max_x, max_y);
    fprintf(fptr, "<polyline points=\"");
    for (size_t i = 0; i < points_number; i++) {
        fprintf(fptr, "%d,%d ", x[i] * 100, y[i] * 100);
    }
    fprintf(fptr, "\" style=\"fill:none;stroke:black;stroke-width:2\"/>\n");
    fprintf(fptr, "</svg>\n");
}

int32_t get_point_numbers(int degree) {
    return 1 << (2 * degree);
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argv == NULL) {
        return error(MISSING_ARGUMENTS);
    }

    // Consts that define arguments index
    static const int ARGUMENTS_COUNT = 6;
    static const int SOLUTION_TYPE_ARGUMENT = 0; // Optional argument
    static const int BENCHMARK_ARGUMENT = 1; // Optional argument
    static const int CURVE_DEGREE_ARGUMENT = 2; // Must be specified
    static const int OUTPUT_FILE_ARGUMENT = 3; // Must be specified
    static const int HELP_ARGUMENT = 4; // Optional argument
    static const int AVERAGE_BENCHMARK_ARGUMENT = 5; // Optional argument

    bool argument_is_specified[ARGUMENTS_COUNT];
    for (size_t i = 0; i < ARGUMENTS_COUNT; i++) {
        argument_is_specified[i] = false;
    }

    static const int SOLUTIONS_COUNT = 3;

    int solution_type = 0;
    int number_of_benchmarking_cycles = 1;
    int moore_curve_degree = -1;
    const char* output_file;

    for (size_t i = 1; i < argc;) {
        if (expect_word("-V", argv[i], &i)) {
            argument_is_specified[SOLUTION_TYPE_ARGUMENT] = true;
            solution_type = number_or_default(argc, argv, &i, -1);
            continue;
        } else if (expect_word("-B", argv[i], &i)) {
            argument_is_specified[BENCHMARK_ARGUMENT] = true;
            number_of_benchmarking_cycles = number_or_default(argc, argv, &i, 1);
            continue;
        } else if (expect_word("-n", argv[i], &i)) {
            argument_is_specified[CURVE_DEGREE_ARGUMENT] = true;
            moore_curve_degree = number_or_default(argc, argv, &i, -1);
            continue;
        } else if (expect_word("-o", argv[i], &i)) {
            argument_is_specified[OUTPUT_FILE_ARGUMENT] = true;
            output_file = argv[i++];
            continue;
        } else if (expect_word("-AB", argv[i], &i)) {
            argument_is_specified[AVERAGE_BENCHMARK_ARGUMENT] = true;
            continue;
        } else if (expect_word("-h", argv[i], &i) || expect_word("--help", argv[i], &i)) {
            argument_is_specified[HELP_ARGUMENT] = true;
            continue;
        }
        return error(UNKNOWN_ARGUMENT);
    }

    if (argument_is_specified[HELP_ARGUMENT]) {
        print_help_message();
        return 0;
    }

    if (!argument_is_specified[CURVE_DEGREE_ARGUMENT] || moore_curve_degree == -1 || !argument_is_specified[OUTPUT_FILE_ARGUMENT]) {
        return missing_argument_error(!argument_is_specified[OUTPUT_FILE_ARGUMENT] ? "Output file" : "Curve degree");
    }

    if (moore_curve_degree < 1 || moore_curve_degree > 15) {
        return invalid_moore_curve_degree();
    }

    if (solution_type < 0 || solution_type >= SOLUTIONS_COUNT) {
        return invalid_solution_type(SOLUTIONS_COUNT);
    }

    if (number_of_benchmarking_cycles < 1) {
        return invalid_number_of_benchmarking_cycles();
    }

    if (!argument_is_specified[BENCHMARK_ARGUMENT] && argument_is_specified[AVERAGE_BENCHMARK_ARGUMENT]) {
        return invalid_average_benchmark();
    }

    double summary_time = 0.0;
    const int32_t point_numbers = get_point_numbers(moore_curve_degree);
    for (int cycle = 0; cycle < number_of_benchmarking_cycles; cycle++) {
        coord_t* x = (coord_t*) malloc(sizeof(coord_t) * point_numbers);
        if (x == NULL) {
            return failed_malloc();
        }

        coord_t* y = (coord_t*) malloc(sizeof(coord_t) * point_numbers);
        if (y == NULL) {
            return failed_malloc();
        }


        // Calculate moore curve points
        FILE *moore_curve_fptr;
        moore_curve_fptr = fopen(output_file, "w");
        if (moore_curve_fptr == NULL) {
            return failed_to_open_file(output_file);
        }
        summary_time += calc_moore_curve_points(moore_curve_degree, x, y, solution_type, argument_is_specified[BENCHMARK_ARGUMENT]);
        if (malloc_is_failed()) {
            return failed_malloc();
        }
        print_moore_curve_points(moore_curve_fptr, point_numbers, x, y);
        fclose(moore_curve_fptr);


        // Print result to svg file with name svg_result.xml
        FILE *svg_fptr;
        svg_fptr = fopen(SVG_FILE_NAME, "w");
        if (svg_fptr == NULL) {
            return failed_to_open_file(SVG_FILE_NAME);
        }
        print_to_svg(svg_fptr, moore_curve_degree, point_numbers, x, y);
        fclose(svg_fptr);

        free(x);
        free(y);
    }

    if (argument_is_specified[AVERAGE_BENCHMARK_ARGUMENT]) {
        printf("Average time: %f\n", summary_time / number_of_benchmarking_cycles);
    }

    return 0;
}

int error(const char* error) {
    fprintf(stderr, "%s\n", error);
    return -1;
}

int error_with_two_string(const char* error1, const char* error2) {
    fprintf(stderr, "%s%s\n", error1, error2);
    return -1;
}

int error_and_number(const char* error, int32_t number) {
    fprintf(stderr, "%s%d\n", error, number);
    return -1;
}

int missing_argument_error(const char *argument) {
    return error_with_two_string(argument, " must be specified");
}

int invalid_moore_curve_degree() {
    return error("Invalid moore curve degree. The number must be between 1 and 15");
}

int invalid_solution_type(int max_supported_solution_number) {
    return error_and_number("Unsupported solution type. Use the number between 0 and ", max_supported_solution_number);
}

int invalid_number_of_benchmarking_cycles() {
    return error("Invalid number of benchmarking cycles. The number must be at least 1");
}

int invalid_average_benchmark() {
    return error("Benchmark parameter must be specified too");
}

int failed_malloc() {
    return error("Failed memory allocation. Moore curve degree is too big");
}

int failed_to_open_file(const char *file_name) {
    return error_with_two_string("Failed to open the file ", file_name);
}

void print_help_message() {
    printf("Usage: make\n./moore_curve [ARGUMENT 1] [ARGUMENT 2] ...\n\n");
    printf("Implementation calculates moore curve points for the given N and prints the result to the given file. It generates svg file too.\n\n");
    printf("Run arguments:\n");
    printf("       -V <Number>       Specifies which solution is used to find the answer.\n");
    printf("                         Print 0 for iterative solution, 1 for grey code solution, 2 for recursive solution.\n");
    printf("                         By default, the iterative solution is used.\n");
    printf("       -B <Number>       Enables benchmarking. You can also specify the number of function calls.\n");
    printf("       -AB               If specified prints the average benchmarking. You can also specify the number of function calls.\n");
    printf("       -n <Number>       Determines the degree N of the moore curve. Argument must be specified.\n");
    printf("       -o <File name>    Defines the file to which the result will be written in svg format. Argument must be specified.\n");
    printf("       -h, --help        Shows help message and exits the program.\n");
}