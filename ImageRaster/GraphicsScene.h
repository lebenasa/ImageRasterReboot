#pragma once
#include "qgraphicsscene.h"
#include "Enums.h"

class ArrowItem;
class RectItem;
class EllipseItem;
class LineRuler;
class RectRuler;
class PolyRuler;
class PathRuler;
class MarkerItem;

class GraphicsScene :
	public QGraphicsScene
{
	Q_OBJECT
	//Internal state:
	bool isDragged;
	bool hasTmpItem, hasTmpItem2;
	int click;
	bool interceptLeftClick;

	//Temporary shapes:
	QGraphicsRectItem *tmpRect;
	ArrowItem *tmpArrow;
	RectItem *tmpRectItem;
	EllipseItem *tmpCircle;
	QGraphicsLineItem *tmpLine, *tmpLine2;

	//Store event pos:
	QPointF p1, p2, p3, p4, p5, p6;
	QPolygonF polyBuilder;

	QRectF rectFrom3Point(const QPointF&, const QPointF&, const QPointF&);
	QRectF rectFromCenter(const QPointF&);

	int appState;
	int rulerState;
	int markerType;
public:
	GraphicsScene(QObject* parent = 0);
	~GraphicsScene();

	int polyClick() const { return click; }
	
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

public slots:
	void setAppState(int state);
	void setRulerState(int state);
	void setMarkerType(int type);

	void resetStates();

	//CMR : Click-Move-Release
	void cmrSelect(QGraphicsSceneMouseEvent *event, MouseState ms);
	void cmrMarkerArrow(QGraphicsSceneMouseEvent *event, MouseState ms);
	void cmrMarkerRectangle(QGraphicsSceneMouseEvent *event, MouseState ms);
	void cmrMarkerCircle(QGraphicsSceneMouseEvent *event, MouseState ms);
	void cmrRulerLine(QGraphicsSceneMouseEvent *event, MouseState ms);
	void cmrRulerRectangle(QGraphicsSceneMouseEvent *event, MouseState ms);
	void cmrRulerCircle(QGraphicsSceneMouseEvent *event, MouseState ms);
	void cmrRulerCentertoCenter(QGraphicsSceneMouseEvent *event, MouseState ms);
	void cmrRulerPolygon(QGraphicsSceneMouseEvent *event, MouseState ms);
	void cmrRulerMulti(QGraphicsSceneMouseEvent *event, MouseState ms);
	void finishPolygon();

signals:
	void appStateChanged(int state);

	void mouseEvent(QGraphicsSceneMouseEvent *event, MouseState state);
	void addMarker(MarkerItem*);
	void addLR(LineRuler*);
	void addRR(RectRuler*);
	void addCR(const QPointF&, const QPointF&, const QPointF&);
	void addC2(const QPointF&, const QPointF&, const QPointF&, const QPointF&, const QPointF&, const QPointF&);
	void addPR(PolyRuler*);
	void addMR(PathRuler*);

	void helpText(const QString&);
	void posChanged(const QString&);
};
