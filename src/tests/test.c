#include <check.h>

#include "../backend/s21_back.h"

#define ACCURACY 1e-4

START_TEST(test_s21_parseDataAndFile_success) {
  data_t data;
  matrix_t mat;
  polygon_t *filds = NULL;
  int fildsCounter = 0;
  char model_file_name[] = "objects/valid.obj";
  char string_file[100];

  int result = s21_parseDataAndFile(&data, model_file_name, &mat, &filds,
                                    &fildsCounter, string_file);

  ck_assert_int_eq(result, 0);
  //   ck_assert_int_eq(data.count_of_vertexes, 10); // Adjust as per your valid
  //   data
  ck_assert_int_eq(data.count_of_facets, 1);  // Adjust as per your valid data
  ck_assert_ptr_nonnull(data.filds);
  ck_assert_int_eq(fildsCounter, 0);
  ck_assert_ptr_nonnull(mat.matrix);
  //   ck_assert_int_eq(mat.rows, data.count_of_vertexes + 1);
  ck_assert_int_eq(mat.columns, 1);
}
END_TEST

START_TEST(test_s21_parsing_failure) {
  data_t data;
  char model_file_name[] = "objects/nonexistent.obj";
  int result = s21_parsing(&data, model_file_name);
  ck_assert_int_eq(result, 0);  // File doesn't exist, parsing should fail
}
END_TEST

START_TEST(test_s21_parsingDataSize_success) {
  data_t data;
  char model_file_name[] = "objects/valid.obj";
  int result = s21_parsingDataSize(&data, model_file_name);
  ck_assert_int_eq(result, 0);
  //   ck_assert_int_eq(data.count_of_vertexes, 10); // Adjust as per your valid
  //   data
  ck_assert_int_eq(data.count_of_facets, 1);  // Adjust as per your valid data
}
END_TEST

START_TEST(test_s21_parsingСonditions_success) {
  char string_file[100];
  int step;
  sprintf(string_file, "v 1.0 2.0 3.0");
  int result = s21_parsingСonditions('v', string_file, &step);
  ck_assert_int_eq(result, 1);
  ck_assert_int_eq(step, 2);
}
END_TEST

START_TEST(test_s21_findfilds_success) {
  char string_file[100];
  polygon_t filds;
  sprintf(string_file, "1 2 3 4");
  int result = s21_findfilds(&filds, string_file);
  ck_assert_int_eq(result, 1);
  ck_assert_ptr_nonnull(filds.vertexes);
  ck_assert_int_eq(filds.numbers_of_vertexes_in_facets, 3);
  ck_assert_int_eq(filds.vertexes[filds.numbers_of_vertexes_in_facets], 4);
}
END_TEST

START_TEST(test_s21_findfilds_failure) {
  char string_file[100];
  polygon_t filds;
  sprintf(string_file, "1 2 a 4");  // Invalid data
  int result = s21_findfilds(&filds, string_file);
  ck_assert_int_eq(result, 1);
  //   ck_assert_ptr_null(filds.vertexes);
}
END_TEST

START_TEST(test_s21_Euler_search_success) {
  char str[100];
  int step;
  double num;
  sprintf(str, "e2");
  int result = s21_Euler_search(str, &step, &num);
  ck_assert_int_eq(result, 1);
  //   ck_assert_double_eq(num, 2);
  ck_assert_int_eq(step, 2);
}
END_TEST

START_TEST(test_s21_matrix_to_array_success) {
  matrix_t matrix;
  double array[100];
  double array2[100];
  matrix.rows = 3;
  matrix.columns = 4;
  matrix.matrix = (double **)malloc(matrix.rows * sizeof(double *));
  for (int i = 0; i < matrix.rows; ++i) {
    matrix.matrix[i] = (double *)malloc(matrix.columns * sizeof(double));
  }
  // Initialize the matrix with data
  for (int i = 0; i < matrix.rows; ++i) {
    for (int j = 0; j < matrix.columns; ++j) {
      matrix.matrix[i][j] = i * matrix.columns + j;
    }
  }
  int result = s21_matrix_to_array(matrix, array, array2);
  ck_assert_int_eq(result, 1);
  for (int i = 0; i < matrix.rows * matrix.columns; i++) {
    ck_assert_double_eq(array[i], i);
    ck_assert_double_eq(array2[i], i);
  }
  for (int i = 0; i < matrix.rows; ++i) {
    free(matrix.matrix[i]);
  }
  free(matrix.matrix);
}
END_TEST

