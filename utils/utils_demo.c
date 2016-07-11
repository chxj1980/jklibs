//
// Created by v on 16-7-11.
// Author: jmdvirus@roamter.com
//

#include <stdio.h>
#include <string.h>

#include "utils.h"

// Demo of base 64
int base64_demo() {
    printf("Start base 64 Demo\n");
    const char *test_str = "windows+7+download+%3D+value+%3F+sss";
    char save_pos[512] = {0};
    b64_decode(test_str, save_pos, sizeof(save_pos));
    printf("Decode out : %s\n", save_pos);

    char save_en_pos[512] = {0};
    b64_encode(save_pos, strlen(save_pos), save_en_pos, sizeof(save_en_pos));
    printf("Encode out: %s\n", save_en_pos);

    printf("End base 64 Demo\n");
}

int main(){
    base64_demo();

    return 0;
}