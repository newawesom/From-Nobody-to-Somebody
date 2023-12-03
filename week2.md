# 第二周学习笔记

前言：本来以为第二周任务能少一点，结果还是很多力，而且学习的难度都不小（悲），没办法既然选择了软件这条路就应该走到底，加油！

## g++

很好用的一款编译器，简单使用，无论是单文件还是多文件都能很方便地编译。

### 单文件

写好一个简单的.cpp文件，在文件所在文件夹下打开终端，输入：

```
$g++ test.cpp -o test 
$./test
```

第一行代表用g++编译文件test.cpp文件生成一个可执行文件test。

第二行代表运行可执行文件test。

### 多文件

只需要将单文件替换为罗列的多文件即可。

```
$g++ testspeaker.cpp speaker.cpp -o testspeaker
$./testspeaker
```

注意不包含自己写的头文件。

------

## CMake

### 安装

使用sudo apt install 安装的CMake版本过于古老，所以我选择到CMake的官方网站去下载Linux版本最新稳定版的CMake，经过解压缩，编译，安装，终于装好了CMake。

### 使用

#### 单文件

编写一个简单的C语言文件main.c ，然后在main.c的相同目录下编写CMakeLists.txt，内容如下：

```cmake
cmake_minimum_required(VERSION 2.8)

project(demo)

add_executable(main main.c)
```

第一行代表CMake的最低版本；第二行代表本工程的信息，工程名为demo；第三行代表最终生成的elf文件名为main，使用的源文件是main.c。

在该目录下打开终端，输入:

```
$cmake .
```

编译完成后，该目录下会出现一下几个文件，

![](/home/nh/Pictures/Screenshot from 2023-12-02 16-34-57.png)

在终端输入make（记得先保存文件），得到可执行文件main，输入./main运行文件就可以得到结果了。

#### 同目录多文件

在之前单文件添加2个文件testFunc.c和testFunc.h。

只需修改CMakeLists.txt，在add_executable的参数里把testFunc.c加进来（同样不用加头文件）。

其他步骤同单文件。

**另**：文件太多不想打字怎么办？

没关系，CMake中提供了一个命令可以把指定目录下的所有源文件储存在一个变量之中，这个命令就是

```cmake
aux_souce_directory(dir var)
```

第一个参数指目录，第二个参数var用于存放源文件列表中的变量。

在add_executable()命令中将所有源文件写成${VAR}，执行cmake和make，并运行main即可运行成功。

#### 不同目录的多个源文件

一般来说，我们在文件比较多的情况下会对文件进行分类。

正规来说，一般会将源文件放在src目录下，头文件放在include目录下，生成的对象文件放在build目录下，最终输出的elf文件放在bin目录下，这样使得结构更清晰。

在最外层目录下新建一个CMakeLists.txt，内容如下，

```cmake
cmake_minimum_requried (VERSION 2.8)

project (demo)

add_subdirectory (src)

```

这里指定src目录下存放了源文件，执行cmake时会进入src目录下去找src目录下的CMakeLists.txt，所以在src目录下也建立了一个CMakeLists.txt，内容如下，

```cmake
aux_source_directory (. SRC_LIST)

include_directories (../include)

add_executable (main ${SRC_LIST})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
```

EXECUTABLE_OUT_PATH和PROJECT_SOURCE_DIR为CMake自带的与定义变量，代表**目标二进制可执行文件的存放位置**和**工程的根目录**。

最后一行的意义是把elf文件的位置设置为工程根目录下的bin目录。

运行cmake，不过先切换到build目录下，输入cmake..，Makefile会在build目录下生成，然后在build目录下运行make，切换到bin目录下，发现main已经生成，运行即可。

-------

## Vim

### 安装

直接使用sudo apt install vim命令即可安装。

### 使用（四种模式）

#### 命令模式

用vim打开文件，默认处于命令模式，在该模式下可以移动光标，使用快捷键对文件内容进行修改。

打开方式，在终端输入：

```
$vim README.md
```

即可。如果文件不存在，则自动创建一个。

#### 编辑模式

在命令模式输入a或者i进入编辑模式，可以对文件的内容进行编辑和修改。

#### 末行模式

在命令模式下输入冒号：即可进入末行模式，可以用于文件的查找、替换、保存、退出等操作。

一些简单的命令：

```
:wq #保存并退出
:q! #强制退出，不保存
:w #保存文件
```

#### 可视化模式

不知道干啥的。好像使用来快速复制内容的？

### 我的理解

