# 相机标定

基本模型是针孔相机模型，真实的针孔模型不利于快速成像，所以需要用到透镜，这样势必会对图像造成畸变，这时需要用到相机标定来矫正。Opencv中的APIcv::calibrateCamera()实现这一过程。

## 原理解释

### 相机模型

针孔相机模型是相机的基本模型，通过小孔成像原理，以光轴作为z轴，将实际坐标系下的点（X,Y,Z）映射到相机成像平面上，
$$
x_{screen}=f_x\frac{X}{Z}+c_x
\\
y_{screem}=f_y\frac{Y}{Z}+c_y
$$
由于该映射过程中将一个三维向量映射为一个二维向量，为了方便处理，将该二维向量进行射影变换，
$$
\vec{q}=M\cdot\vec{Q}
\\
\vec{q}=
\begin{bmatrix}
x\\
y\\
w
\end{bmatrix}
,
M=
\begin{bmatrix}
f_x	&	0	&	c_x\\
0	&	f_y	&	c_y\\
0	&	0	&	1
\end{bmatrix}
,
\vec{Q}=
\begin{bmatrix}
X\\
Y\\
Z
\end{bmatrix}
$$
w就是Z坐标。

### Rodrigues变换

用一向量表示旋转矩阵，向量的方向代表旋转轴的方向，向量的模值代表旋转的角度，
$$
\vec{r}=
\begin{bmatrix}
r_x	&	r_y	&	r_z
\end{bmatrix}
$$
转化为旋转矩阵为
$$
R=\cos\theta\cdot I_3+(1-\cos\theta)\cdot \vec{r}\cdot \vec{r}^{T}+\sin\theta\cdot
\begin{bmatrix}
0	&	-r_z	&	r_y\\
r_z	&	0	&	-r_x\\
r_y	&	r_x	&	0
\end{bmatrix}
$$
此方法为Rodrigues变换，API为

```c++
void cv::Rodrigues(
	cv::InputArray src,//输入向量或矩阵
    cv::OutputArray dst,//输出向量或矩阵
    cv::OutputArray jacobian = cv::noArray//（可选）雅阁比矩阵
)
```

**使用此API主要用于i将cv::solvePnP()和cv::calibrateCamera()的输出从Rodrigues格式转为旋转矩阵格式，jacobian可为默认noArray()**

### 透镜畸变

主要的两个透镜畸变是径向畸变和切向畸变，径向畸变是由透镜的形状造成的，切向畸变是由整个相机的组装过程形成的。

#### 径向畸变

径向畸变是指在成像边缘附近的像素位置产生显著的畸变，此畸变源于“鱼眼”或者“筒形”效应，靠近透镜边缘的光线将显得更加弯曲，可以用r=0附近的泰勒级数的前几项来描述。

通常，成像装置上的某一点的径向位置可以根据以下等式进行调整：
$$
x_{corrected}=x\cdot (1+k_1r^2+k_2r^4+k_3r^6)\\
y_{corrected}=y\cdot (1+k_1r^2+k_2r^4+k_3r^6)
$$
(x,y)为畸变点的原始位置，(Xcorrected,Ycorrected)是矫正后的新位置，随着距离光学中心径向距离的增加，外部点的偏移越来越向内偏移。

#### 切向畸变

这种畸变是由于制造上的缺陷使得透镜不与成像平面平行而产生的。

切向畸变可以用两个额外的参数p1和p2来表示，得到：
$$
x_{corrected}=x+[2p_1xy+p_2(r^2+2x^2)]\\
y_{corrretecd}=y+[p_1(r^2+2y^2)+2p_2xy]
$$


角点所以我们一共需要5个畸变参数，这5个参数将会被储存在一个5×1的矩阵[k1,k2,p1,p2,k3]当中。

### 标定

**接下来就是如何计算相机的内参数矩阵和畸变向量，OpenCV为我们提供了相关的函数来计算，实际过程中是通过cv::calibrateCamera()来完成的，在这个过程中，标定的方法是把相机对准一个已知的结构，通过多个视角观察，就可以计算拍摄时的方位以及相机的内部参数。** 

#### 标定板

