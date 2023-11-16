#ifndef S21_BACK_H
#define S21_BACK_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define constants for success and failure
#define SUCCESS 1
#define FAILURE 0

// Define the maximum size for strings
#define S_SIZE 1024

// Define a structure for matrices
typedef struct matrix_struct {
  double **matrix;
  int rows;
  int columns;
} matrix_t;

// Define error codes as an enum
enum {
  OK = 0,
  INCORRECT_MATRIX = 1,
  CALC_ERROR = 2,
};

// Define a structure for filds
typedef struct facets {
  unsigned int *vertexes;
  int numbers_of_vertexes_in_facets;
} polygon_t;

// Define a structure for data
typedef struct data {
  int count_of_vertexes;
  int count_of_facets;
  double *array;
  double max_coord;
  matrix_t matrix_3d;
  polygon_t *filds;
  unsigned int *arrayfilds;
  unsigned int sizefilds;
  double *array_const;
} data_t;

int s21_parseDataAndFile(data_t *data, char *model_file_name, matrix_t *mat,
                         polygon_t **filds, int *fildsCounter,
                         char *string_file);
// int s21_parseFile(data_t *data, matrix_t *mat, polygon_t *polygon, FILE *f,
// char *string_file, int *row, int *fildsCounter); int
// s21_parseVertex(matrix_t *mat, char *string_file, int step, int *row); int
// s21_parsePolygon(polygon_t *polygon, char *string_file, int
// *fildsCounter); int s21_finalizeParsing(data_t *data); Function prototypes
int s21_string_to_int(char *str, int *step, int *number);
int s21_create_matrix(int rows, int columns, matrix_t *result);
void s21_remove_matrix(matrix_t *A);
int s21_parsing(data_t *data, char *model_file_name);
int s21_parsingDataSize(data_t *data, char *model_file_name);
int s21_parsingСonditions(char c, char *string_file, int *step);
void s21_remove_data(data_t *data);
int s21_findfilds(polygon_t *filds, char *string_file);
int s21_arrayfilds(data_t *data);
void s21_remove_filds(data_t *data);
int s21_string_to_double(char *str, int *step, double *number);
int s21_Euler_search(char *str, int *step, double *num);
int s21_matrix_to_array(matrix_t matrix, double *array, double *array2);
void s21_remove_array(data_t *data);
int s21_skip_space(char *str, int *step);
int s21_is_space(char c);
int s21_num_digits(int num);
int s21_is_digit(char c);
void s21_array_minmax(data_t data, double *max_coord);
void s21_move_x(data_t *data, long double a_now, long double a_prev);
void s21_move_y(data_t *data, long double a_now, long double a_prev);
void s21_move_z(data_t *data, long double a_now, long double a_prev);
void s21_rotate_x(data_t *data, long double angle, long double angle_prev);
void s21_rotate_y(data_t *data, long double angle, long double angle_prev);
void s21_rotate_z(data_t *data, long double angle, long double angle_prev);
long double s21_degres_to_radians(long double angle);
void s21_scale(data_t *data, long double scale, long double scale_prev);

#endif
