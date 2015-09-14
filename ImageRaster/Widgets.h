#pragma once

#include "qlabel.h"
class LabelDelegate :
	public QLabel
{
	Q_OBJECT
	Q_PROPERTY(QString label READ label WRITE setLabel USER true)
public:
	LabelDelegate(QWidget* parent=0) :
		QLabel(parent) { }
	~LabelDelegate(void) { }

public slots:
	QString label() const { return text(); }
	void setLabel(QString l) { setText(l); }
};

#include "qcombobox.h"
class ColorEditor :
	public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(QColor color READ color WRITE setColor USER true NOTIFY colorChanged)

	void populateList();
public:
	ColorEditor(QWidget *widget=0);

	QColor color() const;
	void setColor(QColor c);

public slots:
	void widgetColorChanged(int);

signals:
	void colorChanged(QColor);
};

class FontSizeCombo:
	public QComboBox
{
	Q_OBJECT
	Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize USER true NOTIFY fontSizeChanged)

	void populateList() {
		QStringList items;
		items << "8" << "10" << "11" << "12" << "14" << "16" << "18" << "20"
			<< "24" << "32" << "48" << "60" << "72" << "100";
		addItems(items);
	}
public:
	FontSizeCombo(QWidget *widget=0):
		QComboBox(widget) 
	{ 
		populateList();
		setCurrentIndex(0);
		connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetIndexChanged(int)));
		connect(this, SIGNAL(currentTextChanged(const QString&)), this, SLOT(widgetTextChanged(const QString&)));
	}
	~FontSizeCombo() { }
	int fontSize() const { return this->currentText().toInt(); }
	void setFontSize(int size) {
		int i = findData(size, int(Qt::DisplayRole));
		if (i >= 0) setCurrentIndex(i);
		else setCurrentText(QString::number(size));
	}
public slots:
	void widgetIndexChanged(int) { emit fontSizeChanged(fontSize()); }
	void widgetTextChanged(const QString&) { emit fontSizeChanged(fontSize()); }
signals:
	void fontSizeChanged(int);
};

