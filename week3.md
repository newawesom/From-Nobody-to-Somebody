# 第三周学习笔记

## OpenCV

### 写能够编译OpenCV的CMakeLists.txt文件

```cmake
cmake_minimum_required(VERSION 2.8)

project(test)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(test test2.cpp)

target_link_libraries(test ${OpenCV_LIBS})
```

### 加载、修改和保存图片

```c++
Mat img = imread("1.jepg", [可选参数]);//读取一张图片
//可选参数 >0原图 0灰度图 <0RGB通道输出图
namedWindow("Test", CV_WINDOW_AUTOSIZE);
imshow("Test", img);//创建一这个自动大小的窗口，展示img变量中的图片

Mat output;
cvtColor(img, output_img, [可选参数模式]);//CV_BGR2GRAY转换为灰度图
imwrite("output.jpeg", output_img);//输出转化后的图片到output.jpeg
```

**重点：**

​	1.加载>>imread() 

​	2.修改>>cvtColor()

​	3.保存>>imwirte()

### Mat对象

Mat对象由两个部分组成，对象的头部分和指针部分。

> 复制可以分为部分复制和完全复制两类，实际上Mat对象只是创建了一个指向文件的指针，部分复制是将A的地址赋给B，也就是AB指向同一个文件，而完全复制则是开辟出一块新的内存空间用于存放与A指向文件相同的文件。

可以使用Mat对象直接创建一个纯色图片，

```c++
Mat M([rows],[colums],[type参数],Scalar(255,255,255));//参数可选CV_8UC3代表3通道
```

获取一张已知的图片的type类型可以使用.type()引用，

```c++
//Img.type()
```

### 读取和修改图像的像素

读取单通道灰度图的像素值使用函数gray_src.at<uchar>(rows,cols)获取位于rows行cols列的像素的像素值。

读取三通道RGB图        vector<vector<Point>> contours;的像素值可以使用rgb_src.at<Vec3b>(rows,cols)[0/1/2]，与单通道不同的是由于RGB图的像素储存了三个通道的值，所以该函数返回的是一个数组指针，引用值0/1/2可以分别获得B/G/R三个通道的值。+

```c++
//单通道灰度图
int gray = gray_src.at<uchar>(rows, cols);
//三通道RGB图
int b = rgb_src.at<Vec3b
>(rows, cols)[0];
int g = rgb_src.at<Vec3b>(rows, cols)[1];
int r = rgb_src.at<Vec3b>(rows, cols)[2];
```

要修改像素的话，直接在函数上进行修改即可。

### 图像混合

使用函数addWeighted可以实现张图片的线性混合。

```c++
void cv::addWeighted(InputArray[src1], double[alpha], InputArray[src2], double[beta], double[gamma], OutputArray[dst], int[dtype = -1]);
```

该函数使得图像按一下算法进行线性混合，
$$
G(x) = \alpha F_{1}(x)+\beta F_{2}(x)+\gamma
\\
\alpha + \beta = 1
$$
阿尔法和贝塔为权重，伽玛为修正值。

### 调整图像的亮度与对比度

调整图像亮度和对比度属于像素操作变换-点操作，运用以下公式
$$
g(i,j)=\alpha f(i,j)+\beta
\\
其中\alpha >0，\beta 为增益变量
$$
要想提高图像的亮度，需要增加像素的整体值，增大贝塔的值即可，

要想提高图像的对比度，需要增大像素之间的差值，增大阿尔法的值即可。

### 绘制形状和文字

绘制一个点需要知道点的坐标，需要用到的API有Point 和Scalar两个函数，

```c++
Point p;//创建一个点对象
p.x = 1;
p.y = 2;
//或者p = Pont(10, 8);
Scalar(b, g, r);//bgr代表三个通道
//Scalar本身为一个数组
```

绘制线，

```c++
int Lines(int x1, int y1, int x2, int y2)
{
    Point p1 = Point(x1, y1);//p1位置坐标
    Point p2 = Point(x2, y2);//p2位置坐标
    Scalar color = Scalar(0, 0, 255);
    line(Img, p1, p2, color, 1, LINE_8);//1的位置代表线宽，LINE_8代表线的类型
    //线的类型还有LINE_4、LINE_AA（高斯平滑处理）
}
```

绘制矩形，

```c++
int Rectangle(int x1, int y1, int x2, int y2)
{
    Rect rect = Rect(x1, y1, x2, y2);//Rect 为一个矩形对象
    Scalar color = Scalar(0, 0, 255);
    rectangle(Img, rect, color, 2, LINE_8);//2的位置代表线宽
}
```

绘制椭圆，

```c++
int Ellipse(int x,int y, int a, int b, float arg, float arg_sta, float arg_end)
{
    Point p = Point(x, y);
    Scalar color = Scalar(0, 0, 255);
    ellipse(Img, p, Size(a, b), arg, arg_sta, arg_end, color, 3, LINE_8);
    //x,y代表椭圆的中心，a、b代表长轴和短轴，arg代表倾斜程度，arg_sta和arg_end代表椭圆绘制的起点和终点，范围均为角度制下的0到360度，3代表线宽
}
```

绘制圆，

```c++

        vector<vector<Point>> contours;int Circle(int x, int y, float r)
{
	Point p = Point(x, y);
	Scalar color = Scalar(0,0,255);
	circle(Img, Point, r, color, 4, LINE_8);
}
```

## 附：

这周因为有马原考试，所以opencv没有学很多（555下周没有考试应该就好了吧），作业颜色识别模块也没有写出来，只做了形状识别的部分，下周一定补上。
