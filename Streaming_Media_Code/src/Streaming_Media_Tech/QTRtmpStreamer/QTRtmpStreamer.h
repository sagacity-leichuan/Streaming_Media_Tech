#pragma once

#include <QtWidgets/QWidget>
#include <QCloseEvent>
#include "ui_QTRtmpStreamer.h"

class QTRtmpStreamer : public QWidget
{
	Q_OBJECT

public:
	QTRtmpStreamer(QWidget *parent = Q_NULLPTR);

public slots:
	void Stream();

private:
	void closeEvent(QCloseEvent *event);
private:
	Ui::QTRtmpStreamerClass ui;
};
