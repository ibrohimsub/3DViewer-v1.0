#include "s21_back.h"

int s21_parseDataAndFile(data_t *data, char *model_file_name, matrix_t *mat,
                         polygon_t **filds, int *fildsCounter,
                         char *string_file) {
  int flag = 1;
  FILE *f = NULL;
  int row = 1;

  if (!s21_parsingDataSize(data, model_file_name)) {
    flag = 0;
  } else {
    *filds = (polygon_t *)calloc(data->count_of_facets, sizeof(polygon_t));

    if (*filds == NULL) {
      flag = 0;
    } else if (s21_create_matrix(data->count_of_vertexes + 1, 3, mat) != 0) {
      flag = 0;
    } else if ((f = fopen(model_file_name, "r")) == NULL) {
      flag = 0;
    } else {
      while (flag && fgets(string_file, S_SIZE, f)) {
        int step = 0;

        if (s21_parsingСonditions('v', string_file, &step)) {
          for (int i = 0; i < 3; i++) {
            int s = 0;
            double num = 0;
            s21_string_to_double(&string_file[step], &s, &num);
            mat->matrix[row][i] = num;
            step += s;
          }
          row++;
        } else if (s21_parsingСonditions('f', string_file, 0)) {
          if (!s21_findfilds(&(*filds)[*fildsCounter], string_file)) {
            flag = 0;
          } else {
            (*fildsCounter)++;
          }
        }
        string_file[0] = 0;
      }
      fclose(f);

      if (flag) {
        data->matrix_3d = *mat;
        data->filds = *filds;

        if (!s21_arrayfilds(data)) {
          flag = 0;
        }
      }
    }
  }

  return flag;
}

int s21_parsing(data_t *data, char *model_file_name) {
  int flag = 1;
  char string_file[S_SIZE] = {'\0'};
  matrix_t mat = {0};
  int fildsCounter = 0;

  flag = s21_parseDataAndFile(data, model_file_name, &mat, &data->filds,
                              &fildsCounter, string_file);

  if (flag) {
    data->array =
        (double *)calloc((data->count_of_vertexes + 1) * 3, sizeof(double));
    data->array_const =
        (double *)calloc((data->count_of_vertexes + 1) * 3, sizeof(double));

    if (data->array_const != NULL && data->array != NULL) {
      s21_matrix_to_array(data->matrix_3d, data->array, data->array_const);
      s21_array_minmax(*data, &data->max_coord);
    } else {
      flag = 0;
    }
  }

  return flag;
}

// Function to parse the number of vertices and filds
int s21_parsingDataSize(data_t *data, char *model_file_name) {
  int flag = 1;
  FILE *f;
  char string_file[S_SIZE] = {'\0'};

  if ((f = fopen(model_file_name, "r")) != NULL) {
    while (fgets(string_file, S_SIZE, f)) {
      if (s21_parsingСonditions('v', string_file, NULL)) {
        data->count_of_vertexes++;
      } else if (s21_parsingСonditions('f', string_file, NULL)) {
        data->count_of_facets++;
      }
      string_file[0] = 0;
    }
    fclose(f);
  } else {
    flag = 0;
  }

  return flag;
}

// Check if a line matches certain parsing conditions
int s21_parsingСonditions(char c, char *string_file, int *step) {
  int flag = 0;
  int s = 0;

  // Skip leading whitespace
  s21_skip_space(string_file, &s);

  if (string_file[s] == c && string_file[s + 1] == ' ') {
    flag = 1;
  }

  // Update the step if provided
  step ? (*step = s + 2) : 0;

  return flag;
}

// Function to free memory allocated for the data structure
void s21_remove_data(data_t *data) {
  s21_remove_array(data);
  s21_remove_matrix(&data->matrix_3d);
  s21_remove_filds(data);

  // Reset data structure members
  data->count_of_facets = 0;
  data->count_of_vertexes = 0;
  data->sizefilds = 0;
  data->max_coord = 0;
  data->matrix_3d.matrix = NULL;
}

// Function to free memory allocated for array arrays
void s21_remove_array(data_t *data) {
  // Free array arrays and set pointers to NULL
  free(data->array);
  free(data->array_const);
  data->array = NULL;
  data->array_const = NULL;
}

// Function to free memory allocated for filds
void s21_remove_filds(data_t *data) {
  if (data->filds != NULL) {
    // Free memory for individual filds and their vertexes
    for (int i = 0; i < data->count_of_facets; i++) {
      if (data->filds[i].vertexes != NULL) {
        free(data->filds[i].vertexes);
      }
    }
    free(data->filds);
  }

  // Free memory for arrayfilds and set pointers to NULL
  free(data->arrayfilds);
  data->filds = NULL;
  data->arrayfilds = NULL;
}

