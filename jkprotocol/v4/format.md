
I don't know how to design the protocol, but I will try.

Each line with 32bits, 2bytes
+--------------------------------------------------------------+
| version(4bits) | crypt(2bits) | ack(1bit) | reserve(25bits)
+--------------------------------------------------------------+
| length(32bits)
+--------------------------------------------------------------+
| data
+--------------------------------------------------------------+

- version: start from 0001
- crypt: 00 - not crypt, 01 - aes, 10,11
- ack: 1 is ack for last command
- length: what the hole length of the data

For now, we only support TCP

