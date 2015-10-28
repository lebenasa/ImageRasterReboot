#include "stdafx.h"
#include "GraphicsScene.h"
#include "GraphicsItem.h"
#include "RulerItem.h"
#include "MarkerItem.h"

GraphicsScene::GraphicsScene(QObject* parent):
	QGraphicsScene(parent), isDragged(false), hasTmpItem(false), hasTmpItem2(false),
	click(0), interceptLeftClick(false)
{
	appState = 0;
	markerType = 0;
	rulerState = 0;
	setAppState(appState);
}

GraphicsScene::~GraphicsScene(void)
{
}

void GraphicsScene::setAppState(int curState) {
	if (appState == curState) return;
	appState = curState;

	resetStates();

	//Connect to correct event handler:
	using namespace States;
	if (AppState::Select == appState || AppState::Scale == appState || AppState::Legend == appState || AppState::Calibration == appState) {
		disconnect(this, &GraphicsScene::mouseEvent, this, 0);
		connect(this, &GraphicsScene::mouseEvent, this, &GraphicsScene::cmrSelect);
		for (auto view : views())
			view->setDragMode(QGraphicsView::RubberBandDrag);
	}
	else if (AppState::Marker == appState) {
		setMarkerType(markerType);
		for (auto view : views())
			view->setDragMode(QGraphicsView::NoDrag);
	}
	else if (AppState::Measure == appState) {
		setRulerState(rulerState);
		for (auto view : views())
			view->setDragMode(QGraphicsView::NoDrag);
	}

	emit appStateChanged(curState);
}

void GraphicsScene::setMarkerType(int mode) {
	using namespace States;
	if (AppState::Marker != appState) return;
	disconnect(this, &GraphicsScene::mouseEvent, this, 0);
	switch (mode) {
	case MarkerType::Arrow:
		connect(this, &GraphicsScene::mouseEvent, this, &GraphicsScene::cmrMarkerArrow);
		break;
	case MarkerType::Ellipse:
		connect(this, &GraphicsScene::mouseEvent, this, &GraphicsScene::cmrMarkerCircle);
		break;
	case MarkerType::Box:
		connect(this, &GraphicsScene::mouseEvent, this, &GraphicsScene::cmrMarkerRectangle);
		break;
	}
}

void GraphicsScene::setRulerState(int mode) {
	using namespace States;
	if (AppState::Measure != appState) return;
	disconnect(this, &GraphicsScene::mouseEvent, this, 0);
	switch (mode) {
	case RulerState::Line:
		connect(this, &GraphicsScene::mouseEvent, this, &GraphicsScene::cmrRulerLine);
		break;
	case RulerState::Rect:
		connect(this, &GraphicsScene::mouseEvent, this, &GraphicsScene::cmrRulerRectangle);
		break;
	case RulerState::Circle:
		connect(this, &GraphicsScene::mouseEvent, this, &GraphicsScene::cmrRulerCircle);
		break;
	case RulerState::Circle2:
		connect(this, &GraphicsScene::mouseEvent, this, &GraphicsScene::cmrRulerCentertoCenter);
		break;
	case RulerState::Polygon:
		connect(this, &GraphicsScene::mouseEvent, this, &GraphicsScene::cmrRulerPolygon);
		break;
	case RulerState::Path:
		connect(this, &GraphicsScene::mouseEvent, this, &GraphicsScene::cmrRulerMulti);
		break;
	}
	rulerState = mode;
}

void GraphicsScene::resetStates() {
	//Handle temporary items and variables:
	if (hasTmpItem) {
		for (auto item : items()) {
			if (tmpCircle == item)
				delete tmpCircle;
			else if (tmpLine == item)
				delete tmpLine;
			else if (tmpArrow == item)
				delete tmpArrow;
		}
		hasTmpItem = false;
	}
	if (hasTmpItem2) {
		for (auto item : items()) {
			if (tmpLine2 == item)
				delete tmpLine2;
		}
		hasTmpItem = false;
	}
	click = 0;
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	emit mouseEvent(event, MouseState::Click);
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	emit mouseEvent(event, MouseState::Move);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	emit mouseEvent(event, MouseState::Release);
}

void GraphicsScene::cmrSelect(QGraphicsSceneMouseEvent *event, MouseState ms) {
	if (MouseState::Click == ms) {
		QGraphicsScene::mousePressEvent(event);
	}
	else if (MouseState::Move == ms) {
		QGraphicsScene::mouseMoveEvent(event);
	}
	else if (MouseState::Release == ms) {
		QGraphicsScene::mouseReleaseEvent(event);
	}
}