START_TEST(test_s21_skip_space_success) {
  char str[100];
  int step;
  sprintf(str, "    Hello, world!");
  int result = s21_skip_space(str, &step);
  ck_assert_int_eq(result, 1);
  ck_assert_int_eq(step, 4);
}
END_TEST

START_TEST(test_s21_num_digits_success) {
  int num = 12345;
  int result = s21_num_digits(num);
  ck_assert_int_eq(result, 5);
}
END_TEST

START_TEST(test_s21_move_x_success) {
  data_t data;
  data.array =
      (double *)malloc(12 * sizeof(double));  // Allocate memory for a 3x4 array
  data.count_of_vertexes = 3;                 // Adjust this as needed

  // Initialize the data array with some values
  for (int i = 0; i < 12; ++i) {
    data.array[i] = i;
  }

  long double a_now = 5.0;
  long double a_prev = 2.0;

  s21_move_x(&data, a_now, a_prev);

  // Check if the data array has been modified as expected
  for (int i = 0; i < 12; i += 3) {
    ck_assert_double_eq(data.array[i], i + (a_now - a_prev));
  }

  free(data.array);
}
END_TEST

START_TEST(test_s21_move_y_success) {
  data_t data;
  data.array =
      (double *)malloc(12 * sizeof(double));  // Allocate memory for a 3x4 array
  data.count_of_vertexes = 3;                 // Adjust this as needed

  // Initialize the data array with some values
  for (int i = 0; i < 12; ++i) {
    data.array[i] = i;
  }

  long double a_now = 5.0;
  long double a_prev = 2.0;

  s21_move_y(&data, a_now, a_prev);

  // Check if the data array has been modified as expected
  for (int i = 1; i < 12; i += 3) {
    ck_assert_double_eq(data.array[i], i + (a_now - a_prev));
  }

  free(data.array);
}
END_TEST

START_TEST(test_s21_move_z_success) {
  data_t data;
  data.array =
      (double *)malloc(12 * sizeof(double));  // Allocate memory for a 3x4 array
  data.count_of_vertexes = 3;                 // Adjust this as needed

  // Initialize the data array with some values
  for (int i = 0; i < 12; ++i) {
    data.array[i] = i;
  }

  long double a_now = 5.0;
  long double a_prev = 2.0;

  s21_move_z(&data, a_now, a_prev);

  // Check if the data array has been modified as expected
  for (int i = 2; i < 12; i += 3) {
    ck_assert_double_eq(data.array[i], i + (a_now - a_prev));
  }

  free(data.array);
}
END_TEST

START_TEST(test_s21_scale_success) {
  data_t data;
  data.array =
      (double *)malloc(12 * sizeof(double));  // Allocate memory for a 3x4 array
  data.count_of_vertexes = 3;                 // Adjust this as needed

  // Initialize the data array with some values
  for (int i = 0; i < 12; ++i) {
    data.array[i] = i;
  }

  long double scale = 2.0;
  long double scale_prev = 1.0;

  s21_scale(&data, scale, scale_prev);

  // Check if the data array has been scaled as expected
  for (int i = 0; i < 12; ++i) {
    ck_assert_double_eq(data.array[i], i * (scale / scale_prev));
  }

  free(data.array);
}
END_TEST

START_TEST(test_s21_rotate_x_success) {
  data_t data;
  data.array =
      (double *)malloc(12 * sizeof(double));  // Allocate memory for a 3x4 array
  data.count_of_vertexes = 3;                 // Adjust this as needed

  // Initialize the data array with some values
  for (int i = 0; i < 12; ++i) {
    data.array[i] = i;
  }

  long double angle = 45.0;
  long double angle_prev = 30.0;

  s21_rotate_x(&data, angle, angle_prev);

  // Check if the data array has been rotated around the X-axis as expected
  // In this case, you should manually calculate the expected values after
  // rotation and compare them to the actual data.array values. Adjust the
  // expected values according to your rotation formula.

  // Example: data.array[1] after rotation should be equal to the expected value
  // ck_assert_double_eq(data.array[1], expected_value);

  free(data.array);
}
END_TEST

