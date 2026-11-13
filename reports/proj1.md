# CS61C Fall 2026 Project1 snek

官方要求：https://cs61c.org/fa26/projects/proj1/ 。固定starter 83024ffe49b8ecf1f76ed2ec2c5db7d6fea5b88c。只修改原项目允许的game.c、snake.c、custom_tests.c；另加独立workflow/shell验收与此报告。原接口/Makefile/单元测试/21组输入参考/哈希工具/交互程序/Provenance manifest不改。

实现七代码任务：可修改18×20默认板、完整释放、逐行打印、按蛇编号更新移动/碰撞/吃食/回调、fgets分块动态读行、ragged board加载及从tail追踪head编号、文件/stdin与输出错误处理。行最终收缩为实际字符串长度；加载用动态行指针表，部分分配失败回收；有效题目棋盘为封闭、每蛇至少3格。不声称任意畸形蛇拓扑的解析安全。

自定义测试穷举256字符的分类、四方向映射与坐标0/UINT_MAX边界，并测试1MiB行、末行无换行、空流/无蛇、ragged多蛇编号。远端仅用原Makefile构建，原十单元组/21集成与stdin等价性、Valgrind以及独立oracle和交互检查。原单元main失败也返回0，因此验证脚本检查全部成功组且无失败文本。无新增Python文件，本地不编译运行。

这是公开自学代码验收，不冒充官方Hive编辑、个人Provenance记录、反馈表或Gradescope。manifest原样保留，不伪造历史编辑记录；官方这些过程/身份要求未完成。c3bd5189967f345f03b61b08640c4c764d44d98e对应run https://github.com/edidada/cs61c-fa26-proj1/actions/runs/43469050953 completed/success：原10单元组、21集成及缺失输入测试、21份stdin原参考对照、24次Valgrind（单元/自定义/free模式及21 stdin）、512步独立oracle、原交互程序PTY方向输入与真实定时移动均通过。最终报告提交还需对应CI，最终SHA/工件归档langnote。

## 验收范围与实现边界

原十单元组的main即便失败也返回0，因此脚本同时计数十个成功组并拒绝失败文本；原集成目标还运行缺失输入检查，输出为21文件通过加1错误码检查。Valgrind使用非零error-exitcode并把所有泄漏类型视作错误，24份日志均保存。自定义helper分类穷举768次，坐标两方向各1280次，加8次映射，共3336个断言场景；另有长行、EOF、空板和不规则双蛇。失败的自定义main已返回非零，未改原单元main。

独立oracle固定64个ragged多蛇棋盘，每板8步，逐次通过真实stdin pipe启动CLI；坐标模型检查移动/尾部释放/碰壁/死亡冻结，不复用C助手计算期望。原21 fixture提供吃食、方向、复杂蛇与大棋盘回归。PTY分别传入w/a/s/d检查原交互程序显示的头方向，并以原定时线程捕捉前进及吃到食物的帧，文本工件可重放检查。原interactive starter没有有效quit处理，也没有正常释放/关闭生命周期；保持不改，由fixture外部SIGTERM结束，不把CLI的Valgrind结果套到它上面。

read_line使用fgets，对完整行保留换行、EOF处最后一行保留原内容；有效课程板为有换行的封闭文本，超长行不预设固定宽。malloc/realloc错误回收已分配行；不解析任意含NUL或损坏/成环蛇图，也未做分配失败注入。initialize_snakes额外识别已有x为死亡，保证CLI保存后继续运行不复活。正常更新按初始tail扫描编号顺序处理，并在吃食后调用原deterministic_food。
