#include <iostream>
#include <vector>
#include <cstring> // for memset
using namespace std;

void push(int i, int j, int light[5][6]) {
    light[i][j] ^= 1;
    if(i > 0) light[i-1][j] ^= 1;
    if(i < 4) light[i+1][j] ^= 1;
    if(j > 0) light[i][j-1] ^= 1;
    if(j < 5) light[i][j+1] ^= 1;
}

// 修复2: 检查函数没问题
int check(int light[5][6]) {
    for(int j = 0; j < 6; j++) {
        if(light[4][j]) return 0;
    }
    return 1;
}

int main() {
    int light[5][6];
    int answer[5][6];
    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 6; j++) {
            cin >> light[i][j];
        }
    }
    
    for(int mask = 0; mask < (1 << 6); mask++) {
        // 每次枚举需要重置数组和答案
        int temp_light[5][6];
        int temp_answer[5][6] = {0};
        memcpy(temp_light, light, sizeof(light));
        
        // 根据mask设置第一行的按钮
        for(int j = 0; j < 6; j++) {
            if(mask & (1 << j)) {
                push(0, j, temp_light);
                temp_answer[0][j] = 1;
            }
        }
        
        // 修复7: 贪心策略处理后续行
        for(int i = 1; i < 5; i++) {
            for(int j = 0; j < 6; j++) {
                if(temp_light[i-1][j]) {
                    push(i, j, temp_light);
                    temp_answer[i][j] = 1;
                }
            }
        }
        
        // 检查是否成功
        if(check(temp_light)) {
            for(int i = 0; i < 5; i++) {
                for(int j = 0; j < 6; j++) {
                    cout << temp_answer[i][j];
                    if(j < 5) cout << " ";
                }
                cout << endl;
            }
            return 0; 
        }
    }
    
    return 0;
}
