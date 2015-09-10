#include "stdafx.h"
#include "GraphicsItem.h"

//GraphicsArrowItem
GraphicsArrowItem::GraphicsArrowItem(const QLineF& line, QGraphicsItem* parent):
	QGraphicsLineItem(line, parent)
{
	arrowHead = Both;
	heads = compose();
}

std::vector<QPolygonF> GraphicsArrowItem::compose() {
	const qreal pi = 3.14;
	QLineF l = QLineF(line());
	double arrowSize = (l.length() < 200.0) ? 10.0 : l.length()/20.0;
	//double arrowSize = 10.0;
	int dx = l.p2().x() - l.p1().x();
	int dy = l.p2().y() - l.p1().y();
	double angle = ::acos(dx/l.length());
	if (dx >= 0 && dy > 0)
		angle = (pi * 2) - angle;
	else if (dx >= 0 && dy <= 0)
		{ }
	else if (dx < 0 && dy > 0)
		angle = (pi * 2) - angle;
	else if (dx < 0 && dy <= 0)
		{ }
	QPointF P1 = l.p1() + QPointF(::sin(angle+pi/3.0)*arrowSize, ::cos(angle+pi/3.0)*arrowSize);
	QPointF P2 = l.p1() + QPointF(::sin(angle+pi-pi/3.0)*arrowSize, ::cos(angle+pi-pi/3.0)*arrowSize);
	QPointF P3 = l.p2() + QPointF(::sin(angle-pi/3.0)*arrowSize, ::cos(angle-pi/3.0)*arrowSize);
	QPointF P4 = l.p2() + QPointF(::sin(angle+pi+pi/3.0)*arrowSize, ::cos(angle+pi+pi/3.0)*arrowSize);

	std::vector<QPolygonF> cont;
	//double off = 0.5*arrowSize*1.732;
	//QPointF offset(off, off);
	if (arrowHead == Start) {
		QPolygonF polybuilder;
		polybuilder.push_back(P1);
		polybuilder.push_back(l.p1());
		polybuilder.push_back(P2);
		polybuilder.push_back(P1);
		cont.push_back(polybuilder);
		//QLineF newline(line());
		//newline.setP1(newline.p1()-offset);
	}
	else if (arrowHead == End) {
		QPolygonF polybuilder;
		polybuilder.push_back(P3);
		polybuilder.push_back(l.p2());
		polybuilder.push_back(P4);
		polybuilder.push_back(P3);
		cont.push_back(polybuilder);
	}
	else if (arrowHead == Both) {
		QPolygonF polybuilder;
		polybuilder.push_back(P1);
		polybuilder.push_back(l.p1());
		polybuilder.push_back(P2);
		polybuilder.push_back(P1);
		QPolygonF polybuilder2;
		polybuilder2.push_back(P3);
		polybuilder2.push_back(l.p2());
		polybuilder2.push_back(P4);
		polybuilder2.push_back(P3);
		cont.push_back(polybuilder);
		cont.push_back(polybuilder2);
	}
	return cont;
}

void GraphicsArrowItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
		QWidget *widget) {
	painter->setPen(pen());
	double mod = 10 / line().length();
	QPointF offset = (line().p2()-line().p1())*mod;
	painter->drawLine(line().p1()+offset, line().p2()-offset);
	QPen p = pen();
	p.setJoinStyle(Qt::MiterJoin);
	for (auto l : heads) {
		QGraphicsPolygonItem arrow(l);
		arrow.setPen(p);
		arrow.setBrush(pen().color());
		arrow.paint(painter, option, widget);
	}
	if (isSelected()) {
		QPen dash(Qt::black, 1, Qt::DashLine);
		painter->setPen(dash);
		painter->drawRect(boundingRect());
	}
}

QPainterPath GraphicsArrowItem::shape() const {
	QPainterPath path = QGraphicsLineItem::shape();
	for (auto l : heads)
		path.addPath(QGraphicsPolygonItem(l).shape());
	return path;
}

QRectF GraphicsArrowItem::boundingRect() const {
	QLineF l = line();
	double arrowSize = (l.length() < 40.0) ? 2.0 : l.length()/20.0;
	double extra = (pen().width() + ::sin(3.14/3.0) * arrowSize) / 2.0;
	QPointF p1 = l.p1();
	QPointF p2 = l.p2();
	return QRectF(p1, QSizeF(p2.x() - p1.x(), p2.y() - p1.y())).normalized().adjusted(-extra, -extra, extra, extra);
}

void GraphicsArrowItem::setLine(const QLineF& line) {
	QGraphicsLineItem::setLine(line);
	heads.clear();
	heads = compose();
}

void GraphicsArrowItem::setArrowhead(ArrowHead head) {
	arrowHead = head;
	heads.clear();
	heads = compose();
}

