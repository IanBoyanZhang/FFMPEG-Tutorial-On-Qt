#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "../engine/ffmpengworker.h"
#include <QtConcurrent>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    QLabel *m_label;
    QVBoxLayout *m_layout;
    FFmpengWorker *m_ffmpegWorker;

public slots:
    void onUpdateScreen(QImage img);
    void displayFinished();

private:
};

#endif // WIDGET_H
