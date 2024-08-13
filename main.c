#include <stdio.h>
#include <malloc.h>
#include <assert.h>

typedef short bool_t;
#define TRUE (bool_t)1
#define FALSE (bool_t)0

typedef struct vector {
    float *vec;
    int len;
} vector_t;

#define CREATE_EMPTY_VECTOR(v) \
vector_t v; \
v.vec = NULL; \
v.len = 0;

#define CREATE_VECTOR(v, l) \
vector_t v; \
v.vec = calloc(l, sizeof(float)); \
v.len = l;

#define DESTROY_VECTOR(v) \
free(v.vec);

#define PRINT_VECTOR(v) \
printf("["); \
for (int i=0; i<v.len; i++) printf("%0.3f ", v.vec[i]);\
printf("]\n");


typedef struct vector_2d {
    float **vec;
    int len1d;
    int len2d;
} vector_2d_t;

#define CREATE_VECTOR_2D(v, l2, l1) \
vector_2d_t v; \
v.vec = calloc(l2, sizeof(float*)); \
for (int i=0; i<l2;i++) {\
    v.vec[i] = calloc(l1, sizeof(float));\
} \
v.len1d = l1; \
v.len2d = l2;

#define DESTROY_VECTOR_2D(v) \
for (int i=0; i<v.len2d;i++) {\
free(v.vec[i]);}\
free(v.vec);

#define PRINT_VECTOR_2D(v) \
printf("[\n"); \
for (int i=0; i<v.len2d;i++) {\
    printf("["); \
    for (int j=0; j<v.len1d; j++) printf("%0.3f ", v.vec[i][j]);\
    printf("]\n"); }\
printf("]\n");



void linspace(float start, float end, int steps, vector_t v) {
    assert(steps > 0);

    float interval = (end-start)/(float)(steps-1);

    for (int i=0; i<steps; i++) {
        v.vec[i] = start+ interval*i;
    }
}

vector_2d_t DeBoor_function(vector_t points, vector_t knots, int k) {
    if (k==0) {

        CREATE_VECTOR_2D(vec2, points.len, knots.len-1);

        bool_t boarder_left;
        bool_t boarder_right;
        for (int i=0; i<points.len;i++) {
            for (int j = 0; j < knots.len - 1; j++) {
                boarder_left = (points.vec[i] >= knots.vec[j]) ? TRUE : FALSE;
                boarder_right = (points.vec[i] < knots.vec[j + 1]) ? TRUE : FALSE;
                vec2.vec[i][j] = (float)(boarder_left*boarder_right);
            }
        }

        return vec2;
    } else {
        vector_2d_t b_km1 = DeBoor_function(points, knots, k - 1);
        printf("k=%d: \n", k-1);
        PRINT_VECTOR_2D(b_km1);

        CREATE_VECTOR_2D(vec2, points.len, b_km1.len1d-1);

        float part0 = 0;
        float part1 = 0;
        for (int i = 0; i < vec2.len2d; i++) {
            for (int j = 0; j < vec2.len1d; j++) {
                part0 = b_km1.vec[i][j] == 0.f ? 0.f :
                        (points.vec[i] - knots.vec[j]) / (knots.vec[j + k] - knots.vec[j]) * b_km1.vec[i][j];
#if 1
                part1 = b_km1.vec[i][j+1] == 0.f ? 0.f :
                        (knots.vec[j + k + 1] - points.vec[i]) / (knots.vec[j + k + 1] - knots.vec[j + 1]) *
                        b_km1.vec[i][j+1];
                vec2.vec[i][j] = part0 + part1;
#else
                part1 = b_km1.vec[i+1][j+1] == 0.f ? 0.f :
                        (knots.vec[j + k + 1] - points.vec[i]) / (knots.vec[j + k + 1] - knots.vec[j + 1]) *
                        b_km1.vec[i+1][j+1];
                vec2.vec[i][j] = part0 + part1;
#endif

            }
        }

        DESTROY_VECTOR_2D(b_km1);

        return vec2;
    }
}


int main() {
    printf("Calculation of Basic Function in B-Spline!\n");

    int N = 4; // the number of the points
    int K= 1;  // the order of Bspline
    int M = N+K+1; // the number of the knots

    CREATE_VECTOR(points, N);
    CREATE_VECTOR(knots, M);

    linspace(0.111f,0.900f, N, points);
    linspace(0,1,M, knots);

    PRINT_VECTOR(points);
    PRINT_VECTOR(knots);

    vector_2d_t b = DeBoor_function(points, knots, K);
    printf("============================\nresult:\n");
    PRINT_VECTOR_2D(b);

    DESTROY_VECTOR(knots);
    DESTROY_VECTOR(points);

    return 0;
}

