#include "QTRtmpStreamer.h"
#include <iostream>
#include "XController.h"
#include <QMessageBox>
using namespace std;
static bool isStream = false;


QTRtmpStreamer::QTRtmpStreamer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

}

void QTRtmpStreamer::Stream()
{
	cout << "Stream";
	if (isStream)
	{
		isStream = false;
		ui.startButton->setText(QString::fromLocal8Bit("开始"));
		XController::Get()->Stop();
	}
	else
	{
		isStream = true;
		ui.startButton->setText(QString::fromLocal8Bit("停止"));
		QString url = ui.inUrl->text();
		bool ok = false;
		int camIndex = url.toInt(&ok);
		if (!ok)
		{
			XController::Get()->inUrl = url.toStdString();
		}
		else
		{
			XController::Get()->camIndex = camIndex;
		}
		XController::Get()->outUrl = ui.outUrl->text().toStdString();
		XController::Get()->Set("b", (ui.facelevel->currentIndex() + 1) * 3);
		XController::Get()->Start();
	}
}

void QTRtmpStreamer::closeEvent(QCloseEvent * event)
{
	QMessageBox message_cd(QMessageBox::NoIcon, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("您是否要关闭此软件？"));
	message_cd.addButton(QString::fromLocal8Bit("确定"), QMessageBox::AcceptRole);
	message_cd.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);
	if (message_cd.exec() == QMessageBox::AcceptRole)
	{
		event->accept();
		XController::Get()->Stop();
	}
	else
	{
		event->ignore();
	}
}