// Function to parse filds
int s21_findfilds(polygon_t *filds, char *string_file) {
  int step = 0;
  int i = 0;
  int flag = 1;

  // Allocate memory for vertexes with an initial capacity of 128
  filds->vertexes = (unsigned int *)calloc(128, sizeof(unsigned int));
  filds->numbers_of_vertexes_in_facets = 0;

  if (filds->vertexes != NULL) {
    while (step < (int)strlen(string_file)) {
      int s = 0;
      double num = 0;

      // Convert numbers from the string to integers
      if (s21_string_to_double(&string_file[step], &s, &num)) {
        step += s;

        // Check if the previous character is a space, indicating a new vertex
        if (string_file[step - s21_num_digits((int)num) - 1] == ' ') {
          if (filds->numbers_of_vertexes_in_facets != 0) {
            filds->vertexes[i++] = (unsigned int)num;
          }
          filds->vertexes[i++] = (unsigned int)num;
          filds->numbers_of_vertexes_in_facets++;
        }
      }
      step++;
    }

    // If there are more than one vertex, close the polygon
    if (filds->numbers_of_vertexes_in_facets > 1) {
      filds->vertexes[i++] = filds->vertexes[0];
    }
  } else {
    flag = 0;
  }
  return flag;
}

// Function to search for Euler's number
int s21_Euler_search(char *str, int *step, double *num) {
  int e = 0;
  int negative = 1;
  int flag = 1;
  int i = 0;

  if (str[i] == 'e' || str[i] == 'E') {
    i++;
    if (str[i] && (str[i] == '-' || str[i] == '+')) {
      if (str[i] == '-') {
        negative = -1;
      }
      i++;
    }
    if (str[i] && s21_is_digit(str[i]) == 1) {
      e = (e * 10) + (int)(str[i] - '0');
      i++;
    } else {
      flag = 0;
    }
    if (str[i] && s21_is_digit(str[i]) == 1) {
      e = (e * 10) + (int)(str[i] - '0');
    }
  } else {
    flag = 0;
  }
  double ten = 10;

  if (flag == 1) {
    if (negative == -1) {
      ten = 0.1;
    }
    for (int i = 0; i < e; i++) {
      *num *= ten;
    }
  }
  *step = i;
  return flag;
}

// Function to convert a matrix to an array
int s21_matrix_to_array(matrix_t matrix, double *array, double *array2) {
  int flag = 1;
  int arraySize = 0;

  for (int i = 0; i < matrix.rows; i++) {
    for (int j = 0; j < matrix.columns; j++) {
      array[arraySize] = array2[arraySize] = matrix.matrix[i][j];
      arraySize++;
    }
  }
  return flag;
}

// Function to skip leading spaces
int s21_skip_space(char *str, int *step) {
  int flag = 0;
  *step = 0;
  while (str[*step] != '\0') {
    if (str[*step] == ' ' || str[*step] == '\t' || str[*step] == '\n' ||
        str[*step] == '\v' || str[*step] == '\f' || str[*step] == '\r') {
      (*step)++;
      flag = 1;
    } else {
      break;
    }
  }
  return flag;
}

// Function to count the number of digits in an integer
int s21_num_digits(int num) {
  int count = 0;
  if (num == 0) {
    count = 1;
  } else if (num < 0) {
    count++;
    num = -num;
  }
  while (num > 0) {
    count++;
    num /= 10;
  }
  return count;
}

// Function to check if a character is a digit
int s21_is_digit(char c) { return (c >= '0' && c <= '9'); }

// Function to calculate the maximum coordinate for scaling
void s21_array_minmax(data_t data, double *max_coord) {
  *max_coord = 0;
  double max_array = 0;
  double min_array = 0;

  // Iterate through the data.array array to find the minimum and maximum
  // values
  for (int i = 0; i < data.count_of_vertexes; i++) {
    if (data.array[i] > max_array) {
      max_array = data.array[i];
    }
    if (data.array[i] < min_array) {
      min_array = data.array[i];
    }
  }

  // Use a ternary operation to set *max_coord to the larger absolute value
  *max_coord = (fabs(min_array) > max_array) ? fabs(min_array) : max_array;

  // Scale *max_coord by 1.2 for better visualization
  *max_coord *= 1.2f;
}

// Function to calculate the total size of filds
int s21_arrayfilds(data_t *data) {
  int flag = 1;

  // Calculate the total size of filds
  data->sizefilds = 0;
  for (int i = 0; i < data->count_of_facets; i++) {
    data->sizefilds += data->filds[i].numbers_of_vertexes_in_facets * 2;
  }

  // Allocate memory for data->arrayfilds
  data->arrayfilds =
      (unsigned int *)calloc(data->sizefilds, sizeof(unsigned int));

  if (data->arrayfilds != NULL) {
    data->sizefilds = 0;

    // Copy vertexes from data->filds to data->arrayfilds
    for (int i = 0; i < data->count_of_facets; i++) {
      for (int j = 0; j < data->filds[i].numbers_of_vertexes_in_facets * 2;
           j++) {
        data->arrayfilds[data->sizefilds++] = data->filds[i].vertexes[j];
      }
    }
  } else {
    flag = 0;
  }
  return flag;
}

