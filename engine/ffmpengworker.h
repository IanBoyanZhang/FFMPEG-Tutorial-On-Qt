#ifndef FFMPENGWORKER_H
#define FFMPENGWORKER_H

#include <QObject>
#include <QPixmap>
#include <QList>
#include <QImage>

class FFmpengWorker : public QObject
{
    Q_OBJECT
public:
    static FFmpengWorker*       instance();

    static QList<QPixmap>*      getScreenCaps(QString fileUrl);
    void                        displayToScreen(QString fileUrl);


signals:
    void                        displayScreenUpdate(QPixmap *pixmap);

public slots:
private:
    explicit                    FFmpengWorker(QObject *parent = 0);
    QPixmap*                    m_displayPixmap = NULL;
};

#endif // FFMPENGWORKER_H