void GraphicsScene::cmrMarkerArrow(QGraphicsSceneMouseEvent *event, MouseState ms)	{
	if (Qt::RightButton == event->button()) {
		return;
	}
	if (MouseState::Click == ms) {
		p1 = event->scenePos();
		if (!sceneRect().contains(p1)) return;
		//if (items(p1).count() > 1) return;	//To ensure no overlap
		if (items(p1).count() >= 1) {
			for (auto item : items(p1)) {
				if (auto i = qgraphicsitem_cast<MarkerItem*>(item)) {
					if (i->isSinglePoint()) {
						auto ctr = i->boundingRect().topLeft();
						auto chk = QRectF(ctr.x() - 5, ctr.y() - 5, 10, 10);
						if (chk.contains(p1)) return;
					}
					auto ctr = i->boundingRect().center();
					auto chk = QRectF(ctr.x() - 5, ctr.y() - 5, 10, 10);
					if (chk.contains(p1)) return;
				}
			}
		}
		isDragged = true;
		p2 = p1 + QPointF(1, 1);					//To ensure a valid QLineF
	}
	else if (MouseState::Move == ms) {
		if (!isDragged) return;
		p2 = event->scenePos();
		if (sceneRect().contains(p2)) {
			if (hasTmpItem)
				tmpArrow->setLine(QLineF(p1, p2));
			else {
				tmpArrow = new ArrowItem(QLineF(p1, p2));
				tmpArrow->setArrowhead(ArrowHead::Start);
				tmpArrow->setPenColor(Qt::black, Qt::white);
				addItem(tmpArrow);
			}
			hasTmpItem = true;
		}
		else {
			if (hasTmpItem) delete tmpArrow;
			hasTmpItem = false;
			isDragged = false;
		}
	}
	else if (MouseState::Release == ms) {
		if (!isDragged) return;
		isDragged = false;
		if (hasTmpItem) delete tmpArrow;
		hasTmpItem = false;
		p2 = (sceneRect().contains(event->scenePos())) ? event->scenePos() : p2;
		//Call create arrow marker action:
		QLineF l(p1, p2);
		if (l.length() > 10) {
			auto marker = new MarkerItem(l, States::Arrow);
			emit addMarker(marker);
		}
		else {
			auto marker = new MarkerItem(p1, States::Arrow);
			emit addMarker(marker);
		}
	}
}

void GraphicsScene::cmrMarkerRectangle(QGraphicsSceneMouseEvent *event, MouseState ms)	{
	if (Qt::RightButton == event->button()) {
		return;
	}
	if (MouseState::Click == ms) {
		p1 = event->scenePos();
		if (!sceneRect().contains(p1)) return;
		//if (items(p1).count() > 1) return;	//To ensure no overlap
		if (items(p1).count() >= 1) {
			for (auto item : items(p1)) {
				if (auto i = qgraphicsitem_cast<MarkerItem*>(item)) {
					//if (i->isSinglePoint()) return;
					auto ctr = i->boundingRect().center();
					auto chk = QRectF(ctr.x() - 5, ctr.y() - 5, 10, 10);
					if (chk.contains(p1)) return;
				}
			}
		}
		isDragged = true;
		p2 = p1 + QPointF(1, 1);					//To ensure a valid QLineF
	}
	else if (MouseState::Move == ms) {
		if (!isDragged) return;
		p2 = event->scenePos();
		if (sceneRect().contains(p2)) {
			if (!hasTmpItem) {
				tmpRectItem = new RectItem(QRectF(p1, p2).normalized());
				tmpRectItem->setPenColor(Qt::black, Qt::white);
				addItem(tmpRectItem);
			}
			else {
				tmpRectItem->setRect(QRectF(p1, p2).normalized());
			}
			hasTmpItem = true;
		}
		else {
			if (hasTmpItem) delete tmpRectItem;
			hasTmpItem = false;
			isDragged = false;
		}
	}
	else if (MouseState::Release == ms) {
		if (!isDragged) return;
		isDragged = false;
		if (hasTmpItem) delete tmpRectItem;
		hasTmpItem = false;
		p2 = (sceneRect().contains(event->scenePos())) ? event->scenePos() : p2;
		//Call create arrow marker action:
		QLineF l(p1, p2);
		if (l.length() > 10) {
			auto marker = new MarkerItem(QRectF(p1, p2).normalized(), States::Box);
			emit addMarker(marker);
		}
		else {
			auto marker = new MarkerItem(p1, States::Box);
			emit addMarker(marker);
		}
	}
}

