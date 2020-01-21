#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QTextCursor>
#include <QStringList>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui
{
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected slots:
    void slotOpenClose(void);
    void slotSendData(void);
    void slotAutoRepeat(void);
    void slotReadData(void);
    void slotClearRx(void);

private:
    void updateTxRxCnt(void);

private:
    Ui::Widget *ui;
    QSerialPort *serial;
    QTimer *timer;
    quint32 cntTx;
    quint32 cntRx;
    quint32 cntHexMode;
    quint32 cntCharMode;
};

#endif // WIDGET_H
