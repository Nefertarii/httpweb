# httpweb
linux下用epoll和posix实现的项目
===============================
在尽量仅只使用posix和c++自带的库函数  制作一个能处理页面/文件请求，并有交互功能 且用mysql数据库和redis数据库存储相关数据

整体框架构思/计划
-------------
		 启动服务器时调用socket bind listen 监听所有来自80端口的访问请求
		 	接受新的连接 
				-每有一个新的连接都在此处处理 并纳入epoll列表中变更状态为等待读
				-记录数据(连接的时间 对方地址)
			处理已连接的对端发送的请求
				-读取并分析请求(请求的类型/文件/或其他数据...)
					-由读取到的请求进一步细化处理
						--请求的文件类型、文件名、请求文件大小、请求类型...
						--GET请求 分析 处理(添加http头,文件处理...)
						--POST请求 分析 处理(添加http头,表单处理...)
				-记录数据(按情况分出请求的语句)
				-更改该连接的状态至待写入并储存至缓存中
		 	发送需要的文件/数据
				-记录数据(记录发送的文件名 时间 对方地址)
				-发送http头
				-发送文件
				-发送完成后更改该连接状态至待读取否则再下次写入中继续未完成的处理

[httphead.h](https://github.com/Nefertarii/httpweb/blob/main/httphead.h)   		用于处理http请求和添加需发送的http头

[jsonprocess.h](https://github.com/Nefertarii/httpweb/blob/main/jsonprocess.h)   	今后的JSON处理都在此文件中添加

[localinfo.h](https://github.com/Nefertarii/httpweb/blob/main/localinfo.h)   		临时的代替mySQL

[process.h](https://github.com/Nefertarii/httpweb/blob/main/jsonprocess.h)  		细分服务器的每次操作

[record.h](https://github.com/Nefertarii/httpweb/blob/main/localinfo.h)    		将有函数用于记录每次操作

[serverprocess.h](https://github.com/Nefertarii/httpweb/blob/main/localinfo.h)    	服务器所有操作都在此中进行修改

[serverror.h](https://github.com/Nefertarii/httpweb/blob/main/localinfo.h)    		服务器的出错码

[servhead.h](https://github.com/Nefertarii/httpweb/blob/main/localinfo.h)    		修改后的Socket函数以及其他需要使用的函数
