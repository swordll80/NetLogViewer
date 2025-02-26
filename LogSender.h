#pragma once

#include <QObject>
#include <QLocalSocket>

class LogSender : public QObject {
	Q_OBJECT
public:
	explicit LogSender(QObject *parent = nullptr);
public://文本日志
	void sendTextLog(const QString &message);
	void sendTextLog(const QString &message,
		const QString &func_name, const QString &file_name, const int file_line);
	void sendTextLog(const QString &message, const QString &func_name);
public://图像日志
	void sendImageLog(const QPixmap &image);
private:
	QLocalSocket *socket_;
	long long log_index_;
};

extern LogSender gLogSender;

#define Log2Net(s) gLogSender.sendTextLog(s,__FUNCTION__)
#define Log2NetFL(s) gLogSender.sendTextLog(s,__FUNCTION__, __FILE__,__LINE__)