//class GraphicsMultiLineItem
GraphicsMultiLineItem::GraphicsMultiLineItem(const QPolygonF& r, QGraphicsItem* parent):
	QAbstractGraphicsShapeItem(parent)
{
	myPoly = r;
	QPointF last;
	for (auto point : myPoly) {
		if (point == *myPoly.begin()) {
			last = point;
			continue;
		}
		QLineF line = QLineF(last, point);
		lines.append(line);
		last = point;
	}
}

GraphicsMultiLineItem::~GraphicsMultiLineItem() {
}

QRectF GraphicsMultiLineItem::boundingRect() const {
	QGraphicsPolygonItem dummy(myPoly);
	int extra = 5;
	QRectF rect = dummy.boundingRect();
	return rect.normalized().adjusted(-extra, -extra, extra, extra);
}

QPainterPath GraphicsMultiLineItem::shape() const {
	QPainterPath path;
	path.addPolygon(myPoly);
	return path;
}

void GraphicsMultiLineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	QWidget *widget)
{
	painter->setPen(pen());
	for (auto line : lines)
		painter->drawLine(line);
	if (isSelected()) {
		QPen dash(Qt::black, 1, Qt::DashLine);
		painter->setPen(dash);
		painter->drawRect(boundingRect());
	}
}

void GraphicsMultiLineItem::setPolygon(const QPolygonF& poly) {
	myPoly = poly;
	lines.clear();
	QPointF last;
	for (auto point : myPoly) {
		if (point == *myPoly.begin()) {
			last = point;
			continue;
		}
		QLineF line = QLineF(last, point);
		lines.append(line);
		last = point;
	}
}

//ArrowItem
ArrowItem::ArrowItem(const QLineF& line, QGraphicsItem* parent):
	GraphicsItemInterface<GraphicsArrowItem, QLineF>(line, parent)
{
	GraphicsArrowItem::setArrowhead(Start);
}

//ArrowItem::ArrowItem(const ArrowItem& c):
//	GraphicsItemInterface<GraphicsArrowItem, QLineF>(c.line(), c.parentItem())
//{
//	setArrowhead(Start);
//}

void ArrowItem::setLine(const QLineF& line) {
	GraphicsArrowItem::setLine(line);
	bg->setLine(line);
}

void ArrowItem::setArrowhead(ArrowHead head) {
	GraphicsArrowItem::setArrowhead(head);
	bg->setArrowhead(head);
}

//RectItem
RectItem::RectItem(const QRectF& rect, QGraphicsItem* parent):
	GraphicsItemInterface<QGraphicsRectItem, QRectF>(rect, parent)
{
}

//RectItem::RectItem(const RectItem& c):
//	GraphicsItemInterface<QGraphicsRectItem, QRectF>(c.rect(), c.parentItem())
//{
//}

void RectItem::setRect(const QRectF& rect) {
	QGraphicsRectItem::setRect(rect);
	bg->setRect(rect);
}

//EllipseItem
EllipseItem::EllipseItem(const QRectF& rect, QGraphicsItem* parent):
	GraphicsItemInterface<QGraphicsEllipseItem, QRectF>(rect, parent)
{
}

//EllipseItem::EllipseItem(const EllipseItem& c):
//	GraphicsItemInterface<QGraphicsEllipseItem, QRectF>(c.rect(), c.parentItem())
//{
//}

void EllipseItem::setRect(const QRectF& rect) {
	QGraphicsEllipseItem::setRect(rect);
	bg->setRect(rect);
}

//PolygonItem
PolygonItem::PolygonItem(const QPolygonF& poly, QGraphicsItem* parent):
	GraphicsItemInterface<QGraphicsPolygonItem, QPolygonF>(poly, parent)
{
}

//PolygonItem::PolygonItem(const PolygonItem& c):
//	GraphicsItemInterface<QGraphicsPolygonItem, QPolygonF>(c.polygon(), c.parentItem())
//{
//}

void PolygonItem::setPolygon(const QPolygonF& poly) {
	QGraphicsPolygonItem::setPolygon(poly);
	bg->setPolygon(poly);
}

//MultiLineItem
MultiLineItem::MultiLineItem(const QPolygonF& poly, QGraphicsItem* parent):
	GraphicsItemInterface<GraphicsMultiLineItem, QPolygonF>(poly, parent)
{
}

void MultiLineItem::setPolygon(const QPolygonF& poly) {
	GraphicsMultiLineItem::setPolygon(poly);
	bg->setPolygon(poly);
}

//SimpleTextItem
SimpleTextItem::SimpleTextItem(const QString& text, QGraphicsItem* parent):
	TextItemInterface<QGraphicsSimpleTextItem>(text, parent)
{
}

//SimpleTextItem::SimpleTextItem(const SimpleTextItem& c):
//	TextItemInterface(c.text(), c.parentItem())
//{
//}

//RichTextItem
RichTextItem::RichTextItem(const QString& text, QGraphicsItem* parent):
	QGraphicsTextItem(text, parent)
{
}