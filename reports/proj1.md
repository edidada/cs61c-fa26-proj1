# CS61C Fall 2026 Project1 snek

官方要求：https://cs61c.org/fa26/projects/proj1/ 。固定starter 83024ffe49b8ecf1f76ed2ec2c5db7d6fea5b88c。只修改原项目允许的game.c、snake.c、custom_tests.c；另加独立workflow/shell验收与此报告。原接口/Makefile/单元测试/21组输入参考/哈希工具/交互程序/Provenance manifest不改。

实现七代码任务：可修改18×20默认板、完整释放、逐行打印、按蛇编号更新移动/碰撞/吃食/回调、fgets分块动态读行、ragged board加载及从tail追踪head编号、文件/stdin与输出错误处理。行最终收缩为实际字符串长度；加载用动态行指针表，部分分配失败回收；有效题目棋盘为封闭、每蛇至少3格。不声称任意畸形蛇拓扑的解析安全。

自定义测试穷举256字符的分类、四方向映射与坐标0/UINT_MAX边界，并测试1MiB行、末行无换行、空流/无蛇、ragged多蛇编号。远端仅用原Makefile构建，原十单元组/21集成与stdin等价性、Valgrind以及独立oracle和交互检查。原单元main失败也返回0，因此验证脚本检查全部成功组且无失败文本。无新增Python文件，本地不编译运行。

这是公开自学代码验收，不冒充官方Hive编辑、个人Provenance记录、反馈表或Gradescope。manifest原样保留，不伪造历史编辑记录；官方这些过程/身份要求未完成。当前尚待远端结果，不计验收完成。
