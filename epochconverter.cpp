#include "epochconverter.h"
#include "ui_epochconverter.h"
//#include "strptime.h"

EpochConverter::EpochConverter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EpochConverter)
{
    ui->setupUi(this);

    setFixedSize(geometry().width(),geometry().height());

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(RefreshEpoch()));
    m_timer->start(1000);

    qDebug() << "[DEBUG] sizeof(time_t) = " << sizeof(time_t);

    connect(ui->uiBtnConvertEpoch, SIGNAL(clicked()), this, SLOT(TimestampToDate()));
    connect(ui->uiBtnConvertDate, SIGNAL(clicked()), this, SLOT(DateToTimestamp()));

    //connect(ui->uiSpinBoxDay, SIGNAL(valueChanged(int)), this, SLOT(CheckDay(int)));

    ui->uiBtnConvertEpoch->setEnabled(true);
    ui->uiBtnConvertDate->setEnabled(true);
    ui->uiLCDCurrentEpoch->display((int)time(NULL));
}

EpochConverter::~EpochConverter()
{
    if (m_timer->isActive())
        m_timer->stop();

    if (m_timer)
        delete m_timer;

    delete ui;
}

void EpochConverter::RefreshEpoch()
{
    time_t tNow(time(NULL));
    ui->uiLCDCurrentEpoch->display(static_cast<int>(tNow));

    //ui->uiLineEditCurrentEpoch->setText(QString::number(tNow));
    //qDebug() << "[DEBUG] Current Epoch : = " << tNow;
}

void EpochConverter::TimestampToDate()
{
    struct tm tmGMT;
    struct tm tmLocal;
    char szGMTDate[40];
    char szLocalDate[40];

    memset(szGMTDate, 0, sizeof(szGMTDate));
    memset(szLocalDate, 0, sizeof(szLocalDate));
    memset(&tmGMT, 0, sizeof(tmGMT));
    memset(&tmLocal, 0, sizeof(tmLocal));

    if (!ui->uiLineEditUserEpoch->text().isEmpty())
    {
        bool bOk = false;
        qDebug() << "[DEBUG] TimestampToDate requested.";
        //qDebug() << "[DEBUG] time_t max = " << std::numeric_limits<time_t>::max();
        //strtoul(ui->uiLineEditUserEpoch->text().toStdString().c_str(), nullptr, 10);
        //time_t llEpoch = static_cast<time_t>(ui->uiLineEditUserEpoch->text().toULongLong(&bOk));
        time_t llEpoch;
        if (sizeof(time_t) > 4)
            llEpoch = ui->uiLineEditUserEpoch->text().toLongLong(&bOk);
        else
            llEpoch = ui->uiLineEditUserEpoch->text().toLong(&bOk);

        if (!bOk)
        {
            QMessageBox::critical(this, tr("Error"), tr("The number you entered exceeds the maximum epoch you can represent in your system.\n"
                                                        "The universe will probably disappear before that date (if your system is a 64-bit) !"
                                                        " or your are not ready for 2038 (you have a 32-bit OS)."
                                                        ));
            ui->uiLBEpochToGMTDateString->setText("");
            ui->uiLBEpochToLocalDateString->setText("");
            return;
        }

        qDebug() << "[DEBUG] Epoch = " << llEpoch;

        tmGMT = *gmtime(&llEpoch);
        strftime(szGMTDate, sizeof(szGMTDate), "%a, %d %b %Y %H:%M:%S GMT", &tmGMT);

        tmLocal = *localtime(&llEpoch);
        strftime(szLocalDate, sizeof(szLocalDate), "%d/%m/%Y at %H:%M:%S %Z", &tmLocal);

        ui->uiLBEpochToGMTDateString->setText(szGMTDate);
        ui->uiLBEpochToLocalDateString->setText(szLocalDate);
    }
    else
    {
        qDebug() << "[DEBUG] Timestamp is empty.";
    }
}

