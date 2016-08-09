#include "widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Tutorial 01 , making screen caps");
    m_layout = new QVBoxLayout;
    this->setLayout(m_layout);
    QList<QPixmap> * pixmaps = FFmpengWorker::getScreenCaps("/home/tommego/Videos/b.mp4");
    qDebug()<<pixmaps->count();
    if(!pixmaps)
        return;
    foreach(QPixmap it , *pixmaps){
        QLabel *label =new QLabel(this);
        label->setPixmap(it);
        m_layout->addWidget(label);
    }
}

Widget::~Widget()
{

}
