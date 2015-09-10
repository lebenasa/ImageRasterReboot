#include "stdafx.h"
#include "ScaleItem.h"

//ScaleItem
ScaleItem::ScaleItem(const QLineF& line, QGraphicsItem* parent):
	QAbstractGraphicsShapeItem(parent), myLine(line), 
	myPenWidth(5)
{
	myText1 = new SimpleTextItem("", this);
	myText2 = new SimpleTextItem("", this);
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

QPointF ScaleItem::calculateTextPos1() {
	QLineF l = line();
	if (l.dx() == 0.0)
		return l.p2() + QPointF(15, -1.0*myText1->boundingRect().height()/2);
	else
		return l.p1() + QPointF(-1.0*myText1->boundingRect().width()/2, 15);
	return QPointF();
}

QPointF ScaleItem::calculateTextPos2() {
	QLineF l = line();
	if (l.dx() == 0.0)
		return l.p1() + QPointF(15, -1.0*myText2->boundingRect().height()/2);
	else
		return l.p2() + QPointF(-1.0*myText2->boundingRect().width()/2, 15);
	return QPointF();
}

//SimpleScale
SimpleScaleItem::SimpleScaleItem(const QLineF& line, QGraphicsItem* parent):
	ScaleItem(line, parent)
{
	setLine(line);
	setPen(QPen(Qt::black, 5));
	setText1("0");
}

QRectF SimpleScaleItem::boundingRect() const {
	QLineF l = line();
	double arrowSize = 5.0;
	double extra = (pen().width() + ::sin(3.14/3.0) * arrowSize) / 2.0;
	QPointF p1 = l.p1();
	QPointF p2 = l.p2();
	return QRectF(p1, QSizeF(p2.x() - p1.x(), p2.y() - p1.y())).normalized().adjusted(-extra, -extra, extra, extra);
}

QPainterPath SimpleScaleItem::shape() const {
	QPainterPath path;
	path.addPath(QGraphicsLineItem(line()).shape());
	path.addPath(QGraphicsLineItem(t1).shape());
	path.addPath(QGraphicsLineItem(t2).shape());
	return path;
}

void SimpleScaleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	QWidget *widget) 
{
	painter->setPen(pen());
	painter->drawLine(line());
	painter->drawLine(t1);
	painter->drawLine(t2);
	if (isSelected()) {
		QPen dash(Qt::black, 1, Qt::DashLine);
		painter->setPen(dash);
		painter->drawRect(boundingRect());
	}
}

void SimpleScaleItem::setLine(const QLineF& l) {
	QLineF line = QLineF(mapFromScene(l.p1()), mapFromScene(l.p2()));
	int size = 5;
	if (line.dx() == 0.0) {
		//vertical line
		t1 = QLineF(line.p1() + QPointF(-size, 0), line.p1() + QPointF(size, 0));
		t2 = QLineF(line.p2() + QPointF(-size, 0), line.p2() + QPointF(size, 0));
	}
	else {
		t1 = QLineF(line.p1() + QPointF(0, -size), line.p1() + QPointF(0, size));
		t2 = QLineF(line.p2() + QPointF(0, -size), line.p2() + QPointF(0, size));
	}
	ScaleItem::setLine(line);
}

//RulerScale
RulerScaleItem::RulerScaleItem(const QLineF& line, QGraphicsItem* parent):
	ScaleItem(line, parent)
{
	setLine(line);
	setPen(QPen(Qt::black, 5));
	setText1("0");
}

QRectF RulerScaleItem::boundingRect() const {
	QLineF l = line();
	double arrowSize = 6.0;
	double extra = (pen().width() + ::sin(3.14/3.0) * arrowSize) / 2.0;
	QPointF p1 = l.p1();
	QPointF p2 = l.p2();
	return QRectF(p1, QSizeF(p2.x() - p1.x(), p2.y() - p1.y())).normalized().adjusted(-extra, -extra, extra, extra);
}

QPainterPath RulerScaleItem::shape() const {
	QPainterPath path;
	path.addPath(QGraphicsLineItem(line()).shape());
	for (auto tick : ticks)
		path.addPath(QGraphicsLineItem(tick).shape());
	return path;
}

void RulerScaleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	QWidget *widget) 
{
	painter->setPen(pen());
	painter->drawLine(line());
	painter->drawLines(ticks);
	if (isSelected()) {
		QPen dash(Qt::black, 1, Qt::DashLine);
		painter->setPen(dash);
		painter->drawRect(boundingRect());
	}
}

