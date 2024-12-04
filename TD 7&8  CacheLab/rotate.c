/*YESSIN MOAKHER yessin.moakher*/

/* 
 * rotate.c - Matrix rotation
 *
 * Each rotation function must have a prototype of the form:
 * void rotate(int N, int A[N][N], int B[N][N]);
 *
 * A rotate function is evaluated by counting the number of misses
 * on a 1KB direct-mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_rotate(int N, int A[N][N], int B[N][N]);

/* 
 * rotate_submit - This is the solution rotate function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Rotate submission", as the driver
 *     searches for that string to identify the rotate function to
 *     be graded. 
 */
char rotate_submit_desc[] = "Rotate submission";
void rotate_submit(int N, int A[N][N], int B[N][N]) {
    int i, j, x, y;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

    if (N == 32) {
        for (i = 0; i < N; i +=8) {
            for (j = 0; j < N; j +=8) {
                for (x = i; x < i + 8; x++) {
                    tmp0 = A[x][j + 0];
                    tmp1 = A[x][j + 1];
                    tmp2 = A[x][j + 2];
                    tmp3 = A[x][j + 3];
                    tmp4 = A[x][j + 4];
                    tmp5 = A[x][j + 5];
                    tmp6 = A[x][j + 6];
                    tmp7 = A[x][j + 7];

                    B[j + 0][N - 1 - x] = tmp0;
                    B[j + 1][N - 1 - x] = tmp1;
                    B[j + 2][N - 1 - x] = tmp2;
                    B[j + 3][N - 1 - x] = tmp3;
                    B[j + 4][N - 1 - x] = tmp4;
                    B[j + 5][N - 1 - x] = tmp5;
                    B[j + 6][N - 1 - x] = tmp6;
                    B[j + 7][N - 1 - x] = tmp7;
                }
            }
        }
    } 

   else {
      /*intended for N>=67*/
        int blockSize = 16;
        for (i = 0; i < N; i += blockSize) {
            for (j = 0; j < N; j += blockSize) {
                for (x = i; x < i + blockSize && x < N; ++x) {
                    for (y = j; y < j + blockSize && y < N; ++y) {
                        B[y][N - 1 - x] = A[x][y];
                    }
                }
            }
        }
    }
}


/* 
 * You can define additional rotate functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * rotate - A simple baseline rotate function, not optimized for the cache.
 */
char rotate_desc[] = "Simple row-wise scan rotate";
void rotate(int N, int A[N][N], int B[N][N])
{
  int row, col;
  
  for(row=0; row < N; ++row) {
    for(col=0; col < N; ++col) {
      B[col][N-1-row] = A[row][col];
    }
  }
}

/*
 * registerFunctions - This function registers your rotate
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     rotate strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerRotateFunction(rotate_submit, rotate_submit_desc); 

    /* Register any additional rotate functions */
    registerRotateFunction(rotate, rotate_desc); 

}

/* 
 * is_rotate - This helper function checks if B is the rotate of
 *     A. You can check the correctness of your rotate by calling
 *     it before returning from the rotate function.
 */
int is_rotate(int N, int A[N][N], int B[N][N])
{
  int row, col;
  
  for(row=0; row < N; ++row) {
    for(col=0; col < N; ++col) {
      if (B[col][N-1-row] != A[row][col])
	return 0;
    }
  }
  return 1;
}