void EpochConverter::DateToTimestamp()
{
    qDebug() << "[DEBUG] TimestampToDate requested.";

    struct tm tmDate;
    struct tm tmNow;
    time_t tEpoch = 0;
    time_t tNow = time(NULL);
    tmNow = *localtime(&tNow);

    int iYear = ui->uiSpinBoxYear->value() - 1900;
    int iMonth = ui->uiSpinBoxMonth->value() - 1;
    int iDay = ui->uiSpinBoxDay->value();
    int iHour = ui->uiSpinBoxHour->value();
    int iMinute = ui->uiSpinBoxMinute->value();
    int iSecond = ui->uiSpinBoxSecond->value();
    QString qstrTZ = ui->uiComboBoxTZ->itemText(ui->uiComboBoxTZ->currentIndex()); // itemData returns a QVariant.

    memset(&tmDate, 0, sizeof(tmDate));
    tmDate.tm_year = iYear;
    tmDate.tm_mon = iMonth;
    tmDate.tm_mday = iDay;
    tmDate.tm_hour = iHour;
    tmDate.tm_min = iMinute;
    tmDate.tm_sec = iSecond;
    #ifdef _WINDOWS
    /* In Windows, it is preferable to put the correct GMT offset, as according to the system settings,
     * if we keep it at zero, we might have an incorrect epoch with mktime(). */
    tmDate.tm_isdst = (qstrTZ == "GMT") ? 0 : GetGMTOffset();
    #else
    tmDate.tm_isdst = 0;
    #endif

    #ifdef _WINDOWS
    tEpoch = (qstrTZ == "GMT") ? _mkgmtime(&tmDate) : mktime(&tmDate);
    #else
    tEpoch = (qstrTZ == "GMT") ? timegm(&tmDate) : mktime(&tmDate);
    #endif

    if (tEpoch == -1)
    {
        QMessageBox::critical(this, tr("Error"), tr("Unable to convert the human date to epoch date ! Maybe your system is not 2038 ready !"));
        ui->uiLBDateToEpoch->setText("");
        ui->uiLBDateToEpochMs->setText("");
        ui->uiLBDateToGMTString->setText("");
        ui->uiLBDateString->setText("");
        return;
    }

    ui->uiLBDateToEpoch->setText(QString::number(tEpoch));
    ui->uiLBDateToEpochMs->setText(QString::number(static_cast<qlonglong>(tEpoch)*1000));

    // ...
    struct tm tmGMT;
    struct tm tmLocal;
    char szGMTDate[40];
    char szLocalDate[40];
    memset(szGMTDate, 0, sizeof(szGMTDate));
    memset(szLocalDate, 0, sizeof(szLocalDate));
    memset(&tmGMT, 0, sizeof(tmGMT));
    memset(&tmLocal, 0, sizeof(tmLocal));

    tmGMT = *gmtime(&tEpoch);
    strftime(szGMTDate, sizeof(szGMTDate), "%a, %d %b %Y %H:%M:%S GMT", &tmGMT);

    tmLocal = *localtime(&tEpoch);
    strftime(szLocalDate, sizeof(szLocalDate), "%d/%m/%Y at %H:%M:%S", &tmLocal);

    ui->uiLBDateToGMTString->setText(szGMTDate);
    ui->uiLBDateString->setText(szLocalDate);
}

long EpochConverter::GetGMTOffset()
{
    time_t now = time(NULL);

    struct tm gm = *gmtime(&now);
    time_t gmt = mktime(&gm);

    struct tm loc = *localtime(&now);
    time_t local = mktime(&loc);

    return difftime(local, gmt);
}

/*
void EpochConverter::CheckDay(int iDay)
{
    char szDate[20];
    memset(szDate, 0, 20);

    int iMonth = ui->uiSpinBoxMonth->value();
    int iYear = ui->uiSpinBoxYear->value();

    sprintf_s(szDate, 20, "%d-%d-%d", iDay, iMonth, iYear);

    struct tm tmResult;
    if (strptime(szDate, "%d-%m-%Y", &tmResult))
    {
        qDebug() << "[DEBUG] Requested date : " << szDate;
    }
    else
        QMessageBox::critical(this, tr("Error"), tr("Invalid date !"));

}*/
