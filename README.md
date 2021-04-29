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
				-更改该连接的状态至待写入
		 	发送需要的文件/数据
				-记录数据(记录发送的文件名 时间 对方地址)
				-发送http头
				-发送文件
				-更改该连接状态至待读取

[httphead.h](https://github.com/Nefertarii/httpweb/blob/main/httphead.h)   用于处理http请求 和添加需发送的http头
-----------

[servhead.h](https://github.com/Nefertarii/httpweb/blob/main/servhead.h)   包含了修改后的Socket函数以及其他需要使用的函数
------------

[serverprocess.h](https://github.com/Nefertarii/httpweb/blob/main/serverprocess.h)   服务器所有操作都在该类中进行
--------------

[jsonprocess.h](https://github.com/Nefertarii/httpweb/blob/main/jsonprocess.h)  今后的JSON处理都在此文件中添加
--------------

[localinfo.h](https://github.com/Nefertarii/httpweb/blob/main/localinfo.h)    暂用本地文件代替mysql读取信息
-------------



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
	2021-3-25 修改:重构过程中把常用量保存了下来 方便获取， 同时serverprocess类、serverhead 已经大部分完成
	2021-3-26 修改:重构已全部完成 准备测试程序
	2021-3-29 修改:修复了5个知道的bug 新增了5个不知道的bug 删除了server.h直接添加整合至server类中
	2021-3-29 修改:重构后的程序因为使用了大量的指针，经常出现错误
	2021-3-31 修改:重构后的程序已能正常运行 准备添加对POST请求的处理和GET请求的详细分类.
	2021-4-02 修改:添加了对POST请求的分析 以及GET请求失败的情况 Clientinfo中添加了参数以保证登录的实现
	2021-4-03 修改:添加了POST请求的处理
	2021-4-05 修改:对http头进行了修改,考虑添加对中断(SIGINT)和其他错误进行处理 
	2021-4-05 修改:稍加完善了POST处理 程序的基础流程已能使用 后两天开始写前端页面
	2021-4-05 问题:浏览器有时会进入挂起状态 暂不清楚原因
	2021-4-07 计划:前端的页面比较耗时 同时也在整理相关知识在服务器部署完成后进行展示
	2021-4-11 修改:前端的页面已大致完成
	2021-4-12 修改:修改了前端的注册/登录页
	2021-4-14 计划:后端对POST请求的处理随着前端增大 
		  目前过于复杂 POST的处理内容会随着前端开发越来越多 暂停开发POST 明天开始去除POST的其他选项 只用GET 和POST的登录、注册、和一些互动功能 并完善逻辑 定为完整版
	2021-4-16 修改:在简化过程中用 vector shared_ptr 代替了原来的数组和指针
	2021-4-17 修改:重新用shared_ptr后也改进了类 把原来散开的函数都做成了类方便修改
	2021-4-18 修改:修改后的程序已能进行基本的接收、处理、发送    过程中发现手动调用exit()会自动执行类的析构函数
	2021-4-23 修改:添加了类似errno的错误处理 修改了整体构造
	2021-4-25 修改:对前端不是很了解 导致后端的POST处理暂时无法推进
	2021-4-26 修改:对POST请求已经可以处理 正在处理c++连接至mysql的问题
	2021-4-27 计划:目前对POST请求已经有了分类 剩下的只有前端的js编写和后端分类别的处理
		       mysql也已经成功测试连通 这两天准备对数据库的表进行分析和学习
	2021-4-28 计划:mysql建表 存入数据，现在服务器在发送大文件时会导致整个网络阻塞直至请求发送完毕需要修改
	2021-4-29 计划:在找mysql过程中又了解到了redis数据库 两者似乎可以结合使用 各有各的长处
