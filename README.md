本实验代码修改自[https://github.com/RicheyJang/RJFireWall](https://github.com/RicheyJang/RJFireWall)，报告修改自[https://github.com/DAchilles/Netfilter-Firewall](https://github.com/DAchilles/Netfilter-Firewall)。

# 使用方式

即演示时输入的指令：

## 1 规则过滤
```bash
./fwtest rule add #加一条drop 127.0.0.1
sudo hping3 127.0.0.1 #测试TCP(都不行)
sudo hping3 127.0.0.1 -1 #测试ICMP(都不行)
```

## 2 规则维护
```bash
./fwtest rule ls #展示规则
./fwtest rule change #改成只禁TCP
./fwtest rule ls #再次展示规则
sudo hping3 127.0.0.1 #测试TCP(不行)
sudo hping3 127.0.0.1 -1 #测试ICMP(行)
```

## 3 默认动作
```bash
ifconfig #找到自己的ip
ping 192.168.217.130 #ping自己(能ping通)
./fwtest rule default drop
ping 192.168.217.130 #ping自己(不能ping通)
./fwtest rule default accept
ping 192.168.217.130 #ping自己(恢复能ping通)
```

## 4 连接管理
```bash
# 右键文件夹新建终端
# 在第一个终端
sudo hping3 127.0.0.1 -1 #测试ICMP(行)
# 在第二个终端(快速！)
./fwtest ls connect #显示连接表信息(有信息)
# 第一个终端停止ping，等一会儿
# 在第二个终端
./fwtest ls connect #显示连接表信息(显示无连接)
```

## 5 状态检测
```bash
sudo hping3 127.0.0.1  #非法状态TCP
./fwtest ls log #显示非法状态被DROP的条目
sudo hping3 127.0.0.1 -1 #非法状态TCP
# 在第二个终端(快速！)
./fwtest ls connect #显示连接表信息(显示合法状态在状态表中时Establish)
```

## 6 日志审计
```bash
./fwtest ls log > log.txt #能够保存日志
#双击打开log.txt文件
#用ctrl+f对日志进行查询
```

## 7 NAT转换
```bash
ifconfig #查看当前ip地址
# 切换主机
ipconfig #看主机的ip地址
./fwtest nat add #添加一条当前ip地址映射到192.168.80.121的
sudo hping3 192.168.202.249 -1 #开始ping 192.168.202.249
# 另一个终端
sudo tcpdump #开启监听(可以看到来自NAT结果的发包)
```

## 8 基本界面
非常友好、简洁、直白、明确，有提示信息

## 9 遇到的问题
1. 规则维护的“修改”比较难设计，
    我希望不用重复输入，但又想复用“添加”规则的代码，
    所以设计成输入特定值代表不修改规则。

    > 此处是由于原来的代码没有规则修改功能，不能照抄，必须改改。

2. NAT的时候，我发现删除后NAT的影响没有消除，
    后来仔细一看代码发现只有一条规则的时候，
    消除影响的代码没有被调用，
    这是常见的有表头的链表没有做额外处理导致的错误。
    就像新建链表的时候需要对表头节点做额外的处理，容易忘。

    > 此处是原来的代码的bug。