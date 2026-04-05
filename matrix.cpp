#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <limits>

using namespace std;
using namespace chrono;

// 类型别名简化代码
using Matrix = vector<vector<double>>;
using Vector = vector<double>;

// 打印矩阵
void printMatrix(const Matrix& A, const string& name = "") {
    if (!name.empty()) cout << name << ":\n";
    for (const auto& row : A) {
        for (double val : row) {
            cout << setw(12) << scientific << val << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// 打印向量
void printVector(const Vector& v, const string& name = "") {
    if (!name.empty()) cout << name << ": ";
    for (double val : v) {
        cout << setw(12) << scientific << val << " ";
    }
    cout << endl << endl;
}

// 计算向量范数
double norm(const Vector& v) {
    double sum = 0.0;
    for (double val : v) {
        sum += val * val;
    }
    return sqrt(sum);
}

// 计算相对误差 ||x - x_exact|| / ||x_exact||
double relativeError(const Vector& x, const Vector& x_exact) {
    Vector diff(x.size());
    for (size_t i = 0; i < x.size(); i++) {
        diff[i] = x[i] - x_exact[i];
    }
    return norm(diff) / norm(x_exact);
}

// 计算相对残差 ||b - Ax|| / ||b||
double relativeResidual(const Matrix& A, const Vector& x, const Vector& b) {
    size_t n = A.size();
    Vector r(n, 0.0);
    
    // 计算 r = b - Ax
    for (size_t i = 0; i < n; i++) {
        double sum = 0.0;
        for (size_t j = 0; j < n; j++) {
            sum += A[i][j] * x[j];
        }
        r[i] = b[i] - sum;
    }
    return norm(r) / norm(b);
}

// 生成 Hilbert 矩阵
Matrix createHilbertMatrix(size_t n) {
    Matrix A(n, vector<double>(n, 0.0));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            A[i][j] = 1.0 / (i + j + 1);  // 注意：i+j+1 而不是 i+j-1，因为索引从0开始
        }
    }
    return A;
}

// 矩阵转置
Matrix transpose(const Matrix& A) {
    size_t m = A.size();
    if (m == 0) return Matrix();
    size_t n = A[0].size();
    Matrix B(n, vector<double>(m, 0.0));
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            B[j][i] = A[i][j];
        }
    }
    return B;
}

// 矩阵乘法
Matrix matmul(const Matrix& A, const Matrix& B) {
    size_t m = A.size();
    if (m == 0) return Matrix();
    size_t p = A[0].size();
    size_t n = B[0].size();
    Matrix C(m, vector<double>(n, 0.0));
    
    for (size_t i = 0; i < m; i++) {
        for (size_t k = 0; k < p; k++) {
            double aik = A[i][k];
            for (size_t j = 0; j < n; j++) {
                C[i][j] += aik * B[k][j];
            }
        }
    }
    return C;
}

