#pragma once
#include "qgraphicsitem.h"
#include "qabstractitemmodel.h"
#include "Enums.h"

class ArrowItem;
class RectItem;
class EllipseItem;
class SimpleTextItem;

class MarkerContainer;
class MarkerItem :
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
	SimpleTextItem* text;
	int mySize;

	QPointF calculateTextPos() const;

	int Index;
	MarkerContainer* Container;
	bool HasContainer;
public:
	MarkerItem(const QPointF& Clicked, int type=0, QGraphicsItem* parent=0);
	MarkerItem(const QLineF& Line, int type=0, QGraphicsItem* parent=0);
	MarkerItem(const QRectF& Rect, int type=1, QGraphicsItem* parent=0);
	//MarkerItem(const MarkerItem& c);
	~MarkerItem();

	enum  { Type = UserType + 8 };
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
	void setHasBackground(bool show);
	bool hasBackground() const;

	bool hasContainer() { return HasContainer; }
	void unsetContainer() { HasContainer = false; }
	void setContainer(MarkerContainer* container, int index) { 
		Container = container; 
		Index = index;
		HasContainer = true;
	}
	MarkerContainer* container() { return Container; }
	int index() const { return Index; }

	static QLineF lineFromClick(const QPointF& click, int size);
	static QRectF rectFromClick(const QPointF& click, int size);
};

class MarkerContainer 
{
	QList<MarkerItem*> markers;
	int myType;
	bool visible;
	int mySize;
	int myPenWidth;
	QColor myColor1, myColor2;
	QString myLabel;
	QFont myFont;
	int myFontSize;
	bool myHasBackground;

	int Index;
public:
	MarkerContainer();
	//MarkerContainer(const MarkerContainer&);
	~MarkerContainer() {}

	void addMarker(MarkerItem* item);
	void popMarker() { if (!markers.isEmpty()) markers.removeLast(); }

	QList<MarkerItem*> items() { return markers; }

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

	void setSize(int);		//Only applicable to 1-click marker item
	int size();

	void setLabel(const QString& txt);
	QString label() const;
	void setFont(const QFont& font);
	QFont font() const;
	void setFontSize(int size);
	int fontSize() const;
	void setHasBackground(bool show);
	bool hasBackground() const;

	void setVisible(bool show);
	bool isVisible() const {
		return visible;
	}

	int count() const {
		return markers.size();
	}

	void setIndex(int i) { Index = i; }
	int index() const { return Index; }
	void removeAt(int i) { if (!markers.isEmpty()) markers.removeAt(i); }
};

class MarkerModel:
	public QAbstractTableModel
{
	Q_OBJECT
	QList<MarkerContainer*> myData;
public:
	MarkerModel(QObject* parent=0);
	~MarkerModel();

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool insertMarkerContainer(MarkerContainer* c, const QModelIndex& parent = QModelIndex());
	bool removeMarkerContainer(MarkerContainer* c);

	void addMarker(MarkerItem* item, int row);
	void popMarker(int row) { 
		myData.at(row)->popMarker(); 
		emit dataChanged(index(0, 1), index(myData.size()-1, 1)); 
	}
	void setCurrentType(int type, int row) { if (!myData.isEmpty()) myData.at(row)->setType(type); }
	int currentType(int row) const { if (!myData.isEmpty()) return myData.at(row)->activeType(); else return 0; }
	void removeMarker(MarkerItem* item);
	MarkerContainer* at(int i) { if (!myData.isEmpty()) return myData.at(i); else return nullptr; }
};

