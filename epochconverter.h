#ifndef EPOCHCONVERTER_H
#define EPOCHCONVERTER_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QThread>
#include <ctime>
#include <cstdlib>
#include <sstream>

namespace Ui {
class EpochConverter;
}

class EpochConverter : public QWidget
{
    Q_OBJECT

public:
    explicit EpochConverter(QWidget* parent = 0);
    ~EpochConverter();

public slots:
    void RefreshEpoch();
    void TimestampToDate();
    void DateToTimestamp();
    //void CheckDay(int);


private:
    // Helpers...
    static long GetGMTOffset();

    Ui::EpochConverter* ui;
    QTimer* m_timer;

};

#endif // EPOCHCONVERTER_H
