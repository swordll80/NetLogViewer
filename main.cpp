#include "NetLogViewer.h"
#include <QtWidgets/QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
	NetLogViewer w;
	w.show();
	return a.exec();
}

/*
开发环境：win10 vs2013 qt5.5.1 x86
*/

/*
乱码问题
	发送端main函数设置GBK时，需要确保发送的是UTF-8字符串
	发送端示例：
	Log2Net(QStringLiteral("编辑器"));
	QByteArray data = message.toUtf8();

关键点：
	QString QStringLiteral 为 UTF-16 编码

UTF-16 与 UTF-8 互转
	将 QStringLiteral UTF-16 转换为 UTF-8 编码的 QByteArray：
		QString str = QStringLiteral("你好，世界！");
		QByteArray utf8Data = str.toUtf8(); // 转换为 UTF-8 编码

	从 UTF-8 编码的 QByteArray 创建 QString UTF-16：
		QByteArray utf8Data = "你好，世界！";
		QString str = QString::fromUtf8(utf8Data); // 从 UTF-8 编码创建 QString
*/
