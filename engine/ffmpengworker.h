#ifndef FFMPENGWORKER_H
#define FFMPENGWORKER_H

#include <QObject>
#include <QPixmap>
#include <QList>
#include <QImage>
#include <QtConcurrent>

class FFmpengWorker : public QObject
{
    Q_OBJECT
public:
    static FFmpengWorker*       instance();
    explicit                    FFmpengWorker(QObject *parent = 0);


    static QList<QPixmap>*      getScreenCaps(QString fileUrl);
    void                        displayToScreen(QString fileUrl);


signals:
    void                        displayScreenUpdate(QImage);
    void                        displayFinished();

public slots:
private:


//    QPixmap*                    m_displayPixmap = NULL;
};

#endif // FFMPENGWORKER_H
