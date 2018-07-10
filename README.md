TMemter是一个通用的服务器程序压力测试程序，用于模拟高并发的客户请求，以测试服务器在高压下的稳定性。<br/>
TMemeter有如下特点：<br/>
1、目前只支持TCP服务器程序<br/>
2、效率高（I/O复用epoll实现）<br/>

使用方法：<br/>
> ./build.sh <br/>
> cd build <br/>
> cd bin <br/>
> ./TMeter <connections> <timeout> <ip> <port> <br/>