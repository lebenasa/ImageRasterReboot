#pragma once
#include "qgraphicsitem.h"
#include "qabstractitemmodel.h"

enum ArrowHead {
	Start, End, Both
};

//Interfaces:
template<typename T, typename base_shape>
class GraphicsItemInterface:
	public T
{
	bool m_isMarker;
	int penwidth;
protected:
	T* bg;
public:
	GraphicsItemInterface(const base_shape& shape, QGraphicsItem* parent=0):
		T(shape, parent)
	{
		bg = new T(shape, this);
		bg->setFlag(QGraphicsItem::ItemStacksBehindParent);
		this->setFlag(QGraphicsItem::ItemIsSelectable);
		m_isMarker = false;
		QPen pen1 = this->pen();
		QPen pen2 = bg->pen();
		pen1.setJoinStyle(Qt::MiterJoin);
		pen2.setJoinStyle(Qt::MiterJoin);
		this->setPen(pen1);
		bg->setPen(pen2);
	}
	~GraphicsItemInterface(void) {}

	void setPenColor(const QColor& shape, const QColor& outline) {
		QPen pen1 = this->pen();
		QPen pen2 = bg->pen();
		pen1.setColor(shape);
		pen2.setColor(outline);
		this->setPen(pen1);
		bg->setPen(pen2);
	}
	void setColor1(const QColor& shape_color) {
		setPenColor(shape_color, color1());
	}
	QColor color1() const {
		return this->pen().color();
	}
	void setColor2(const QColor& outline_color) {
		setPenColor(color1(), outline_color);
	}
	QColor color2() const {
		return bg->pen().color();
	}
	void setPenWidth(int width) {
		penwidth = width;
		QPen pen1 = this->pen();
		QPen pen2 = bg->pen();
		pen1.setWidth(width);
		pen2.setWidth(width+1);
		this->setPen(pen1);
		bg->setPen(pen2);
	}
	int penWidth() const {
		return penwidth;
	}

	void setIsMarker(bool mark) {
		m_isMarker = mark;
	}
	bool isMarker() const {
		return m_isMarker;
	}
};

template<typename T>
class TextItemInterface:
	public T
{
	QGraphicsRectItem* frame;
	int mySize;
public:
	TextItemInterface(const QString& text, QGraphicsItem* parent=0):
		T(text, parent)
	{
		frame = new QGraphicsRectItem(this->boundingRect(), this);
		frame->setPen(Qt::NoPen);
		frame->setBrush(QBrush(QColor(255, 255, 255, 128)));
		frame->setFlag(QGraphicsItem::ItemStacksBehindParent);
	}
	~TextItemInterface() {}
	void setText(const QString& text) {
		T::setText(text);
		frame->setRect(this->boundingRect());
	}
	void setFont(const QFont& font) {
		if (mySize <= 0)
			this->setVisible(false);
		else 
			if (!this->isVisible())
				this->setVisible(true);
		QFont f = font;
		f.setPointSize(mySize > 0 ? mySize : 8);
		T::setFont(f);
		frame->setRect(boundingRect());
	}
	void setFontSize(int size) {
		mySize = size;
		QFont f = this->font();
		f.setPointSize(size > 0 ? size : 8);
		setFont(f);
		if (size <= 0)
			setVisible(false);
		else
			setVisible(true);
	}
	int fontSize() const {
		return mySize;
	}
	void setHasBackground(bool show) {
		frame->setVisible(show);
	}
	bool hasBackground() const {
		return frame->isVisible();
	}
};

//Base Primitives:
class GraphicsArrowItem:
	public QGraphicsLineItem
{
	std::vector<QPolygonF> heads;
	std::vector<QPolygonF> compose();
	ArrowHead arrowHead;
public:
	GraphicsArrowItem(const QLineF& line, QGraphicsItem* parent=0);
	~GraphicsArrowItem() {}

	void setLine(const QLineF& line);
	void setArrowhead(ArrowHead head);

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget = 0);
	QPainterPath shape() const;
};

