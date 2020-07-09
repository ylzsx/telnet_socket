本程序需要在linux机器上方可编译运行。

使用者可以根据自己需要简单修改makefile文件，通过make命令编译。

服务器启动：

```shell
./output/service
```

客户端启动：

```shell
./output/service + IP地址
如：./output/service 127.0.0.1
```

端口号默认为5555，如果和本机端口冲突，可以自行在`wrap.h`中修改。