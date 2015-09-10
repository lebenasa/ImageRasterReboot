#pragma once
#include "GraphicsItem.h"
#include <functional>

class LegendItem :
	public QGraphicsItemGroup
{
	int myType;
	QPointF clicked;
	QLineF line;
	QRectF rect;
	bool _isSinglePoint;
	ArrowItem* arrow;
	RectItem* box;
	EllipseItem* circle;
	RichTextItem* text;
	int mySize;

	std::function<void(QKeyEvent*)> func;

	QPointF calculateTextPos() const;
public:
	LegendItem(const QPointF& topLeft, int type=0, QGraphicsItem* parent=0);
	~LegendItem();

	enum  { Type = UserType + 20 };
	int type() const { return Type; }

	enum Mode {
		SinglePoint, Line, Rectangle
	} mode;

	bool isSinglePoint() const {
		return _isSinglePoint;
	}

	void setType(int type);
	int activeType() const {
		return myType;
	}

	void setPenColor(const QColor& shape, const QColor& outline);
	void setColor1(const QColor& shape_color) {
		setPenColor(shape_color, color2());
	}
	QColor color1() const;
	void setColor2(const QColor& outline_color) {
		setPenColor(color1(), outline_color);
	}
	QColor color2() const;
	void setPenWidth(int width);
	int penWidth() const;

	void setSize(int);
	int size() const;

	void setLabel(const QString& txt);
	QString label() const;
	void setFont(const QFont& font);
	QFont font() const;
	void setFontSize(int size);
	int fontSize() const;

	QRectF getRect();

	void moveTo(const QPointF& pos);

	void setTextEventHandler(std::function<void(QKeyEvent*)> func) {
		text->setKeyPressFunc(func);
	}
};

class MarkerModel;
class LegendGroup :
	public QGraphicsItemGroup
{
	int mySize;
	int myPenWidth;
	QFont myFont;
	int myFontSize;
	QPointF origin;
	MarkerModel* model;

	QList<QGraphicsItem*> legends;
	QList<RichTextItem*> texts;
	RectItem* frame;

	void calculatePos();
	void updateRect();
protected:
	QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value) {
		if (QGraphicsItem::ItemPositionHasChanged == change)
			origin = value.toPointF();
		return QGraphicsItemGroup::itemChange(change, value);
	}
public:
	LegendGroup(const QPointF& topLeft, MarkerModel* model, QGraphicsItem* parent=0);
	~LegendGroup();

	void updateModel();

	enum  { Type = UserType + 21 };
	int type() const { return Type; }

	void setPenWidth(int width);
	int penWidth() const;

	void setSize(int);
	int size() const;

	void setFont(const QFont& font);
	QFont font() const;
	void setFontSize(int size);
	int fontSize() const;
};
