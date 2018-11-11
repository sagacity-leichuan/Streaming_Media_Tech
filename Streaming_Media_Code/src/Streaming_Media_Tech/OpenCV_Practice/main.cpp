
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "XMediaEncode.h"
#include "XRtmp.h"
extern "C"
{
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
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

//初始化像素格式上下文
void test004()
{
	//相机的rtsp url
	char *inUrl = "rtsp://admin:@192.168.10.30:554/ch0_0.264";
	VideoCapture cam;
	namedWindow("video");

	//像素格式转换上下文
	SwsContext *vsc = NULL;

	try
	{
		////////////////////////////////////////////////////////////////
		/// 1 使用opencv打开rtsp相机
		cam.open(inUrl);
		if (!cam.isOpened())
		{
			throw exception("cam open failed!");
		}
		cout << inUrl << " cam open success" << endl;
		int inWidth = (int)cam.get(CAP_PROP_FRAME_WIDTH);
		int inHeight = (int)cam.get(CAP_PROP_FRAME_HEIGHT);
		int fps = (int)cam.get(CAP_PROP_FPS);

		///2 初始化格式转换上下文
		vsc = sws_getCachedContext(vsc,
			inWidth, inHeight, AV_PIX_FMT_BGR24,	 //源宽、高、像素格式
			inWidth, inHeight, AV_PIX_FMT_YUV420P,//目标宽、高、像素格式
			SWS_BICUBIC,  // 尺寸变化使用算法
			0, 0, 0
		);
		if (!vsc)
		{
			throw exception("sws_getCachedContext failed!");
		}
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
		if (vsc)
		{
			sws_freeContext(vsc);
			vsc = NULL;
		}
		cerr << ex.what() << endl;
	}
	getchar();
}

//rtsp数据源到rtmp推流      要重点复习 
void test005()
{
	cout << "void test005()!" << endl;
	//相机的rtsp url
	char *inUrl = "rtsp://admin:@192.168.10.30:554/ch0_0.264";
	//nginx-rtmp 直播服务器rtmp推流URL
	char *outUrl = "rtmp://192.168.10.181/live";

	//注册所有的编解码器
	avcodec_register_all();

	//注册所有的封装器
	av_register_all();

	//注册所有网络协议
	avformat_network_init();


	VideoCapture cam;
	Mat frame;
	namedWindow("video");

	//像素格式转换上下文
	SwsContext *vsc = NULL;

	//输出的数据结构
	AVFrame *yuv = NULL;

	//编码器上下文
	AVCodecContext *vc = NULL;

	//rtmp flv 封装器
	AVFormatContext *ic = NULL;


	try
	{	////////////////////////////////////////////////////////////////
		/// 1 使用opencv打开rtsp相机
		cam.open(inUrl);
		if (!cam.isOpened())
		{
			throw exception("cam open failed!");
		}
		cout << inUrl << " cam open success" << endl;
		int inWidth = (int)cam.get(CAP_PROP_FRAME_WIDTH);
		int inHeight = (int)cam.get(CAP_PROP_FRAME_HEIGHT);
		int fps = (int)cam.get(CAP_PROP_FPS);

		///2 初始化格式转换上下文
		vsc = sws_getCachedContext(vsc,
			inWidth, inHeight, AV_PIX_FMT_BGR24,	 //源宽、高、像素格式
			inWidth, inHeight, AV_PIX_FMT_YUV420P,//目标宽、高、像素格式
			SWS_BICUBIC,  // 尺寸变化使用算法
			0, 0, 0
		);
		if (!vsc)
		{
			throw exception("sws_getCachedContext failed!");
		}

		///3 初始化输出的数据结构
		yuv = av_frame_alloc();
		yuv->format = AV_PIX_FMT_YUV420P;
		yuv->width = inWidth;
		yuv->height = inHeight;
		yuv->pts = 0;
		//分配yuv空间
		int ret = av_frame_get_buffer(yuv, 32);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}

		///4 初始化编码上下文，分为以下三步
		//a 找到编码器
		AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
		if (!codec)
		{
			throw exception("Can`t find h264 encoder!");
		}
		//b 创建编码器上下文
		vc = avcodec_alloc_context3(codec);
		if (!vc)
		{
			throw exception("avcodec_alloc_context3 failed!");
		}
		//c 配置编码器参数
		vc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //全局参数
		vc->codec_id = codec->id;
		vc->thread_count = 8;

		vc->bit_rate = 50 * 1024 * 8;//压缩后每秒视频的bit位大小 50kB
		vc->width = inWidth;
		vc->height = inHeight;
		vc->time_base = { 1,fps };
		vc->framerate = { fps,1 };

		//画面组的大小，多少帧一个关键帧
		vc->gop_size = 50;
		vc->max_b_frames = 0;
		vc->pix_fmt = AV_PIX_FMT_YUV420P;
		//d 打开编码器上下文
		ret = avcodec_open2(vc, 0, 0);

		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}
		cout << "avcodec_open2 success!" << endl;

		///5 输出封装器和视频流配置
		//a 创建输出封装器上下文
		ret = avformat_alloc_output_context2(&ic, 0, "flv", outUrl);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}

		//b 添加视频流 
		AVStream *vs = avformat_new_stream(ic, NULL);
		if (!vs)
		{
			throw exception("avformat_new_stream failed");
		}
		vs->codecpar->codec_tag = 0;
		//从编码器复制参数
		avcodec_parameters_from_context(vs->codecpar, vc);
		av_dump_format(ic, 0, outUrl, 1);


		///打开rtmp 的网络输出IO
		ret = avio_open(&ic->pb, outUrl, AVIO_FLAG_WRITE);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}

		//写入封装头
		ret = avformat_write_header(ic, NULL);
		if (ret != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(ret, buf, sizeof(buf) - 1);
			throw exception(buf);
		}

		AVPacket pack;
		memset(&pack, 0, sizeof(pack));
		int vpts = 0;
		
		//死循环
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


			///rgb to yuv
			//输入的数据结构
			uint8_t *indata[AV_NUM_DATA_POINTERS] = { 0 };
			//indata[0] bgrbgrbgr
			//plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr 
			indata[0] = frame.data;
			int insize[AV_NUM_DATA_POINTERS] = { 0 };
			//一行（宽）数据的字节数
			insize[0] = frame.cols * frame.elemSize();
			int h = sws_scale(vsc, indata, insize, 0, frame.rows, //源数据
				yuv->data, yuv->linesize);
			if (h <= 0)
			{
				continue;
			}
			cout << h << " " << flush;

			///h264编码
			yuv->pts = vpts;
			vpts++;
			ret = avcodec_send_frame(vc, yuv);
			if (ret != 0)
				continue;

			ret = avcodec_receive_packet(vc, &pack);
			if (ret != 0 || pack.size > 0)
			{
				cout << "*" << pack.size << flush;
			}
			else
			{
				continue;
			}
			//推流
			pack.pts = av_rescale_q(pack.pts, vc->time_base, vs->time_base);
			pack.dts = av_rescale_q(pack.dts, vc->time_base, vs->time_base);
			pack.duration = av_rescale_q(pack.duration, vc->time_base, vs->time_base);
			ret = av_interleaved_write_frame(ic, &pack);
			if (ret == 0)
			{
				cout << "#" << flush;
			}
		}

	}
	catch (exception &ex)
	{
		if (cam.isOpened())
			cam.release();
		if (vsc)
		{
			sws_freeContext(vsc);
			vsc = NULL;
		}

		if (vc)
		{
			avio_closep(&ic->pb);
			avcodec_free_context(&vc);
		}

		cerr << ex.what() << endl;
	}
	getchar();
}

