#ifndef SIGNALSCOLLECT_H
#define SIGNALSCOLLECT_H
#include <QObject>

class SignalsCollect : public QObject
{
    Q_OBJECT
public:
    explicit SignalsCollect(QObject *parent = nullptr);
signals:
    //Brief：radius改变信号
    void radiusChanged();
};

#endif // SIGNALSCOLLECT_H
