#include "stdafx.h"
#include "RulerItem.h"
#include <math.h>

QPointF LineRuler::calculateTextPos() const {
	QRectF bound = TextRect();
	//int dx = line().p2().x() - line().p1().x();
	//int dy = line().p2().y() - line().p1().y();
	//QPointF pos = boundingRect().bottomLeft() + QPointF(boundingRect().width()/2, 0);
	//QPointF offset = QPointF(-bound.width()/2, 0);
	//if (line().length() > bound.width()) {
	//	label->setTransformOriginPoint(bound.center());
	//	label->setRotation(M_PI-line().angle());
	//	pos = boundingRect().center();
	//}
	//if (line().length() < bound.width()) {
	//	pos = boundingRect().bottomLeft() + QPointF(boundingRect().width()/2, 0);
	//	offset = QPointF(-bound.width()/2, 0);
	//}
	auto pos = boundingRect().center();
	auto offset = QPointF(-1.0 * bound.width() / 2, -1.0 * bound.height() / 2);
	return pos + offset;
}

QPointF RectRuler::calculateTextPos() const {
	QPointF bl = boundingRect().bottomLeft();
	bl.setX(bl.x() + boundingRect().width()/2 - TextRect().width()/2);
	bl.setY(bl.y() + 10);
	return bl;
}

QPointF CircleRuler::calculateTextPos() const {
	QPointF bl = boundingRect().bottomLeft();
	bl.setX(bl.x() + boundingRect().width()/2 - TextRect().width()/2);
	bl.setY(bl.y() + 10);
	return bl;
}

QPointF TriCircleRuler::calculateTextPos() const {
	QPointF bl = boundingRect().bottomLeft();
	bl.setX(bl.x() + boundingRect().width()/2 - TextRect().width()/2);
	bl.setY(bl.y() + 10);
	return bl;
}

QPointF PolyRuler::calculateTextPos() const {
	QPointF bl = boundingRect().bottomLeft();
	bl.setX(bl.x() + boundingRect().width()/2 - TextRect().width()/2);
	bl.setY(bl.y() + 10);
	return bl;
}

QPointF PathRuler::calculateTextPos() const {
	QPointF bl = boundingRect().bottomLeft();
	bl.setX(bl.x() + boundingRect().width()/2 - TextRect().width()/2);
	bl.setY(bl.y() + 10);
	return bl;
}

Circle2Ruler::Circle2Ruler(const QRectF& c1, const QRectF& c2, QGraphicsItem* parent):
	LineRuler(QLineF(c1.center(), c2.center()), parent)
{
	circle1 = new EllipseItem(c1, this);
	circle2 = new EllipseItem(c2, this);
}

void Circle2Ruler::setRect1(const QRectF& c1) {
	circle1->setRect(c1);
	setLine(QLineF(circle1->rect().center(), circle2->rect().center()));
}

void Circle2Ruler::setRect2(const QRectF& c2) {
	circle2->setRect(c2);
	setLine(QLineF(circle1->rect().center(), circle2->rect().center()));
}

void Circle2Ruler::setPenColor(const QColor& shape, const QColor& outline) {
	LineRuler::setPenColor(shape, outline);
	circle1->setPenColor(shape, outline);
	circle2->setPenColor(shape, outline);
}

void Circle2Ruler::setPenWidth(int width) {
	LineRuler::setPenWidth(width);
	circle1->setPenWidth(width);
	circle2->setPenWidth(width);
}

//RulerModel
Unit operator*(const Unit& u, const Unit& v) {
	Unit res = u;
	res.value *= v.value;
	res.power += v.power;
	return res;
}
Unit operator*(const Unit& u, double scalar) {
	Unit res = u;
	res.value *= scalar;
	return res;
}
Unit operator/(const Unit& u, const Unit& v) {
	Unit res = u;
	res.value /= v.value;
	res.power -= v.power;
	return res;
}
Unit operator/(const Unit& u, double scalar) {
	Unit res = u;
	res.value /= scalar;
	return res;
}

RulerModel::RulerModel(QObject* parent):
	QAbstractTableModel(parent) 
{
}

int RulerModel::columnCount(const QModelIndex& parent) const {
	return 12;
}

