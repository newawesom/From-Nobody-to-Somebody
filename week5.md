# 第五周

## OpenCV学习笔记

### 模糊图像

模糊处理可以对图像预处理时候降低噪声。

像素计算原理
$$
g(i,j)=\sum_{k,l}f(i+k,j+l)h(k.l)
$$
其中，h(k,l)是某一卷积算子。

其实，简单

来说，就是对像素和其周围的像素输入一个函数，输出到该像素中。

#### 一些简单的卷积算子

- 归一化滤波
  $$
  K=\frac{1}{K_{width}K_{height}}
  \left[
  \begin{matrix}
   1      & 1      & \cdots & 1      \\
   1      & 1      & \cdots & 1      \\
   \vdots & \vdots & \ddots & \vdots \\
   1      & 1      & \cdots & 1      \\
  \end{matrix}
  \right]
  $$
  权重均为1

  API：

  ```C++
  blur(Mat src, Mat dst, Size(xradius,yradius),Point(-1,-1));//-1,-1代表默认卷积核在中心
  ```

- 高斯滤波

$$
G_{0}(x,y)=Ae^{\frac{-(x-\mu_{x})^{2}}{2\sigma_{x}^{2}}+\frac{-(y-\mu_{y})^{2}}{2\sigma_{y}^{2}}}
$$

​		权重按正态分布

​		API：		

```c++
GaussianBlur(Mat src,Mat dst,Size(11,11),sigmax,sigmay);//x和y必须是正数且为奇数
```

------

### 腐蚀和膨胀

腐蚀和膨胀属于形态学操作，形态学有四个基本操作：腐蚀、膨胀、开、闭。

#### 腐蚀

轮廓任意形状的结构元素B在图像A上面移动，其中心为B的锚点，将结构元素中的像素的最大值赋予其锚点的像素值，此过程称为腐蚀。

操作前：

|   11   |   12   |   13   |
| :----: | :----: | :----: |
| **14** | **15** | **16** |
| **17** | **18** | **19** |

操作后：

|   11   |    12    |   13   |
| :----: | :------: | :----: |
| **14** | ***19*** | **16** |
| **17** |  **18**  | **19** |

API：

- 获取结构元素

```c++
圆形狂Mat getstructuringElement(int shape Size ksize, Point anchor);
```

-形状(MORPH_RECT\MORPH_CROSS\MORPH_ELLIPSE)

-大小 必须为奇数

-锚点 默认是Point(-1,-1)u意为中心像素

- ### 腐蚀

  ```c++
  void erode(Mat src,Mat dst, kernel);
  ```

  $$
  dst(x,y)=\max_{(x',y'):element(x',y')\neq0}src(x+x',y+y')
  $$

#### 膨胀

与腐蚀唯一不同的点在于，用结构元素中像素最小值替换锚点的像素值。

- 膨胀

  ```c++
  void dilate(Mat src,Mat dst, kernel);
  ```

  $$
  dst(x,y)=\min_{(x',y'):element(x',y')\neq0}src(x+x',y+y')
  $$


------

### 形态学操作

#### 开操作

先服饰后膨胀

用于去除小的明亮对象（对象是前景色，背景黑色）

#### 闭操作

先膨胀后腐蚀

可以填充小的洞（对象是前景色，背景为黑色）

#### 形态学梯度

膨胀减去腐蚀
$$
dst=morph_{grad}(src,element)=dilate(src,element)-erode(src,element)
$$
又称为基本梯度（其他还包括内部梯度、方向梯度）

#### 顶帽

原图像与开操作之间的差值图像

#### 黑帽

原图像与闭操作之间的差值图像

```c++
void morphologyEX(Mat src, Mat dst, CV_MOP_[MODE], kernel);
//MODE可选OPEN开操作、CLOSE闭操作、、BLACKHAT黑帽、MOP_GRADIENT形态学梯度
Mat kernel;//kernel为结构元素圆形狂
```

------

### 基本阈值操作threshold

阈值是图像分割的标尺。

#### 阈值类型

二值化，大于某一像素值的变为255,低于该阈值变为0.
$$
dst(x,y)=
\begin{cases}
255,src(x,y)\geqslant threshold\\
0,src(x,y)\leqslant threshold\\
\end{cases}
$$


反二值化，大于某一值变为0,小于某一值变为255.
$$
dst(x,y)=
\begin{cases}
0,src(x,y)\geqslant threshold\\
255,src(x,y)\leqslant threshold\\
\end{cases}
$$
截断，超过阈值等于阈值
$$
dst(x,y)=
\begin{cases}
threshold,src(x,y)\geqslant threshold\\
src(x,y),src(x,y)\leqslant threshold\\
\end{cases}
$$
阈值取零，低于阈值取0
$$
dst(x,y)=
\begin{cases}
src(x,y),src(x,y)\geqslant threshold\\
0,src(x,y)\leqslant threshold\\
\end{cases}
$$
阈值反取0,大于阈值取0
$$
dst(x,y)=
\begin{cases}
0,src(x,y)\geqslant threshold\\
src(x,y),src(x,y)\leqslant threshold\\
\end{cases}
$$
API

```c++
void threshold(Mat gray_src,dst,threshold_value,threshold_max,[TYPE]);
//[TYPE]可选
//THRESH_BINARY二值化
//THRESH_BINARY_INV反二值化
//THRESH_TOZERO阈值取0
//THRESH_TOZERO_INV反阈值取0
//THRESH_TRUNC截断
//THRESH_OTSU自动，此时将threshold_value改为取值下届
//THRESH_TRIANGLE三角自动取值
```