使用棋盘格作为标定物，除此以外还可以使用二维码，圆网格等二维或者是三图案作为标定物，这里使用棋盘格。

用cv::findChessboardCorners()找到角点，

```c++
bool cv::findChessboardCorners(
	cv::InputArray image,//输入一张棋盘格的图片，格式为8UC1或者是8UC3
    cv::Size	patternSize,//表示棋盘每行和每列有多少个角点
    cv::OutputArray corners,//记录角点位置的输出矩阵，用像素坐标表示
    int		flags	=cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE//默认方式是根据平均亮度对图像进行阈值化，使用自适应阈值
);//如果找到角点则返回true
```

使用cv::drawChessboardCorners()绘制角点

```c++
void cv::drawChessboardCorners(
	cv::InputOutputArrays image,//需要绘制的棋盘格图片，格式为8UC3
    cv::Size	patternenSize,//同上一函数
    cv::InputArray	coners,//同上一函数
    bool patternWasFound//表示是否完整找到所有角点
);
```

### 棋盘中角点的个数和参数个数

我们需要求解四个相机内在参数（fx,fy,cx,cy）和五个（或者更多）畸变参数——由三个径向参数和两个切向参数组成。内在参数控制实际物体与生成图像之间的线性射影变换。因此，它们与外参数矩阵合在一起告诉我们物体实际位于何处。

外参数包括三个旋转参数和三个平移参数，一个视图共有六个外参数。

假设有N个角点和K个棋盘图像，一共提供2NK个约束，有因子2是因为每个角点有xy两个坐标值。

忽略畸变参数，我们有4个内在参数和6K个外参数需要求解。

可得出求解的前提是
$$
2\cdot N\cdot K \geqslant 6\cdot K+4
\\
Result:
\\
(N-3)\cdot K \geqslant 2
$$
但是K的值必须大于1,因为单应性矩阵可以从四个（x,y）坐标值中得出最多8个参数，也就是说只需要四个点就可以表达平面透视图可以做的一切：它在四个方向上伸展出正方形的边。所以无论有再多的角点，我们只得到四个有用的角点信息。

理论上我们只需要2个图像，实际需要10个以上的图像，这种差异是因为内在参数对非常小的噪声具有很高的灵敏度。



----

**Caution!**

*以下含有大量的数学计算，不想看可以跳过*

### 问题的实质