class GraphicsMultiLineItem:
	public QAbstractGraphicsShapeItem
{
	QPolygonF myPoly;
	QVector<QLineF> lines;
public:
	GraphicsMultiLineItem(const QPolygonF& r, QGraphicsItem* parent=0);
	~GraphicsMultiLineItem();

	QRectF boundingRect() const;

	QPainterPath shape() const;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget = 0);

	void setPolygon(const QPolygonF& poly);
	QPolygonF polygon() const { return myPoly; }
};

//Basic items:
class ArrowItem:
	public GraphicsItemInterface<GraphicsArrowItem, QLineF>
{
public:
	ArrowItem(const QLineF& line, QGraphicsItem* parent=0);
	//ArrowItem(const ArrowItem& c);
	~ArrowItem() {}

	enum  { Type = UserType + 1 };
	int type() const { return Type; }

	void setLine(const QLineF& line);
	void setArrowhead(ArrowHead head);
};

class RectItem:
	public GraphicsItemInterface<QGraphicsRectItem, QRectF>
{
public:
	RectItem(const QRectF& rect, QGraphicsItem* parent=0);
	//RectItem(const RectItem& c);
	~RectItem() {}

	enum  { Type = UserType + 2 };
	int type() const { return Type; }

	void setRect(const QRectF& rect);
};

class EllipseItem:
	public GraphicsItemInterface<QGraphicsEllipseItem, QRectF>
{
public:
	EllipseItem(const QRectF& rect, QGraphicsItem* parent=0);
	//EllipseItem(const EllipseItem& c);
	~EllipseItem() {}

	enum  { Type = UserType + 3 };
	int type() const { return Type; }

	void setRect(const QRectF& rect);
};

class PolygonItem:
	public GraphicsItemInterface<QGraphicsPolygonItem, QPolygonF>
{
public:
	PolygonItem(const QPolygonF& polygon, QGraphicsItem* parent=0);
	//PolygonItem(const PolygonItem& c);
	~PolygonItem() {}

	enum  { Type = UserType + 4 };
	int type() const { return Type; }

	void setPolygon(const QPolygonF& polygon);
};

class MultiLineItem:
	public GraphicsItemInterface<GraphicsMultiLineItem, QPolygonF>
{
public:
	MultiLineItem(const QPolygonF& polygon, QGraphicsItem* parent=0);
	//PolygonItem(const PolygonItem& c);
	~MultiLineItem() {}

	enum  { Type = UserType + 5 };
	int type() const { return Type; }

	void setPolygon(const QPolygonF& polygon);
};

//Texts:
class SimpleTextItem:
	public TextItemInterface<QGraphicsSimpleTextItem>
{
public:
	SimpleTextItem(const QString& text, QGraphicsItem* parent=0);
	//SimpleTextItem(const SimpleTextItem& c);
	~SimpleTextItem() {}

	enum  { Type = UserType + 6 };
	int type() const { return Type; }
};

#include <functional>
class RichTextItem:
	public QGraphicsTextItem
{
	int mySize;
	std::function<void(QKeyEvent* event)> keyPressFunc;
protected:
	void keyReleaseEvent(QKeyEvent* event) { 
		QGraphicsTextItem::keyReleaseEvent(event);
		keyPressFunc(event);
	}
public:
	RichTextItem(const QString& text, QGraphicsItem* parent=0);
	~RichTextItem() {}

	enum  { Type = UserType + 7 };
	int type() const { return Type; }

	void setFontSize(int size) {
		mySize = size;
		QFont f = this->font();
		f.setPointSize(size);
		setFont(f);
	}
	int fontSize() const {
		return mySize;
	}

	void setKeyPressFunc(std::function<void(QKeyEvent* event)> Func) {
		keyPressFunc = Func;
	}
};