void GraphicsScene::cmrMarkerCircle(QGraphicsSceneMouseEvent *event, MouseState ms)	{
	if (Qt::RightButton == event->button()) {
		return;
	}
	if (MouseState::Click == ms) {
		p1 = event->scenePos();
		if (!sceneRect().contains(p1)) return;
		//if (items(p1).count() > 1) return;	//To ensure no overlap
		if (items(p1).count() >= 1) {
			for (auto item : items(p1)) {
				if (auto i = qgraphicsitem_cast<MarkerItem*>(item)) {
					//if (i->isSinglePoint()) return;
					auto ctr = i->boundingRect().center();
					auto chk = QRectF(ctr.x() - 5, ctr.y() - 5, 10, 10);
					if (chk.contains(p1)) return;
				}
			}
		}
		isDragged = true;
		p2 = p1 + QPointF(1, 1);					//To ensure a valid QLineF
	}
	else if (MouseState::Move == ms) {
		if (!isDragged) return;
		p2 = event->scenePos();
		if (sceneRect().contains(p2)) {
			if (hasTmpItem)
				tmpCircle->setRect(QRectF(p1, p2).normalized());
			else {
				tmpCircle = new EllipseItem(QRectF(p1, p2).normalized());
				tmpCircle->setPenColor(Qt::black, Qt::white);
				addItem(tmpCircle);
			}
			hasTmpItem = true;
		}
		else {
			if (hasTmpItem) delete tmpCircle;
			hasTmpItem = false;
			isDragged = false;
		}
	}
	else if (MouseState::Release == ms) {
		if (!isDragged) return;
		isDragged = false;
		if (hasTmpItem) delete tmpCircle;
		hasTmpItem = false;
		p2 = (sceneRect().contains(event->scenePos())) ? event->scenePos() : p2;
		//Call create arrow marker action:
		QLineF l(p1, p2);
		if (l.length() > 10) {
			auto marker = new MarkerItem(QRectF(p1, p2).normalized(), States::Ellipse);
			emit addMarker(marker);
		}
		else {
			auto marker = new MarkerItem(p1, States::Ellipse);
			emit addMarker(marker);
		}
	}
}

void GraphicsScene::cmrRulerLine(QGraphicsSceneMouseEvent *event, MouseState ms)	{
	if (Qt::RightButton == event->button()) {
		//TBA: Context menu
		return;
	}

	if (MouseState::Click == ms) {
		p1 = event->scenePos();
		if (!sceneRect().contains(p1)) return;
		//if (items(p1).count() > 1) return;	//To ensure no overlap
		isDragged = true;
		p2 = p1 + QPointF(1, 1);					//To ensure a valid QLineF
	}
	else if (MouseState::Move == ms) {
		if (!isDragged) return;
		p2 = event->scenePos();
		if (Qt::ControlModifier == event->modifiers()) {
			QLineF dummy(p1, p2);
			double x = (dummy.dx() > dummy.dy()) ? p2.x() : p1.x();
			double y = (dummy.dx() > dummy.dy()) ? p1.y() : p2.y();
			p2 = QPoint(x, y);
		}
		if (sceneRect().contains(p2)) {
			if (!hasTmpItem) {
				tmpArrow = new ArrowItem(QLineF(p1, p2));
				tmpArrow->setPenColor(Qt::black, Qt::white);
				tmpArrow->setArrowhead(ArrowHead::Both);
				addItem(tmpArrow);
			}
			else
				tmpArrow->setLine(QLineF(p1, p2));
			hasTmpItem = true;
		}
		else {
			if (hasTmpItem) delete tmpArrow;
			hasTmpItem = false;
			isDragged = false;
		}
	}
	else if (MouseState::Release == ms) {
		if (!isDragged) return;
		isDragged = false;
		p2 = (sceneRect().contains(event->scenePos())) ? event->scenePos() : p2;
		if (Qt::ControlModifier == event->modifiers()) {
			QLineF dummy(p1, p2);
			double x = (dummy.dx() > dummy.dy()) ? p2.x() : p1.x();
			double y = (dummy.dx() > dummy.dy()) ? p1.y() : p2.y();
			p2 = QPoint(x, y);
		}
		//Call create arrow marker action:
		QLineF line = QLineF(p1, p2);
		if (hasTmpItem) delete tmpArrow;
		hasTmpItem = false;	
		if (line.length() < 5.0)
			return;
		LineRuler* ruler = new LineRuler(line);
		emit addLR(ruler);
	}
}

