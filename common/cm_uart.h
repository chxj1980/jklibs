//
// Created by v on 18-6-21.
//

#ifndef SRC_CM_UART_H
#define SRC_CM_UART_H

#ifdef __cplusplus
extern "C" {
#endif

int cm_uart_open(char *serial_port);

void cm_uart_close(int fd);

int cm_uart_init(int fd, int speed, int flow_ctrl, int databits, int stopbits, int parity);

int cm_uart_recv(int fd, unsigned char *rcv_buf, int data_len, int sec, int usec);

int cm_uart_send(int fd, unsigned char *send_buf, int data_len);

#ifdef __cplusplus
}
#endif

#endif //SRC_CM_UART_H
