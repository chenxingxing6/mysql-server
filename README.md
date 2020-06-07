## MySql Server 5.7源码分析

###### 一、目录结构

```html
BUILD：里面包含各个平台，各个编译器下进行编译的脚本；
client：客户端工具，所有客户端工具都在这里，比如mysql,mysqlbinlog,mysqladmin,mysqldump等；
cmake：为CMake编译服务的，这里定义了很多在CMake编译时使用的方法或变量；
cmd-line-utils：一些小工具；
dbug：提供一些调试用的宏定义，可以很好地跟踪数据库执行到的执行函数、运行栈桢等信息，可以定位一些问题；
extra：包含了用来做网络消息认证的SSL包，并提供了comp_err、resolveip等小工具；
include：MySQL代码包含的所有头文件，这里不包括存储引擎的头文件；
libbinlogevents：MySQL 5.7 引擎开始新增的、用于解析Binlog的lib服务；
libmysql：用来创建嵌入式系统的MySQL客户端API；
libmysqld：MySQL服务器的核心级API文件，也用来开发嵌入式系统；
mysql-test：mysqld的测试工具；
mysys：MySQL自己实现的一些常用的数据结构和算法，比如array,list和hash，以及一些区分不同底层操作系统平台的函数封装，比如my_file,my_fopen等函数，这一类型的函数都以my开头；
mysys_ssl：MySQL中SSL相关的服务；
plugin：包括一些系统内置的插件，比如auth,password_validation等，同时包含了可动态载入的插件，比如fulltext,semisync等；
regex：一些关于正则表达式的算法实现；
scripts：包含一些系统工具脚本，比如mysql_install_db,mysqld_safe及mysqld_multi等；
sql：MySQL服务器主要代码，这里包含了main函数（main.cc），将会生成mysqld可执行文件；
sql-common：存放部分服务器端和客户端都会用到的代码；
storage：所有存储引擎的源代码都在这个目录中，文件夹名一般就是其存储引擎的名称，包括innobase,myisam,blackhole,ndb及perfschema等；
strings：包含很多字符串处理的函数，比如strmov,strappend及my_atof等函数；
support-files：my.cnf示例配置文件及编译所需的一些工具；
unittest：单元测试文件目录；
vio：虚拟网络IO处理系统，是对不同平台或不同协议的网络通信API的封装；
win：在windows平台编译所需的文件和一些说明；
zlib：zlib压缩算法库；
zifeiy：大晚上的我讲一个笑话给你听，就是没有zifeiy这个文件夹，我多加了一行逗大家乐一下然后就去睡觉了，晚安～
```


---

##### 二、业务流程

2.1 入口函数sql/main.cc
```c++
int main(int argc, char **argv)
{
  return mysqld_main(argc, argv);
}
```

2.2 sql/mysqld.cc(开始初始化)
- My_init()全局变量
- sys_var_init()初始化变量
- mysql_audit_initialize()审计接口
- logger.init_base()日志功能
- 初始化大量配置信息（线程池，配置变量）
- init_server_components() 服务模块
- network_init()网络模块，socket监听
- start_signal_handler()创建pid
- init_status_vars() status变量
- start_handler_manager()创建manager线程
- handler_connections_sockets()处理函数
- create_new_thread()创建新线程
- .....


2.3 sql/item.h
ITEM_类是mysql的每一个子系统都要用到的东西。之所以称之为ITEM_类，是因为从ITEM基类派生出许多子类甚至孙类。这些派生类被用来存储和处理mysql系统里的许多种数据，其中包括：参数、标识符
时间、字段、函数、数值、字符串等。ITEM基类定义在/sql/item.h源代码文件里，实现在/sql/item.cc源代码里
```html
case MYSQL_TYPE_NULL:
  case MYSQL_TYPE_DECIMAL:
  case MYSQL_TYPE_ENUM:
  case MYSQL_TYPE_SET:
  case MYSQL_TYPE_TINY_BLOB:
  case MYSQL_TYPE_MEDIUM_BLOB:
  case MYSQL_TYPE_LONG_BLOB:
  case MYSQL_TYPE_BLOB:
  case MYSQL_TYPE_GEOMETRY:
  case MYSQL_TYPE_STRING:
  case MYSQL_TYPE_VAR_STRING:
  case MYSQL_TYPE_VARCHAR:
  case MYSQL_TYPE_BIT:
  case MYSQL_TYPE_NEWDECIMAL:
  case MYSQL_TYPE_JSON:
```

2.4 sql/lex.h
LEX结构是SQL命令在mysql系统内部的表示形式。SQL命令的各组成部分---字段、表、表达式等全都井井有条地存储在LEX结构里

2.5 include/mysql_com.h
NET结构保存着mysql服务器与客户端进行通信所需要的所有信息。buff成员变量用来存放原始的通信数据包（sql命令就保存在这些数据包里）。两个重要函数
 1) my_net_write()函数，把数据包从NET结构写到网络协议
 2) my_net_read()函数，把数据包从网络协议读入NET结构


2.6 sql/sql_class.cc
THD是一个重要的类，线程类是线程执行成功的最大关键，在mysql服务器里，几乎每一个子系统或函数都要用到THD类。

---
##### 三、体系结构
MySQL的体系结构主要由四层组成：网络连接层、服务层、储存引擎层 、文件系统层。
3.1 网络连接层
负责管理网络连接、授权认证、安全访问等等。
在服务端会维护一个线程池来接受客户端的请求，每个客户端的请求都对应着一个线程。连接的时候服务端会对客户端进行登录认证，认证之后还会验证客户端是否有对应的操作权限。使用线程池的方式，可以减少对线程的创建和销毁的开销。

3.2 服务层
服务层是MySQL的核心，MySQL的核心服务都是在这层实现的。

主要包含：查询解析，SQL执行计划分析，SQL执行计划优化，查询缓存，以及跨存储引擎的功能都在这一层实现：存储过程，触发器，视图等。

查询缓存：在解析查询之前，服务器会检查查询缓存，如果能找到对应的查询，服务器不必进行查询解析、优化和执行的过程，直接返回缓存中的结果集。

解析器与预处理器：MySQL会解析查询，并创建了一个内部数据结构（解析树）。这个过程解析器主要通过语法规则来验证和解析。例如：SQL中是否使用了错误的关键字或者关键字的顺序是否正确等等；预处理会根据MySQL的规则进一步检查解析树是否合法；比如要查询的数据表和数据列是否存在等。

查询优化器：优化器将其转化成查询计划。多数情况下，一条查询可以有很多种执行方式，最后都返回相应的结果。优化器的作用就是找到这其中最好的执行计划。优化器并不关心使用的什么存储引擎，但是存储引擎对优化查询是有影响的。优化器要求存储引擎提供容量或某个具体操作的开销信息来评估执行时间。

查询引擎：在完成解析和优化阶段以后，MySQL会生成对应的执行计划，查询执行引擎根据执行计划给出的指令调用存储引擎的接口得出结果。


3.3 存储引擎层
负责MySQL中数据的存储与提取。

服务器中的查询执行引擎通过API与存储引擎进行通信，通过接口屏蔽了不同存储引擎之间的差异。储存引擎是针对表不是针对库的。

MySQL采用插件式的存储引擎。MySQL提供了许多存储引擎，每种存储引擎有不同的特点。可以根据不同的业务特点，选择最适合的存储引擎。如果对于存储引擎的性能不满意，可以通过修改源码来得到想要达到的性能。


3.4 文件系统层
主要是将数据库的数据存储在操作系统的文件系统之上，并完成与存储引擎的交互。

---