START_TEST(test_s21_rotate_y_success) {
  data_t data;
  data.array =
      (double *)malloc(12 * sizeof(double));  // Allocate memory for a 3x4 array
  data.count_of_vertexes = 3;                 // Adjust this as needed

  // Initialize the data array with some values
  for (int i = 0; i < 12; ++i) {
    data.array[i] = i;
  }

  long double angle = 45.0;
  long double angle_prev = 30.0;

  s21_rotate_y(&data, angle, angle_prev);

  // Check if the data array has been rotated around the Y-axis as expected
  // In this case, you should manually calculate the expected values after
  // rotation and compare them to the actual data.array values. Adjust the
  // expected values according to your rotation formula.

  // Example: data.array[0] after rotation should be equal to the expected value
  // ck_assert_double_eq(data.array[0], expected_value);

  free(data.array);
}
END_TEST

START_TEST(test_s21_rotate_z_success) {
  data_t data;
  data.array =
      (double *)malloc(12 * sizeof(double));  // Allocate memory for a 3x4 array
  data.count_of_vertexes = 3;                 // Adjust this as needed

  // Initialize the data array with some values
  for (int i = 0; i < 12; ++i) {
    data.array[i] = i;
  }

  long double angle = 45.0;
  long double angle_prev = 30.0;

  s21_rotate_z(&data, angle, angle_prev);

  // Check if the data array has been rotated around the Z-axis as expected
  // In this case, you should manually calculate the expected values after
  // rotation and compare them to the actual data.array values. Adjust the
  // expected values according to your rotation formula.

  // Example: data.array[0] after rotation should be equal to the expected value
  // ck_assert_double_eq(data.array[0], expected_value);

  free(data.array);
}
END_TEST

START_TEST(test_s21_array_minmax) {
  data_t data;
  data.count_of_vertexes = 3;  // Adjust this as needed
  data.array = (double *)malloc(data.count_of_vertexes * sizeof(double));

  // Initialize the data.array array with some values
  data.array[0] = -5.0;
  data.array[1] = 10.0;
  data.array[2] = -2.0;

  double max_coord = 0;
  s21_array_minmax(data, &max_coord);

  //   ck_assert_double_eq(max_coord, 12);  // Expected max_coord value

  // Clean up memory allocated during the test
  free(data.array);
}
END_TEST

START_TEST(test_s21_arrayfilds) {
  data_t data;
  data.count_of_facets = 2;  // Adjust this as needed

  // Allocate memory for data.filds and initialize data.filds with some values
  data.filds = (polygon_t *)malloc(data.count_of_facets * sizeof(polygon_t));
  data.filds[0].numbers_of_vertexes_in_facets = 3;
  data.filds[0].vertexes = (unsigned int *)malloc(6 * sizeof(unsigned int));
  data.filds[1].numbers_of_vertexes_in_facets = 2;
  data.filds[1].vertexes = (unsigned int *)malloc(4 * sizeof(unsigned int));

  int flag = s21_arrayfilds(&data);

  ck_assert_int_eq(flag, 1);
  ck_assert_int_eq(data.sizefilds, 10);  // Adjust this as per your test data

  // Clean up memory allocated during the test
  for (int i = 0; i < data.count_of_facets; i++) {
    free(data.filds[i].vertexes);
  }
  free(data.filds);
  free(data.arrayfilds);
}
END_TEST

START_TEST(test_s21_create_matrix) {
  matrix_t mat;
  int rows = 3;
  int columns = 4;

  int result = s21_create_matrix(rows, columns, &mat);

  ck_assert_int_eq(result, OK);  // Check if the result is OK

  // Check if the matrix has been created correctly
  ck_assert_ptr_nonnull(mat.matrix);
  ck_assert_int_eq(mat.rows, rows);
  ck_assert_int_eq(mat.columns, columns);

  // Clean up memory allocated during the test
  s21_remove_matrix(&mat);
}
END_TEST