void GraphicsScene::cmrRulerRectangle(QGraphicsSceneMouseEvent *event, MouseState ms)	{
	if (Qt::RightButton == event->button()) {
		return;
	}
	if (MouseState::Click == ms) {
		p1 = event->scenePos();
		if (!sceneRect().contains(p1)) return;
		//if (items(p1).count() > 1) return;	//To ensure no overlap
		isDragged = true;
		p2 = p1 + QPointF(1, 1);					//To ensure a valid QLineF
	}
	else if (MouseState::Move == ms) {
		if (!isDragged) return;
		p2 = event->scenePos();
		if (sceneRect().contains(p2)) {
			if (!hasTmpItem) {
				tmpRectItem = new RectItem(QRectF(p1, p2).normalized());
				tmpRectItem->setPenColor(Qt::black, Qt::white);
				addItem(tmpRectItem);
			}
			else {
				tmpRectItem->setRect(QRectF(p1, p2).normalized());
			}
			hasTmpItem = true;
		}
		else {
			if (hasTmpItem) delete tmpRectItem;
			hasTmpItem = false;
			isDragged = false;
		}
	}
	else if (MouseState::Release == ms) {
		if (!isDragged) return;
		isDragged = false;
		if (hasTmpItem) delete tmpRectItem;
		hasTmpItem = false;
		p2 = (sceneRect().contains(event->scenePos())) ? event->scenePos() : p2;
		QLineF line = QLineF(p1, p2);
		if (line.length() < 5.0)
			return;
		QRectF r = QRectF(p1, p2);
		RectRuler* ruler = new RectRuler(r.normalized());
		emit addRR(ruler);
	}
}

void GraphicsScene::cmrRulerCircle(QGraphicsSceneMouseEvent *event, MouseState ms)	{
	if (Qt::RightButton == event->button()) {
		return;
	}
	if (MouseState::Click == ms) {
		p1 = event->scenePos();
		if (!sceneRect().contains(p1)) return;
		//if (items(p1).count() > 1) return;	//To ensure no overlap
		isDragged = true;
		p2 = p1 + QPointF(1, 1);					//To ensure a valid QLineF
	}
	else if (MouseState::Move == ms) {
		if (!isDragged) return;
		p2 = event->scenePos();
		if (sceneRect().contains(p2)) {
			auto r = QRectF(p1, p2).normalized();
			auto ln = 0.5 * QLineF(p1, p2).length();
			auto circ = QRectF(r.center() - QPointF(ln, ln), r.center() + QPointF(ln, ln));
			if (!hasTmpItem) {
				tmpCircle = new EllipseItem(circ);
				tmpCircle->setPenColor(Qt::black, Qt::white);
				addItem(tmpCircle);
			}
			else {
				tmpCircle->setRect(circ);
			}
			hasTmpItem = true;
		}
		else {
			if (hasTmpItem) delete tmpCircle;
			hasTmpItem = false;
			isDragged = false;
		}
	}
	else if (MouseState::Release == ms) {
		if (!isDragged) return;
		isDragged = false;
		if (hasTmpItem) delete tmpCircle;
		hasTmpItem = false;
		p2 = (sceneRect().contains(event->scenePos())) ? event->scenePos() : p2;
		QLineF line = QLineF(p1, p2);
		QRectF r = QRectF(p1, p2).normalized();
		auto ln = 0.5 * line.length();
		auto circ = QRectF(r.center() - QPointF(ln, ln), r.center() + QPointF(ln, ln));
		CircleRuler* ruler = new CircleRuler(circ);
		emit addC1(ruler);
	}
}