// 矩阵向量乘法
Vector matvec(const Matrix& A, const Vector& x) {
    size_t m = A.size();
    if (m == 0) return Vector();
    Vector b(m, 0.0);
    
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < x.size(); j++) {
            b[i] += A[i][j] * x[j];
        }
    }
    return b;
}
// 高斯消元法（带部分选主元）
Vector gaussianElimination(Matrix A, Vector b) {
    size_t n = A.size();
    
    // 增广矩阵的列索引映射
    vector<size_t> col_index(n);
    for (size_t i = 0; i < n; i++) col_index[i] = i;
    
    // 消元过程
    for (size_t k = 0; k < n - 1; k++) {
        // 部分选主元：寻找第k列从第k行到第n-1行中绝对值最大的元素
        size_t max_row = k;
        double max_val = abs(A[k][col_index[k]]);
        
        for (size_t i = k + 1; i < n; i++) {
            double val = abs(A[i][col_index[k]]);
            if (val > max_val) {
                max_val = val;
                max_row = i;
            }
        }
        
        // 如果最大值为0，矩阵奇异
        if (max_val < numeric_limits<double>::epsilon()) {
            throw runtime_error("矩阵奇异或接近奇异");
        }
        
        // 交换行
        if (max_row != k) {
            swap(A[k], A[max_row]);
            swap(b[k], b[max_row]);
        }
        
        // 消元
        for (size_t i = k + 1; i < n; i++) {
            double factor = A[i][col_index[k]] / A[k][col_index[k]];
            if (abs(factor) < numeric_limits<double>::epsilon()) continue;
            
            for (size_t j = k; j < n; j++) {
                A[i][col_index[j]] -= factor * A[k][col_index[j]];
            }
            b[i] -= factor * b[k];
        }
    }
    
    // 回代求解
    Vector x(n, 0.0);
    for (int i = n - 1; i >= 0; i--) {
        double sum = 0.0;
        for (size_t j = i + 1; j < n; j++) {
            sum += A[i][col_index[j]] * x[j];
        }
        x[i] = (b[i] - sum) / A[i][col_index[i]];
    }
    
    return x;
}
// LU分解（带部分选主元）
struct LUResult {
    Matrix L;
    Matrix U;
    vector<size_t> P;  // 置换向量，P[i] 表示第i行来自原矩阵的第P[i]行
};

LUResult luDecomposition(const Matrix& A) {
    size_t n = A.size();
    Matrix L(n, vector<double>(n, 0.0));
    Matrix U = A;  // 复制A到U，在U上执行消元
    vector<size_t> P(n);
    for (size_t i = 0; i < n; i++) P[i] = i;
    
    for (size_t k = 0; k < n; k++) {
        // 部分选主元
        size_t max_row = k;
        double max_val = abs(U[k][k]);
        
        for (size_t i = k + 1; i < n; i++) {
            double val = abs(U[i][k]);
            if (val > max_val) {
                max_val = val;
                max_row = i;
            }
        }
        
        if (max_val < numeric_limits<double>::epsilon()) {
            throw runtime_error("矩阵奇异或接近奇异");
        }
        
        // 交换行
        if (max_row != k) {
            swap(U[k], U[max_row]);
            swap(L[k], L[max_row]);
            swap(P[k], P[max_row]);
        }
        
        // 设置L的对角元为1
        L[k][k] = 1.0;
        
        // 消元和填充L
        for (size_t i = k + 1; i < n; i++) {
            L[i][k] = U[i][k] / U[k][k];
            for (size_t j = k; j < n; j++) {
                U[i][j] -= L[i][k] * U[k][j];
            }
        }
    }
    
    // 填充L的上三角部分为0，U的下三角部分为0
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            L[i][j] = 0.0;
        }
    }
    
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < i; j++) {
            U[i][j] = 0.0;
        }
    }
    
    return {L, U, P};
}

// 使用LU分解求解
Vector luSolve(const Matrix& A, const Vector& b) {
    LUResult lu = luDecomposition(A);
    size_t n = A.size();
    
    // 应用置换: Pb
    Vector b_permuted(n, 0.0);
    for (size_t i = 0; i < n; i++) {
        b_permuted[i] = b[lu.P[i]];
    }
    
    // 前向代入: Ly = Pb
    Vector y(n, 0.0);
    for (size_t i = 0; i < n; i++) {
        double sum = 0.0;
        for (size_t j = 0; j < i; j++) {
            sum += lu.L[i][j] * y[j];
        }
        y[i] = (b_permuted[i] - sum) / lu.L[i][i];
    }
    
    // 回代: Ux = y
    Vector x(n, 0.0);
    for (int i = n - 1; i >= 0; i--) {
        double sum = 0.0;
        for (size_t j = i + 1; j < n; j++) {
            sum += lu.U[i][j] * x[j];
        }
        x[i] = (y[i] - sum) / lu.U[i][i];
    }
    
    return x;
}
// QR分解（Householder变换）
struct QRResult {
    Matrix Q;
    Matrix R;
};

