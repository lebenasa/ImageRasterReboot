#include "stdafx.h"
#include "MarkerItem.h"
#include "GraphicsItem.h"

//MarkerItem
MarkerItem::MarkerItem(const QPointF& Clicked, int type, QGraphicsItem* parent):
	QGraphicsItemGroup(parent), clicked(Clicked), myType(type)
{
	mode = SinglePoint;
	_isSinglePoint = true;
	mySize = 20;
	arrow = new ArrowItem(QLineF(clicked, clicked+QPointF(mySize, mySize)));
	arrow->setArrowhead(ArrowHead::Start);
	box = new RectItem(QRectF(clicked-QPointF(mySize, mySize), clicked+QPointF(mySize, mySize)));
	circle = new EllipseItem(QRectF(clicked-QPointF(mySize, mySize), clicked+QPointF(mySize, mySize)));
	arrow->setIsMarker(true);
	box->setIsMarker(true);
	circle->setIsMarker(true);
	text = new SimpleTextItem("");
	setFlag(QGraphicsItem::ItemIsSelectable);
	addToGroup(arrow);
	addToGroup(box);
	addToGroup(circle);
	addToGroup(text);
	setType(type);
	HasContainer = false;
}

MarkerItem::MarkerItem(const QLineF& Line, int type, QGraphicsItem* parent):
	QGraphicsItemGroup(parent), line(Line), myType(type)
{
	mode = MarkerItem::Line;
	_isSinglePoint = false;
	arrow = new ArrowItem(line);
	arrow->setArrowhead(ArrowHead::Start);
	QRectF tmp(line.p1()-(line.p2()-line.p1()), line.p2());
	box = new RectItem(tmp.normalized());
	circle = new EllipseItem(tmp.normalized());
	arrow->setIsMarker(true);
	box->setIsMarker(true);
	circle->setIsMarker(true);
	text = new SimpleTextItem("");
	setFlag(QGraphicsItem::ItemIsSelectable);
	addToGroup(arrow);
	addToGroup(box);
	addToGroup(circle);
	addToGroup(text);
	setType(type);
	HasContainer = false;
}

MarkerItem::MarkerItem(const QRectF& Rect, int type, QGraphicsItem* parent):
	QGraphicsItemGroup(parent), rect(Rect.normalized()), myType(type)
{
	mode = Rectangle;
	_isSinglePoint = false;
	arrow = new ArrowItem(QLineF(rect.center(), rect.bottomRight()));
	arrow->setArrowhead(ArrowHead::Start);
	box = new RectItem(rect);
	circle = new EllipseItem(rect);
	arrow->setIsMarker(true);
	box->setIsMarker(true);
	circle->setIsMarker(true);
	text = new SimpleTextItem("");
	setFlag(QGraphicsItem::ItemIsSelectable);
	addToGroup(arrow);
	addToGroup(box);
	addToGroup(circle);
	addToGroup(text);
	setType(type);
	HasContainer = false;
}

QLineF MarkerItem::lineFromClick(const QPointF& Clicked, int size) {
	return QLineF(Clicked, Clicked + QPointF(size, size));
}

QRectF MarkerItem::rectFromClick(const QPointF& Clicked, int size) {
	return QRectF(Clicked-QPointF(size, size), Clicked+QPointF(size, size));
}

//MarkerItem::MarkerItem(const MarkerItem& c):
//	myType(c.myType), mode(c.mode), _isSinglePoint(c._isSinglePoint), mySize(c.mySize), 
//	clicked(c.clicked), line(c.line), rect(c.rect), 
//	arrow(c.arrow), box(c.box), circle(c.circle), text(c.text)
//{
//	setFlag(QGraphicsItem::ItemIsSelectable);
//	arrow->setIsMarker(true);
//	box->setIsMarker(true);
//	circle->setIsMarker(true);
//	addToGroup(arrow);
//	addToGroup(box);
//	addToGroup(circle);
//	addToGroup(text);
//	setType(myType);
//}

