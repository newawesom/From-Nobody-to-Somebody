#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>
#include <string.h>
using namespace std;
using namespace cv;

void colordetec(int b, int g, int r);
int main(int argc, char** argv)
{
    char test[15];
    cin >> test;
    Mat input_img = imread(test);
    if(input_img.empty())
    {
        cout << "Could not load images..." << endl;
        return -1;
    }
    namedWindow("Input",CV_WINDOW_AUTOSIZE);
    imshow("Input",input_img); 
    Mat gray;
    Mat binary;
    Mat output_img;
    output_img = input_img.clone();
    cvtColor(input_img, gray, COLOR_BGR2GRAY);
    for (int j = 0; j < gray.rows ; j++)
    {
        for (int k = 0; k < gray.cols ; k++)
        {
            if(gray.at<uchar>(j, k) != 255)
            {
                    gray.at<uchar>(j, k) = 0;
            }
        }
    }
    //二值化
    threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);
    binary = ~binary;
        int counts = 1;
        vector<vector<Point>> contours;
        vector<Point> point;
        vector<Vec4i> hireachy;
        findContours(binary, contours, hireachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());//寻找边界
        for(size_t t = 0; t < contours.size(); t++)
        {
            int epsilon = 0.01 * arcLength(contours[t], true);
            approxPolyDP(contours[t], point, epsilon, true);//边界拟合
            Rect rect = boundingRect(contours[t]);//外接矩形
            string number = to_string(counts);
            if (point.size() == 3)
            {
                drawContours(output_img, contours, t, Scalar(255,0,255), 5, LINE_8, Mat(), 0, Point());
                putText(output_img, number, rect.tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0),1, LINE_8);
                int x = rect.x + cvRound(rect.width/2.0);
                int y = rect.y + cvRound(rect.height/2.0);
                int b = output_img.at<Vec3b>(y,x)[0];
                int g = output_img.at<Vec3b>(y,x)[1];
                int r = output_img.at<Vec3b>(y,x)[2];
                cout << "<" << number << ">\t" <<"<" << "Triangle" << ">\t";
                colordetec(b, g, r);
                counts++;
            }
            else if(point.size() == 4)
            {
                drawContours(output_img, contours, t, Scalar(255,0,255), 5, LINE_8, Mat(), 0, Point());
                putText(output_img, number, rect.tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0),1, LINE_8);
                int x = rect.x + cvRound(rect.width/2.0);
                int y = rect.y + cvRound(rect.height/2.0);
                int b = output_img.at<Vec3b>(y,x)[0];
                int g = output_img.at<Vec3b>(y,x)[1];
                int r = output_img.at<Vec3b>(y,x)[2];
                cout << "<" << number << ">\t" << "<" << "Square" << ">\t";
                colordetec(b, g, r);
                counts++;
            }
            else{
                drawContours(output_img, contours, t, Scalar(255,0,255), 5, LINE_8, Mat(), 0, Point());
                putText(output_img, number, rect.tl(), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0),1, LINE_8);
                int x = rect.x + cvRound(rect.width/2.0);
                int y = rect.y + cvRound(rect.height/2.0);
                int b = output_img.at<Vec3b>(y,x)[0];
                int g = output_img.at<Vec3b>(y,x)[1];
                int r = output_img.at<Vec3b>(y,x)[2];
                cout << "<" << number << ">\t" << "<" << "Circle" << ">\t";
                colordetec(b, g, r);
                counts++;
            }
        }
        cout << "There are all the outputs of the image." <<endl;
        cout << "The sum is " << counts << endl;
    //绘制框选边界曲线
    namedWindow("output", CV_WINDOW_FREERATIO);
    imshow("output", output_img);
    waitKey(0);
    return 0;
}
void colordetec(int b, int g, int r)
{
    if(!b)
    {
        if (!g)
        {
            if (!r)
            {
                cout << "<" << "BLack" << ">" <<endl;
            }
            else
            {
                cout << "<" << "Red" << ">" <<endl;
            }
        }
        else
        {
            if (!r)
            {
                cout << "<" << "Green" << ">" <<endl;
            }
            else
            {
                cout << "<" << "Yellow" << ">" <<endl;
            }
        }
    }
    else{
        cout << "<" << "Blue" << ">" <<endl;
    }
}