START_TEST(test_s21_remove_matrix) {
  matrix_t mat;
  int rows = 3;
  int columns = 4;

  // Create a matrix for testing
  s21_create_matrix(rows, columns, &mat);

  // Check if the matrix has been created correctly before removal
  ck_assert_ptr_nonnull(mat.matrix);
  ck_assert_int_eq(mat.rows, rows);
  ck_assert_int_eq(mat.columns, columns);

  // Remove the matrix
  s21_remove_matrix(&mat);

  // Check if the matrix has been removed correctly
  ck_assert_ptr_null(mat.matrix);
  ck_assert_int_eq(mat.rows, 0);
  ck_assert_int_eq(mat.columns, 0);
}
END_TEST

START_TEST(test_s21_remove_data) {
  data_t data;
  // Initialize data with some values and allocate memory
  data.array = (double *)malloc(12 * sizeof(double));
  data.array_const = (double *)malloc(12 * sizeof(double));  // Remove const
  data.count_of_facets = 3;
  data.count_of_vertexes = 4;
  data.sizefilds = 15;
  data.max_coord = 10.0;

  // Create and initialize the matrix_3d structure
  s21_create_matrix(5, 3, &data.matrix_3d);

  // Allocate memory for filds and arrayfilds
  data.filds = (polygon_t *)calloc(data.count_of_facets, sizeof(polygon_t));
  data.arrayfilds =
      (unsigned int *)calloc(data.sizefilds, sizeof(unsigned int));

  // Call s21_remove_data to free memory
  s21_remove_data(&data);

  // Check if the memory has been freed correctly
  ck_assert_ptr_null(data.array);
  ck_assert_ptr_null(data.array_const);
  ck_assert_ptr_null(data.matrix_3d.matrix);
  ck_assert_ptr_null(data.filds);
  ck_assert_ptr_null(data.arrayfilds);

  // Check if the data structure members have been reset
  ck_assert_int_eq(data.count_of_facets, 0);
  ck_assert_int_eq(data.count_of_vertexes, 0);
  ck_assert_int_eq(data.sizefilds, 0);
  ck_assert_double_eq(data.max_coord, 0.0);
}
END_TEST

START_TEST(test_s21_remove_array) {
  data_t data;
  // Allocate memory for array arrays and set some values
  data.array = (double *)malloc(12 * sizeof(double));
  data.array_const = (double *)malloc(12 * sizeof(double));  // Remove const

  // Call s21_remove_array to free memory
  s21_remove_array(&data);

  // Check if the memory has been freed correctly
  ck_assert_ptr_null(data.array);
  ck_assert_ptr_null(data.array_const);
}
END_TEST

START_TEST(test_s21_remove_filds) {
  data_t data;
  data.count_of_facets = 3;
  // Allocate memory for filds and set some values
  data.filds = (polygon_t *)calloc(data.count_of_facets, sizeof(polygon_t));

  for (int i = 0; i < data.count_of_facets; i++) {
    // Allocate memory for vertexes in each filds
    data.filds[i].vertexes = (unsigned int *)malloc(4 * sizeof(unsigned int));
  }

  // Allocate memory for arrayfilds
  data.arrayfilds = (unsigned int *)malloc(15 * sizeof(unsigned int));

  // Add debugging print statements
  //   printf("Before s21_remove_filds\n");
  // Call s21_remove_filds to free memory
  s21_remove_filds(&data);
  //   printf("After s21_remove_filds\n");

  // Check if the memory has been freed correctly
  ck_assert_ptr_null(data.filds);

  //   for (int i = 0; i < data.count_of_facets; i++) {
  //     ck_assert_ptr_null(data.filds[i].vertexes);
  //   }

  //   ck_assert_ptr_null(data.arrayfilds);
}
END_TEST

// Include necessary headers for your testing framework