------

### 发现轮廓&绘制轮廓

图像中物体区别于环境的边缘为轮廓。

发现轮廓的API：

```c++
void cv::findContours(InputOutputArray binImg,//输入二值化的图像
                     OutputArrayOfArrays contours,//全部发现的轮廓对象，为二维数组
                     OutputArray hierachy,//图像的拓扑结构（可选）
                     int mode,//轮廓返回的方式，建议为RETR_TREE
                     int method,//发现方法，建议为CHAIN_APPROX_SIMPLE
                     Point offset=Point()//轮廓像素的位移，默认为（0,0）无位移
                     );
```

绘制轮廓的API:

```c++
void cv:drawContours(
    InputOutputArray binImg,//输出图像
    OutputArrayOfArrays contours,//全部发现的轮廓对象，为二维数组
    int contourldx,//轮廓索引号
    const Scalar & color，//颜色
    int thickness,//线宽
    int linetype,//线型LINE_8
    InputArray hierarchy,//拓扑结构图
    int maxlevel,//最大层数，0只绘制当前的，1表示绘制当前及其内嵌的轮廓
    Point offset=Point()//位移轮廓
);
```

思路：

1. 输入图像转为灰度图
2. 使用Canny进行边缘提取
3. 使用findContours寻找轮廓
4. 使用drawContours绘制轮廓

绘制轮廓时，使用for循环绘制所有大小的轮廓：

```c++
for (size_t t = 0; t < contours.size(); t++)
{
    Scalar color = Scalar(0,0,0);
    drawContours(dst contours, t, color, 2, LINE_8, hierachy, 0, Point(0,0));
}
```

------

### 轮廓周围绘制矩形框和圆形框

```c++
void cv::approxPolyDP(InputArray curve, OutputArray approxCurve, double epsilon, bool closed);//减少多边形的轮廓点数
```

轮廓周围绘制矩形和圆

```c++
Rect cv::boundingRect(InputArray points);//得到轮廓的最小外接正矩形
RotatedRect cv::minAreaRect(InputArray points);//得到一个旋转的最小外接矩形
Ellipse cv::minEnclosingCircle(Input Array points, Point2f&center, float radius);//得到最小区域圆形
Ellipse cv::fitEllipse(InputArray points);//得到最小椭圆
```

------

### 自定义线性滤波

#### 卷积

卷积是图像处理中的一个操作，是kernel在图像上的每个像素上的操作。

kernel本质上是一个固定大小的矩阵数组，其中心点称为锚点(anchor point)。
$$
f_{1}(t)*f_2(t)=\int_{-\infty}^{+\infty}f_1(\tau)f_2(t-\tau)\,d\tau
\\不是这个卷积
$$

$$
H(x,y)=\sum_{i=0}^{M_{i-1}}\sum_{j=0}^{M_{j-1}}I(x+i-a_i,y+j-a_j)K(i,j)
\\是这个卷积
$$

常见算子：

Robert算子
$$
\left[
\begin{matrix}
1	& 0\\
0	& -1\\
\end{matrix}
\right]
\\
\left[
\begin{matrix}
0	&1\\
-1	&0
\end{matrix}
\right]
$$
Sobel算子
$$
\left[
\begin{matrix}
-1	&0	&1\\
-2	&0	&2\\
-1	&0	&1
\end{matrix}
\right]
\\
\left[
\begin{matrix}
-1	&-2	&-1\\
0	&0	&0\\
1	&2	&1
\end{matrix}
\right]
$$
Laplace算子
$$
\left[
\begin{matrix}
0	&-1	&0\\
-1	&4	&-1\\
0	&-1	&0
\end{matrix}
\right]
$$
自定义算子方式：

```c++
Mat kernel = (Mat_<int>(x,y) << [依次填写]);
```

```c++
filter2D(Mat src,//输入图像
         Mat dst,//模糊图像
         int depth.//图像深度32/8,默认为-1
         Mat kernel,//卷积核/模板
         Point anchor,//锚点位置，默认中心为Point(-1,-1)
         double delta//计算出来的像素值的修正值，默认为0不修正
        );
```

------

### HighGUI

#### 视频的读取

视频由一系列的图像组成，构成视频的一张图像称为“帧（frame）”，两帧之间的间隔时间称为帧率，单位是（帧/秒）。

首先定义一个VideoCapture的对象来打开和读取视频流

参数可选0（打开相机1），1（打开相机2），...，字符串打开指定的文件名。

可以用VideoCapture::isOpened()来检查是否打开成功

```c++
if(!captRefrnc.isOpened())
{
    cout<<"Could not open reference" <<endl;
    return -1;
}
```

从视频中保存每一帧图像，使用>>符释放每一帧图像并保存到Mat格式。由于视频流是连续的，所以每次调用read函数后及时保存图像。

检查视频的分辨率和帧数，使用get函数完成

```c++
Size refS = Size((int) captRefrnc.get(CV_CAP_PROP_FRAME_WIDTH),(int) captRefrnc.get(CV_CAP_PROP_FRAME_HRIGHT));
```

设置这些值需要用到set函数，第一个参数代表需要设定的属性ID，第二个参数是需要设定的值。