MarkerItem::~MarkerItem() {
	delete arrow;
	delete box;
	delete circle;
	delete text;
}

void MarkerItem::setType(int type) {
	myType = type;
	arrow->hide();
	box->hide();
	circle->hide();
	switch (myType) {
	case 0:
		arrow->show();
		break;
	case 1:
		box->show();
		break;
	case 2:
		circle->show();
		break;
	default:
		arrow->show();
		break;
	}
	text->setPos(calculateTextPos());
}

QPointF MarkerItem::calculateTextPos() const {
	if (myType == 0) {
		QLineF l(arrow->line());
		QRectF bound = text->boundingRect();
		int dx = l.p2().x() - l.p1().x();
		int dy = l.p2().y() - l.p1().y();
		QPointF pos = l.p2();
		QPointF offset(0, 0);
		if (dx >= 0 && dy <=0) {

		}
		else if (dx >= 0 && dy <= 0) {
			offset.setX(0);
			offset.setY(-bound.height());
		}
		else if (dx < 0 && dy > 0) {
			offset.setX(-bound.width());
			offset.setY(0);
		}
		else if (dx < 0 && dy <= 0) {
			offset.setX(-bound.width());
			offset.setY(-bound.height());
		}
		return pos + offset;
	}
	else {
		QRectF r(box->rect());
		QRectF bound = text->boundingRect();
		QPointF offset(0, 0);
		return r.normalized().bottomRight() + offset;
	}
}

void MarkerItem::setPenColor(const QColor& shape, const QColor& outline) {
	arrow->setPenColor(shape, outline);
	box->setPenColor(shape, outline);
	circle->setPenColor(shape, outline);
}

QColor MarkerItem::color1() const {
	return arrow->color1();
}

QColor MarkerItem::color2() const {
	return arrow->color2();
}

void MarkerItem::setPenWidth(int width) {
	arrow->setPenWidth(width);
	box->setPenWidth(width);
	circle->setPenWidth(width);
}

int MarkerItem::penWidth() const {
	return arrow->penWidth();
}

void MarkerItem::setSize(int s) {
	if (mode != SinglePoint)
		return;
	mySize = s;
	QLineF l(clicked, clicked+QPointF(mySize, mySize));
	QRectF r(clicked-QPointF(mySize, mySize), clicked+QPointF(mySize, mySize));
	arrow->setLine(l);
	box->setRect(r);
	circle->setRect(r);
	text->setPos(calculateTextPos());
}

int MarkerItem::size() const {
	return mySize;
}

void MarkerItem::setLabel(const QString& txt) {
	text->setText(txt);
	text->setPos(calculateTextPos());
}

QString MarkerItem::label() const {
	return text->text();
}

void MarkerItem::setFont(const QFont& font) {
	text->setFont(font);
	text->setPos(calculateTextPos());
}

QFont MarkerItem::font() const {
	return text->font();
}

void MarkerItem::setFontSize(int size) {
	text->setFontSize(size);
	text->setPos(calculateTextPos());
}

int MarkerItem::fontSize() const {
	return text->fontSize();
}

void MarkerItem::setHasBackground(bool show) {
	text->setHasBackground(show);
}

bool MarkerItem::hasBackground() const {
	return text->hasBackground();
}

//MarkerContainer
#include "RasterSettings.h"
MarkerContainer::MarkerContainer() {
	auto s = Settings("MarkerL");
	myColor1 = s.color1();
	myColor2 = s.color2();
	myPenWidth = s.penWidth();
	myFont = s.font();
	myFontSize = s.fontSize();
	mySize = s.size();
	myHasBackground = s.hasBackground();
	myType = 0;
	visible = true;
}

//MarkerContainer::MarkerContainer(const MarkerContainer& c):
//	myType(c.myType), visible(c.visible), myHasBackground(c.myHasBackground),
//	myColor1(c.myColor1), myColor2(c.myColor2), myPenWidth(c.myPenWidth), mySize(c.mySize), 
//	myLabel(c.myLabel), myFont(c.myFont), myFontSize(c.myFontSize)
//{
//	for (auto marker : c.markers) {
//		MarkerItem* m = new MarkerItem(*marker);
//		markers.append(m);
//	}
//}