int s21_string_to_double(char *str, int *step, double *number) {
  int flag = 1;
  double num = 0;
  int dot = 0;
  int i = 0;
  int neg = 1;
  s21_skip_space(str, &i);
  if (s21_is_digit(str[i]) == 0) {
    if (str[i] == '-' || str[i] == '+') {
      if (str[i] == '-') {
        neg = -1;
      }
      i++;
    } else {
      flag = 0;
    }
  }
  if (flag == 1) {
    for (;; i++) {
      if (s21_is_digit(str[i]) == 0) {
        if (str[i] != '.') {
          break;
        }
      }
      if (str[i] == '.') {
        dot = 10;
      }
      if (dot == 0) {
        num = (num * 10) + (double)(str[i] - '0');
      } else if (str[i] != '.') {
        num += (double)(str[i] - '0') / dot;
        dot *= 10;
      }
    }
  }
  *step = i;
  if (s21_Euler_search(&str[i], &i, &num)) {
    *step = i;
  }
  *number = num * neg;
  return flag;
}

// Function to move the 3D model along the X-axis
void s21_move_x(data_t *data, long double a_now, long double a_prev) {
  if (data->array) {
    for (int i = 0; i < (data->count_of_vertexes + 1) * 3; i += 3) {
      data->array[i] += (a_now - a_prev);
    }
  }
}

// Function to move the 3D model along the Y-axis
void s21_move_y(data_t *data, long double a_now, long double a_prev) {
  if (data->array) {
    for (int i = 1; i < (data->count_of_vertexes + 1) * 3; i += 3) {
      data->array[i] += (a_now - a_prev);
    }
  }
}

// Function to move the 3D model along the Z-axis
void s21_move_z(data_t *data, long double a_now, long double a_prev) {
  if (data->array) {
    for (int i = 2; i < (data->count_of_vertexes + 1) * 3; i += 3) {
      data->array[i] += (a_now - a_prev);
    }
  }
}

// Function to rotate the 3D model around the X-axis
void s21_rotate_x(data_t *data, long double angle, long double angle_prev) {
  angle = s21_degres_to_radians(angle);
  angle_prev = s21_degres_to_radians(angle_prev);
  if (data->array) {
    for (int i = 0; i < (data->count_of_vertexes + 1) * 3; i += 3) {
      long double y = data->array[i + 1];
      long double z = data->array[i + 2];

      data->array[i + 1] =
          y * cosl(angle - angle_prev) + z * sinl(angle - angle_prev);
      data->array[i + 2] =
          -y * sinl(angle - angle_prev) + z * cosl(angle - angle_prev);
    }
  }
}

// Function to rotate the 3D model around the Y-axis
void s21_rotate_y(data_t *data, long double angle, long double angle_prev) {
  angle = s21_degres_to_radians(angle);
  angle_prev = s21_degres_to_radians(angle_prev);
  if (data->array) {
    for (int i = 0; i < (data->count_of_vertexes + 1) * 3; i += 3) {
      long double x = data->array[i];
      long double z = data->array[i + 2];

      data->array[i] =
          x * cosl(angle - angle_prev) + z * sinl(angle - angle_prev);
      data->array[i + 2] =
          -x * sinl(angle - angle_prev) + z * cosl(angle - angle_prev);
    }
  }
}

// Function to rotate the 3D model around the Z-axis
void s21_rotate_z(data_t *data, long double angle, long double angle_prev) {
  angle = s21_degres_to_radians(angle);
  angle_prev = s21_degres_to_radians(angle_prev);
  if (data->array) {
    for (int i = 0; i < (data->count_of_vertexes + 1) * 3; i += 3) {
      long double x = data->array[i];
      long double y = data->array[i + 1];

      data->array[i] =
          x * cosl(angle - angle_prev) - y * sinl(angle - angle_prev);
      data->array[i + 1] =
          x * sinl(angle - angle_prev) + y * cosl(angle - angle_prev);
    }
  }
}

// Function to convert degrees to radians
long double s21_degres_to_radians(long double angle) {
  return angle * M_PI / 180.0;
}

// Function to scale the 3D model
void s21_scale(data_t *data, long double scale, long double scale_prev) {
  if (data->array) {
    for (int i = 0; i < (data->count_of_vertexes + 1) * 3; i++) {
      data->array[i] *= scale / scale_prev;
    }
  }
}

// Function to create a matrix
int s21_create_matrix(int rows, int columns, matrix_t *result) {
  int res = OK;
  if (rows > 0 && columns > 0) {
    result->matrix = (double **)calloc(rows, sizeof(double *));
    if (result->matrix) {
      result->matrix[0] = (double *)calloc(rows * columns, sizeof(double));
      for (int i = 1; i < rows; i++) {
        result->matrix[i] = result->matrix[0] + i * columns;
      }
    } else {
      res = CALC_ERROR;
    }
  } else {
    res = INCORRECT_MATRIX;
  }
  result->rows = rows;
  result->columns = columns;
  return res;
}

// Function to free memory allocated for a matrix
void s21_remove_matrix(matrix_t *A) {
  if (A->matrix) {
    free(A->matrix[0]);
    free(A->matrix);
  }
  A->columns = 0;
  A->rows = 0;
  A->matrix = NULL;
}
