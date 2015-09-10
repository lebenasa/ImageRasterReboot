#pragma once
#include "GraphicsItem.h"

class ScaleItem :
	public QAbstractGraphicsShapeItem
{
	QLineF myLine;
	int myPenWidth;
	void calculateTextPos() { myText1->setPos(calculateTextPos1()); myText2->setPos(calculateTextPos2()); }
	virtual QPointF calculateTextPos1();
	virtual QPointF calculateTextPos2();
	SimpleTextItem* myText1, *myText2;
protected:
public:
	ScaleItem(const QLineF& line, QGraphicsItem* parent=0);
	~ScaleItem() { }

	enum { Type = UserType + 19 };
	int type() const { return Type; }

	QLineF line() const { return myLine; }
	void setLine(const QLineF& line) { myLine = line; calculateTextPos(); }

	int penWidth() const { return myPenWidth; }
	void setPenWidth(int width) {
		myPenWidth = width;
		QPen pen1 = pen();
		pen1.setWidth(width);
		setPen(pen1);
	}

	QString text1() const { return myText1->text(); }
	void setText1(const QString& txt) { myText1->setText(txt); calculateTextPos(); }

	QString text2() const { return myText2->text(); }
	void setText2(const QString& txt) { myText2->setText(txt); calculateTextPos(); }

	QFont font() const { return myText1->font(); }
	void setFont(const QFont& f) { 
		myText1->setFont(f);
		myText2->setFont(f);
		calculateTextPos();
	}

	int fontSize() const { return myText1->fontSize(); }
	void setFontSize(int size) { 
		myText1->setFontSize(size);
		myText2->setFontSize(size);
		calculateTextPos();
	}

	bool hasBackground() { return myText1->hasBackground(); }
	void setHasBackground(bool show) { myText1->setHasBackground(show); myText2->setHasBackground(show); }
};

class SimpleScaleItem:
	public ScaleItem
{
	QLineF t1, t2;
public:
	SimpleScaleItem(const QLineF& line, QGraphicsItem* parent=0);
	~SimpleScaleItem() { }

	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget = 0);

	void setLine(const QLineF& line);
};

class SimpleScale:
	public GraphicsItemInterface<SimpleScaleItem, QLineF>
{
public:
	SimpleScale(const QLineF& line, QGraphicsItem* parent=0):
		GraphicsItemInterface<SimpleScaleItem, QLineF>(line, parent) { bg->setText1(""); }
	~SimpleScale() { }

	void setLine(const QLineF& line) {
		SimpleScaleItem::setLine(line);
		bg->setLine(line);
	}
};

class RulerScaleItem:
	public ScaleItem
{
	QVector<QLineF> ticks;
public:
	RulerScaleItem(const QLineF& line, QGraphicsItem* parent=0);
	~RulerScaleItem() { }

	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget = 0);

	void setLine(const QLineF& line);
};

class RulerScale:
	public GraphicsItemInterface<RulerScaleItem, QLineF>
{
public:
	RulerScale(const QLineF& line, QGraphicsItem* parent=0):
		GraphicsItemInterface<RulerScaleItem, QLineF>(line, parent) { bg->setText1(""); }
	~RulerScale() { }

	void setLine(const QLineF& line) {
		RulerScaleItem::setLine(line);
		bg->setLine(line);
	}
};

class BarScale:
	public ScaleItem
{
public:
	BarScale(const QLineF& line, QGraphicsItem* parent=0);
	~BarScale() { }

	QRectF boundingRect() const;
	QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget = 0);

	void setLine(const QLineF& line);
};