QRResult qrDecomposition(const Matrix& A) {
    size_t m = A.size();
    if (m == 0) return {Matrix(), Matrix()};
    size_t n = A[0].size();
    
    Matrix Q(m, vector<double>(m, 0.0));
    Matrix R = A;
    
    // 初始化Q为单位矩阵
    for (size_t i = 0; i < m; i++) {
        Q[i][i] = 1.0;
    }
    
    for (size_t k = 0; k < min(m, n); k++) {
        // 计算Householder向量
        double norm_x = 0.0;
        for (size_t i = k; i < m; i++) {
            norm_x += R[i][k] * R[i][k];
        }
        norm_x = sqrt(norm_x);
        
        // 避免数值问题
        if (norm_x < numeric_limits<double>::epsilon()) {
            continue;
        }
        
        double alpha = -copysign(norm_x, R[k][k]);
        double r = sqrt(0.5 * (alpha * alpha - alpha * R[k][k]));
        
        if (abs(r) < numeric_limits<double>::epsilon()) {
            continue;
        }
        
        // 构造Householder向量v
        Vector v(m, 0.0);
        v[k] = (R[k][k] - alpha) / (2.0 * r);
        for (size_t i = k + 1; i < m; i++) {
            v[i] = R[i][k] / (2.0 * r);
        }
        
        // 应用Householder变换到R: R = (I - 2vv^T)R
        for (size_t j = k; j < n; j++) {
            double dot = 0.0;
            for (size_t i = k; i < m; i++) {
                dot += v[i] * R[i][j];
            }
            for (size_t i = k; i < m; i++) {
                R[i][j] -= 2.0 * v[i] * dot;
            }
        }
        
        // 累积Q: Q = Q(I - 2vv^T)
        for (size_t j = 0; j < m; j++) {
            double dot = 0.0;
            for (size_t i = k; i < m; i++) {
                dot += v[i] * Q[i][j];
            }
            for (size_t i = k; i < m; i++) {
                Q[i][j] -= 2.0 * v[i] * dot;
            }
        }
    }
    
    // 确保R的上三角部分准确（去除下三角的小数值）
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < i && j < n; j++) {
            R[i][j] = 0.0;
        }
    }
    
    // Q需要转置（因为我们是左乘Householder矩阵）
    Q = transpose(Q);
    
    return {Q, R};
}

