#include "stdafx.h"
#include "LegendItem.h"

//LegendItem
LegendItem::LegendItem(const QPointF& Clicked, int type, QGraphicsItem* parent):
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
	text = new RichTextItem("");
	text->setTextInteractionFlags(Qt::TextEditable);
	//setFlag(QGraphicsItem::ItemIsSelectable);
	addToGroup(arrow);
	addToGroup(box);
	addToGroup(circle);
	addToGroup(text);
	setType(type);
}

LegendItem::~LegendItem() {
	delete arrow;
	delete box;
	delete circle;
	delete text;
}

void LegendItem::setType(int type) {
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

QPointF LegendItem::calculateTextPos() const {
	QRectF r(box->rect());
	QRectF bound = text->boundingRect();
	QPointF offset(mySize, (r.height()-bound.height())/2);
	return r.normalized().topRight() + offset;
}

void LegendItem::setPenColor(const QColor& shape, const QColor& outline) {
	arrow->setPenColor(shape, outline);
	box->setPenColor(shape, outline);
	circle->setPenColor(shape, outline);
}

QColor LegendItem::color1() const {
	return arrow->color1();
}

QColor LegendItem::color2() const {
	return arrow->color2();
}

void LegendItem::setPenWidth(int width) {
	arrow->setPenWidth(width);
	box->setPenWidth(width);
	circle->setPenWidth(width);
}

int LegendItem::penWidth() const {
	return arrow->penWidth();
}

void LegendItem::setSize(int s) {
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

int LegendItem::size() const {
	return mySize;
}

void LegendItem::setLabel(const QString& txt) {
	text->setPlainText(txt);
	text->setPos(calculateTextPos());
}

QString LegendItem::label() const {
	return text->toPlainText();
}

void LegendItem::setFont(const QFont& font) {
	text->setFont(font);
	text->setPos(calculateTextPos());
}

QFont LegendItem::font() const {
	return text->font();
}

void LegendItem::setFontSize(int size) {
	text->setFontSize(size);
	text->setPos(calculateTextPos());
}

int LegendItem::fontSize() const {
	return text->fontSize();
}

QRectF LegendItem::getRect() {
	QPointF tl, br;
	QRectF r, t;
	r = box->boundingRect();
	t = text->boundingRect();
	if (r.height() > t.height()) {
		tl = mapToScene(r.topLeft() - QPointF(t.width() + 10, 0));
		br = mapToScene(r.bottomRight());
	}
	else {
		tl = mapToScene(t.topLeft());
		br = mapToScene(t.bottomRight() + QPointF(r.width() + 10, 0));
	}
	return QRectF(tl, br);
}

void LegendItem::moveTo(const QPointF& p) {
	QPointF pos = mapFromScene(p);
	arrow->setPos(pos);
	box->setPos(pos);
	circle->setPos(pos);
	text->setPos(calculateTextPos());
}

//LegendGroup
#include "MarkerItem.h"
#include "RasterSettings.h"
LegendGroup::LegendGroup(const QPointF& Clicked, MarkerModel* model, QGraphicsItem* parent):
	QGraphicsItemGroup(parent), origin(Clicked), model(model)
{
	mySize = 20;
	frame = new RectItem(QRectF());
	frame->setBrush(QColor(255, 255, 255, 192));
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
	addToGroup(frame);
	auto s = Settings("LegendL");
	myPenWidth = s.penWidth();
	mySize = s.size();
	myFont = s.font();
	myFontSize = s.fontSize();
	updateModel();
}

LegendGroup::~LegendGroup() {
	delete frame;
}

void LegendGroup::updateModel() {
	QList<QGraphicsItem*> pool;
	for (auto l : legends) { removeFromGroup(l); pool.append(l); }
	for (auto t : texts) { removeFromGroup(t); pool.append(t); }
	for (auto l : pool) delete l;
	legends.clear();
	texts.clear();
	for (int i=0; i<model->rowCount(); i++) {
		auto marker = model->at(i);
		QString l = marker->label() + "\t:  " + QString::number(marker->count());
		QColor color1 = marker->color1();
		QColor color2 = marker->color2();
		int type = marker->activeType();
		if (0 == type) {
			QRectF r = MarkerItem::rectFromClick(origin + QPointF(0, (mySize*2 + myPenWidth + 10) * (i+1)), mySize);
			QLineF l(r.topLeft(), r.bottomRight());
			auto marker = new ArrowItem(l);
			marker->setArrowhead(ArrowHead::Start);
			marker->setColor1(color1);
			marker->setColor2(color2);
			marker->setPenWidth(myPenWidth);
			legends.append(marker);
			addToGroup(marker);
		}
		else if (1 == type) {
			auto marker = new RectItem(MarkerItem::rectFromClick(origin + QPointF(0, (mySize*2 + myPenWidth + 10) * (i+1)), mySize));
			marker->setColor1(color1);
			marker->setColor2(color2);
			marker->setPenWidth(myPenWidth);
			legends.append(marker);
			addToGroup(marker);
		}
		else {
			auto marker = new EllipseItem(MarkerItem::rectFromClick(origin + QPointF(0, (mySize*2 + myPenWidth + 10) * (i+1)), mySize));
			marker->setColor1(color1);
			marker->setColor2(color2);
			marker->setPenWidth(myPenWidth);
			legends.append(marker);
			addToGroup(marker);
		}
		auto label = new RichTextItem(l);
		label->setFont(myFont);
		label->setFontSize(myFontSize);
		label->setKeyPressFunc([this](QKeyEvent* event) { updateRect(); });
		label->setTextInteractionFlags(Qt::TextEditorInteraction);
		label->setFlag(QGraphicsItem::ItemIsFocusable);
		QPointF textPos = legends.at(i)->boundingRect().center() + 
			QPointF(legends.at(i)->boundingRect().width()/2+20, -label->boundingRect().height()*0.5);
		label->setPos(textPos);
		label->setZValue(1000);
		texts.append(label);
		addToGroup(label);
	}
	calculatePos();
}

void LegendGroup::updateRect() {
	QPointF tl = mapFromItem(legends.at(0), legends.at(0)->boundingRect().topLeft());
	//Search for longest words:
	int count = 0;
	RichTextItem* txt;
	for (auto t : texts) {
		if (t->toPlainText().count() > count) {
			count = t->toPlainText().count();
			txt = t;
		}
	}
	QPointF br = mapFromItem(texts.at(texts.size()-1), texts.at(texts.size()-1)->boundingRect().bottomRight());
	br.setX(tl.x()+legends.at(0)->boundingRect().width()+txt->boundingRect().width()+10);
	frame->setRect(QRectF(tl + QPointF(-10, -10), br + QPointF(10, 10)));
}

void LegendGroup::calculatePos() {
	//legends.at(0)->setPos(origin);
	//QPointF tl = legends.at(0)->pos();
	//for (int i=0; i < legends.size(); ++i) {
	//	auto mark = legends.at(i);
	//	auto text = texts.at(i);
	//	int space = (mark->boundingRect().height() > text->boundingRect().height()) ? 
	//		mark->boundingRect().height()/2 : text->boundingRect().height()/2;
	//	QPointF markPos(tl + QPointF(0, i*(mySize+myPenWidth+10)+space));
	//	if (i != 0) mark->setPos(markPos);
	//	QPointF textPos = mapFromItem(mark, mark->boundingRect().topRight() + 
	//		QPointF(15, (mark->boundingRect().height()-text->boundingRect().height())/2));
	//	text->setPos(textPos);
	//}
	updateRect();
}

void LegendGroup::setPenWidth(int width) {
	myPenWidth = width;
	updateModel();
}

int LegendGroup::penWidth() const {
	return myPenWidth;
}

void LegendGroup::setSize(int s) {
	mySize = s;
	updateModel();
}

int LegendGroup::size() const {
	return mySize;
}

void LegendGroup::setFont(const QFont& font) {
	myFont = font;
	for (auto text : texts)
		text->setFont(font);
}

QFont LegendGroup::font() const {
	return myFont;
}

void LegendGroup::setFontSize(int size) {
	myFontSize = size;
	for (auto text : texts)
		text->setFontSize(size);
}

int LegendGroup::fontSize() const {
	return myFontSize;
}