Vim我的理解就是一个短小精焊的文件编辑器，能直接在终端完成对文件内容的修改等等。

--------

## Git

Git的内容非常丰富，这几乎花了我一个下午的时间来学习、配置、搭建我的Git库和Github库。

### Git起源

Git自出生起就是开源精神的代表，一座开源的丰碑。它的出现不光是linux社区打破BitMover公司对于分布式版本控制系统的垄断，更使得Github这一世界最大开源社区发扬光大。

### Git安装

使用sudo apt install git 命令即可安装。

### Git使用

#### 配置Git环境

```
git config --global user.name "用户名"
git config --global user.email“邮箱”
```

初次使用Git需要配置用户名和邮箱。

#### 创建本地仓库

在空目录下进入终端，输入git init即可初始化当前仓库。

#### 新建文件添加到本地仓库

新建一个文件，例如test,c。

```
git add test.c
git commit -m "add a new file test.c"
```

使用git add命令将文件添加到本地仓库的缓存区，在使用git commit命令提交，并添加注释。

git可以分为三个区，工作区、缓存区、本地仓库，一般我们编辑文件只会在工作区修改，使用git add命令将文件添加到缓存区，使用git commit命令将文件从缓存区提交到本地仓库。

如果发现文件修改错误，但还没有添加到缓存区，可以使用git checkout --file来返回修改之前的状态。

使用git status命令可以查看当前那些文件处于那些区。

#### 查看仓库日志和回滚

```
git log
```

可以查看当前的仓库修改日志，包括commit时生成的哈系id、提交者时间以及注释。

想要回滚仓库版本时，使用

```
git reset --hard 回滚id
```

该命令对于单个文件依然有效。

#### Git分支

##### 创建分支

```
git checkout -b dev
```

参数-b用于创建分支

##### 切换分支

```
git checkout master
```

切换到master分

支。

##### 合并分支

```
git merge dev
```

将分支dev合并到主分区。

##### 查看分支

```
git branch -a
```

查看所有分支包括远程分支。

### Github使用

首先，确保电脑能够进入github.com网站，如果进不去，则需要配置host文件或者找加速器，我这里用的是watt toolkit的github加速，免费但是安装过程很繁琐，费老大劲了。

注册一个github的帐号。

在本地创建一个ssh的key，将生成的公匙添加到github上。

#### 在Github上创建仓库

右上角+号出选择

New repository，复制CODE中的SSH链接。

使用命令

```
git remote add origin [Address]
```

然后使用git push推送到远程。

```
git push -u origin master
```

这里终端发出了一个问题，无法访问我的远程仓库，去CSDN上查找了相关的问题后发现是校园网的问题，需要在终端修改host文件，添加github的443端口才解决了问题。

之后在github上收到了新分支master的创立提醒，看到了目录下的新文件。

#### 从Github上拉取远程仓库到本地仓库

新建一个空目录，不要git init。

使用命令

```
git clone [Address]
```

即可自动将该目录初始化为本地仓库。

#### Github提交本地仓库到远程仓库

```
git add main.c
git commit -m "This is just a test."
git push origin master
```

使用这三个命令就能将本地仓库中的修改推送到Github上。

**还有就是不要在文件名中添加中文，不然推送到Github上的文件名是一推乱码。**

最后贴上我的Github主页（虽然什么也没有）

[点我](https://github.com/newawesom)

------

## C++学习

介于我已经完成了对C语言的学习，并且我之前有接触过C++，我这周用了比较短的时间去了解C++中的知识，重点学习了类与对象的相关概念，简单了解了类定义和对象数据结构的相关函数与使用。

-----

## OpenCV学习

OpenCV全称Open Computer Vision 开源计算机视觉，之前做C语言大作业的时候用过一次，不过是跟这视频一步步做没什么自己的体会，现在要认真地自己去学习了。

### 安装OpenCV

看到了作业里的要求，没有用包安装，到官网上下载了3.4.5的资源包，解压后用CMake编译安装的，也按照教程设置了环境变量和路径，执行pkg-config opencv --modversion 能够输出3.4.5的版本号。

到samples目录下跑测试文件也能够打开摄像头，Opencv就这么安装好了，可以开始学习了。

-----

555第一周作业留的有点多，这一周的时间全用来做第一周剩下来的作业了，而且这一周还有很多学业上的事情，所以这一周的时间只够我做到这里了。

不过下一周课结了两门，时间上有多了一些，应该按时完成作业的吧（但愿...）

这周日下午再去学一学Opencv。