首先，假定求解其他参数时相机没有发生畸变，对于每个视图，我们得到单应性矩阵H，将H拆成列向量的形式，有
$$
H=[\vec{h_1},\vec{h_2},\vec{h_3}]
\\
\vec{h_1}=s\cdot M\cdot \vec{r_1}\\
\vec{h_2}=s\cdot M\cdot \vec{r_2}\\
\vec{h_3}=s\cdot M\cdot \vec{t}`
$$

> $$
> (\vec{a}\vec{b})^T=\vec{b}^T\vec{a}
> $$

可得
$$
约束1\\
\vec{r_1}^T\vec{r_2}=0\\
\vec{h_1}^TM^{-T}M^{-1}\vec{h_2}=0,M^{-T}\equiv(M^{-1})^T
$$
由
$$
||\vec{r_1}||=||\vec{r_2}||
$$
得
$$
约束2\\
\vec{h_1}^TM^{-T}M^{-1}\vec{h_1}=\vec{h_2}^TM^{-T}M^{-1}\vec{h_2}
$$
为了简化，定义矩阵B
$$
B=M^{-T}M^{-1}=
\begin{bmatrix}
B_{11}	&	B_{12}	&	B_{13}\\
B_{21}	&	B_{22}	&	B_{23}\\
B_{13}	&	B_{23}	&	B_{33}
\end{bmatrix}
$$
可解得，
$$
B=
\begin{bmatrix}
\frac{1}{f_x^2}	&	0	&	-\frac{c_x}{f_x^2}\\
0	&	\frac{1}{f_y^2}	&	-\frac{c_y}{f_y^2}\\
-\frac{c_x}{f_x^2}	&	-\frac{c_y}{f_y^2}	&	\left(\frac{c_x}{f_x^2}+\frac{c_y}{f_y^2}+1\right)
\end{bmatrix}
$$
使用B矩阵两个约束有着相同的形式，将矩阵乘开，观察每一项的组成，由于B是对称矩阵，它可以写成六维向量的点积。
$$
\vec{h_i}^TB\vec{h_j}=\vec{v_{i,j}}^T\vec{b}\\
[h_{i,1}h_{j,1}(h_{i,1}h_{j,2}+h_{i,2}h_{j,1})h_{i,2}h_{j,2}(h_{i,3}h_{j,1}+h_{i,1}h_{j,3})(h_{i,3}h_{j,2}+h_{i,2}h_{j,3}h_{i,3}h_{j,3})]
\begin{bmatrix}
B_{11}\\
B_{12}\\
B_{22}\\
B_{13}\\
B_{23}\\
B_{33}
\end{bmatrix}
$$
使用这个定义，两个约束可以写成：
$$
\begin{bmatrix}
v_{12}^{T}\\
(v_{11}-v_{22})^T
\end{bmatrix}
\vec{b}=0
$$
如果同时采用K个棋盘图像，那么我们可以将K个方程组合到一起
$$
V\vec{b}=0
$$
如果K>=2，则方程对
$$
\vec{b}=[B_{12}\quad B_{12}\quad B_{22}\quad B_{13}\quad B_{23}\quad B_{33}]^T
$$
有解。

可以从B中导出相机的内在参数
$$
fx=λ/B11fy=λB11B12B22−B122cx=B13fx2λcy=B12B13−B11B23B11B22−B122\\
其中，λ=B33−B132+cy(B12B13−B11−B23)B11
$$
外参数可以从单应性条件的方程中的出：
$$
\vec{r_1}=\mu M^{-1}\vec{h_1}\\
\vec{r_2}=\mu M_{-1}\vec{h_2}\\
\vec{r_3}=\vec{r_1}×\vec{r_2}\\
\vec{t}=\mu M^{-1}\vec{h_3}\\
其中，\mu由正交条件\frac{\mu-1}{M^{-1}\vec{h}}来确定 
$$
尽管我们做了那么多的工作，我们还没有处理相机的透镜畸变。我们使用前面得到的相机的内在参数以及设置为0的畸变参数作为我们的初始值，来求解更大的系统方程。

由于畸变，我们对图像的感知的点实际上是在错误的地方。如果相机的针孔是完好的，令(xp,yp)为点的位置，令(xd,yd)为其畸变的位置，然后
$$
\begin{bmatrix}
x_p\\
y_p
\end{bmatrix}=
\begin{bmatrix}
f_x\frac{X_W}{Z_W}+c_x\\
f_y\frac{Y_W}{Z_W}+c_y
\end{bmatrix}
$$
通过下列变换，可以得到没有畸变的标定结果：
$$
\begin{bmatrix}
x_p\\
y_p
\end{bmatrix}=
(1+k_1r^2+k_2r^4+k_3r^6)
\begin{bmatrix}
x_d\\
y_d
\end{bmatrix}+
\begin{bmatrix}
2p_1x_dy_d+p_2(r^2+2x_d^2)\\
p_1(r^2+2y_d^2)+2p_2x_dy_d
\end{bmatrix}
$$
收集并求解这些方程并找到畸变参数，之后就能重新估计内在参数和外参数，然而我们惊讶的发现这些复杂的工作OpenCV早就给你封装到cv::calibrateCamera()当中了！

----

### 标定函数

使用函数cv:calibrateCamera()可以直接由图像的角点得到相机的内在参数、畸变参数、旋转向量和平移向量。

```c++
double cv::calibrateCamera(
	cv::InputArrayOfArrays	objectPoints,//输入向量的向量 ，每个向量包含特定图像上的点的坐标
	cv::InputArrayOfArrays	imagePoints,//也是向量的标准向量，并且包含每个图像中找到的每个点的位置
    cv::Size	imageSize,//提取imagePoints中的点的图像有多大
    cv::InputOutputArrays	cameraMatrix,//返回内在参数，格式为3×3矩阵
    cv::InputOutputArrays	distCoeffs,//返回畸变参数，返回五维向量
    cv::OutputArrayOfArrays	rvecs,//向量的向量，像输入点的矩阵为旋转矩阵
    cv::OutputArrayOfArrays	tvecs,//同上，为平移矩阵
    int	flags	=	0,//可选精度控制参数
    cv::TermCriteria	criteria	=	cv::TermCriteria(cv::TermCriteria::COUNT	|	cv::TermCriteria::EPS,30,DBL_EPSILON)//终止标准，有迭代次数标准和误差标准两种
);
```

### 仅使用cv::sovlePnP()计算外参数

```c++
bool cv::sovlePnP(
	cv::InputArray	objectPoints,//和上一个函数类似，不过为来自物体的单个视图
    cv::InputArray	imagePoints,//同上
    cv::InputArray	cameraMatrix,//非输出，需要提供内参数和畸变参数
    cv::InputArray	distCoeffts,//同上
    cv::InputArray	rvec,//同上一函数
    cv::InputArray	tvec,
    bool	useExtrinsicGuess	=	false,//可设置为true以表示rvec和tvec参数中的当前值应被视为求解初始猜测值
    int	flages	=	cv::ITERNATIVE
);
```

### 矫正

我们需要纠正畸变带来的影响，使用cv::calibrateCamera()中得到的畸变系数，生产矫正后的图像。

#### 矫正映射

进行图片的矫正时，必须指定输入图像中的每个像素在输出图像中移动到的位置，称为”矫正映射“。  

首先使用cv::initUndistortRectifyMap()计算矫正映射

```c++
void cv::initUndistortRectifyMap(
	cv::InputArray	cameraMatrix,
    cv::InputArray	distCoeffs,
    cv::InputArray	R,//设置为cv::noArray()即可
    cv::InputArray	newCameraMatrix,//对于单目图像，设置为cv::noArray()
    cv::Size	size,//用来进行输出映射的尺寸，对应于用来矫正的图像尺寸
    int		m1type,//在CV_32FC1和CV_16SC2中选择，对应于map1的类型
    cv::OutputArray	map1,
    cv::OutputArray	map2
);
```

### 使用cv::remap()矫正图像

```c++
void remap(
	cv::InputArray	src,//源图像，传入图像
    cv::OutputArray	dst,//目标图像
    cv::InputArray	map1,//接入上一个函数的输出
    cv::InputArray	map2,
    int		interpolation	=	cv::INTER_LINEAR,
    int 	borderMode	=	cv::BORDER_CONSTANT,
    const cv::Scalar& borderValue	=	cv::Scalar()
);	
```

#### 使用cv::undistort()矫正图像

在某些情况下只需要矫正一个图像。或者对每一个图像重新计算矫正映射，

```c++
void cv::undistort(
	cv::InputArray	src,
    cv::OutputArray	dst,
    cv::InputArray	cameraMatrix,
    cv::InputArray	distCoeffs,
    cv::InputArray	newCameraMatrix	=	noArray()
);
```

## 实验过程

### CMake编译sample下 的calibration.cpp文件

新建文件夹calibration，把cpp文件和CMake文件拷贝到该文件夹下，终端编译。

### 准备输入文件

将data文件夹下的若干张准备好的棋盘格图片拷贝到calibration文件夹下，创建xml文件，输入

```xml
<?xml version="1.0"?>
<opencv_storage>
<images>
left01.jpg
left02.jpg
left03.jpg
left04.jpg
left05.jpg
left06.jpg
left07.jpg
left08.jpg
left09.jpg
left11.jpg
left12.jpg
left13.jpg
left14.jpg
right01.jpg
right02.jpg
right03.jpg
right04.jpg
right05.jpg
right06.jpg
right07.jpg
right08.jpg
right09.jpg
right11.jpg
right12.jpg
right13.jpg
right14.jpg
</images>
</opencv_storage>
```

保存，运行calibration程序，在终端输入

```bash
./calibration -w=9 -h=6 -s=0.025 -o=camera.yml -op -oe VID5.xml
```

即可得到yml格式的输出文件，打开yml文件，可以查阅到该组照片的内在矩阵和畸变向量。

![](/home/nh/Pictures/Screenshot from 2023-12-31 10-37-03.png)