void MarkerContainer::addMarker(MarkerItem* item) {
	item->setPenColor(myColor1, myColor2);
	item->setPenWidth(myPenWidth);
	item->setSize(mySize);
	item->setFont(myFont);
	item->setFontSize(myFontSize);
	item->setLabel(myLabel);
	item->setHasBackground(myHasBackground);
	item->setVisible(visible);
	item->setType(myType);
	item->setContainer(this, markers.size());
	markers.append(item);
}

void MarkerContainer::setType(int type) {
	myType = type;
	for (auto marker : markers)
		marker->setType(type);
}

void MarkerContainer::setPenColor(const QColor& shape, const QColor& outline) {
	myColor1 = shape;
	myColor2 = outline;
	for (auto marker : markers)
		marker->setPenColor(shape, outline);
}

QColor MarkerContainer::color1() const {
	return myColor1;
}

QColor MarkerContainer::color2() const {
	return myColor2;
}

void MarkerContainer::setPenWidth(int width) {
	myPenWidth = width;
	for (auto marker : markers)
		marker->setPenWidth(width);
}

int MarkerContainer::penWidth() const {
	return myPenWidth;
}

void MarkerContainer::setSize(int s) {
	mySize = s;
	for (auto marker : markers)
		marker->setSize(s);
}

int MarkerContainer::size() {
	return mySize;
}

void MarkerContainer::setLabel(const QString& txt) {
	myLabel = txt;
	for (auto marker : markers)
		marker->setLabel(txt);
}

QString MarkerContainer::label() const {
	return myLabel;
}

void MarkerContainer::setFont(const QFont& font) {
	myFont = font;
	for (auto marker : markers)
		marker->setFont(font);
}

QFont MarkerContainer::font() const {
	return myFont;
}

void MarkerContainer::setFontSize(int size) {
	myFontSize = size;
	for (auto marker : markers)
		marker->setFontSize(size);
}

int MarkerContainer::fontSize() const {
	return myFontSize;
}

void MarkerContainer::setHasBackground(bool show) {
	myHasBackground = show;
	for (auto marker : markers)
		marker->setHasBackground(show);
}

bool MarkerContainer::hasBackground() const {
	return myHasBackground;
}

void MarkerContainer::setVisible(bool show) {
	visible = show;
	for (auto marker : markers)
		marker->setVisible(show);
}

//MarkerModel
MarkerModel::MarkerModel(QObject* parent):
	QAbstractTableModel(parent)
{	
}

MarkerModel::~MarkerModel() { }

int MarkerModel::rowCount(const QModelIndex& parent) const {
	return myData.count();
}

int MarkerModel::columnCount(const QModelIndex& parent) const {
	return 10;
}

QVariant MarkerModel::data(const QModelIndex& index, int role) const {
	auto r = index.row();
	auto c = index.column();
	if (r >= myData.size()) return QVariant();
	MarkerContainer dat = *myData.at(r);
	if (Qt::CheckStateRole == role) {
		if (4 == c)
			return (dat.isVisible()) ? Qt::Checked : Qt::Unchecked;
		else if (8 == c)
			return (dat.hasBackground()) ? Qt::Checked : Qt::Unchecked;
	}
	else if (Qt::DisplayRole == role || Qt::EditRole == role) {
		switch (c) {
		case 0:
			return dat.label();
		case 1:
			return dat.count();
		case 2:
			return dat.color1();
		case 3:
			return dat.color2();
		case 4:
			return dat.isVisible();
		case 5:
			return dat.penWidth();
		case 6:
			return dat.font();
		case 7:
			return dat.fontSize();
		case 8:
			return dat.hasBackground();
		case 9:
			return dat.size();
		}
	}
	else if (Qt::DecorationRole == role) {
		if (2 == c)
			return dat.color1();
		else if (3 == c)
			return dat.color2();
	}
	else if (Qt::FontRole == role) {
		if (6 == c)
			return dat.font();
	}
	return QVariant();
}

