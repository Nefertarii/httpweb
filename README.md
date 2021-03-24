# httpweb
linux下用epoll和posix实现的小项目
===============================
在尽量仅只使用posix和c++自带的库函数  制作一个能处理页面/文件请求，并有交互功能 且用mysql数据库存储相关数据

整体框架构思/计划
-------------
		 启动服务器时调用socket bind listen 监听所有来自80端口的访问请求
		 epoll无限循环 
		 		接受新的连接 
					-每有一个新的连接都在此处处理 并纳入epoll列表中等待状态变更
					-记录数据(连接的时间 对方地址)
				处理已连接的对端发送的请求
					-读取并分析请求(请求的类型/文件/或其他数据...)
					-记录数据(按情况分出请求的语句)
					-更改该连接的状态至需要写入
		 		发送需要的文件/数据
					-由读取到的请求进一步细化处理
							--请求文件类型 请求文件名 请求文件大小 请求类型...
							--处理文件及http头
					-记录数据(记录发送的文件名 时间 对方地址)
					-发送http头
					-发送文件
					-更改该连接状态至等待请求
						
				
2021-3-26:http头的重构 以配合修改的服务器操作函数 并测试可用性
	
class serverporcess 服务器所有操作都在该类中进行 并保存了需要的数据
--------------------
	private
	int epollfd 储存整个epoll列表
 	epoll_event ev 用于修改该次触发的信息
	func:关闭连接
	func:重置读写缓存
	func:epoll_event的状态
	func:记录数据
	
	public
	func:读取信息
	func:处理信息
	func:处理文件
	func:发送信息/文件

servhead.h
--------------
	头文件包含了所有的主要函数
	



目前遇到过的问题及修改
-----------
	2021-3-17 问题:目前在处理png图片时有问题:fstream读取不到完整的数据 很多方法都试用过了 暂时不清楚什么原因   已解决(2021-3-19)改用了linux特有的sendfile函数
	2021-3-18 问题:好像服务器流程又有异常 epoll没有弄透 再研究一下为什么修改了状态没有进行写操作   已解决(2021-3-18)顺序出现了错误
	2021-3-19 修改:用了sendfile函数 去除了原有过于复杂readfile头文件 在server.h中添加了相关函数作为代替   
	2021-3-19 问题:有些资源卡在了挂起状态没有完成传输 暂时不了解情况
	2021-3-22 问题:请求和传输仍然有未清楚的问题 毫无头绪
	2021-3-23 修改:计划重新修改主体函数 目前过于混乱 想制作成类方便管理
	2021-3-24 问题:莫名崩溃(重构时发现的之前为发现的问题)    已解决(2021-3-24)之前有时候程序自动退出 发现是 系统信号-SIGPIPE 的原因
	2021-3-25 修改:重构了所有函数 服务器操作制作成了类方便使用 同时基础的绑定、读、写等函数均完善并添加了异常处理 且添加了缓存在写操作时防止数据量过大无法一次写完 
	
