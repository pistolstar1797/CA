/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 

#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */

char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int row_i, col_i, r, c;
	int q, w, e, a, s, d, z, x;
	
	if (N == 32) {
		for(col_i = 0 ; col_i < N ; col_i += 8) {
			for(row_i = 0 ; row_i < N ; row_i += 8) {
				for(r = row_i ; r < row_i + 8 ; r++) {
					for(c = col_i ; c < col_i + 8 ; c++) {
						if(r != c) {
							B[c][r] = A[r][c];
						} else {
                            z = A[r][c];
                            x = r;
						}
					}

					if (row_i == col_i) {
						B[x][x] = z;
					}
				}
			}
		}
	}

	else if (N == 64) {	
        for(col_i = 0 ; col_i < 64 ; col_i += 8) {
            for(row_i = 0 ; row_i < 64 ; row_i += 8){
                for(c = 0 ; c < 4 ; c++){ 
                    q = A[col_i + c][row_i];
                    w = A[col_i + c][row_i + 1];
                    e = A[col_i + c][row_i + 2];
                    a = A[col_i + c][row_i + 3];
                    s = A[col_i + c][row_i + 4];
                    d = A[col_i + c][row_i + 5];
                    z = A[col_i + c][row_i + 6];
                    x = A[col_i + c][row_i + 7];

                    B[row_i][col_i + c] = q;
                    B[row_i][col_i + c + 4] = d;
                    B[row_i + 1][col_i + c] = w;
                    B[row_i + 1][col_i + c + 4] = z;
                    B[row_i + 2][col_i + c] = e;
                    B[row_i + 2][col_i + c + 4] = x;
                    B[row_i + 3][col_i + c] = a;
                    B[row_i + 3][col_i + c + 4] = s;
                }

                q = A[col_i + 4][row_i + 4];
                s = A[col_i + 4][row_i + 3];
                w = A[col_i + 5][row_i + 4];
                d = A[col_i + 5][row_i + 3];
                e = A[col_i + 6][row_i + 4];
                z = A[col_i + 6][row_i + 3];
                a = A[col_i + 7][row_i + 4];
                x = A[col_i + 7][row_i + 3];

                B[row_i + 4][col_i] = B[row_i + 3][col_i + 4];
                B[row_i + 4][col_i + 1] = B[row_i + 3][col_i + 5];
                B[row_i + 4][col_i + 2] = B[row_i + 3][col_i + 6];
                B[row_i + 4][col_i + 3] = B[row_i + 3][col_i + 7];
                B[row_i + 4][col_i + 4] = q;
                B[row_i + 4][col_i + 5] = w;
                B[row_i + 4][col_i + 6] = e;
                B[row_i + 4][col_i + 7] = a;
                B[row_i + 3][col_i + 4] = s;
                B[row_i + 3][col_i + 5] = d;
                B[row_i + 3][col_i + 6] = z;
                B[row_i + 3][col_i + 7] = x;

                for(c = 0 ; c < 3 ; c++){
                    q = A[col_i + 4][row_i + 5 + c];
                    w = A[col_i + 5][row_i + 5 + c];
                    e = A[col_i + 6][row_i + 5 + c];
                    a = A[col_i + 7][row_i + 5 + c];
                    s = A[col_i + 4][row_i + c];
                    d = A[col_i + 5][row_i + c];
                    z = A[col_i + 6][row_i + c];
                    x = A[col_i + 7][row_i + c];

                    B[row_i + 5 + c][col_i] = B[row_i + c][col_i + 4];
                    B[row_i + 5 + c][col_i + 1] = B[row_i + c][col_i + 5];
                    B[row_i + 5 + c][col_i + 2] = B[row_i + c][col_i + 6];
                    B[row_i + 5 + c][col_i + 3] = B[row_i + c][col_i + 7];
                    B[row_i + 5 + c][col_i + 4] = q;
                    B[row_i + 5 + c][col_i + 5] = w;
                    B[row_i + 5 + c][col_i + 6] = e;
                    B[row_i + 5 + c][col_i + 7] = a;
                    B[row_i + c][col_i + 4] = s;
                    B[row_i + c][col_i + 5] = d;
                    B[row_i + c][col_i + 6] = z;
                    B[row_i + c][col_i + 7] = x;
                }
            }
        }
    }

	else {
		for (col_i = 0 ; col_i < M ; col_i += 16) {
			for (row_i = 0 ; row_i < N ; row_i += 16) {	
				for(r = row_i ; (r < N) && (r < row_i + 16) ; r++) {
					for(c = col_i ; (c < M) && (c < col_i + 16) ; c++) {
						if (r != c) {
							B[c][r] = A[r][c];
						} else {
							z = A[r][c];
							x = r;
						}
					}
					
					if(row_i == col_i) {
						B[x][x] = z;
					}
				}
			}
		}
	}
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */

char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, tmp;

    for (i = 0 ; i < N ; i++) {
        for (j = 0 ; j < M ; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */

void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */

int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    int i, j;

    for (i = 0 ; i < N ; i++) {
        for (j = 0 ; j < M ; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }

    return 1;
}