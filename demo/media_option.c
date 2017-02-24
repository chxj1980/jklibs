//
// Created by v on 17-1-16.
//

#include <stdio.h>

// Find valid resolution from aside values
// ex: 1088 -> 1080
int find_resolution_aside_height(int v) {
    int heights[5] = { 480, 720, 1080, 1280, 0 };
    int i = 0;
    for (i; i < 5; i ++) {
        if (abs(v - heights[i]) < 10) {
            return heights[i];
        }
    }

    return 0;
}

int main() {
    int x = 1088;
    int ret = find_resolution_aside_height(x);
    printf("ret: %d\n", ret);
    return 0;
}