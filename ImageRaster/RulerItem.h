#pragma once
#include "GraphicsItem.h"
#include "qabstractitemmodel.h"

class RulerItemInterface 
{
public:
	virtual void setColor1(const QColor& shape_color)=0;
	virtual QColor color1() const=0;
	virtual void setColor2(const QColor& outline_color)=0;
	virtual QColor color2() const=0;
	virtual void setPenWidth(int width)=0;
	virtual int penWidth() const=0;

	virtual void setText(const QString& text)=0;
	virtual QString text() const=0;

	virtual void setFont(const QFont& f)=0;
	virtual QFont font() const=0;
	virtual void setFontSize(int size)=0;
	virtual int fontSize() const=0;
	virtual void setHasBackground(bool show)=0;
	virtual bool hasBackground() const=0;
	virtual void setVisible(bool)=0;
	virtual bool isVisible() const=0;
	
	virtual ~RulerItemInterface() {}
};

template<typename R, typename B>
class RulerItem:
	public R,
	public RulerItemInterface
{
protected:
	SimpleTextItem* label;
	QRectF TextRect() const {
		return label->boundingRect();
	}
public:
	RulerItem(const B& shape, QGraphicsItem* parent=0):
		R(shape, parent), label(new SimpleTextItem("", this))
	{	}
	~RulerItem() override { }

	void setColor1(const QColor& shape_color) override { R::setPenColor(shape_color, color1()); }
	QColor color1() const override { return R::color1(); }
	void setColor2(const QColor& outline_color) override { R::setPenColor(color1(), outline_color); }
	QColor color2() const override { return R::color2(); }
	void setPenWidth(int width) override { R::setPenWidth(width); }
	int penWidth() const override { return R::penWidth(); }

	QString text() const override { return label->text(); }
	void setTextPos(const QPointF& pos) { label->setPos(pos); }

	QFont font() const override { return label->font(); }
	int fontSize() const override { return label->fontSize(); }
	void setHasBackground(bool show) override {	label->setHasBackground(show); }
	bool hasBackground() const override { return label->hasBackground(); }
	void setVisible(bool show) override { label->setVisible(show); }
	bool isVisible() const override { return label->isVisible(); }
};

class LineRuler:
	public RulerItem<ArrowItem, QLineF>
{
	QPointF calculateTextPos() const;
public:
	LineRuler(const QLineF& line, QGraphicsItem* parent=0):
		RulerItem<ArrowItem, QLineF>(line, parent) { setArrowhead(ArrowHead::Both); }
	~LineRuler() { }

	enum  { Type = UserType + 9 };
	int type() const { return Type; }

	void setText(const QString& text) override {
		label->setText(text);
		label->setPos(calculateTextPos());
	}
	void setFont(const QFont& f) override {
		label->setFont(f);
		label->setPos(calculateTextPos());
	}
	void setFontSize(int size) override {
		label->setFontSize(size);
		label->setPos(calculateTextPos());
	}
};

class RectRuler:
	public RulerItem<RectItem, QRectF>
{
	QPointF calculateTextPos() const;
public:
	RectRuler(const QRectF& rect, QGraphicsItem* parent=0):
		RulerItem<RectItem, QRectF>(rect, parent) {}
	~RectRuler() { }

	enum  { Type = UserType + 10 };
	int type() const { return Type; }

	void setText(const QString& text) override {
		label->setText(text);
		label->setPos(calculateTextPos());
	}
	void setFont(const QFont& f) override {
		label->setFont(f);
		label->setPos(calculateTextPos());
	}
	void setFontSize(int size) override {
		label->setFontSize(size);
		label->setPos(calculateTextPos());
	}
};

class CircleRuler:
	public RulerItem<EllipseItem, QRectF>
{
	QPointF calculateTextPos() const;
public:
	CircleRuler(const QRectF& rect, QGraphicsItem* parent=0):
		RulerItem<EllipseItem, QRectF>(rect, parent) {}

	enum  { Type = UserType + 29 };
	int type() const { return Type; }

	void setText(const QString& text) override {
		label->setText(text);
		label->setPos(calculateTextPos());
	}
	void setFont(const QFont& f) override {
		label->setFont(f);
		label->setPos(calculateTextPos());
	}
	void setFontSize(int size) override {
		label->setFontSize(size);
		label->setPos(calculateTextPos());
	}
};

struct Triplet {
	QPointF p1, p2, p3;
	Triplet(const QPointF& p1 = QPointF(), const QPointF& p2 = QPointF(), const QPointF& p3 = QPointF()):
		p1(p1), p2(p2), p3(p3) { }
};