void RulerScaleItem::setLine(const QLineF& l) {
	QLineF line = QLineF(mapFromScene(l.p1()), mapFromScene(l.p2()));
	//Generate ticks:
	double may = 10.0;
	double mid = 5.0;
	double min = 1.0;
	ticks.clear();
	if (line.dx() == 0.0) {
		//vertical line
		for (int i=0; i<=10; ++i) {
			for (int j=1; j<10; ++j) {
				if (j==5 && i<10) {
					QLineF tick = QLineF(line.p1()+QPointF(mid, (0.1*i+0.01*j)*line.length()), line.p1()+QPointF(-mid, (0.1*i+0.01*j)*line.length()));
					ticks.append(tick);
					continue;
				}
				//QLineF tick = QLineF(line.p1()+QPointF(min, (0.1*i+0.01*j)*line.length()), line.p1()+QPointF(-min, (0.1*i+0.01*j)*line.length()));
				//ticks.append(tick);
			}
			QLineF tick = QLineF(line.p1()+QPointF(may, 0.1*i*line.length()), line.p1()+QPointF(-may, 0.1*i*line.length()));
			ticks.append(tick);
		}
	}
	else {
		for (int i=0; i<=10; ++i) {
			for (int j=1; j<10; ++j) {
				if (j==5 && i<10) {
					QLineF tick = QLineF(line.p1()+QPointF((0.1*i+0.01*j)*line.length(), mid), line.p1()+QPointF((0.1*i+0.01*j)*line.length(), -mid));
					ticks.append(tick);
					continue;
				}
				//QLineF tick = QLineF(line.p1()+QPointF((0.1*i+0.01*j)*line.length(), min), line.p1()+QPointF((0.1*i+0.01*j)*line.length(), -min));
				//ticks.append(tick);
			}
			QLineF tick = QLineF(line.p1()+QPointF(0.1*i*line.length(), may), line.p1()+QPointF(0.1*i*line.length(), -may));
			ticks.append(tick);
		}
	}
	ScaleItem::setLine(line);
}

//BarScale
BarScale::BarScale(const QLineF& line, QGraphicsItem* parent):
	ScaleItem(line, parent)
{
	setLine(line);
	//QImage txt(64, 64, QImage::Format_RGB32);
	//txt.fill(Qt::black);
	//QPainter painter(&txt);
	//painter.setBrush(Qt::white);
	//painter.setPen(Qt::NoPen);
	//painter.drawRect(QRect(32, 0, 32, 32));
	QPen myPen;
	myPen.setBrush(Qt::black);
	myPen.setCapStyle(Qt::FlatCap);
	setPen(myPen);
	setPenWidth(10);
	setText1("0");
}

QRectF BarScale::boundingRect() const {
	QLineF l = line();
	double arrowSize = 5.0;
	double extra = (pen().width() + ::sin(3.14/3.0) * arrowSize) / 2.0;
	QPointF p1 = l.p1();
	QPointF p2 = l.p2();
	return QRectF(p1, QSizeF(p2.x() - p1.x(), p2.y() - p1.y())).normalized().adjusted(-extra, -extra, extra, extra);
}

QPainterPath BarScale::shape() const {
	QPainterPath path;
	path.addPath(QGraphicsLineItem(line()).shape());
	return path;
}

void BarScale::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	QWidget *widget) 
{
	painter->setPen(pen());
	painter->drawLine(line());
	QPen dot(Qt::white, pen().width(), Qt::SolidLine, Qt::FlatCap);
	QLineF l = line();
	if (l.dx() == 0.0) {
		//vertical line
		for (int i=1; i<=9; i+=2) {
			QLineF li = QLineF(l.p1()+QPointF(0, 0.1*i*l.length()), l.p1()+QPointF(0, 0.1*(i+1)*l.length()));
			QGraphicsLineItem stripe(li, this);
			stripe.setPen(dot);
			stripe.paint(painter, option, widget);
		}
	}
	else {
		for (int i=1; i<=9; i+=2) {
			QLineF li = QLineF(l.p1()+QPointF(0.1*i*l.length(), 0), l.p1()+QPointF(0.1*(i+1)*l.length(), 0));
			QGraphicsLineItem stripe(li, this);
			stripe.setPen(dot);
			stripe.paint(painter, option, widget);
		}
	}
	if (isSelected()) {
		QPen dash(Qt::black, 1, Qt::DashLine);
		painter->setPen(dash);
		painter->drawRect(boundingRect());
	}
}

void BarScale::setLine(const QLineF& l) {
	QLineF line = QLineF(mapFromScene(l.p1()), mapFromScene(l.p2()));
	ScaleItem::setLine(line);
}