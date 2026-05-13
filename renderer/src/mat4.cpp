#include "renderer/mat4.h"

mat4::mat4(float pCells[4][4]){
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            cells[i][j] = pCells[i][j];
        }
    }
}

mat4::mat4(float aa, float ab, float ac, float ad, 
        float ba, float bb, float bc, float bd,
        float ca, float cb, float cc, float cd,
        float da, float db, float dc, float dd) 
{
    float pCells[4][4] = {
        {aa, ab, ac, ad},
        {ba, bb, bc, bd},
        {ca, cb, cc, cd},
        {da, db, dc, dd}
    };
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            cells[i][j] = pCells[i][j];
        }
    }
}

mat4::mat4(){
    mat4 identity = mat4::Identity();
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            cells[i][j] = identity.cells[i][j];
        }
    }
}

mat4 mat4::Identity(){
    float cells[4][4] = {
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    return mat4(cells);
}

mat4 operator+(const mat4 &m1, const mat4 &m2){
    float result[4][4];
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            result[i][j] = m1.cells[i][j] + m2.cells[i][j];
        }
    }
    return mat4(result);
}
mat4 operator-(const mat4 &m1, const mat4 &m2){
    float result[4][4];
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            result[i][j] = m1.cells[i][j] - m2.cells[i][j];
        }
    }
    return mat4(result);
}
mat4 operator*(const mat4 &m, float f){
    float result[4][4];
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            result[i][j] = m.cells[i][j] * f;
        }
    }
    return mat4(result);
}
mat4 operator*(const mat4 &m, int i){
    float result[4][4];
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            result[i][j] = m.cells[i][j] * i;
        }
    }
    return mat4(result);
}
mat4 operator*(const mat4 &m1, const mat4 &m2){
    float result[4][4];
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            result[i][j] = (m1.cells[i][0] * m2.cells[0][j]) 
            + (m1.cells[i][1] * m2.cells[1][j]) 
            + (m1.cells[i][2] * m2.cells[2][j]) 
            + (m1.cells[i][3] * m2.cells[3][j]);
        }
    }
    return mat4(result);
}
vec4 operator*(const mat4 &m, const vec4 &v){
    return vec4(
        (m.cells[0][0] * v.x) + (m.cells[0][1] * v.y) + (m.cells[0][2] * v.z) + (m.cells[0][3] * v.w),
        (m.cells[1][0] * v.x) + (m.cells[1][1] * v.y) + (m.cells[1][2] * v.z) + (m.cells[1][3] * v.w),
        (m.cells[2][0] * v.x) + (m.cells[2][1] * v.y) + (m.cells[2][2] * v.z) + (m.cells[2][3] * v.w),
        (m.cells[3][0] * v.x) + (m.cells[3][1] * v.y) + (m.cells[3][2] * v.z) + (m.cells[3][3] * v.w)
    );
}

// Returns determinant of the 3x3 submatrix formed by removing row i and column j from m
float det3x3(mat4 m, int i, int j){
    float n[3][3];
    int mi = 0;
    int mj = 0;
    for (int ni = 0; ni < 3; ni++){
        mj = 0;
        if (mi == i){
            mi++;
        }
        for (int nj = 0; nj < 3; nj++){
            if (mj == j){
                mj++;
            }
            n[ni][nj] = m.cells[mi][mj];
            mj++;
        }
        mi++;
    }
    return n[0][0]*n[1][1]*n[2][2] + n[0][1]*n[1][2]*n[2][0] + n[0][2]*n[1][0]*n[2][1] 
    - n[0][2]*n[1][0]*n[2][0] - n[0][1]*n[1][0]*n[2][2] - n[0][0]*n[1][2]*n[2][1];
}

mat4 cofactor(mat4 m){
    float cells[4][4];
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            cells[i][j] = pow(-1, i + j) * det3x3(m, i, j);
        }
    }
    return mat4(cells);
}

float determinant(mat4 m){
    float det = 0;
    for (int j = 0; j < 4; j++){
        det += pow(-1, j) * det3x3(m, 0, j) * m.cells[0][j];
    }
    return det;
}

mat4 transpose(mat4 m){
    float cells[4][4] = {
        {m.cells[0][0], m.cells[1][0], m.cells[2][0], m.cells[3][0]},
        {m.cells[0][1], m.cells[1][1], m.cells[2][1], m.cells[3][1]},
        {m.cells[0][2], m.cells[1][2], m.cells[2][2], m.cells[3][2]},
        {m.cells[0][3], m.cells[1][3], m.cells[2][3], m.cells[3][3]}
    };
    return mat4(cells);
}

mat4 adjugate(mat4 m){
    return transpose(cofactor(m));
}

mat4 inverse(mat4 m){
    float det = determinant(m);
    if (det < 0.000001f){
        // no inverse
        return m;
    }
    mat4 adj = adjugate(m);

    return adj * (1.0f/det);
}

