#include "QTRtmpStreamer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTRtmpStreamer w;
	w.show();
	return a.exec();
}
