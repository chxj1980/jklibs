//
// Created by v on 18-6-21.
//

#include "cm_uart_ex.h"

CMUart::CMUart() {
    iFD = 0;
}

CMUart::~CMUart() {
    if (iFD > 0) {
        cm_uart_close(iFD);
    }
}

int CMUart::init(const char *dev, int speed, int flow_ctrl, int databits, int stopbits, int parity) {
    iFD = cm_uart_open((char*)dev);
    if (iFD <= 0) {
        return iFD;
    }
    int ret = cm_uart_init(iFD, speed, flow_ctrl, databits, stopbits, parity);
    if (ret != 0) {
        return -2;
    }
    return iFD;
}

int CMUart::recv(char *data, int len) {
    return cm_uart_recv(iFD, (unsigned char*)data, len, 1, 0);
}

int CMUart::send(char *data, int len) {
    return cm_uart_send(iFD, (unsigned char*)data, len);
}