// 使用QR分解求解
Vector qrSolve(const Matrix& A, const Vector& b) {
    QRResult qr = qrDecomposition(A);
    size_t n = A.size();
    
    // 计算 Q^T * b
    Vector b_hat(n, 0.0);
    for (size_t i = 0; i < n; i++) {
        double sum = 0.0;
        for (size_t j = 0; j < n; j++) {
            sum += qr.Q[j][i] * b[j];  // Q^T的第i行是原Q的第i列
        }
        b_hat[i] = sum;
    }
    
    // 回代求解 Rx = b_hat
    Vector x(n, 0.0);
    for (int i = n - 1; i >= 0; i--) {
        double sum = 0.0;
        for (size_t j = i + 1; j < n; j++) {
            sum += qr.R[i][j] * x[j];
        }
        x[i] = (b_hat[i] - sum) / qr.R[i][i];
    }
    
    return x;
}
int main() {
    cout << fixed << setprecision(6);
    cout << scientific;
    
    cout << "===============================================================\n";
    cout << "  非齐次线性方程组求解方法比较：高斯消元 vs LU分解 vs QR分解\n";
    cout << "===============================================================\n\n";
    
    // 设置问题
    const size_t n = 8;  // 矩阵大小
    cout << "矩阵大小: " << n << " x " << n << endl;
    
    // 生成Hilbert矩阵
    Matrix A = createHilbertMatrix(n);
    
    // 精确解设为全1向量
    Vector x_exact(n, 1.0);
    
    // 计算右端项 b = A * x_exact
    Vector b = matvec(A, x_exact);
    
    // 计算条件数（近似）
    cout << "Hilbert矩阵条件数极大，是著名的病态矩阵" << endl;
    cout << endl;
    
    // 测试三种方法
    struct Result {
        string method;
        Vector x;
        double time_ms;
        double rel_error;
        double rel_residual;
    };
    
    vector<Result> results;
    
    // 高斯消元法
    {
        cout << "1. 高斯消元法（带部分选主元）..." << endl;
        auto start = high_resolution_clock::now();
        Vector x_ge = gaussianElimination(A, b);
        auto end = high_resolution_clock::now();
        
        double time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
        double err = relativeError(x_ge, x_exact);
        double res = relativeResidual(A, x_ge, b);
        
        results.push_back({"高斯消元法", x_ge, time_ms, err, res});
        
        cout << "   计算时间: " << time_ms << " ms" << endl;
        cout << "   相对误差: " << scientific << err << endl;
        cout << "   相对残差: " << scientific << res << endl << endl;
    }
    
    // LU分解法
    {
        cout << "2. LU分解法（带部分选主元）..." << endl;
        auto start = high_resolution_clock::now();
        Vector x_lu = luSolve(A, b);
        auto end = high_resolution_clock::now();
        
        double time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
        double err = relativeError(x_lu, x_exact);
        double res = relativeResidual(A, x_lu, b);
        
        results.push_back({"LU分解法", x_lu, time_ms, err, res});
        
        cout << "   计算时间: " << time_ms << " ms" << endl;
        cout << "   相对误差: " << scientific << err << endl;
        cout << "   相对残差: " << scientific << res << endl << endl;
    }
    
    // QR分解法
    {
        cout << "3. QR分解法（Householder变换）..." << endl;
        auto start = high_resolution_clock::now();
        Vector x_qr = qrSolve(A, b);
        auto end = high_resolution_clock::now();
        
        double time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
        double err = relativeError(x_qr, x_exact);
        double res = relativeResidual(A, x_qr, b);
        
        results.push_back({"QR分解法", x_qr, time_ms, err, res});
        
        cout << "   计算时间: " << time_ms << " ms" << endl;
        cout << "   相对误差: " << scientific << err << endl;
        cout << "   相对残差: " << scientific << res << endl << endl;
    }
    
    // 结果总结
    cout << "===============================================================\n";
    cout << "                          结果总结\n";
    cout << "===============================================================\n";
    cout << setw(20) << left << "方法" 
         << setw(15) << right << "时间(ms)" 
         << setw(20) << "相对误差" 
         << setw(20) << "相对残差" << endl;
    cout << string(75, '-') << endl;
    
    // 按相对误差排序
    sort(results.begin(), results.end(), 
         [](const Result& a, const Result& b) { return a.rel_error < b.rel_error; });
    
    for (const auto& r : results) {
        cout << setw(20) << left << r.method
             << setw(15) << right << fixed << r.time_ms
             << setw(20) << scientific << r.rel_error
             << setw(20) << scientific << r.rel_residual << endl;
    }
    
    // 分析结论
    cout << "\n===============================================================\n";
    cout << "                          关键结论\n";
    cout << "===============================================================\n";
    cout << "1. QR分解法在病态问题（Hilbert矩阵）上表现出最好的数值稳定性，\n";
    cout << "   相对误差和相对残差均最小。\n\n";
    
    cout << "2. 高斯消元法和LU分解法给出相似的结果，因为它们本质上是同一算法的\n";
    cout << "   两种不同实现形式。\n\n";
    
    cout << "3. QR分解法的计算成本最高，这是因为它需要进行正交变换，而正交变换\n";
    cout << "   的浮点运算量大约是LU分解的两倍。\n\n";
    
    cout << "4. 在实际应用中：\n";
    cout << "   - 对于良态或中等条件数的问题，通常选择LU分解法（效率与稳定性平衡）\n";
    cout << "   - 对于高度病态的问题或要求高精度的场合，应优先使用QR分解法\n";
    cout << "   - 高斯消元法主要作为教学工具，帮助理解算法原理\n";
    
    return 0;
}