class TriCircleRuler:
	public RulerItem<EllipseItem, QRectF>
{
	QPointF calculateTextPos() const;
	Triplet originalPoints;
public:
	TriCircleRuler(const QRectF& rect, QGraphicsItem* parent=0):
		RulerItem<EllipseItem, QRectF>(rect, parent) {}
	~TriCircleRuler() { }

	enum  { Type = UserType + 11 };
	int type() const { return Type; }

	void setText(const QString& text) override {
		label->setText(text);
		label->setPos(calculateTextPos());
	}
	void setFont(const QFont& f) override {
		label->setFont(f);
		label->setPos(calculateTextPos());
	}
	void setFontSize(int size) override {
		label->setFontSize(size);
		label->setPos(calculateTextPos());
	}

	void setTriplet(const QPointF& p1, const QPointF& p2, const QPointF& p3) { originalPoints = Triplet(p1, p2, p3); }
	Triplet getTriplet() const { return originalPoints; }
};

class Circle2Ruler:
	public LineRuler
{
	EllipseItem* circle1, *circle2;
	Triplet originalPoints1, originalPoints2;
public:
	Circle2Ruler(const QRectF& c1, const QRectF& c2, QGraphicsItem* parent=0);
	~Circle2Ruler() { }

	enum  { Type = UserType + 12 };
	int type() const { return Type; }


	void setRect1(const QRectF& c1);
	void setRect2(const QRectF& c2);

	void setPenColor(const QColor& shape, const QColor& outline);
	void setPenWidth(int width);

	void setTriplet1(const QPointF& p1, const QPointF& p2, const QPointF& p3) { 
		originalPoints1 = Triplet(p1, p2, p3);
	}
	void setTriplet2(const QPointF& p1, const QPointF& p2, const QPointF& p3) { 
		originalPoints2 = Triplet(p1, p2, p3);
	}
	Triplet getTriplet1() const { return originalPoints1; }
	Triplet getTriplet2() const { return originalPoints2; }
};

class PolyRuler:
	public RulerItem<PolygonItem, QPolygonF>
{
	QPointF calculateTextPos() const;
public:
	PolyRuler(const QPolygonF& poly, QGraphicsItem* parent=0):
		RulerItem<PolygonItem, QPolygonF>(poly, parent) {}
	~PolyRuler() { }

	enum  { Type = UserType + 13 };
	int type() const { return Type; }

	void setText(const QString& text) override {
		label->setText(text);
		label->setPos(calculateTextPos());
	}
	void setFont(const QFont& f) override {
		label->setFont(f);
		label->setPos(calculateTextPos());
	}
	void setFontSize(int size) override {
		label->setFontSize(size);
		label->setPos(calculateTextPos());
	}
};

class PathRuler:
	public RulerItem<MultiLineItem, QPolygonF>
{
	QPointF calculateTextPos() const;
public:
	PathRuler(const QPolygonF& poly, QGraphicsItem* parent=0):
		RulerItem<MultiLineItem, QPolygonF>(poly, parent) {}
	~PathRuler() { }

	enum  { Type = UserType + 14 };
	int type() const { return Type; }

	void setText(const QString& text) override {
		label->setText(text);
		label->setPos(calculateTextPos());
	}
	void setFont(const QFont& f) override {
		label->setFont(f);
		label->setPos(calculateTextPos());
	}
	void setFontSize(int size) override {
		label->setFontSize(size);
		label->setPos(calculateTextPos());
	}
};

struct Unit {
	enum UnitType { um, mm, cm, dm, m };
	double value;
	int power;
	QString prefix;
	bool isUnused;
	Unit(double val = 0.0, QString p = "", int pow = 1, UnitType type = um): 
		value(val), power(pow), map(type), prefix(p), isUnused(true) { }
	Unit operator=(const Unit& u) { return Unit(u); }
	double getVal() const {
		switch (map) {
		case um:
			return value/1;
		case mm:
			return value/ pow(1000, power);
		case cm:
			return value / pow(10000, power);
		case dm:
			return value/ pow(100000, power);
		case m:
			return value/ pow(1000000, power);
		default:
			return value/1;
		}
	}
	double getMod() const {
		switch (map) {
		case um:
			return 1.0;
		case mm:
			return 1000;
		case cm:
			return 10000;
		case dm:
			return 100000;
		case m:
			return 1000000;
		default:
			return 1;
		}
	}
	QString getUnit() const {
		switch (map) {
		case um:
			return QString::fromLatin1("µm");
		case mm:
			return "mm";
		case cm:
			return "cm";
		case dm:
			return "dm";
		case m:
			return "m";
		default:
			return QString::fromLatin1("µm");
		}
	}
	QString repr() {
		return QString("%1:\t%2 %3%4").arg(prefix).arg(QString::number(getVal(), 'f', 2)).arg(getUnit()).
			arg((power==2)?QString::fromLatin1("²"):(power==1)?"":QString("^%1").arg(power));
	}

