#pragma once

#include <QtWidgets/QMainWindow>
#include <QLocalServer>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPixmap>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPoint>
#include <QPainter>
#include <QMenuBar>
#include <QToolBar>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextCursor>
#include <QTextDocument>

//支持鼠标放大、缩小、拖拽图片的Label，右键恢复
class ImageLabel : public QLabel {
	Q_OBJECT

public:
	explicit ImageLabel(QWidget *parent = nullptr) : QLabel(parent), scaleFactor(1.0), isDragging(false) {
		setAlignment(Qt::AlignCenter);
		setMouseTracking(true); // 启用鼠标跟踪
	}

	void setPixmap(const QPixmap &pixmap) {
		originalPixmap = pixmap;
		QLabel::setPixmap(pixmap);
		update(); // 触发重绘
	}

protected:
	void wheelEvent(QWheelEvent *event) override {
		if (event->angleDelta().y() > 0) {
			scaleFactor *= 1.1; // 放大
		}
		else {
			scaleFactor /= 1.1; // 缩小
		}
		update();
	}

	void mousePressEvent(QMouseEvent *event) override {
		if (event->button() == Qt::LeftButton) {
			isDragging = true;
			dragStartPosition = event->pos(); // 记录拖动起始位置
		}
		else if (event->button() == Qt::RightButton) {
			// 右键点击：恢复原图并填充显示
			resetImage();
		}
	}

	void mouseMoveEvent(QMouseEvent *event) override {
		if (isDragging) {
			QPoint delta = event->pos() - dragStartPosition;
			dragStartPosition = event->pos();
			offset += delta;
			update();
		}
	}

	void mouseReleaseEvent(QMouseEvent *event) override {
		if (event->button() == Qt::LeftButton) {
			isDragging = false;
		}
	}

	void paintEvent(QPaintEvent *event) override {
		//QLabel::paintEvent(event); // 调用基类的 paintEvent
		if (originalPixmap.isNull()) return;
		QPainter painter(this);
		painter.fillRect(rect(), Qt::white);
		QSize scaledSize = originalPixmap.size() * scaleFactor;
		QRect drawRect(offset, scaledSize);
		painter.drawPixmap(drawRect, originalPixmap);
	}

private:
	void resetImage() {
		scaleFactor = 1.0;
		offset = QPoint(0, 0);
		update();
	}

	QPixmap originalPixmap; // 原始图像
	double scaleFactor;      // 缩放比例
	bool isDragging;         // 是否正在拖动
	QPoint offset;           // 图像偏移量
	QPoint dragStartPosition; // 拖动起始位置
};

//接受显示网络文本日志和图像日志
//方便分析qt程序源码与界面的关系、执行流程
class NetLogViewer : public QMainWindow {
	Q_OBJECT
public:
	NetLogViewer(QWidget *parent = nullptr);

private slots:
	void onNewConnection();
	void onReadyRead();
	void searchText();
	void findNext();
	void findPrevious();

private:
	void createActions();
	void findText(const QString &text);
private:
	QLocalServer *server;
	QPlainTextEdit *textEdit;
	QString lastSearchText;
	bool caseSensitive = false; // 是否区分大小写
	bool wholeWords = false;    // 是否全词匹配
	ImageLabel *imageLabel; //QLabel
	QByteArray buffer; // 用于存储未完整接收的数据
};