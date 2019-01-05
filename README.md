# FFMPEG_And_Other_Study
	About FFmpeg and OpenCV practices, related projects and streaming media related technical notes
	The relevant code and resources are from the learning reference code of CaojunXia teacher , source website: http://www.ffmpeg.club/

## Development Environment:
	WIN7 64-bit Visual Studio 2015
	QT 5.9 32 bit
	FFmpeg 3.3 32 bit
	Opencv 3.2 32 bit
	There is a dll file required by the executable program in the bin directory, and the corresponding dll file needs to be added according to the library version.

## FFMPEG_Practice subproject
	About FFMPEG related test and practice code.
	1. Use files to push to RTMP
## OpenCV_Practice subproject
	About OpenCV related test and practice code.
	1. Simple test display picture
	2. Simple test Mat object
	3. Simple test to get data from USB camera and webcam
	4. Use OpenCV to do rtsp to rtmp and add exception handling
	5. Initialize the pixel format context
	6. rtsp data source to rtmp push stream
	7. opencv_rtsp_to_rtmp_class package refactoring code
## Qt_Audio_Rtmp subproject
	About the test code related to the implementation of audio push stream in FFMPEG in the QT library.
	1. QT recording audio test
	2. Audio push
	3. Audio push stream code reconstruction
## Qt_Audio_Rtmp_Class Subproject
	A class wrapper for test code related to audio pushstream implementation in conjunction with FFMPEG in the QT library.
	1. Using the packaged audio recording interface, recording buffer queue processing
## Audio_And_video_Synchronization subproject
	About the combination of FFMPEG and OpenCV in the QT library to achieve camera audio and video acquisition and image beauty and push flow.
## QTRtmpStreamer subproject
	The specific implementation of streaming live.
