#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QString>
#include <QByteArray>

#include <QList>
#include <QDebug>
#include <QFileDialog>
#include <QPixmap>

#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>
#include <QBuffer>
#include <QImage>
#include <QImageReader>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    /* locate */
    QList<QString> picList;
    int picId;
    int picNum;

    /* network */
    QTcpServer *tcpServer;
    QTcpSocket *curSocket;

    qint64 totalBytes; // 文件总大小
    qint64 recvBytes; // 已经收到的文件大小
    qint64 fileNameSize; // 文件名大小
    QString fileName; // 文件名

    QByteArray inBuffer; // 接收缓冲区

    QFile *localFile;
};
#endif // MAINWINDOW_H