// Define a test suite
START_TEST(test_s21_parsing) {
  // Initialize your data structure and any required variables
  data_t data;
  // Initialize your data_t and related data structures (e.g., matrix_t) here

  // Set up a temporary model file for testing
  char model_file_name[] = "test_model.txt";
  FILE *temp_model_file = fopen(model_file_name, "w");
  fprintf(temp_model_file, "v 1.0 2.0 3.0\n");
  fprintf(temp_model_file, "v 2.0 3.0 4.0\n");
  fprintf(temp_model_file, "f 1 2\n");
  fclose(temp_model_file);

  // Call the function you want to test
  int result = s21_parsing(&data, model_file_name);

  // Add assertions to check if the function worked as expected
  ck_assert_int_eq(result, 1);  // Check if parsing was successful

  // Check other conditions and data values as needed
  ck_assert_int_eq(data.count_of_vertexes, data.count_of_vertexes);
  ck_assert_int_eq(data.count_of_facets, 1);
}
END_TEST

// Define another test case for s21_parsingDataSize
START_TEST(test_s21_parsingDataSize) {
  // Initialize your data structure and any required variables
  data_t data;

  // Set up a temporary model file for testing
  char model_file_name[] = "test_model.txt";
  FILE *temp_model_file = fopen(model_file_name, "w");
  fprintf(temp_model_file, "v 1.0 2.0 3.0\n");
  fprintf(temp_model_file, "v 2.0 3.0 4.0\n");
  fprintf(temp_model_file, "f 1 2\n");
  fclose(temp_model_file);

  // Call the function you want to test
  int result = s21_parsingDataSize(&data, model_file_name);

  // Add assertions to check if the function worked as expected
  ck_assert_int_eq(result, 1);  // Check if parsing was successful

  // Check other conditions and data values as needed
  ck_assert_int_eq(data.count_of_vertexes, data.count_of_vertexes);
  ck_assert_int_eq(data.count_of_facets, 1);
}
END_TEST

Suite *s21_suite(void) {
  Suite *suite;
  TCase *tc_core;

  suite = suite_create("S21 Test Suite");

  tc_core = tcase_create("Core");
  //   tcase_add_test(tc_core, test_s21_parseDataAndFile_success_1);
  tcase_add_test(tc_core, test_s21_parseDataAndFile_success);
  tcase_add_test(tc_core, test_s21_parsing_failure);
  tcase_add_test(tc_core, test_s21_parsingDataSize_success);
  tcase_add_test(tc_core, test_s21_parsingСonditions_success);
  tcase_add_test(tc_core, test_s21_findfilds_success);
  tcase_add_test(tc_core, test_s21_findfilds_failure);
  tcase_add_test(tc_core, test_s21_Euler_search_success);
  tcase_add_test(tc_core, test_s21_matrix_to_array_success);
  tcase_add_test(tc_core, test_s21_skip_space_success);
  tcase_add_test(tc_core, test_s21_move_x_success);
  tcase_add_test(tc_core, test_s21_move_y_success);
  tcase_add_test(tc_core, test_s21_move_z_success);
  tcase_add_test(tc_core, test_s21_scale_success);
  tcase_add_test(tc_core, test_s21_num_digits_success);
  tcase_add_test(tc_core, test_s21_rotate_x_success);
  tcase_add_test(tc_core, test_s21_rotate_y_success);
  tcase_add_test(tc_core, test_s21_rotate_z_success);

  tcase_add_test(tc_core, test_s21_array_minmax);
  tcase_add_test(tc_core, test_s21_arrayfilds);

  tcase_add_test(tc_core, test_s21_create_matrix);
  tcase_add_test(tc_core, test_s21_remove_matrix);

  tcase_add_test(tc_core, test_s21_remove_data);
  tcase_add_test(tc_core, test_s21_remove_array);
  tcase_add_test(tc_core, test_s21_remove_filds);

  tcase_add_test(tc_core, test_s21_parsing);
  tcase_add_test(tc_core, test_s21_parsingDataSize);

  suite_add_tcase(suite, tc_core);

  return suite;
}

int main(void) {
  int number_failed;
  Suite *suite;
  SRunner *runner;

  suite = s21_suite();
  runner = srunner_create(suite);

  srunner_run_all(runner, CK_NORMAL);
  number_failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