//void GraphicsScene::cmrRulerCircle(QGraphicsSceneMouseEvent *event, MouseState ms)	{
//	if (Qt::RightButton == event->button()) {
//		return;
//	}
//	if (MouseState::Click == ms) {
//		if (!sceneRect().contains(event->scenePos())) return;
//		if (items(event->scenePos()).count() > 1) return;	//To ensure no overlap
//		if (0 == click) {
//			p1 = event->scenePos();
//			++click;
//			tmpCircle = new EllipseItem(rectFromCenter(p1));
//			tmpCircle->setPenColor(Qt::black, Qt::white);
//			tmpCircle->setPenWidth(2);
//			addItem(tmpCircle);
//			hasTmpItem = true;
//		}
//		else if (1 == click) {
//			p2 = event->scenePos();
//			++click;
//			EllipseItem* item = new EllipseItem(rectFromCenter(p2), tmpCircle);
//			item->setPenColor(Qt::black, Qt::white);
//			item->setPenWidth(2);
//			addItem(item);
//		}
//		else if (2 == click) {
//			p3 = event->scenePos();
//			click = 0;
//			delete tmpCircle;
//			hasTmpItem = false;
//			emit addCR(p1, p2, p3);
//		}
//	}
//	else if (MouseState::Move == ms) {
//	}
//	else if (MouseState::Release == ms) {
//	}
//}

void GraphicsScene::cmrRulerCentertoCenter(QGraphicsSceneMouseEvent *event, MouseState ms)	{
	if (Qt::RightButton == event->button()) {
		interceptLeftClick = true;
		return;
	}
	if (interceptLeftClick) {
		interceptLeftClick = false;
		return;
	}
	if (MouseState::Click == ms) {
		if (!sceneRect().contains(event->scenePos())) return;
		//if (items(event->scenePos()).count() > 1) return;	//To ensure no overlap
		if (0 == click) {
			p1 = event->scenePos();
			++click;
			tmpCircle = new EllipseItem(rectFromCenter(p1));
			tmpCircle->setPenColor(Qt::black, Qt::white);
			tmpCircle->setPenWidth(2);
			addItem(tmpCircle);
			hasTmpItem = true;
		}
		else if (1 == click) {
			p2 = event->scenePos();
			++click;
			EllipseItem* item = new EllipseItem(rectFromCenter(p2), tmpCircle);
			item->setPenColor(Qt::black, Qt::white);
			item->setPenWidth(2);
			addItem(item);
		}
		else if (2 == click) {
			p3 = event->scenePos();
			++click;
			auto item = new EllipseItem(rectFromCenter(p3), tmpCircle);
			item->setPenColor(Qt::black, Qt::white);
			item->setPenWidth(2);
			addItem(item);
			auto item2 = new EllipseItem(rectFrom3Point(p1, p2, p3), tmpCircle);
			item2->setPenColor(Qt::black, Qt::white);
			item2->setPenWidth(2);
			addItem(item2);
		}
		else if (3 == click) {
			p4 = event->scenePos();
			++click;
			EllipseItem* item = new EllipseItem(rectFromCenter(p4), tmpCircle);
			item->setPenColor(Qt::black, Qt::white);
			item->setPenWidth(2);
			addItem(item);
		}
		else if (4 == click) {
			p5 = event->scenePos();
			++click;
			EllipseItem* item = new EllipseItem(rectFromCenter(p5), tmpCircle);
			item->setPenColor(Qt::black, Qt::white);
			item->setPenWidth(2);
			addItem(item);
		}
		else if (5 == click) {
			p6 = event->scenePos();
			click = 0;
			delete tmpCircle;
			hasTmpItem = false;
			emit addC2(p1, p2, p3, p4, p5, p6);
		}
	}
	else if (MouseState::Move == ms) {

	}
	else if (MouseState::Release == ms) {

	}
}

void GraphicsScene::cmrRulerPolygon(QGraphicsSceneMouseEvent *event, MouseState ms)	{
	if (Qt::RightButton == event->button()) {
		return;
	}
	if (interceptLeftClick) {
		interceptLeftClick = false;
		return;
	}
	QPen pen = QPen(Qt::black);
	pen.setWidth(2);
	if (MouseState::Click == ms) {
		if (!sceneRect().contains(event->scenePos())) return;
		//if (items(event->scenePos()).count() > 1) return;	//To ensure no overlap
		if (0 == click) {
			p1 = event->scenePos();
			p3 = p1;
			polyBuilder.clear();
			polyBuilder << p1;
			hasTmpItem2 = false;
			++click;
		}
		else if (click > 0) {
			p2 = event->scenePos();
			++click;
			if (!hasTmpItem) {
				tmpLine = new QGraphicsLineItem(QLineF(p1, p2));
				tmpLine->setPen(pen);
				addItem(tmpLine);
				hasTmpItem = true;
			}
			else {
				QGraphicsLineItem *item = new QGraphicsLineItem(QLineF(p1, p2), tmpLine);
				item->setPen(pen);
				//addItem(item);
			}
			polyBuilder << p2;
			p1 = p2;
		}
	}
	else if (MouseState::Move == ms) {
		if (click == 0) return;
		p2 = event->scenePos();
		if (sceneRect().contains(p2)) {
			if (!hasTmpItem2) {
				tmpLine2 = new QGraphicsLineItem(QLineF(p1, p2));
				tmpLine2->setPen(pen);
				addItem(tmpLine2);
			}
			else
				tmpLine2->setLine(QLineF(p1, p2));			
			hasTmpItem2 = true;
		}
		else {
			if (hasTmpItem2) delete tmpLine2;
			hasTmpItem2 = false;
		}
	}
	else if (MouseState::Release == ms) {
	}
}

