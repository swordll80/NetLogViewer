#include "NetLogViewer.h"
#include <QLocalSocket>
#include <QVBoxLayout>
#include <QDataStream>
#include <QSplitter>

NetLogViewer::NetLogViewer(QWidget *parent) : QMainWindow(parent) {
	// 设置窗口
	setWindowTitle("LogViewer Support search by Ctrl+F F3 Shift+F3");
	resize(800, 600);
	//
	textEdit = new QPlainTextEdit(this);
	textEdit->setReadOnly(true);
	imageLabel = new ImageLabel(this);//new QLabel(this);
	imageLabel->setAlignment(Qt::AlignCenter);
	imageLabel->setMinimumSize(400, 350); // 设置最小大小
	imageLabel->setStyleSheet("border: 2px solid blue;");
	// 创建 QSplitter，并设置方向为垂直方向
	QSplitter *splitter = new QSplitter(Qt::Vertical, this);
	splitter->addWidget(textEdit);
	splitter->addWidget(imageLabel);
	setCentralWidget(splitter);
	//
	createActions();
	// 启动本地服务器
	server = new QLocalServer(this);
	if (!server->listen("MyLogServer")) {
		textEdit->appendPlainText("Failed to start server!");
		return;
	}
	connect(server, &QLocalServer::newConnection, this, &NetLogViewer::onNewConnection);
}

void NetLogViewer::onNewConnection() {
	QLocalSocket *socket = server->nextPendingConnection();
	connect(socket, &QLocalSocket::readyRead, this, &NetLogViewer::onReadyRead);
}

void NetLogViewer::onReadyRead() {
	QLocalSocket *socket = qobject_cast<QLocalSocket *>(sender());
	if (!socket) return;

	// 将新数据追加到缓冲区
	buffer.append(socket->readAll());

	// 解析缓冲区中的数据
	while (true) {
		// 检查是否有足够的数据读取数据包长度
		if (buffer.size() < sizeof(quint32)) {
			break; // 数据不足，等待更多数据
		}

		// 读取数据包长度
		QDataStream stream(&buffer, QIODevice::ReadOnly);
		stream.setVersion(QDataStream::Qt_5_5);

		quint32 packetSize;
		stream >> packetSize;

		// 检查是否有足够的数据读取完整的数据包
		if (buffer.size() < packetSize) {
			break; // 数据不足，等待更多数据
		}

		// 读取数据包类型
		QString logType;
		stream >> logType;

		// 根据数据包类型处理数据
		if (logType == "TEXT") {
			//QString logMessage;
			//stream >> logMessage;
			quint32 dataSize;
			stream >> dataSize;
			QByteArray data;
			data.resize(dataSize);
			//stream.skipRawData(sizeof(dataSize));
			stream.readRawData(data.data(), dataSize);
			QString logMessage = QString::fromUtf8(data);
			textEdit->appendPlainText(logMessage);
		}
		else if (logType == "IMAGE") {
			QByteArray imageData;
			stream >> imageData;
			qDebug() << "Received image data size:" << imageData.size();
			QPixmap pixmap;
			if (pixmap.loadFromData(imageData, "PNG")) {
				qDebug() << "Loaded pixmap size:" << pixmap.size();
				imageLabel->setPixmap(pixmap);
				imageLabel->update();
			}
			else {
				textEdit->appendPlainText("Failed to load image from data!");
				qDebug() << "Failed to load image from data!";
			}
		}
		else {
			QString unknownLog = "Unknown log type: " + logType;
			textEdit->appendPlainText(unknownLog);
			qDebug() << unknownLog;
		}

		// 移除已解析的数据
		buffer.remove(0, packetSize);
	}
}
void NetLogViewer::searchText() {
	// 弹出搜索对话框
	QString searchText = QInputDialog::getText(this, tr("Search"), tr("Enter text to search:"));

	if (searchText.isEmpty()) {
		return; // 如果用户未输入内容，直接返回
	}

	// 保存搜索文本
	lastSearchText = searchText;

	// 调用搜索功能
	findText(searchText);
}
void NetLogViewer::findNext() {
	if (lastSearchText.isEmpty()) {
		return; // 如果没有搜索过，直接返回
	}

	QTextCursor cursor = textEdit->textCursor();
	cursor = textEdit->document()->find(lastSearchText, cursor);

	if (cursor.isNull()) {
		QMessageBox::information(this, tr("Search"), tr("No more matches found."));
		return;
	}

	// 高亮显示匹配的文本
	QTextCharFormat highlightFormat;
	highlightFormat.setBackground(Qt::yellow); // 设置高亮背景色

	cursor.mergeCharFormat(highlightFormat);
	textEdit->setTextCursor(cursor);
	textEdit->ensureCursorVisible();
}

