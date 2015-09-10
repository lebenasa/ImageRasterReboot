#pragma once
#include "qgraphicsview.h"
class GraphicsView :
	public QGraphicsView
{
	Q_OBJECT
	double zoomLevel;
public:
	GraphicsView(QWidget* parent=0);
	GraphicsView(QGraphicsScene* scene, QWidget* parent=0);
	~GraphicsView();

	double zoom() const { return zoomLevel; }
protected:
	void wheelEvent(QWheelEvent*);

public slots:
	void zoomIn();
	void zoomOut();
	void zoomToFit();
	void zoomToActual();
	void setZoomLevel(double);
	void setScene(QGraphicsScene* scene) {
		QGraphicsView::setScene(scene);
		zoomToFit();
	}
	void setSceneRect(const QRectF& rect) {
		QGraphicsView::setSceneRect(rect);
		zoomToFit();
	}
signals:
	void zoomChanged(double);
};

