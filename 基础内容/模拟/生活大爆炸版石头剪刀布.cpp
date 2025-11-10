#include <iostream>
#include <vector>
using namespace std;

bool isWin(int a, int b) {
    if (a == b) return false; // 平局
    if (a == 0 && (b == 2 || b == 3)) return true; // 剪刀赢布、蜥蜴人
    if (a == 1 && (b == 0 || b == 3)) return true; // 石头赢剪刀、蜥蜴人
    if (a == 2 && (b == 1 || b == 4)) return true; // 布赢石头、斯波克
    if (a == 3 && (b == 2 || b == 4)) return true; // 蜥蜴人赢布、斯波克
    if (a == 4 && (b == 0 || b == 1)) return true; // 斯波克赢剪刀、石头
    return false;
}

int main() {
    int N, NA, NB;
    cin >> N >> NA >> NB;

    vector<int> A(NA), B(NB);
    for (int i = 0; i < NA; ++i) cin >> A[i];
    for (int i = 0; i < NB; ++i) cin >> B[i];

    int scoreA = 0, scoreB = 0;

    for (int i = 0; i < N; ++i) {
        int a = A[i % NA];
        int b = B[i % NB];

        if (isWin(a, b)) scoreA++;
        else if (isWin(b, a)) scoreB++;
    }

    cout << scoreA << " " << scoreB << endl;

    return 0;
}