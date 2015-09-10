#include "stdafx.h"
#include "GraphicsView.h"

GraphicsView::GraphicsView(QWidget* parent) :
	QGraphicsView(parent), zoomLevel(1.0)
{
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

GraphicsView::GraphicsView(QGraphicsScene* scene, QWidget* parent) :
	QGraphicsView(scene, parent), zoomLevel(1.0)
{
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}


GraphicsView::~GraphicsView(void)
{
}

void GraphicsView::wheelEvent(QWheelEvent* event) {
	QPoint wheel = event->angleDelta();
	if (wheel.x() > 0 || wheel.y() > 0) zoomIn();
	else if (wheel.x() < 0 || wheel.y() < 0) zoomOut();
}

void GraphicsView::setZoomLevel(double level) {
	setTransform(QTransform::fromScale(level, level), false);
	emit zoomChanged(level);
}

void GraphicsView::zoomIn() {
	if (zoomLevel < 1.0)
		zoomLevel += 0.05;
	else if (zoomLevel >= 1.0 && zoomLevel < 3.0)
		zoomLevel += .25;
	else if (zoomLevel >= 3.0 && zoomLevel <= 16.0)
		zoomLevel += .5;
	setZoomLevel(zoomLevel);
}

void GraphicsView::zoomOut() {
	if (zoomLevel <= 1.0 && zoomLevel >= .1)
		zoomLevel -= 0.05;
	else if (zoomLevel > 1.0 && zoomLevel <= 3.0)
		zoomLevel -= .25;
	else if (zoomLevel > 3.0)
		zoomLevel -= .5;
	setZoomLevel(zoomLevel);
}

void GraphicsView::zoomToFit() {
	int z = qRound((0.9*this->height()/sceneRect().height())*10);
	zoomLevel = 1.0 * z / 10;
	setZoomLevel(zoomLevel);
}

void GraphicsView::zoomToActual() {
	zoomLevel = 1.0;
	setZoomLevel(zoomLevel);
}


