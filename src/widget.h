#ifndef WIDGET_H
#define WIDGET_H

#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QStringList>
#include <QTextCursor>
#include <QTimer>
#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define TXT_BUFF_SIZE (1024 * 8)

namespace Ui
{
class Widget;
}

class Widget : public QWidget
{
	Q_OBJECT

public:
	explicit Widget(QWidget* parent = 0);
	~Widget();

protected slots:
	void slotOpenClose(void);
	void slotSendData(void);
	void slotAutoRepeat(void);
	void slotReadData(void);
	void slotClearRx(void);
	void slotReflush(void);

private:
	void updateTxRxCnt(void);
	void rebindPorts(void);

private:
	Ui::Widget* ui;
	QSerialPort* serial;
	QTimer* timer;
	quint32 cntTx;
	quint32 cntRx;
	quint32 cntHexMode;
	quint32 cntCharMode;
};

#endif // WIDGET_H
