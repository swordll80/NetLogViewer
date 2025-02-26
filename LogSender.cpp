#include "LogSender.h"
#include <QDataStream>
#include <QImage>
#include <QPixmap>
#include <QBuffer>
#include <QTextCodec>

LogSender gLogSender;
// 互斥锁
QMutex gMutexSendPic;
QMutex gMutexSendStr;
QMutex gMutexSendStrFunc;
QMutex gMutexSendStrFuncFileLine;

LogSender::LogSender(QObject *parent) : QObject(parent) {
	log_index_ = 0;
	socket_ = new QLocalSocket(this);
	socket_->connectToServer("MyLogServer");
	if (socket_->state() != QLocalSocket::ConnectedState){
		qDebug() << "Error: connectToServer MyLogServer";
	}
}

//sendTextLog(QStringLiteral("编辑器"));//QString QStringLiteral为UTF-16 编码
void LogSender::sendTextLog(const QString &message) {
	QMutexLocker locker(&gMutexSendStr); // 获取互斥锁
	if (socket_->state() != QLocalSocket::ConnectedState){
		socket_->connectToServer("MyLogServer");
	}
	if (socket_->state() == QLocalSocket::ConnectedState) {

		// 直接将 UTF-16 字符串转换为 QByteArray
		QByteArray data = message.toUtf8();

		// 构造数据包
		QByteArray packet;
		QDataStream stream(&packet, QIODevice::WriteOnly);
		stream.setVersion(QDataStream::Qt_5_5);

		// 写入数据包长度
		stream << static_cast<quint32>(0); // 预留 packetSize

		// 写入数据包类型
		stream << QString("TEXT");

		// 写入数据包内容
		//stream << message;
		quint32 dataSize = data.size();
		stream << dataSize;// 发送数据长度（4 字节）
		stream.writeRawData(data.data(), dataSize);
		stream.device()->seek(0);
		stream << static_cast<quint32>(packet.size()); // 写入 packetSize

		// 发送数据包
		socket_->write(packet);
		socket_->waitForBytesWritten();
	}
}

void LogSender::sendTextLog(const QString &message, const QString &func_name)
{
	QMutexLocker locker(&gMutexSendStrFunc); // 获取互斥锁
	++log_index_;
	// 设置 func_name 占 40 字节，不足的部分在右侧补空格
	QString paddedFuncName = func_name.leftJustified(40, ' ');
	QString str = QString("[%1][%2]")
		.arg(log_index_)
		.arg(paddedFuncName);
	if (message.length() > 0)	{
		str += message;
	}
	sendTextLog(str);
}

void LogSender::sendTextLog(const QString &message,
	const QString &func_name, const QString &file_name, const int file_line)
{
	QMutexLocker locker(&gMutexSendStrFuncFileLine); // 获取互斥锁
	// 处理 file_name，剔除 \src\ 关键字前面的内容
	QString processed_file_name = file_name;
	int src_index = file_name.indexOf("\\src\\");
	if (src_index != -1) {
		processed_file_name = file_name.mid(src_index + 5);
	}
	++log_index_;
	QString str = QString("[%1][%2][%3:%4]")
		.arg(log_index_)
		.arg(func_name)
		.arg(processed_file_name)
		.arg(file_line);
	if (message.length()>0)	{
		str += message;
	}
	sendTextLog(str);
}

//gLogSender.sendImageLog(pSubWin->grab());
void LogSender::sendImageLog(const QPixmap &pixmap) {
	QMutexLocker locker(&gMutexSendPic); // 获取互斥锁
	if (socket_->state() != QLocalSocket::ConnectedState){
		socket_->connectToServer("MyLogServer");
	}
	// 先发送一个text 标识？图上打标？
	if (socket_->state() == QLocalSocket::ConnectedState) {
		// 检查图像是否有效
		if (pixmap.isNull()) {
			qDebug() << "Pixmap is null!";
			return;
		}
		qDebug() << "Pixmap size:" << pixmap.size(); // 打印图像大小

		// 将 QPixmap 转换为 QByteArray
		QByteArray imageData;
		QBuffer buffer(&imageData);
		buffer.open(QIODevice::WriteOnly);
		pixmap.save(&buffer, "PNG");

		// 构造数据包
		QByteArray packet;
		QDataStream stream(&packet, QIODevice::WriteOnly);
		stream.setVersion(QDataStream::Qt_5_5);

		// 写入数据包长度
		stream << static_cast<quint32>(0); // 预留 packetSize

		// 写入数据包类型
		stream << QString("IMAGE");

		// 写入数据包内容
		stream << imageData;

		stream.device()->seek(0);
		stream << static_cast<quint32>(packet.size()); // 写入 packetSize
		// 发送数据包
		socket_->write(packet);
		socket_->waitForBytesWritten();
	}
}


//end of file
