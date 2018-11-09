
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;

//简单测试显示图片
void test000()
{
	Mat image = imread("001.jpg");
	namedWindow("img");
	imshow("img", image);
	waitKey(0);
}

//简单测试Mat对象
void test001()
{
	Mat mat(200, 400, CV_8UC3);
	//mat.create(3000, 4000, CV_8UC3);
	//元素大小字节数
	int es = mat.elemSize();
	int size = mat.rows*mat.cols*es;

	//地址遍历连续的Mat
	for (int i = 0; i < size; i += es)
	{
		mat.data[i] = 255;	//B
		mat.data[i + 1] = 100;//G
		mat.data[i + 2] = 100;//R
	}
	namedWindow("mat");
	imshow("mat", mat);
	waitKey(0);
}

//简单测试获取USB摄像头和网络摄像头的数据
void test002()
{
	char *inUrl = "rtsp://admin:@192.168.10.30:554/ch0_0.264";
	VideoCapture cam;
	namedWindow("video");

	//if(cam.open(inUrl))
	if (cam.open(0))
	{
		cout << "open camera success!" << endl;
	}
	else
	{
		cout << "open camera failed!" << endl;

		waitKey(1);
		return;
	}
	Mat frame;
	for (;;)
	{
		cam.read(frame);
		imshow("video", frame);
		waitKey(1);
	}
}

//使用OpenCV做rtsp to rtmp，并添加异常处理
void test003()
{
	//海康相机的rtsp url
	char *inUrl = "rtsp://admin:@192.168.10.30:554/ch0_0.264";
	VideoCapture cam;
	namedWindow("video");

	cam.open(inUrl);
	try
	{
		if (!cam.isOpened())
		{
			throw exception("cam open failed!");
		}
		cout << inUrl << " cam open success" << endl;

		Mat frame;
		for (;;)
		{
			///读取rtsp视频帧，解码视频帧
			if (!cam.grab())
			{
				continue;
			}
			///yuv转换为rgb
			if (!cam.retrieve(frame))
			{
				continue;
			}
			imshow("video", frame);
			waitKey(1);
		}
	}
	catch (exception &ex)
	{
		if (cam.isOpened())
			cam.release();
		cerr << ex.what() << endl;
	}
	getchar();
}



int main(int argc, char *argv[])
{
	//test000();
	//test001();
	//test002();
	test003();
	return 0;
}