void NetLogViewer::findPrevious() {
	if (lastSearchText.isEmpty()) {
		return; // 如果没有搜索过，直接返回
	}

	QTextCursor cursor = textEdit->textCursor();
	QTextDocument::FindFlags options;
	options |= QTextDocument::FindBackward; // 向后查找

	cursor = textEdit->document()->find(lastSearchText, cursor, options);

	if (cursor.isNull()) {
		QMessageBox::information(this, tr("Search"), tr("No more matches found."));
		return;
	}

	// 高亮显示匹配的文本
	QTextCharFormat highlightFormat;
	highlightFormat.setBackground(Qt::yellow); // 设置高亮背景色

	cursor.mergeCharFormat(highlightFormat);
	textEdit->setTextCursor(cursor);
	textEdit->ensureCursorVisible();
}

void NetLogViewer::createActions() {
	// 创建搜索动作
	QAction *searchAction = new QAction(tr("&Search"), this);
	searchAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F)); // 设置快捷键
	connect(searchAction, &QAction::triggered, this, &NetLogViewer::searchText);

	// 创建查找下一个动作
	QAction *findNextAction = new QAction(tr("Find &Next"), this);
	findNextAction->setShortcut(QKeySequence(Qt::Key_F3));
	connect(findNextAction, &QAction::triggered, this, &NetLogViewer::findNext);

	// 创建查找上一个动作
	QAction *findPreviousAction = new QAction(tr("Find &Previous"), this);
	findPreviousAction->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F3));
	connect(findPreviousAction, &QAction::triggered, this, &NetLogViewer::findPrevious);

	// 添加到菜单
	//QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
	//editMenu->addAction(searchAction);
	//editMenu->addAction(findNextAction);
	//editMenu->addAction(findPreviousAction);

	// 添加到工具栏
	//QToolBar *editToolBar = addToolBar(tr("Edit"));
	//editToolBar->addAction(searchAction);
	//editToolBar->addAction(findNextAction);
	//editToolBar->addAction(findPreviousAction);

	// 使快捷键生效
	addAction(searchAction);
	addAction(findNextAction);
	addAction(findPreviousAction);
}

void NetLogViewer::findText(const QString &text) {
	QTextDocument *document = textEdit->document();
	QTextCursor cursor(document);

	// 清除之前的高亮
	QTextCharFormat defaultFormat;
	cursor.setCharFormat(defaultFormat);

	// 设置搜索选项
	QTextDocument::FindFlags options;
	if (caseSensitive) {
		options |= QTextDocument::FindCaseSensitively;
	}
	if (wholeWords) {
		options |= QTextDocument::FindWholeWords;
	}

	// 查找文本
	cursor = document->find(text, cursor, options);

	if (cursor.isNull()) {
		QMessageBox::information(this, tr("Search"), tr("No match found."));
		return;
	}

	// 高亮显示匹配的文本
	QTextCharFormat highlightFormat;
	highlightFormat.setBackground(Qt::yellow); // 设置高亮背景色

	cursor.mergeCharFormat(highlightFormat);
	textEdit->setTextCursor(cursor);
	textEdit->ensureCursorVisible();
}