QVariant RulerModel::data(const QModelIndex& index, int role) const {
	auto r = index.row();
	auto c = index.column();
	if (r >= rulers.size()) return QVariant();
	if (Qt::CheckStateRole == role) {
		if (6 == c)
			return (myHasBackground) ? Qt::Checked : Qt::Unchecked;
	}
	else if (Qt::DisplayRole == role || Qt::EditRole == role) {
		switch (c) {
		case 0:
			return visible;
		case 1:
			return myColor1;
		case 2:
			return myColor2;
		case 3:
			return myPenWidth;
		case 4:
			return myFont;
		case 5:
			return myFontSize;
		case 6:
			return myHasBackground;
		case 7:
			return rulers.at(r)->text();
		case 8:
			return data1.at(r)->value;
		case 9:
			return data2.at(r)->value;
		case 10:
			return data3.at(r)->value;
		case 11:
			return data1.at(r)->getUnit();
		}
	}
	else if (Qt::DecorationRole == role) {
		if (1 == c)
			return myColor1;
		else if (2 == c)
			return myColor2;
	}
	else if (Qt::FontRole == role) {
		if (4 == c)
			return myFont;
	}
	return QVariant();
}

Qt::ItemFlags RulerModel::flags(const QModelIndex& index) const {
	int c = index.column();
	if (0 == c)
		return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
	else if (1 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (2 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (3 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (4 == c)
		return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	else if (5 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (6 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
	else if (7 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else if (8 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	else if (9 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	else if (10 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	else if (11 == c)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return 0;
}

QVariant RulerModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (Qt::DisplayRole != role) return QVariant();
	if (Qt::Horizontal == orientation) {
		if (0 == section)
			return "Visible";
		else if (1 == section)
			return "Color1";
		else if (2 == section)
			return "Color2";
		else if (3 == section)
			return "Pen Width";
		else if (4 == section)
			return "Font";
		else if (5 == section)
			return "Font Size";
		else if (6 == section)
			return "Has Background";
		else if (7 == section)
			return "Text";
		else if (11 == section)
			return "Unit";
	}
	return QString().number(section+1);
}

bool RulerModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	auto r = index.row();
	auto c = index.column();
	if (Qt::CheckStateRole == role) {
		if (6 == c) {
			rulers.at(r)->setHasBackground(value.toBool());
			myHasBackground = value.toBool();
		}
		else if (0 == c) {
			rulers.at(r)->setVisible(value.toBool());
			visible = value.toBool();
		}
		emit dataChanged(index, index);
		return true;
	}
	else if (Qt::DisplayRole == role || Qt::EditRole == role) {
		switch (c) {
		case 0:
			for (auto ruler : rulers)
				ruler->setVisible(value.toBool());
			visible = value.toBool();
			break;
		case 1:
			for (auto ruler : rulers)
				ruler->setColor1(qvariant_cast<QColor>(value));
			myColor1 = qvariant_cast<QColor>(value);
			break;
		case 2:
			for (auto ruler : rulers)
				ruler->setColor2(qvariant_cast<QColor>(value));
			myColor2 = qvariant_cast<QColor>(value);
			break;
		case 3:
			for (auto ruler : rulers)
				ruler->setPenWidth(value.toInt());
			myPenWidth = value.toInt();
			break;
		case 4:
			myFont = qvariant_cast<QFont>(value);
			myFont.setPointSize(myFontSize);
			for (auto ruler : rulers)
				ruler->setFont(myFont);
			break;
		case 5:
			for (auto ruler : rulers)
				ruler->setFontSize(value.toInt());
			myFontSize = value.toInt();
			break;
		case 6:
			for (auto ruler : rulers)
				ruler->setHasBackground(value.toBool());
			myHasBackground = value.toBool();
			break;
		case 7:
			rulers.at(r)->setText(value.toString());
			break;
		case 8:
			data1.at(r)->value = value.toDouble();
			setData(this->index(r, 7), defaultText(r));
			break;
		case 9:
			data2.at(r)->value = value.toDouble();
			setData(this->index(r, 7), defaultText(r));
			break;
		case 10:
			data3.at(r)->value = value.toDouble();
			setData(this->index(r, 7), defaultText(r));
			break;
		}
		emit dataChanged(index, index);
		return true;
	}
	return false;
}

bool RulerModel::insertRows(int row, int count, const QModelIndex& parent) {
	//Don't use this
	return false;
}

bool RulerModel::removeRows(int row, int count, const QModelIndex& parent) {
	beginRemoveRows(parent, row, row+count-1);
	for (int i=row; i<row+count; ++i) {
		auto p = rulers.begin()+i;
		rulers.erase(p);
		auto h = data1.begin()+i;
		data1.erase(h);
		auto w = data2.begin()+i;
		data2.erase(w);
		auto r = data3.begin()+i;
		data3.erase(r);
	}
	endRemoveRows();
	return true;
}

bool RulerModel::removeRuler(RulerItemInterface* ruler, const QModelIndex& parent) {
	int pos;
	bool success = false;
	for (int i=0; i < rowCount(); ++i) {
		auto p = rulers.at(i);
		if (ruler == p) {
			pos = i;
			success = true;
		}
	}
	if (!success) return false;
	beginRemoveRows(parent, pos, pos);
	rulers.removeAt(pos);
	endRemoveRows();
	return true;
}

bool RulerModel::addRuler(RulerItemInterface* r) {
	r->setColor1(myColor1);
	r->setColor2(myColor2);
	r->setPenWidth(myPenWidth);
	r->setFont(myFont);
	r->setFontSize(myFontSize);
	r->setHasBackground(myHasBackground);
	r->setVisible(visible);
	int row = rowCount();
	beginInsertRows(QModelIndex(), row, row);
	rulers.append(r);
	Unit *d1(new Unit()), *d2(new Unit()), *d3(new Unit());
	d1->setUnit(myUnit);
	d2->setUnit(myUnit);
	d3->setUnit(myUnit);
	data1.append(d1);
	data2.append(d2);
	data3.append(d3);
	calculate(row);
	endInsertRows();
	return true;
}

bool RulerModel::insertRuler(RulerItemInterface* r, int row) {
	//Use this
	r->setColor1(myColor1);
	r->setColor2(myColor2);
	r->setPenWidth(myPenWidth);
	r->setFont(myFont);
	r->setFontSize(myFontSize);
	r->setHasBackground(myHasBackground);
	r->setVisible(visible);
	beginInsertRows(QModelIndex(), row, row);
	auto p = rulers.begin() + row;
	rulers.insert(p, r);
	Unit *d1(new Unit()), *d2(new Unit()), *d3(new Unit());
	auto dd1 = data1.begin() + row;
	auto dd2 = data2.begin() + row;
	auto dd3 = data3.begin() + row;
	data1.insert(dd1, d1);
	data2.insert(dd2, d2);
	data3.insert(dd3, d3);
	calculate(row);
	endInsertRows();
	return true;
}

void RulerModel::setFont(const QFont& f)
{
	if (myFont != f)
		setData(index(0, 4), f, Qt::EditRole);
}

//LineRulerModel
void LineRulerModel::calculate(int row) {
	LineRuler* r = (LineRuler*)rulers.at(row);
	QLineF l = r->line();
	data1.at(row)->prefix = "Length";
	data1.at(row)->isUnused = false;
	setData(index(row, 8), sqrt(pow(l.dx()*realWidth, 2)+pow(l.dy()*realHeight, 2)));
}

QVariant LineRulerModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (Qt::DisplayRole != role) return QVariant();
	if (Qt::Horizontal == orientation) {
		if (8 == section)
			return "Length " + QString::fromLatin1("(µm)");
		else if (9 == section)
			return "UNUSED";
		else if (10 == section)
			return "UNUSED";
	}
	return RulerModel::headerData(section, orientation, role);
}

//RectRulerModel
void RectRulerModel::calculate(int row) {
	auto r = (RectRuler*)rulers.at(row);
	auto l = r->rect();
	data1.at(row)->prefix = "Widht";
	data1.at(row)->isUnused = false;
	data2.at(row)->prefix = "Height";
	data2.at(row)->isUnused = false;
	setData(index(row, 8), l.width()*realWidth);
	setData(index(row, 9), l.height()*realHeight);
	Unit u = *data1.at(row) * *data2.at(row);
	data3.at(row)->power = u.power;
	data3.at(row)->prefix = "Area";
	data3.at(row)->isUnused = false;
	setData(index(row, 10), u.value);
}

QVariant RectRulerModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (Qt::DisplayRole != role) return QVariant();
	if (Qt::Horizontal == orientation) {
		if (8 == section)
			return "Width " + QString::fromLatin1("(µm)");
		else if (9 == section)
			return "Height " + QString::fromLatin1("(µm)");
		else if (10 == section)
			return "Area " + QString::fromLatin1("(µm²)");
	}
	return RulerModel::headerData(section, orientation, role);
}

//CircleRulerModel
void CircleRulerModel::calculate(int row) {
	auto r = (CircleRuler*)rulers.at(row);
	auto l = r->rect();
	data1.at(row)->prefix = "Diameter";
	data1.at(row)->isUnused = false;
	data3.at(row)->isUnused = true;
	setData(index(row, 8), l.width()*realWidth);
	Unit u = 3.14 * 0.25 * *data1.at(row) * *data1.at(row);
	data2.at(row)->power = 2;
	data2.at(row)->prefix = "Area";
	data2.at(row)->isUnused = false;
	setData(index(row, 9), u.value);
}

QVariant CircleRulerModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (Qt::DisplayRole != role) return QVariant();
	if (Qt::Horizontal == orientation) {
		if (8 == section)
			return "Diameter " + QString::fromLatin1("(µm)");
		else if (9 == section)
			return "Area " + QString::fromLatin1("(µm²)");
	}
	return RulerModel::headerData(section, orientation, role);
}


//TriCircleRulerModel
QPointF operator*(const QPointF& point, const QPointF& multiplier) {
	QPointF res;
	res.setX(point.x()*multiplier.x());
	res.setY(point.y()*multiplier.y());
	return res;
}

QPointF operator/(const QPointF& point, const QPointF& multiplier) {
	QPointF res;
	res.setX(point.x()/multiplier.x());
	res.setY(point.y()/multiplier.y());
	return res;
}

QRectF rectFrom3Point(const QPointF& p1, const QPointF& p2, const QPointF& p3) {
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

void TriCircleRulerModel::calculate(int row) {
	auto cr = (TriCircleRuler*)rulers.at(row);
	auto tr = cr->getTriplet();
	//Convert to real coordinate
	QPointF realDimension = QPointF(realWidth, realHeight);
	auto pa = tr.p1 * realDimension;
	auto pb = tr.p2 * realDimension;
	auto pc = tr.p3 * realDimension;

	auto r_circle = rectFrom3Point(pa, pb, pc);
	double r_diameter = r_circle.width();
	double r_area = 3.14 * 0.25 * pow(r_diameter, 2);
	
	//Reconvert it to pixel coordinate and draw it
	QPointF center = r_circle.center() / realDimension;
	double may = (0.5*r_diameter)/realWidth;
	double min = (0.5*r_diameter)/realHeight;
	QPointF TL = center - QPointF(may, min);
	QRectF circle = QRectF(TL, QSizeF(may*2, min*2));
	
	cr->setRect(circle);
	data1.at(row)->prefix = "Diameter";
	data1.at(row)->isUnused = false;
	data2.at(row)->prefix = "Area";
	data2.at(row)->power = 2;
	data2.at(row)->isUnused = false;
	setData(index(row, 8), r_diameter);
	setData(index(row, 9), r_area);
}

QVariant TriCircleRulerModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (Qt::DisplayRole != role) return QVariant();
	if (Qt::Horizontal == orientation) {
		if (8 == section)
			return "Diameter " + QString::fromLatin1("(µm)");
		else if (9 == section)
			return "Area " + QString::fromLatin1("(µm²)");
		else if (10 == section)
			return "UNUSED";
	}
	return RulerModel::headerData(section, orientation, role);
}

TriCircleRuler* TriCircleRulerModel::createRuler(const QPointF& p1, const QPointF& p2, const QPointF& p3) {
	auto cr = new TriCircleRuler(rectFrom3Point(p1, p2, p3));
	cr->setTriplet(p1, p2, p3);
	return cr;
}

TriCircleRuler* TriCircleRulerModel::createRuler(const QPointF& p1, const QPointF& p2)
{
	return nullptr;
}

//Circle2RulerModel
void Circle2RulerModel::calculate(int row) {
	auto cr = (Circle2Ruler*)rulers.at(row);
	auto tr1 = cr->getTriplet1();
	//Convert to real coordinate
	QPointF realDimension = QPointF(realWidth, realHeight);
	auto pa = tr1.p1 * realDimension;
	auto pb = tr1.p2 * realDimension;
	auto pc = tr1.p3 * realDimension;

	auto r_circle1 = rectFrom3Point(pa, pb, pc);
	double r_diameter1 = r_circle1.width();
	
	//Reconvert it to pixel coordinate and draw it
	QPointF center1 = r_circle1.center() / realDimension;
	double may1 = (0.5*r_diameter1)/realWidth;
	double min1 = (0.5*r_diameter1)/realHeight;
	QPointF TL1 = center1 - QPointF(may1, min1);
	QRectF circle1 = QRectF(TL1, QSizeF(may1*2, min1*2));
	cr->setRect1(circle1);

	auto tr2 = cr->getTriplet2();
	//Convert to real coordinate
	auto pd = tr2.p1 * realDimension;
	auto pe = tr2.p2 * realDimension;
	auto pf = tr2.p3 * realDimension;

	auto r_circle2 = rectFrom3Point(pd, pe, pf);
	double r_diameter2 = r_circle2.width();
	
	//Reconvert it to pixel coordinate and draw it
	auto center2 = r_circle2.center() / realDimension;
	double may2 = (0.5*r_diameter2)/realWidth;
	double min2 = (0.5*r_diameter2)/realHeight;
	auto TL2 = center2 - QPointF(may2, min2);
	QRectF circle2 = QRectF(TL2, QSizeF(may2*2, min2*2));
	cr->setRect2(circle2);

	QLineF l = cr->line();
	data1.at(row)->prefix = "Distance";
	data1.at(row)->isUnused = false;
	setData(index(row, 8), sqrt(pow(l.dx()*realWidth, 2)+pow(l.dy()*realHeight, 2)));
}

QVariant Circle2RulerModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (Qt::DisplayRole != role) return QVariant();
	if (Qt::Horizontal == orientation) {
		if (8 == section)
			return "Distance " + QString::fromLatin1("(µm)");
		else if (9 == section)
			return "UNUSED";
		else if (10 == section)
			return "UNUSED";
	}
	return RulerModel::headerData(section, orientation, role);
}

Circle2Ruler* Circle2RulerModel::createRuler(const QPointF& p1, const QPointF& p2, const QPointF& p3,
						  const QPointF& p4, const QPointF& p5, const QPointF& p6) 
{
	auto cr = new Circle2Ruler(rectFrom3Point(p1, p2, p3), rectFrom3Point(p4, p5, p6));
	cr->setTriplet1(p1, p2, p3);
	cr->setTriplet2(p4, p5, p6);
	return cr;
}

//PolyRulerModel
void PolyRulerModel::calculate(int row) {
	auto r = (PolyRuler*)rulers.at(row);
	auto poly = r->polygon();
	//Translate coordinate:
	QList<double> xs, ys;
	for (int i=0; i < poly.count(); ++i) {
		auto point = poly.at(i);
		poly[i] = QPointF(point.x() * realWidth, point.y() * realHeight);
	}
	//Calculate area:
	double sum1 = 0, sum2 = 0;
	for (int i=0; i<poly.count()-1; ++i) {
		sum1 += poly.at(i).x() * poly.at(i+1).y();
		sum2 += poly.at(i).y() * poly.at(i+1).x();
	}
	double r_area = abs(sum1 - sum2) / 2.0;

	//Calculate perimeter:
	double r_perimeter = 0.0;
	auto begin = poly.begin();
	QPointF last = *begin;
	for (auto i=poly.begin()+1; i!=poly.end(); ++i) {
		auto now = *i;
		QLineF line(last, now);
		r_perimeter += line.length();
		last = now;
	}

	data1.at(row)->prefix = "Area";
	data1.at(row)->isUnused = false;
	data1.at(row)->power = 2;
	data2.at(row)->prefix = "Perimeter";
	data2.at(row)->isUnused = false;
	setData(index(row, 8), r_area);
	setData(index(row, 9), r_perimeter);
}

QVariant PolyRulerModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (Qt::DisplayRole != role) return QVariant();
	if (Qt::Horizontal == orientation) {
		if (8 == section)
			return "Area " + QString::fromLatin1("(µm²)");
		else if (9 == section)
			return "Perimeter " + QString::fromLatin1("(µm)");
		else if (10 == section)
			return "UNUSED";
	}
	return RulerModel::headerData(section, orientation, role);
}

//PathRulerModel
void PathRulerModel::calculate(int row) {
	auto r = (PathRuler*)rulers.at(row);
	auto poly = r->polygon();
	//Translate coordinate:
	QList<double> xs, ys;
	for (int i=0; i < poly.count(); ++i) {
		auto point = poly.at(i);
		poly[i] = QPointF(point.x() * realWidth, point.y() * realHeight);
	}

	//Calculate perimeter:
	double r_perimeter = 0.0;
	auto begin = poly.begin();
	QPointF last = *begin;
	for (auto i=poly.begin()+1; i!=poly.end(); ++i) {
		auto now = *i;
		QLineF line(last, now);
		r_perimeter += line.length();
		last = now;
	}

	data1.at(row)->prefix = "Path";
	data1.at(row)->isUnused = false;
	setData(index(row, 8), r_perimeter);
}

QVariant PathRulerModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (Qt::DisplayRole != role) return QVariant();
	if (Qt::Horizontal == orientation) {
		if (8 == section)
			return "Path Length " + QString::fromLatin1("(µm)");
		else if (9 == section)
			return "UNUSED";
		else if (10 == section)
			return "UNUSED";
	}
	return RulerModel::headerData(section, orientation, role);
}