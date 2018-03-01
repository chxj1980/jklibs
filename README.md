## jklibs
基于 c/c++ 的综合学习的地方

## CMake
编译采用  makefile, cmake 存在的原因是为了使用 CLion.

目录结构
```
codec - 编解码方案，目前实现基于 x264 实现 yuv 到 h264 的编码
common - 通用功能
config - makefile 使用
demo - 小功能试验
jkprotocol - 个人的一些网络通讯协议设计实现
kfmd5 - nginx 中提取的 md5 实现
protocol - 网上找的 http 客户端的实现，以及基本 json 数据的实验
recordserver - 基于 v4l2 的视频数据采集，编码的程序实现
vdev - 基于 v4l2 的视频采集实现
```

## 编译
查看 Makefile 看如何编译
