#include "renderer/mat3.h"

mat3::mat3(float pCells[3][3]){
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            cells[i][j] = pCells[i][j];
        }
    }
}

mat3::mat3(float aa, float ab, float ac, 
        float ba, float bb, float bc,
        float ca, float cb, float cc) 
{
    float pCells[3][3] = {
        {aa, ab, ac},
        {ba, bb, bc},
        {ca, cb, cc},
    };
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            cells[i][j] = pCells[i][j];
        }
    }
}

mat3::mat3(mat4 m){
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            cells[i][j] = m.cells[i][j];
        }
    }
}

mat3::mat3(){
    mat3 identity = mat3::Identity();
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            cells[i][j] = identity.cells[i][j];
        }
    }
}

mat3 mat3::Identity(){
    float cells[3][3] = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
    };
    return mat3(cells);
}

mat3 operator+(const mat3 &m1, const mat3 &m2){
    float result[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            result[i][j] = m1.cells[i][j] + m2.cells[i][j];
        }
    }
    return mat3(result);
}
mat3 operator-(const mat3 &m1, const mat3 &m2){
    float result[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            result[i][j] = m1.cells[i][j] - m2.cells[i][j];
        }
    }
    return mat3(result);
}
mat3 operator*(const mat3 &m, float f){
    float result[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            result[i][j] = m.cells[i][j] * f;
        }
    }
    return mat3(result);
}
mat3 operator*(const mat3 &m, int i){
    float result[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            result[i][j] = m.cells[i][j] * i;
        }
    }
    return mat3(result);
}
mat3 operator*(const mat3 &m1, const mat3 &m2){
    float result[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            result[i][j] = (m1.cells[i][0] * m2.cells[0][j]) 
            + (m1.cells[i][1] * m2.cells[1][j]) 
            + (m1.cells[i][2] * m2.cells[2][j]);
        }
    }
    return mat3(result);
}
vec3 operator*(const mat3 &m, const vec3 &v){
    return vec3(
        (m.cells[0][0] * v.x) + (m.cells[0][1] * v.y) + (m.cells[0][2] * v.z),
        (m.cells[1][0] * v.x) + (m.cells[1][1] * v.y) + (m.cells[1][2] * v.z),
        (m.cells[2][0] * v.x) + (m.cells[2][1] * v.y) + (m.cells[2][2] * v.z)
    );
}

// Returns determinant of the 2x2 submatrix formed by removing row i and column j from m
float det2x2(mat3 m, int i, int j){
    float n[2][2];
    int mi = 0;
    int mj = 0;
    for (int ni = 0; ni < 2; ni++){
        mj = 0;
        if (mi == i){
            mi++;
        }
        for (int nj = 0; nj < 2; nj++){
            if (mj == j){
                mj++;
            }
            n[ni][nj] = m.cells[mi][mj];
            mj++;
        }
        mi++;
    }
    return n[0][0] * n[1][1] - n[1][0] * n[0][1];
}

mat3 cofactor(mat3 m){
    float cells[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            cells[i][j] = pow(-1, i + j) * det2x2(m, i, j);
        }
    }
    return mat3(cells);
}

mat3 transpose(mat3 m){
    float cells[3][3] = {
        {m.cells[0][0], m.cells[1][0], m.cells[2][0]},
        {m.cells[0][1], m.cells[1][1], m.cells[2][1]},
        {m.cells[0][2], m.cells[1][2], m.cells[2][2]}
    };
    return mat3(cells);
}

float determinant(mat3 m){
    return m.cells[0][0]*m.cells[1][1]*m.cells[2][2] + m.cells[0][1]*m.cells[1][2]*m.cells[2][0] + m.cells[0][2]*m.cells[1][0]*m.cells[2][1] 
    - m.cells[0][2]*m.cells[1][0]*m.cells[2][0] - m.cells[0][1]*m.cells[1][0]*m.cells[2][2] - m.cells[0][0]*m.cells[1][2]*m.cells[2][1];
}

mat3 adjugate(mat3 m){
    return transpose(cofactor(m));
}

mat3 inverse(mat3 m){
    float det = determinant(m);
    if (det < 0.000001f){
        // no inverse
        return m;
    }
    mat3 adj = adjugate(m);

    return adj * (1.0f/det);
}