#include "widget.h"

#include "ui_widget.h"

const char HEXNUM[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

Widget::Widget(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::Widget)
{
	ui->setupUi(this);
	serial = nullptr;
	cntTx = 0;
	cntRx = 0;
	cntHexMode = 0;
	cntCharMode = 0;

	ui->textEditRx->setReadOnly(true);
	ui->textEditRx->setAcceptRichText(false);
	ui->textEditTx->setAcceptRichText(false);

	rebindPorts();

	ui->lineEditBaudRate->setText("115200");

	ui->combDataBits->addItem("5");
	ui->combDataBits->addItem("6");
	ui->combDataBits->addItem("7");
	ui->combDataBits->addItem("8");
	ui->combDataBits->setCurrentIndex(3);

	ui->combParity->addItem("No");
	ui->combParity->addItem("Even");
	ui->combParity->addItem("Odd");

	ui->combStopBits->addItem("1");
	ui->combStopBits->addItem("2");

	ui->lineEditMs->setText("100");

	ui->combWrapCols->addItem("16");
	ui->combWrapCols->addItem("32");
	ui->combWrapCols->addItem("64");
	ui->combWrapCols->addItem("128");
	ui->combWrapCols->addItem("256");
	ui->combWrapCols->setCurrentIndex(2);

	updateTxRxCnt();

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(slotSendData()));
}

Widget::~Widget()
{
	delete ui;
}

void Widget::slotOpenClose(void)
{
	QMessageBox msgBox;

	if (timer->isActive())
	{
		timer->stop();
	}
	if (ui->chkAutoRepeat->isChecked())
	{
		ui->chkAutoRepeat->setChecked(false);
	}

	if (serial == nullptr)
	{
		serial = new QSerialPort();
		serial->setPortName(ui->combPort->currentText());
		if (!serial->open(QIODevice::ReadWrite))
		{
			msgBox.setText("Could not open this serial port.");
			goto _err;
		}
		bool ret = serial->setBaudRate(ui->lineEditBaudRate->text().toInt());
		if (!ret)
		{
			serial->close();
			msgBox.setText("Could not setup baudrate.");
			goto _err;
		}

		serial->setDataBits((QSerialPort::DataBits)(ui->combStopBits->currentText().toInt()));
		serial->setParity((QSerialPort::Parity)(ui->combParity->currentText().toInt()));
		serial->setStopBits((QSerialPort::StopBits)(ui->combStopBits->currentText().toInt()));
		serial->setFlowControl(QSerialPort::NoFlowControl);

		connect(serial, SIGNAL(readyRead()), this, SLOT(slotReadData()));

		ui->btnOpenClose->setText("Close");
		return;
	}

	disconnect(serial, SIGNAL(readyRead()), this, SLOT(slotReadData()));
	if (serial->isOpen())
	{
		serial->close();
	}
	delete serial;
	serial = nullptr;

	ui->btnOpenClose->setText("Open");

	rebindPorts();

	return;

_err:
	if (serial != nullptr)
	{
		delete serial;
		serial = nullptr;
	}
	msgBox.exec();
	return;
}

void Widget::slotSendData(void)
{
	if (serial == nullptr)
	{
		QMessageBox msgBox;
		msgBox.setText("You must open a serial port first.");
		msgBox.exec();
		return;
	}
	if (ui->chkHexMode->isChecked())
	{
		QByteArray buffTx;
		QString buff = ui->textEditTx->toPlainText().toUtf8();
		QStringList buffList = buff.split(" ");
		for (int i = 0; i < buffList.size(); i++)
		{
			quint32 hex = 0;
			sscanf(buffList.at(i).toStdString().data(), "%02x", &hex);
			hex &= 0xff;
			buffTx.append((char)hex);
		}

		if (!buffTx.isEmpty())
		{
			cntTx += serial->write(buffTx);
			buffTx.clear();
		}
	}
	else
	{
		QByteArray ba = ui->textEditTx->toPlainText().toUtf8();
		QString text = ui->textEditTx->toPlainText();
		text = text.replace("\n", "\r\n");
		cntTx += serial->write(text.toUtf8());
	}
	updateTxRxCnt();
}

void Widget::slotAutoRepeat(void)
{
	if (serial == nullptr)
	{
		ui->chkAutoRepeat->setChecked(false);
		QMessageBox msgBox;
		msgBox.setText("You must open a serial port first.");
		msgBox.exec();
		return;
	}

	if (ui->chkAutoRepeat->isChecked())
	{
		timer->start(ui->lineEditMs->text().toInt());
		return;
	}

	if (timer->isActive())
	{
		timer->stop();
	}
}

void Widget::slotReadData(void)
{
	if (serial == nullptr)
	{
		return;
	}

	QByteArray buff;
	buff = serial->readAll();
	if (!buff.isEmpty())
	{
		QString strRx;
		int wrapCols = ui->combWrapCols->currentText().toInt();
		if (ui->chkHexMode->isChecked())
		{
			for (int i = 0; i < buff.size(); i++)
			{
				quint8 ch = buff.at(i);
				strRx.append(HEXNUM[(ch / 16) & 0xf]);
				strRx.append(HEXNUM[(ch % 16) & 0xf]);
				cntHexMode++;
				if (cntHexMode % wrapCols == 0)
				{
					strRx.append("\n");
				}
				else
				{
					strRx.append(" ");
				}
			}
		}
		else
		{
			for (int i = 0; i < buff.size(); i++)
			{
				if (buff.at(i) == '\n' || buff.at(i) == '\r')
				{
					cntCharMode = 0;
				}
				strRx.append(buff.at(i));
				cntCharMode++;
				if (cntCharMode % wrapCols == 0)
				{
					strRx.append("\n");
				}
			}
		}

		ui->textEditRx->insertPlainText(strRx);
		ui->textEditRx->moveCursor(QTextCursor::End);
		cntRx += buff.size();
		updateTxRxCnt();
	}
	buff.clear();

	if (ui->chkAutoClear->isChecked())
	{
		QString txt = ui->textEditRx->toPlainText();
		
		if (txt.size() > TXT_BUFF_SIZE)
		{
			QString res = txt.right(TXT_BUFF_SIZE);
			ui->textEditRx->setText(res);
			ui->textEditRx->moveCursor(QTextCursor::End);
		}
	}
}

void Widget::slotClearRx(void)
{
	cntTx = 0;
	cntRx = 0;
	updateTxRxCnt();
	ui->textEditRx->clear();
}

void Widget::slotReflush(void)
{
	rebindPorts();
}

void Widget::updateTxRxCnt(void)
{
	QString st;
	st.append("[Tx: ");
	st.append(QString::number(cntTx));
	st.append("] / [Rx: ");
	st.append(QString::number(cntRx));
	st.append("]");
	ui->labelTxRxCnt->setText(st);
}

void Widget::rebindPorts(void)
{
	ui->combPort->clear();
	QList<QSerialPortInfo> sps = QSerialPortInfo::availablePorts();
	for (int i = 0; i < sps.size(); i++)
	{
		QSerialPortInfo spInfo = sps.at(i);
		ui->combPort->addItem(spInfo.portName());
	}
}