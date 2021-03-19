# httpweb
linux下用epoll和posix实现的小项目
===============================
在尽量仅只使用posix和c++自带的库函数  制作一个能处理页面/文件请求，并有交互功能 且用mysql数据库存储相关数据

		整体框架构思
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
						
				

	
httphead.h
-----------
	GMTime函数 用于在需要发送的http头中获得当前的GMT时间(格林威治标准时间)	
	responehead_html函数 用于最后一步 处理已经获得的数据(发送的文件长度 时间...) 整合于list<string>中并返回
	http_process函数 用于在请求中获得请求类型以及需要的文件/数据名
	file_process函数 在获得的文件名中分析出文件类型并返回以供http头所用

servhead.h
----------
	readfile函数 在本地读取所需的文件 并按类型读取至list<string>中并返回
	
server.h
--------
	start_server函数 启动服务器 并控制服务器流程的主要函数



目前遇到过的问题及修改
-----------
	2021-3-17 目前在处理png图片时有问题:fstream读取不到完整的数据 很多方法都试用过了 暂时不清楚什么原因   已解决(2021-3-19)改用了linux特有的sendfile函数
	2021-3-18 好像服务器流程又有异常 epoll没有弄透 再研究一下为什么修改了状态没有进行写操作   已解决(2021-3-18)顺序出现了错误
	2021-3-19 用了sendfile函数 去除了原有过于复杂readfile头文件 在server.h中添加了相关函数作为代替   
	2021-3-19 有些资源卡在了挂起状态没有完成传输 暂时不了解情况
	