	void setUnit(const UnitType& t) { map = t; }
private:
	UnitType map;
};

//RulerModel
//-----------------------------HOW TO USE----------------------------------
//Notify the model that calibration profile has changed via setRealMod slot.
//Create rulers and add it to model via addRuler, except for Circles.
//Supply 3-points for circles via createRuler, add the returned item to the scene.
class RulerModel:
	public QAbstractTableModel
{
	Q_OBJECT

protected:
	bool visible;
	int myPenWidth;
	QColor myColor1, myColor2;
	QFont myFont;
	int myFontSize;
	bool myHasBackground;
	virtual void calculate(int row)=0;		//Reimplement this!
	QString defaultText(int row) const {
		QString ntext = (data1.at(row)->isUnused) ? "" : data1.at(row)->repr();
		ntext += (data2.at(row)->isUnused) ? "" : "\n" + data2.at(row)->repr();
		ntext += (data3.at(row)->isUnused) ? "" : "\n" + data3.at(row)->repr();
		return ntext;
	}
	QList<RulerItemInterface*> rulers;
	double realWidth, realHeight;
	QList<Unit*> data1, data2, data3;
	Unit::UnitType myUnit;
public:
	RulerModel(QObject* parent=0);
	~RulerModel() {	}

	int rowCount(const QModelIndex& parent = QModelIndex()) const { return rulers.size(); }
	int columnCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool removeRuler(RulerItemInterface* r, const QModelIndex& parent = QModelIndex());

	bool insertRuler(RulerItemInterface* r, int row);

	void calculateAll() {
		for (int i=0; i<rowCount(); ++i)
			calculate(i);
	}
	//Supply calibrated value divided by scene dimension for this function!!

	void setVisible(bool show) {
		setData(index(0, 0), show);
	}
	bool isVisible() const { return visible; }
public slots:
	void setRealMod(double rWidth, double rHeight) {
		realWidth = rWidth;
		realHeight = rHeight;
		calculateAll();
	}
	bool addRuler(RulerItemInterface* r);
	void setUnit(const Unit::UnitType& unit) {
		myUnit = unit;
		for (int i=0; i<rowCount(); ++i) {
			data1.at(i)->setUnit(unit);
			data2.at(i)->setUnit(unit);
			data3.at(i)->setUnit(unit);
			setData(this->index(i, 7), defaultText(i));
		}
	}
	void textToDefault() { calculateAll(); }

	void setFont(const QFont& f);
};

