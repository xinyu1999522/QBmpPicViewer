#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("BMP Viewer");
    ui->picLabel->setMinimumSize(600 , 400);
    ui->picLabel->setStyleSheet("QLabel{background-color:white; color:black}");
    ui->picLabel->setWordWrap(true);
    ui->picLabel->setFont(QFont("宋体", 18, QFont::Bold, true));
    ui->picLabel->setAlignment(Qt::AlignCenter);

    this->picId = 0;
    this->picNum = 0;

    connect(ui->selectButton , &QAbstractButton::clicked ,
            [=]() {
        this->picId = 0;

        // QStringList files = QFileDialog::getOpenFileNames(... ...);
        this->picList = QFileDialog::getOpenFileNames(this , "选择图片" , "D:/QT/QtProject/QtImage/bmpPic");
        if(this->picNum <= 0) {
            this->picNum = this->picList.size() - 1;
        }

        QString picPath = "";
        picPath = this->picList.at(this->picId);

        if(picPath.contains(".BMP") || picPath.contains(".bmp")) {
            ui->picLabel->setPixmap(QPixmap(picPath));
            // to fill the available space
            ui->picLabel->setScaledContents(true);
        }
    }
            );

    this->totalBytes = 0;
    this->recvBytes = 0;
    this->fileNameSize = 0;
    this->fileName = "";

    tcpServer = new QTcpServer(this);
    if(!tcpServer->listen(QHostAddress::Any , 6666)) {
        qDebug() << tcpServer->errorString();
        close();
    }

    connect(tcpServer , &QTcpServer::newConnection ,
            [=]() {
        inBuffer.clear();
        inBuffer.resize(0);

        curSocket = tcpServer->nextPendingConnection();
        ui->statusLabel->setText(tr("%1:%2").arg(curSocket->peerAddress().toString().split("::ffff:")[1]).arg(curSocket->peerPort()));

        connect(curSocket , &QTcpSocket::readyRead ,
                [=]() {
            QDataStream in(curSocket);

            if(recvBytes <= sizeof(qint64) * 2) {
//                qDebug() << curSocket->bytesAvailable();
                if((curSocket->bytesAvailable() >= sizeof(qint64) * 2) && (fileNameSize == 0)) {
                    in >> totalBytes >> fileNameSize;
//                    qDebug() << "FileNameSize : " << fileNameSize;
                    recvBytes += sizeof(qint64) * 2;
                }

//                qDebug() << curSocket->bytesAvailable();
                if((curSocket->bytesAvailable() >= fileNameSize) && (fileNameSize != 0)) {
                    in >> fileName;
                    ui->statusLabel->setText(tr("接收图片 %1 ...").arg(fileName));
                    recvBytes += fileNameSize;
                    ui->statusLabel->setText(fileName);
                    localFile = new QFile(fileName);
                    if(!localFile->open(QFile::WriteOnly)) {
                        qDebug() << "Warning : Open File Error !";
                        return ;
                    }
                } else {
                    qDebug() << "Warning : Receive PicFileName Error !";
                    return ;
                }
            }
            if(recvBytes < totalBytes) {
                recvBytes += curSocket->bytesAvailable();
                inBuffer += curSocket->readAll();
            }

            ui->progressBar->setMaximum(totalBytes);
            ui->progressBar->setValue(recvBytes);

            if(recvBytes == totalBytes) {
                QBuffer buffer(&inBuffer);
                buffer.open(QIODevice::ReadOnly);
                QImageReader reader(&buffer , "BMP");
                QImage image = reader.read();
                if(!image.isNull()) {
                    image = image.scaled(ui->picLabel->size());
                    ui->picLabel->setPixmap(QPixmap::fromImage(image));
                }
                localFile->write(inBuffer);
                localFile->close();
                inBuffer.clear();
                inBuffer.resize(0);

                this->totalBytes = 0;
                this->recvBytes = 0;
                this->fileNameSize = 0;
                this->fileName = "";
                ui->statusLabel->setText((tr("接收文件 %1 成功 ！ ").arg(fileName)));
            }
        }
                );
    }
            );
}

MainWindow::~MainWindow()
{
    delete ui;
}