//opencv_rtsp_to_rtmp_class封装重构代码    要重点复习
void test006()
{
	cout << "void test006()!" << endl;
	//相机的rtsp url
	char *inUrl = "rtsp://admin:@192.168.10.30:554/ch0_0.264";
	//nginx-rtmp 直播服务器rtmp推流URL
	char *outUrl = "rtmp://192.168.10.181/live";


	//编码器和像素格式转换
	XMediaEncode *me = XMediaEncode::Get(0);

	//封装和推流对象
	XRtmp *xr = XRtmp::Get(0);

	VideoCapture cam;
	Mat frame;
	namedWindow("video");

	int ret = 0;
	try
	{	////////////////////////////////////////////////////////////////
		/// 1 使用opencv打开rtsp相机
		cam.open(inUrl);
		if (!cam.isOpened())
		{
			throw exception("cam open failed!");
		}
		cout << inUrl << " cam open success" << endl;
		int inWidth = (int)cam.get(CAP_PROP_FRAME_WIDTH);
		int inHeight = (int)cam.get(CAP_PROP_FRAME_HEIGHT);
		int fps = (int)cam.get(CAP_PROP_FPS);

		///2 初始化格式转换上下文
		///3 初始化输出的数据结构
		me->inWidth = inWidth;
		me->inHeight = inHeight;
		me->outWidth = inWidth;
		me->outHeight = inHeight;
		me->InitScale();

		///4 初始化编码上下文
		//a 找到编码器
		if (!me->InitVideoCodec())
		{
			throw exception("InitVideoCodec failed!");
		}

		///5 输出封装器和视频流配置
		xr->Init(outUrl);

		//添加视频流 
		xr->AddStream(me->vc);
		xr->SendHead();

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
			//imshow("video", frame);
			//waitKey(1);


			///rgb to yuv
			me->inPixSize = frame.elemSize();
			AVFrame *yuv = me->RGBToYUV((char*)frame.data);
			if (!yuv) continue;

			///h264编码
			AVPacket *pack = me->EncodeVideo(yuv);
			if (!pack) continue;

			xr->SendFrame(pack);


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
	//test003();
	//test004();
	//test005();
	test006();
	return 0;
}