Qt::ItemFlags MarkerModel::flags(const QModelIndex& index) const {
	int c = index.column();
	if (0 == c)
		return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	else if (1 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	else if (2 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (3 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (4 == c)
		return Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	else if (5 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (6 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (7 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (8 == c)
		return Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	else if (9 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	return 0;
}

QVariant MarkerModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (Qt::DisplayRole != role) return QVariant();
	if (Qt::Horizontal == orientation) {
		if (0 == section)
			return "Label";
		else if (1 == section)
			return "Count";
		else if (2 == section)
			return "Color1";
		else if (3 == section)
			return "Color2";
		else if (4 == section)
			return "Visible";
		else if (5 == section)
			return "Pen Width";
		else if (6 == section)
			return "Font";
		else if (7 == section)
			return "Font Size";
		else if (8 == section)
			return "Has Background";
		else if (9 == section)
			return "Size";
	}
	return QString().number(section+1);
}

bool MarkerModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	int c = index.column();
	int r = index.row();
	if (Qt::CheckStateRole == role) {
		if (4 == c)
			myData.at(r)->setVisible(value.toBool());
		else if (8 == c)
			myData.at(r)->setHasBackground(value.toBool());
		emit dataChanged(index, index);
		return true;
	}
	else if (Qt::EditRole == role) {
		switch (c) {
		case 0:
			myData.at(r)->setLabel(value.toString());
			break;
		case 1:
			return false;
		case 2:
			myData.at(r)->setColor1(qvariant_cast<QColor>(value));
			break;
		case 3:
			myData.at(r)->setColor2(qvariant_cast<QColor>(value));
			break;
		case 4:
			myData.at(r)->setVisible(value.toBool());
			break;
		case 5:
			myData.at(r)->setPenWidth(value.toInt());
			break;
		case 6:
			myData.at(r)->setFont(qvariant_cast<QFont>(value));
			break;
		case 7:
			myData.at(r)->setFontSize(value.toInt());
			break;
		case 8:
			myData.at(r)->setHasBackground(value.toBool());
			break;
		case 9:
			myData.at(r)->setSize(value.toInt());
			break;
		}
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

bool MarkerModel::insertRows(int row, int count, const QModelIndex& parent) {
	beginInsertRows(parent, row, row+count-1);
	for (int i=row; i<row+count; ++i) {
		MarkerContainer* dat = new MarkerContainer();
		auto p = myData.begin()+i;
		myData.insert(p, dat);
		dat->setIndex(i);
	}
	endInsertRows();
	for (int i=0; i<rowCount(); ++i)
		myData.at(i)->setIndex(i);
	return true;
}

bool MarkerModel::removeRows(int row, int count, const QModelIndex& parent) {
	beginRemoveRows(parent, row, row+count-1);
	for (int i=row; i<row+count; ++i) {
		myData.removeAt(i);
	}
	endRemoveRows();
	for (int i=0; i<rowCount(); ++i)
		myData.at(i)->setIndex(i);
	return true;
}

bool MarkerModel::insertMarkerContainer(MarkerContainer* c, const QModelIndex& parent) {
	int row = c->index();
	beginInsertRows(parent, row, row);
	auto p = myData.begin()+row;
	myData.insert(p, c);
	c->setIndex(row);
	endInsertRows();
	for (int i=0; i<rowCount(); ++i)
		myData.at(i)->setIndex(i);
	return true;
}

bool MarkerModel::removeMarkerContainer(MarkerContainer* c) {
	int row = c->index();
	removeRow(row);
	return true;
}

void MarkerModel::addMarker(MarkerItem* item, int row) {
	myData.at(row)->addMarker(item);
	emit dataChanged(index(row, 1), index(row, 1)); 
}

void MarkerModel::removeMarker(MarkerItem* item) {
	auto container = item->container();
	int row = container->index();
	container->removeAt(item->index());
	emit dataChanged(index(row, 1), index(row, 1)); 
}
