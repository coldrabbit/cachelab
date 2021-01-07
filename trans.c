/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */

/***
 * editor: songjie
 * time: 2020.1.7
 * function: matrix transpose
 ***/ 

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
    if(M == 32){//32x32 matrix divides to 16  8x8 blocks
        int i, j;//scan the matrix
        int k;//scan the 8x8 blocks by line
        int v0, v1, v2, v3, v4, v5, v6, v7;//deposit some block's one line in A
        for(i = 0; i < N; i += 8){
            for(j = 0; j < M; j += 8){
                for(k = i; k < i + 8; k++){
                    v0 = A[k][j];
                    v1 = A[k][j + 1];
                    v2 = A[k][j + 2];
                    v3 = A[k][j + 3];
                    v4 = A[k][j + 4];
                    v5 = A[k][j + 5];
                    v6 = A[k][j + 6];
                    v7 = A[k][j + 7];

                    B[j][k] = v0;
                    B[j + 1][k] = v1;
                    B[j + 2][k] = v2;
                    B[j + 3][k] = v3;
                    B[j + 4][k] = v4;
                    B[j + 5][k] = v5;
                    B[j + 6][k] = v6;
                    B[j + 7][k] = v7;
                }
            }
        }
    }
    else if(M == 64){//64x64 matrix
        int i, j;
        int k;
        int v0, v1, v2, v3, v4, v5, v6, v7;
        for(i = 0; i < N; i += 8){
            for(j = 0; j < M; j += 8){
                /*8x8 blocks divides to 4  4x4 blocks*/
                for(k = i; k < i + 4; k++){//the A's top left block trans to B's top left, A's top right block trans to B's top right
                    v0 = A[k][j];
                    v1 = A[k][j + 1];
                    v2 = A[k][j + 2];
                    v3 = A[k][j + 3];
                    v4 = A[k][j + 4];
                    v5 = A[k][j + 5];
                    v6 = A[k][j + 6];
                    v7 = A[k][j + 7];

                    B[j][k] = v0;
                    B[j + 1][k] = v1;
                    B[j + 2][k] = v2;
                    B[j + 3][k] = v3;
                    B[j][k + 4] = v4;
                    B[j + 1][k + 4] = v5;
                    B[j + 2][k + 4] = v6;
                    B[j + 3][k + 4] = v7;
                }

                for(k = j; k < j + 4; k++){//the B's top right block trans to B's bottom left, the A's bottom left block trans to B's top right
                    v0 = B[k][i + 4];
                    v1 = B[k][i + 5];
                    v2 = B[k][i + 6];
                    v3 = B[k][i + 7];
                    v4 = A[i + 4][k];
                    v5 = A[i + 5][k];
                    v6 = A[i + 6][k];
                    v7 = A[i + 7][k];

                    B[k][i + 4] = v4;
                    B[k][i + 5] = v5;
                    B[k][i + 6] = v6;
                    B[k][i + 7] = v7;
                    B[k + 4][i] = v0;
                    B[k + 4][i + 1] = v1;
                    B[k + 4][i + 2] = v2;
                    B[k + 4][i + 3] = v3;
                }

                for(k = i + 4; k < i + 8; k++){//the A's bottom right trans to B's bottom right
                    v0 = A[k][j + 4];
                    v1 = A[k][j + 5];
                    v2 = A[k][j + 6];
                    v3 = A[k][j + 7];

                    B[j + 4][k] = v0;
                    B[j + 5][k] = v1;
                    B[j + 6][k] = v2;
                    B[j + 7][k] = v3;
                }
            }
        } 
    }
    else{//61x67 matrix divides to 17x17 blocks can satisfy the demands,about 1950 misses
        int i, j;
        int k, l;
        for(i = 0; i < N; i += 17){
            for(j = 0; j < M; j += 17){
                for(k = i; k < N && k < i + 17; k++){
                    for(l = j; l < M && l < j + 17; l++)
                        B[l][k] = A[k][l];
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
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
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
void registerFunctions()
{
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
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