#include "RasterSettings.h"
class LineRulerModel:
	public RulerModel
{
	void calculate(int row);
public:
	LineRulerModel(QObject* parent=0):
		RulerModel(parent)
	{
		auto s = Settings("Ruler0L");
		myColor1 = s.color1();
		myColor2 = s.color2();
		myPenWidth = s.penWidth();
		myFont = s.font();
		myFontSize = s.fontSize();
		myHasBackground = s.hasBackground();
	}
	~LineRulerModel() {
		auto s = Settings("Ruler0L");
		s.setColor1(myColor1);
		s.setColor2(myColor2);
		s.setPenWidth(myPenWidth);
		s.setFont(myFont);
		s.setFontSize(myFontSize);
		s.setHasBackground(myHasBackground);
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addRuler(LineRuler* r) { return RulerModel::addRuler((RulerItemInterface*)r); }
};

class RectRulerModel:
	public RulerModel
{
	void calculate(int row);
public:
	RectRulerModel(QObject* parent=0):
		RulerModel(parent)
	{
		auto s = Settings("Ruler1L");
		myColor1 = s.color1();
		myColor2 = s.color2();
		myPenWidth = s.penWidth();
		myFont = s.font();
		myFontSize = s.fontSize();
		myHasBackground = s.hasBackground();
	}
	~RectRulerModel() {
		auto s = Settings("Ruler1L");
		s.setColor1(myColor1);
		s.setColor2(myColor2);
		s.setPenWidth(myPenWidth);
		s.setFont(myFont);
		s.setFontSize(myFontSize);
		s.setHasBackground(myHasBackground);
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addRuler(RectRuler* r) { return RulerModel::addRuler((RulerItemInterface*)r) ; }
};

class CircleRulerModel:
	public RulerModel
{
	void calculate(int row);
public:
	CircleRulerModel(QObject* parent=0):
		RulerModel(parent)
	{
		auto s = Settings("Ruler9L");
		myColor1 = s.color1();
		myColor2 = s.color2();
		myPenWidth = s.penWidth();
		myFont = s.font();
		myFontSize = s.fontSize();
		myHasBackground = s.hasBackground();
	}
	~CircleRulerModel() {
		auto s = Settings("Ruler9L");
		s.setColor1(myColor1);
		s.setColor2(myColor2);
		s.setPenWidth(myPenWidth);
		s.setFont(myFont);
		s.setFontSize(myFontSize);
		s.setHasBackground(myHasBackground);
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addRuler(CircleRuler* r) { return RulerModel::addRuler((RulerItemInterface*)r) ; }
};

class TriCircleRulerModel:
	public RulerModel
{
	void calculate(int row);
public:
	TriCircleRulerModel(QObject* parent=0):
		RulerModel(parent)
	{
		auto s = Settings("Ruler2L");
		myColor1 = s.color1();
		myColor2 = s.color2();
		myPenWidth = s.penWidth();
		myFont = s.font();
		myFontSize = s.fontSize();
		myHasBackground = s.hasBackground();
	}
	~TriCircleRulerModel() {
		auto s = Settings("Ruler2L");
		s.setColor1(myColor1);
		s.setColor2(myColor2);
		s.setPenWidth(myPenWidth);
		s.setFont(myFont);
		s.setFontSize(myFontSize);
		s.setHasBackground(myHasBackground);
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addRuler(TriCircleRuler* r) { return RulerModel::addRuler((RulerItemInterface*)r) ; }

	TriCircleRuler* createRuler(const QPointF& p1, const QPointF& p2, const QPointF& p3); //add it to the scene later
	TriCircleRuler* createRuler(const QPointF& p1, const QPointF& p2);
};

class Circle2RulerModel:
	public RulerModel
{
	void calculate(int row);
public:
	Circle2RulerModel(QObject* parent=0):
		RulerModel(parent)
	{
		auto s = Settings("Ruler3L");
		myColor1 = s.color1();
		myColor2 = s.color2();
		myPenWidth = s.penWidth();
		myFont = s.font();
		myFontSize = s.fontSize();
		myHasBackground = s.hasBackground();
	}
	~Circle2RulerModel() {
		auto s = Settings("Ruler3L");
		s.setColor1(myColor1);
		s.setColor2(myColor2);
		s.setPenWidth(myPenWidth);
		s.setFont(myFont);
		s.setFontSize(myFontSize);
		s.setHasBackground(myHasBackground);
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addRuler(Circle2Ruler* r) { return RulerModel::addRuler((RulerItemInterface*)r) ; }

	Circle2Ruler* createRuler(const QPointF& p1, const QPointF& p2, const QPointF& p3,
		const QPointF& p4, const QPointF& p5, const QPointF& p6); //add it to the scene later
};

class PolyRulerModel:
	public RulerModel
{
	void calculate(int row);
public:
	PolyRulerModel(QObject* parent=0):
		RulerModel(parent)
	{
		auto s = Settings("Ruler4L");
		myColor1 = s.color1();
		myColor2 = s.color2();
		myPenWidth = s.penWidth();
		myFont = s.font();
		myFontSize = s.fontSize();
		myHasBackground = s.hasBackground();
	}
	~PolyRulerModel() {
		auto s = Settings("Ruler4L");
		s.setColor1(myColor1);
		s.setColor2(myColor2);
		s.setPenWidth(myPenWidth);
		s.setFont(myFont);
		s.setFontSize(myFontSize);
		s.setHasBackground(myHasBackground);
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addRuler(PolyRuler* r) { return RulerModel::addRuler((RulerItemInterface*)r) ; }
};

class PathRulerModel:
	public RulerModel
{
	void calculate(int row);
public:
	PathRulerModel(QObject* parent=0):
		RulerModel(parent)
	{
		auto s = Settings("Ruler5L");
		myColor1 = s.color1();
		myColor2 = s.color2();
		myPenWidth = s.penWidth();
		myFont = s.font();
		myFontSize = s.fontSize();
		myHasBackground = s.hasBackground();
	}
	~PathRulerModel() {
		auto s = Settings("Ruler5L");
		s.setColor1(myColor1);
		s.setColor2(myColor2);
		s.setPenWidth(myPenWidth);
		s.setFont(myFont);
		s.setFontSize(myFontSize);
		s.setHasBackground(myHasBackground);
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	bool addRuler(PathRuler* r) { return RulerModel::addRuler((RulerItemInterface*)r) ; }
};