void GraphicsScene::cmrRulerMulti(QGraphicsSceneMouseEvent *event, MouseState ms) {
	if (Qt::RightButton == event->button()) {
		return;
	}
	if (interceptLeftClick) {
		interceptLeftClick = false;
		return;
	}
	QPen pen = QPen(Qt::black);
	pen.setWidth(2);
	if (MouseState::Click == ms) {
		if (!sceneRect().contains(event->scenePos())) return;
		//if (items(event->scenePos()).count() > 1) return;	//To ensure no overlap
		if (0 == click) {
			p1 = event->scenePos();
			p3 = p1;
			polyBuilder.clear();
			polyBuilder << p1;
			hasTmpItem2 = false;
			++click;
		}
		else if (click > 0) {
			p2 = event->scenePos();
			++click;
			if (!hasTmpItem) {
				tmpLine = new QGraphicsLineItem(QLineF(p1, p2));
				tmpLine->setPen(pen);
				addItem(tmpLine);
				hasTmpItem = true;
			}
			else {
				QGraphicsLineItem *item = new QGraphicsLineItem(QLineF(p1, p2), tmpLine);
				item->setPen(pen);
				//addItem(item);
			}
			polyBuilder << p2;
			p1 = p2;
		}
	}
	else if (MouseState::Move == ms) {
		if (click == 0) return;
		p2 = event->scenePos();
		if (sceneRect().contains(p2)) {
			if (!hasTmpItem2) {
				tmpLine2 = new QGraphicsLineItem(QLineF(p1, p2));
				tmpLine2->setPen(pen);
				addItem(tmpLine2);
			}
			else
				tmpLine2->setLine(QLineF(p1, p2));
			hasTmpItem2 = true;
		}
		else {
			if (hasTmpItem2) delete tmpLine2;
			hasTmpItem2 = false;
		}
	}
	else if (MouseState::Release == ms) {
	}
}

QRectF GraphicsScene::rectFrom3Point(const QPointF& p1, const QPointF& p2, const QPointF& p3) {
	QPointF mid1 = QPointF((p1.x()+p2.x())/2, (p1.y()+p2.y())/2);
	QPointF mid2 = QPointF((p2.x()+p3.x())/2, (p2.y()+p3.y())/2);
	double m1 = (p2.y()-p1.y())/(p2.x()-p1.x());
	double m2 = (p3.y()-p2.y())/(p3.x()-p2.x());
	double mL1 = -(1.0/m1);
	double mL2 = -(1.0/m2);

	double k1 = -mL1 * mid1.x() + mid1.y();
	double k2 = -mL2 * mid2.x() + mid2.y();

	double x = (k2-k1)/(mL1-mL2);
	double y = mL1 * x + k1;
	QPointF center = QPointF(x, y);
	double radius = sqrt(pow((p1.x()-x), 2) + pow((p1.y()-y), 2));
	QPointF TL = QPointF(x-radius, y-radius);
	return QRectF(TL, QSizeF(2*radius, 2*radius));
}

QRectF GraphicsScene::rectFromCenter(const QPointF& pa) {
	QPointF tl = pa - QPointF(5, 5);
	return QRectF(tl, QSizeF(10, 10));
}

void GraphicsScene::finishPolygon() {
	if (polyBuilder.count() > 2) {
		delete tmpLine;
		hasTmpItem = false;
		delete tmpLine2;
		hasTmpItem2 = false;
		click = 0;
		using namespace States;
		if (rulerState == RulerState::Polygon) {
			polyBuilder << p3;
			PolyRuler* ruler = new PolyRuler(polyBuilder);
			emit addPR(ruler);
		}
		else if (rulerState == RulerState::Path) {
			PathRuler* ruler = new PathRuler(polyBuilder);
			emit addMR(ruler);
		}
	}
}