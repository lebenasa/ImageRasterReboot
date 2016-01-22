#include "stdafx.h"
#include "Logic.h"
#include "GraphicsScene.h"
#include "Docks.h"

//Marker
#include "MarkerItem.h"
MarkerLogic::MarkerLogic(Logic* logic):
	logic(logic), model(new MarkerModel(this)), mapper(new QDataWidgetMapper(this))
{
	mapper->setModel(model);
	connect(mapper, &QDataWidgetMapper::currentIndexChanged, this, &MarkerLogic::indexChanged);
}

void MarkerLogic::addMarker(MarkerItem* item) {
	model->addMarker(item, mapper->currentIndex());
}

void MarkerLogic::popMarker(int row) {
	model->popMarker(row);
}

void MarkerLogic::setType(int type) {
	model->setCurrentType(type, mapper->currentIndex());
}

int MarkerLogic::currentType() const {
	return model->currentType(mapper->currentIndex());
}

int MarkerLogic::count() const 
{ 
	return model->rowCount(); 
}

MarkerContainer* MarkerLogic::at(int i) { 
	return model->at(i); 
}

void MarkerLogic::addMarkerContainer() {
	model->insertRow(model->rowCount());
	mapper->toLast();
}

void MarkerLogic::insertMarkerContainer(MarkerContainer* container) {
	model->insertMarkerContainer(container);
	mapper->toLast();
}

void MarkerLogic::removeMarkerContainer(int row) {
	model->removeRow(row);
}

//Calibration
ProfileLogic::ProfileLogic(Logic* logic)
	: QObject(logic), logic(logic), settings(this)
{
	model = new ProfileModel(this);
	mapper = new QDataWidgetMapper(this);
	mapper->setModel(model);
	mapper->toFirst();
	auto updater = [this](int i){ emit currentProfileChanged(model->at(i).width, model->at(i).height); };
	auto updater2 = [this]() { emit currentProfileChanged(model->at(mapper->currentIndex()).width, model->at(mapper->currentIndex()).height); };
	connect(mapper, &QDataWidgetMapper::currentIndexChanged, updater);
	connect(model, &ProfileModel::dataChanged, updater2);
	profileSync = new QTimer(this);
	profileSync->setInterval(15000);
	connect(profileSync, &QTimer::timeout, model, &ProfileModel::refreshProfile);
	//profileSync->start();
}

void ProfileLogic::appendProfile() {
	Profile prof;
	prof.name = "empty";
	model->insertProfile(model->rowCount(), prof);
}

void ProfileLogic::removeProfile() {
	if (QMessageBox::warning(logic->ui, tr("Delete Profile"), tr("Are you sure? This action is not undoable."),
		QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok) 
	{
		if (model->rowCount() == 1)
			appendProfile();
		model->removeRow(mapper->currentIndex());
	}
}

void ProfileLogic::calibrateProfile() {
	CalibrationWizard *wiz = new CalibrationWizard(logic->myImageName, logic->ui);
	wiz->setWindowTitle("Calibration Wizard - " + model->data(model->index(mapper->currentIndex(), 0)).toString());
	if (wiz->exec() == QDialog::Accepted) {
		model->setData(model->index(mapper->currentIndex(), 1), wiz->XResult());
		model->setData(model->index(mapper->currentIndex(), 2), wiz->YResult());
		Settings("Global").setCustom("_FirstTime", false);
	}
}

void ProfileLogic::resetProfile() {
	if (QMessageBox::warning(logic->ui, tr("Delete Profile"), tr("Are you sure? This action is not undoable."),
		QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok)
	{
		model->resetProfile();
	}
}

//Ruler
RulerLogic::RulerLogic(Logic* logic):
	QObject(logic), logic(logic)
{
	lr = new LineRulerModel(this);
	rr = new RectRulerModel(this);
	c1 = new CircleRulerModel(this);
	cr = new TriCircleRulerModel(this);
	c2 = new Circle2RulerModel(this);
	pr = new PolyRulerModel(this);
	mr = new PathRulerModel(this);
	mapLR = new QDataWidgetMapper(this);
	mapLR->setModel(lr);
	mapRR = new QDataWidgetMapper(this);
	mapRR->setModel(rr);
	mapCR = new QDataWidgetMapper(this);
	mapCR->setModel(cr);
	mapC2 = new QDataWidgetMapper(this);
	mapC2->setModel(c2);
	mapPR = new QDataWidgetMapper(this);
	mapPR->setModel(pr);
	mapMR = new QDataWidgetMapper(this);
	mapMR->setModel(mr);
	mapC1 = new QDataWidgetMapper(this);
	mapC1->setModel(c1);

}

void RulerLogic::setRealMod(double rw, double rh) {
	lr->setRealMod(rw, rh); 
	rr->setRealMod(rw, rh); 
	c1->setRealMod(rw, rh);
	cr->setRealMod(rw, rh); 
	c2->setRealMod(rw, rh); 
	pr->setRealMod(rw, rh);
	mr->setRealMod(rw, rh);
}

void RulerLogic::setUnit(int unit) {
	lr->setUnit((Unit::UnitType)unit);
	rr->setUnit((Unit::UnitType)unit);
	c1->setUnit((Unit::UnitType)unit);
	cr->setUnit((Unit::UnitType)unit);
	c2->setUnit((Unit::UnitType)unit);
	pr->setUnit((Unit::UnitType)unit);
	mr->setUnit((Unit::UnitType)unit);
}

RulerModel* RulerLogic::modelAt(int RulerItemType) {
	switch (RulerItemType) {
	case 9:
		return lr;
	case 10:
		return rr;
	case 11:
		return cr;
	case 12:
		return c2;
	case 13:
		return pr;
	case 14:
		return mr;
	case 29:
		return c1;
	default:
		return lr;
	}
}

//Scale
ScaleLogic::ScaleLogic(Logic* logic):
QObject(logic), logic(logic), scene(logic->getScene()), visible{ false }
{
	hs1 = new SimpleScale(QLineF());
	vs1 = new SimpleScale(QLineF());
	hs2 = new BarScale(QLineF());
	vs2 = new BarScale(QLineF());
	hs3 = new RulerScale(QLineF());
	vs3 = new RulerScale(QLineF());
	hs << hs1 << hs2 << hs3;
	vs << vs1 << vs2 << vs3;
	for (auto s : hs)
		scene->addItem(s);
	for (auto s : vs)
		scene->addItem(s);
	QString arg = QString("ScaleL");
	auto s = Settings(arg);
	unit.value = 100.0;
	myPenWidth = s.penWidth();
	myFont = s.font();
	myFontSize = s.fontSize();
	scaleType = 0;
}

ScaleLogic::~ScaleLogic() {
}

void ScaleLogic::showScale(int h, int v) {
	visible = true;
	updateHScale(h);
	updateVScale(v);
	scene->update();
}

void ScaleLogic::updateHScale(int h) {
	hI = h;
	for (auto s : hs)
		s->setVisible(true);
	hs1->setLine(hLine(hI));
	hs2->setLine(hLine(hI));
	hs3->setLine(hLine(hI));
	updateStyle();
}

void ScaleLogic::updateVScale(int v) {
	vI = v;
	for (auto s : vs)
		s->setVisible(true);
	vs1->setLine(vLine(vI));
	vs2->setLine(vLine(vI));
	vs3->setLine(vLine(vI));
	updateStyle();
}

void ScaleLogic::updateStyle()
{
	setPenWidth(myPenWidth);
	setFont(myFont);
	setFontSize(myFontSize);
	setStyle(scaleType);
}

void ScaleLogic::setPenWidth(int width) {
	myPenWidth = width;
	for (auto s : hs) s->setPenWidth(width);
	for (auto s : vs) s->setPenWidth(width);
}

void ScaleLogic::setLength(const QString& txt) {
	double l = txt.toDouble();
	unit.value = l;
	for (auto s : hs)
		s->setText2(txt + " " + unit.getUnit());
	for (auto s : vs)
		s->setText2(txt + " " + unit.getUnit());
	updateHScale(hI);
	updateVScale(vI);
	scene->update();

	if (!isVisible())
		hideScale();
}

void ScaleLogic::setFont(const QFont& font) {
	myFont = font;
	for (auto s : hs) s->setFont(font);
	for (auto s : vs) s->setFont(font);
}

void ScaleLogic::setFontSize(int size) {
	myFontSize = size;
	for (auto s : hs) s->setFontSize(size);
	for (auto s : vs) s->setFontSize(size);
}

void ScaleLogic::setHasBackground(int state) {
	bool check = true;
	if (state == Qt::Unchecked) check = false;
	for (auto s : hs) s->setHasBackground(check);
	for (auto s : vs) s->setHasBackground(check);
}

void ScaleLogic::setStyle(int style) {
	scaleType = style;
	for (auto s : hs) s->setVisible(false);
	for (auto s : vs) s->setVisible(false);
	switch (style) {
	case 0:
		hs1->setVisible(true);
		vs1->setVisible(true);
		break;
	case 2:
		hs2->setVisible(true);
		vs2->setVisible(true);
		break;
	case 1:
		hs3->setVisible(true);
		vs3->setVisible(true);
		break;
	}
}

void ScaleLogic::setRealMod(double w, double h) {
	realWidth = w;
	realHeight = h;
	setLength(QString::number(unit.getVal()));
}

void ScaleLogic::setUnit(int Unit) {
	unit.setUnit((Unit::UnitType)Unit);
	setLength(QString::number(unit.value));
}

QLineF ScaleLogic::hLine(int h) {
	double xPixel = unit.value * unit.getMod() / realWidth;
	double yPixel = unit.value * unit.getMod() / realHeight;
	double offset = 100;

	double wa = 0.05*scene->width();
	double wm = 0.5*scene->width();
	double wb = 0.9*scene->width();
	double ha = 0.05*scene->height();
	double hm = 0.5*scene->height();
	double hb = 0.9*scene->height();

	QLineF hTL = QLineF(QPointF(wa+offset, ha), QPointF(wa+xPixel+offset, ha));
	QLineF hBL = QLineF(QPointF(wa+offset, hb), QPointF(wa+xPixel+offset, hb));
	QLineF hT  = QLineF(QPointF(wm-0.5*xPixel, ha), QPointF(wm+0.5*xPixel, ha));
	QLineF hC  = QLineF(QPointF(wm-0.5*xPixel, hm), QPointF(wm+0.5*xPixel, hm));
	QLineF hB  = QLineF(QPointF(wm-0.5*xPixel, hb), QPointF(wm+0.5*xPixel, hb));
	QLineF hTR = QLineF(QPointF(wb-xPixel-offset, ha), QPointF(wb-offset, ha));
	QLineF hBR = QLineF(QPointF(wb-xPixel-offset, hb), QPointF(wb-offset, hb));
	QLineF hL;	//!!!
	QLineF hR;	//!!!

	if (0 == h)
		return (hTL);
	else if (1 == h)
		return (hT);
	else if (2 == h)
		return (hTR);
	else if (3 == h)
		return (hL);
	else if (4 == h)
		return (hC);
	else if (5 == h)
		return (hR);
	else if (6 == h)
		return (hBL);
	else if (7 == h)
		return (hB);
	else if (8 == h)
		return (hBR);
	return hTL;
}

QLineF ScaleLogic::vLine(int v) {
	double xPixel = unit.value * unit.getMod() / realWidth;
	double yPixel = unit.value * unit.getMod() / realHeight;
	double offset = 100;

	double wa = 0.05*scene->width();
	double wm = 0.5*scene->width();
	double wb = 0.9*scene->width();
	double ha = 0.05*scene->height();
	double hm = 0.5*scene->height();
	double hb = 0.9*scene->height();

	QLineF vTL = QLineF(QPointF(wa, ha+offset), QPointF(wa, ha+yPixel+offset));
	QLineF vTR = QLineF(QPointF(wb, ha+offset), QPointF(wb, ha+yPixel+offset));
	QLineF vL  = QLineF(QPointF(wa, hm-0.5*yPixel), QPointF(wa, hm+0.5*yPixel));
	QLineF vC  = QLineF(QPointF(wm, hm-0.5*yPixel), QPointF(wm, hm+0.5*yPixel));
	QLineF vR  = QLineF(QPointF(wb, hm-0.5*yPixel), QPointF(wb, hm+0.5*yPixel));
	QLineF vBL = QLineF(QPointF(wa, hb-yPixel-offset), QPointF(wa, hb-offset));
	QLineF vBR = QLineF(QPointF(wb, hb-yPixel-offset), QPointF(wb, hb-offset));
	QLineF vT; //!!!
	QLineF vB; //!!!

	if (0 == v)
		return (vTL);
	else if (1 == v)
		return (vT);
	else if (2 == v)
		return (vTR);
	else if (3 == v)
		return (vL);
	else if (4 == v)
		return (vC);
	else if (5 == v)
		return (vR);
	else if (6 == v)
		return (vBL);
	else if (7 == v)
		return (vB);
	else if (8 == v)
		return (vBR);
	return vTL;
}

void ScaleLogic::hideScale() {
	visible = false;
	for (auto s : hs) s->setVisible(false);
	for (auto s : vs) s->setVisible(false);
}

bool ScaleLogic::isVisible() const
{
	return visible;
}

//Legend
LegendLogic::LegendLogic(Logic* logic, MarkerModel* model):
	QObject(logic), logic(logic), model(model)
{
	auto scene = logic->getScene();
	auto origin = scene->sceneRect().topLeft() + QPointF(200, 200);
	myGroup = new LegendGroup(origin, model);
}

LegendLogic::~LegendLogic() {
	delete myGroup;
}

//Undo
UndoLogic::UndoLogic(Logic* logic):
	logic(logic), markerLogic(logic->marker), scene(logic->scene), ruler(logic->ruler)
{
	stack = new QUndoStack(this);
}

void UndoLogic::addMarkerContainer() {
	auto cmd = new BasicUndo([this]{ markerLogic->model->removeMarkerContainer(markerLogic->model->at(markerLogic->model->rowCount()-1)); }, 
		[this]{ markerLogic->addMarkerContainer(); } );
	cmd->setText("Add new marker set.");
	stack->push(cmd);
}

void UndoLogic::removeMarkerContainer(int row) {
	auto undo = [this](MarkerContainer* cont){ 
		for (auto item : cont->items())
			logic->scene->addItem(item);
		markerLogic->insertMarkerContainer(cont); 
	};
	auto redo = [this](MarkerContainer* cont){ 
		for (auto item : cont->items())
			logic->scene->removeItem(item);
		markerLogic->model->removeMarkerContainer(cont); 
	};
	auto cmd = new ItemUndo<MarkerContainer>(undo, redo, markerLogic->at(row));
	cmd->setText(QString("Remove marker set at  %d").arg(row));
	stack->push(cmd);
}

void UndoLogic::addMarker(MarkerItem* item) {
	auto undo = [this](MarkerItem* i) { logic->scene->removeItem(i); markerLogic->model->removeMarker(i); };
	auto redo = [this](MarkerItem* i) { logic->scene->addItem(i); markerLogic->addMarker(i); };
	auto cmd = new ItemUndo<MarkerItem>(undo, redo, item);
	stack->push(cmd);
}

void UndoLogic::removeMarker(MarkerItem* item) {
	auto undo = [this](MarkerItem* item) { logic->scene->addItem(item); markerLogic->model->addMarker(item, item->container()->index()); };
	auto redo = [this](MarkerItem* item) { logic->scene->removeItem(item); markerLogic->model->removeMarker(item); };
	auto cmd = new ItemUndo<MarkerItem>(undo, redo, item);
	stack->push(cmd);
}

void UndoLogic::addLR(LineRuler* r) {
	auto redo = [this](LineRuler* r) { scene->addItem(r); ruler->lr->addRuler(r); ruler->mapLR->toLast(); };
	auto undo = [this](LineRuler* r) { scene->removeItem(r); ruler->lr->removeRuler(r); ruler->mapLR->toLast(); };
	auto cmd = new ItemUndo<LineRuler>(undo, redo, r);
	stack->push(cmd);
}

void UndoLogic::addRR(RectRuler* r) {
	auto redo = [this](RectRuler* r) { scene->addItem(r); ruler->rr->addRuler(r); ruler->mapRR->toLast(); };
	auto undo = [this](RectRuler* r) { scene->removeItem(r); ruler->rr->removeRuler(r); ruler->mapRR->toLast(); };
	auto cmd = new ItemUndo<RectRuler>(undo, redo, r);
	stack->push(cmd);
	logic->ui->rrDock->emitData();
}

void UndoLogic::addC1(CircleRuler* r)
{
	auto redo = [this](CircleRuler* r) { scene->addItem(r); ruler->c1->addRuler(r); ruler->mapC1->toLast(); };
	auto undo = [this](CircleRuler* r) { scene->removeItem(r); ruler->c1->removeRuler(r); ruler->mapC1->toLast(); };
	auto cmd = new ItemUndo<CircleRuler>(undo, redo, r);
	stack->push(cmd);
	logic->ui->c1Dock->emitData();
}

void UndoLogic::addCR(const QPointF& p1, const QPointF& p2, const QPointF& p3) {
	auto r = ruler->cr->createRuler(p1, p2, p3);
	auto redo = [this](TriCircleRuler* r) { scene->addItem(r); ruler->cr->addRuler(r); ruler->mapCR->toLast(); };
	auto undo = [this](TriCircleRuler* r) { scene->removeItem(r); ruler->cr->removeRuler(r); ruler->mapCR->toLast(); };
	auto cmd = new ItemUndo<TriCircleRuler>(undo, redo, r);
	stack->push(cmd);
}

void UndoLogic::addC2(const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4, const QPointF& p5, const QPointF& p6) {
	auto r = ruler->c2->createRuler(p1, p2, p3, p4, p5, p6);
	auto redo = [this](Circle2Ruler* r) { scene->addItem(r); ruler->c2->addRuler(r); ruler->mapC2->toLast(); };
	auto undo = [this](Circle2Ruler* r) { scene->removeItem(r); ruler->c2->removeRuler(r); ruler->mapC2->toLast(); };
	auto cmd = new ItemUndo<Circle2Ruler>(undo, redo, r);
	stack->push(cmd);
}

void UndoLogic::addPR(PolyRuler* r) {
	auto redo = [this](PolyRuler* r) { scene->addItem(r); ruler->pr->addRuler(r); ruler->mapPR->toLast(); };
	auto undo = [this](PolyRuler* r) { scene->removeItem(r); ruler->pr->removeRuler(r); ruler->mapPR->toLast(); };
	auto cmd = new ItemUndo<PolyRuler>(undo, redo, r);
	stack->push(cmd);
	logic->ui->prDock->emitData();
}

void UndoLogic::addMR(PathRuler* r) {
	auto redo = [this](PathRuler* r) { scene->addItem(r); ruler->mr->addRuler(r); ruler->mapMR->toLast(); };
	auto undo = [this](PathRuler* r) { scene->removeItem(r); ruler->mr->removeRuler(r); ruler->mapMR->toLast(); };
	auto cmd = new ItemUndo<PathRuler>(undo, redo, r);
	stack->push(cmd);
	logic->ui->mlDock->emitData();
}

void UndoLogic::removeLR(LineRuler* r) {
	auto undo = [this](LineRuler* item) { logic->scene->addItem(item); ruler->lr->addRuler(item); };
	auto redo = [this](LineRuler* item) { logic->scene->removeItem(item); ruler->lr->removeRuler(item); };
	auto cmd = new ItemUndo<LineRuler>(undo, redo, r);
	stack->push(cmd);
}

void UndoLogic::removeRR(RectRuler* r) {
	auto undo = [this](RectRuler* item) { logic->scene->addItem(item); ruler->rr->addRuler(item); };
	auto redo = [this](RectRuler* item) { logic->scene->removeItem(item); ruler->rr->removeRuler(item); };
	auto cmd = new ItemUndo<RectRuler>(undo, redo, r);
	stack->push(cmd);
}

void UndoLogic::removeC1(CircleRuler* r)
{
	auto undo = [this](CircleRuler* item) { logic->scene->addItem(item); ruler->c1->addRuler(item); };
	auto redo = [this](CircleRuler* item) { logic->scene->removeItem(item); ruler->c1->removeRuler(item); };
	auto cmd = new ItemUndo<CircleRuler>(undo, redo, r);
	stack->push(cmd);
}

void UndoLogic::removeCR(TriCircleRuler* r) {
	auto undo = [this](TriCircleRuler* item) { logic->scene->addItem(item); ruler->cr->addRuler(item); };
	auto redo = [this](TriCircleRuler* item) { logic->scene->removeItem(item); ruler->cr->removeRuler(item); };
	auto cmd = new ItemUndo<TriCircleRuler>(undo, redo, r);
	stack->push(cmd);
}

void UndoLogic::removeC2(Circle2Ruler* r) {
	auto undo = [this](Circle2Ruler* item) { logic->scene->addItem(item); ruler->c2->addRuler(item); };
	auto redo = [this](Circle2Ruler* item) { logic->scene->removeItem(item); ruler->c2->removeRuler(item); };
	auto cmd = new ItemUndo<Circle2Ruler>(undo, redo, r);
	stack->push(cmd);
}

void UndoLogic::removePR(PolyRuler* r) {
	auto undo = [this](PolyRuler* item) { logic->scene->addItem(item); ruler->pr->addRuler(item); };
	auto redo = [this](PolyRuler* item) { logic->scene->removeItem(item); ruler->pr->removeRuler(item); };
	auto cmd = new ItemUndo<PolyRuler>(undo, redo, r);
	stack->push(cmd);
}

void UndoLogic::removeMR(PathRuler* r) {
	auto undo = [this](PathRuler* item) { logic->scene->addItem(item); ruler->mr->addRuler(item); };
	auto redo = [this](PathRuler* item) { logic->scene->removeItem(item); ruler->mr->removeRuler(item); };
	auto cmd = new ItemUndo<PathRuler>(undo, redo, r);
	stack->push(cmd);
}

void UndoLogic::removeRuler(QGraphicsItem* rulerItem) {
	RulerModel* model = ruler->modelAt(rulerItem->type());
	auto rI = (RulerItemInterface*)rulerItem;
	auto undo = [this](RulerItemInterface* it, RulerModel* mod) { logic->scene->addItem((QGraphicsItem*)it); mod->addRuler(it); };
	auto redo = [this](RulerItemInterface* it, RulerModel* mod) { logic->scene->removeItem((QGraphicsItem*)it); mod->removeRuler(it); };
	auto cmd = new ModelUndo<RulerItemInterface, RulerModel>(undo, redo, rI, model);
	stack->push(cmd);
}

void UndoLogic::addItem(QGraphicsItem* item) {
	auto cmd = new ItemUndo<QGraphicsItem>([this](QGraphicsItem* i) { scene->removeItem(i); }, [this](QGraphicsItem* i) { scene->addItem(i); }, item);
	stack->push(cmd);
}

void UndoLogic::removeItem(QGraphicsItem* item) {
	auto undo = [this](QGraphicsItem* item) { logic->scene->addItem(item); };
	auto redo = [this](QGraphicsItem* item) { logic->scene->removeItem(item); };
	auto cmd = new ItemUndo<QGraphicsItem> ( undo, redo, item ) ;
	stack->push(cmd);
}

void UndoLogic::addItems(QList<QGraphicsItem*> items) {
	stack->beginMacro("Add items");
	for (auto item : items)
		addItem(item);
	stack->endMacro();
}

void UndoLogic::removeItems(QList<QGraphicsItem*> items) {
	stack->beginMacro("Delete items");
	for (auto item : items) {
		if (item->type() == MarkerItem::Type)
			removeMarker((MarkerItem*)item);
		else if (item->type() == LineRuler::Type)
			removeLR((LineRuler*)item);
		else if (item->type() == RectRuler::Type)
			removeRR((RectRuler*)item);
		else if (item->type() == CircleRuler::Type)
			removeC1(qgraphicsitem_cast<CircleRuler*>(item));
		else if (item->type() == TriCircleRuler::Type)
			removeCR((TriCircleRuler*)item);
		else if (item->type() == Circle2Ruler::Type)
			removeC2((Circle2Ruler*)item);
		else if (item->type() == PolyRuler::Type)
			removePR((PolyRuler*)item);
		else if (item->type() == PathRuler::Type)
			removeMR((PathRuler*)item);
		else if (item->type() == ScaleItem::Type)
			item->setVisible(false);
		else if (item->type() == LegendGroup::Type)
			removeItem(item);
		else
			removeItem(item);
	}
	stack->endMacro();
}

//Logic
Logic::Logic(ImageRaster* ui):
	QObject(ui), ui(ui), myHasImage(false)
{
}

Logic::~Logic()
{
}

void Logic::saveImage(const QString& fname) {
	scene->clearSelection();
	QImage img(myImage.width(), myImage.height(), QImage::Format_ARGB32);
	img.fill(Qt::transparent);
	QPainter painter(&img);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	scene->render(&painter);
	img.save(fname);
}

void Logic::saveProfile() {
}

GraphicsScene* Logic::initScene(const QString& fname) {
	myImageName = fname;
	myImage = QPixmap(fname);
	if (myImage.isNull())
		myImage = QPixmap(fname, "JPG");
	if (myImage.isNull())
		myImage = QPixmap(fname, "BMP");
	if (myImage.isNull())
		myImage = QPixmap(fname, "PNG");
	scene = new GraphicsScene(ui);
	auto pixItem = scene->addPixmap(myImage);
	pixItem->setAcceptHoverEvents(true);
	scene->setSceneRect(myImage.rect());
	myHasImage = true;
	ui->setWindowTitle("Image Raster - " + fname);
	connect(ui->modes, &ActionGroup::actionTriggered, scene, &GraphicsScene::setAppState);
	initProfile();
	initMarkers();
	initRuler();
	initScale();
	initLegend();
	initUndo();
	connect(ui->ui.actionBlend, &QAction::triggered, this, &Logic::startBlendWizard);
	connect(ui->ui.actionDelete, &QAction::triggered, this, &Logic::removeSelected);
	connect(ui->ui.actionSelectAll, &QAction::triggered, this, &Logic::selectAllItem);
	connect(ui->ui.graphicsView, &GraphicsView::zoomChanged, [this](double z) { ui->zoom->setText(QString("%1%").arg(z*100, 0, 'g', -1)); });
	connect(scene, &GraphicsScene::helpText, ui->help, &QLabel::setText);

	auto setProfile = [&](int w, int h) { scene->setRealMod(1.0*w/scene->width(), 1.0*h/scene->height()); };
	connect(profile, &ProfileLogic::currentProfileChanged, setProfile);
	setProfile(profile->model->at(profile->mapper->currentIndex()).width, profile->model->at(profile->mapper->currentIndex()).height);

	return scene;
}

void Logic::initMarkers() {
	marker = new MarkerLogic(this);
	auto mapper = marker->map();
	mapper->addMapping(ui->markerDock->label, 0);
	mapper->addMapping(ui->markerDock->count, 1);
	mapper->addMapping(ui->markerDock->color1, 2);
	mapper->addMapping(ui->markerDock->color2, 3);
	mapper->addMapping(ui->markerDock->checkBox, 4);
	mapper->addMapping(ui->markerDock->weight, 5);
	//mapper->addMapping(ui->markerDock->Font, 6);
	mapper->addMapping(ui->markerDock->fontSize, 7);
	mapper->addMapping(ui->markerDock->useBackground, 8);
	mapper->addMapping(ui->markerDock->Size, 9);
	ui->markerDock->view->setModel(mapper->model());
	for (int i=2; i<10; ++i)
		ui->markerDock->view->hideColumn(i);
	connect(ui->markerDock, &MarkerDock::MarkerChanged, marker, &MarkerLogic::setType);
	connect(ui->markerDock, &MarkerDock::MarkerChanged, scene, &GraphicsScene::setMarkerType);
	connect(ui->markerDock, &MarkerDock::dataChanged, mapper, &QDataWidgetMapper::submit);
	connect(ui->markerDock->view->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)),
		mapper, SLOT(setCurrentModelIndex(QModelIndex)));
	connect(marker, &MarkerLogic::indexChanged, [=]() { ui->markerDock->setMode(marker->currentType()); } );
	marker->addMarkerContainer();

	connect(ui->markerDock, &MarkerDock::fontChanged, 
	[this](const QFont& f) {
		auto cont = marker->model->at(marker->map()->currentIndex());
		if (cont->font() != f)
			cont->setFont(f);
	});

	connect(mapper, &QDataWidgetMapper::currentIndexChanged,
	[this]() {
		auto cont = marker->model->at(marker->map()->currentIndex());
		if (cont->font() != ui->markerDock->font())
			ui->markerDock->setFont(cont->font());
	});
}

void Logic::initUndo() {
	undo = new UndoLogic(this);
	auto actionUndo = undo->createUndoAction();
	actionUndo->setIcon(ui->ui.actionUndo->icon());
	actionUndo->setShortcut(QKeySequence::Undo);
	auto actionRedo = undo->createRedoAction();
	actionRedo->setIcon(ui->ui.actionRedo->icon());
	actionRedo->setShortcut(QKeySequence::Redo);
	ui->ui.mainToolBar->insertAction(ui->ui.actionUndo, actionUndo);
	ui->ui.mainToolBar->insertAction(ui->ui.actionUndo, actionRedo);
	ui->ui.mainToolBar->removeAction(ui->ui.actionUndo);
	ui->ui.mainToolBar->removeAction(ui->ui.actionRedo);
	ui->ui.menuEdit->insertAction(ui->ui.actionUndo, actionUndo);
	ui->ui.menuEdit->insertAction(ui->ui.actionUndo, actionRedo);
	ui->ui.menuEdit->removeAction(ui->ui.actionUndo);
	ui->ui.menuEdit->removeAction(ui->ui.actionRedo);
	connect(scene, &GraphicsScene::addMarker, undo, &UndoLogic::addMarker);
	connect(ui->markerDock->add, &QPushButton::clicked, undo, &UndoLogic::addMarkerContainer);
	connect(ui->markerDock->rem, &QPushButton::clicked, undo, &UndoLogic::removeMarkerContainer);

	connect(scene, &GraphicsScene::addLR, undo, &UndoLogic::addLR);
	connect(scene, &GraphicsScene::addRR, undo, &UndoLogic::addRR);
	connect(scene, &GraphicsScene::addC1, undo, &UndoLogic::addC1);
	connect(scene, &GraphicsScene::addCR, undo, &UndoLogic::addCR);
	connect(scene, &GraphicsScene::addC2, undo, &UndoLogic::addC2);
	connect(scene, &GraphicsScene::addPR, undo, &UndoLogic::addPR);
	connect(scene, &GraphicsScene::addMR, undo, &UndoLogic::addMR);
}

void Logic::initProfile() {
	profile = new ProfileLogic(this);
	ui->profileDock->view->setModel(profile->model);
	ui->profileCombo->setModel(profile->model);
	profile->mapper->addMapping(ui->profileDock->X, 1);
	profile->mapper->addMapping(ui->profileDock->Y, 2);

	connect(ui->profileDock->view->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)),
		profile->mapper, SLOT(setCurrentModelIndex(QModelIndex)));
	connect(profile->mapper, &QDataWidgetMapper::currentIndexChanged, ui->profileCombo, &QComboBox::setCurrentIndex);
	connect(ui->profileCombo, SIGNAL(currentIndexChanged(int)), profile->mapper, SLOT(setCurrentIndex(int)));
	//connect(ui->modCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateModifier(int)));
	connect(ui->profileDock->add, &QPushButton::clicked, profile, &ProfileLogic::appendProfile);
	connect(ui->profileDock->remove, &QPushButton::clicked, profile, &ProfileLogic::removeProfile);
	connect(ui->profileDock->calibrate, &QPushButton::clicked, profile, &ProfileLogic::calibrateProfile);
	connect(ui->profileDock->reset, &QPushButton::clicked, profile, &ProfileLogic::resetProfile);

	ui->profileCombo->setCurrentIndex(Settings("Global").custom("_LastProfile", 0).toInt());

	auto s = Settings("Global");
	bool FirstTime = s.custom("_FirstTime", true).toBool();
	if (FirstTime) {
		int res = QMessageBox::information(ui, "Welcome to Image Raster", "Would you like to calibrate Image Raster first?", 
			QMessageBox::Yes, QMessageBox::No);
		if (res == QMessageBox::Yes) {
			ui->ui.actionCalibrate->trigger();
			QMessageBox::information(ui, "Calibrate Profile",
				"You can calibrate your profile by clicking Calibrate button.");
		}
		else {
			QMessageBox::information(ui, "Default Calibration Used",
				"Image Raster is using default calibration profile. You can access calibration menu in Image Raster toolbar.");
		}
	}
}

void Logic::initRuler() {
	ruler = new RulerLogic(this);
	ui->lrDock->view->setModel(ruler->lr);
	ui->rrDock->view->setModel(ruler->rr);
	ui->crDock->view->setModel(ruler->cr);
	ui->c2Dock->view->setModel(ruler->c2);
	ui->prDock->view->setModel(ruler->pr);
	ui->mlDock->view->setModel(ruler->mr);
	ui->c1Dock->view->setModel(ruler->c1);
	for (int i=1; i<11; ++i) {
		if (i == 4) continue;
		ruler->mapLR->addMapping(ui->lrDock->widgets.at(i-1), i);
		ruler->mapRR->addMapping(ui->rrDock->widgets.at(i-1), i);
		ruler->mapC1->addMapping(ui->c1Dock->widgets.at(i-1), i);
		ruler->mapCR->addMapping(ui->crDock->widgets.at(i-1), i);
		ruler->mapC2->addMapping(ui->c2Dock->widgets.at(i-1), i);
		ruler->mapPR->addMapping(ui->prDock->widgets.at(i-1), i);
		ruler->mapMR->addMapping(ui->mlDock->widgets.at(i-1), i);
	}
	for (int i=0; i<=11; ++i) {
		ui->lrDock->view->hideColumn(i);
		ui->rrDock->view->hideColumn(i);
		ui->c1Dock->view->hideColumn(i);
		ui->crDock->view->hideColumn(i);
		ui->c2Dock->view->hideColumn(i);
		ui->prDock->view->hideColumn(i);
		ui->mlDock->view->hideColumn(i);
	}
	ui->lrDock->view->showColumn(8);
	ui->rrDock->view->showColumn(8);
	ui->rrDock->view->showColumn(9);
	ui->rrDock->view->showColumn(10);
	ui->c1Dock->view->showColumn(8);
	ui->c1Dock->view->showColumn(9);
	ui->crDock->view->showColumn(8);
	ui->crDock->view->showColumn(9);
	ui->c2Dock->view->showColumn(8);
	ui->prDock->view->showColumn(8);
	ui->prDock->view->showColumn(9);
	ui->mlDock->view->showColumn(8);

	connect(ui->lrDock, &RulerDock::dataChanged, ruler->mapLR, &QDataWidgetMapper::submit);
	connect(ui->rrDock, &RulerDock::dataChanged, ruler->mapRR, &QDataWidgetMapper::submit);
	connect(ui->c1Dock, &RulerDock::dataChanged, ruler->mapC1, &QDataWidgetMapper::submit);
	connect(ui->crDock, &RulerDock::dataChanged, ruler->mapCR, &QDataWidgetMapper::submit);
	connect(ui->c2Dock, &RulerDock::dataChanged, ruler->mapC2, &QDataWidgetMapper::submit);
	connect(ui->prDock, &RulerDock::dataChanged, ruler->mapPR, &QDataWidgetMapper::submit);
	connect(ui->mlDock, &RulerDock::dataChanged, ruler->mapMR, &QDataWidgetMapper::submit);

	connect(ui->lrDock, &RulerDock::defaultText, ruler->lr, &RulerModel::textToDefault);
	connect(ui->rrDock, &RulerDock::defaultText, ruler->rr, &RulerModel::textToDefault);
	connect(ui->c1Dock, &RulerDock::defaultText, ruler->c1, &RulerModel::textToDefault);
	connect(ui->crDock, &RulerDock::defaultText, ruler->cr, &RulerModel::textToDefault);
	connect(ui->c2Dock, &RulerDock::defaultText, ruler->c2, &RulerModel::textToDefault);
	connect(ui->prDock, &RulerDock::defaultText, ruler->pr, &RulerModel::textToDefault);
	connect(ui->mlDock, &RulerDock::defaultText, ruler->mr, &RulerModel::textToDefault);

	connect(ui->lrDock->view->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), 
		ruler->mapLR, SLOT(setCurrentModelIndex(QModelIndex)));
	connect(ui->rrDock->view->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), 
		ruler->mapRR, SLOT(setCurrentModelIndex(QModelIndex)));
	connect(ui->c1Dock->view->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), 
		ruler->mapC1, SLOT(setCurrentModelIndex(QModelIndex)));
	connect(ui->crDock->view->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), 
		ruler->mapCR, SLOT(setCurrentModelIndex(QModelIndex)));
	connect(ui->c2Dock->view->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), 
		ruler->mapC2, SLOT(setCurrentModelIndex(QModelIndex)));
	connect(ui->prDock->view->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), 
		ruler->mapPR, SLOT(setCurrentModelIndex(QModelIndex)));
	connect(ui->mlDock->view->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), 
		ruler->mapMR, SLOT(setCurrentModelIndex(QModelIndex)));
	
	auto setProfile = [this](int w, int h) { ruler->setRealMod(1.0*w/scene->width(), 1.0*h/scene->height()); };
	connect(profile, &ProfileLogic::currentProfileChanged, setProfile);
	setProfile(profile->model->at(profile->mapper->currentIndex()).width, profile->model->at(profile->mapper->currentIndex()).height);

	connect(ui->modCombo, SIGNAL(currentIndexChanged(int)), ruler, SLOT(setUnit(int)));
	ruler->setUnit(ui->modCombo->currentIndex());

	connect(ui->rulerTools, &RulerTools::RulerChanged, scene, &GraphicsScene::setRulerState);
	connect(ui->rulerTools, &RulerTools::RulerChanged, ruler, &RulerLogic::setState);
	actionFinish = new QAction("Finish", this);
	connect(ui, &ImageRaster::rulerContextMenu, this, &Logic::viewRulerContextMenu);
	connect(actionFinish, &QAction::triggered, scene, &GraphicsScene::finishPolygon);

	ui->lrDock->emitData();
	ui->rrDock->emitData();
	ui->c1Dock->emitData();
	ui->crDock->emitData();
	ui->c2Dock->emitData();
	ui->prDock->emitData();
	ui->mlDock->emitData();

	connect(ui->lrDock, &RulerDock::fontChanged, ruler->lr, &RulerModel::setFont);
	connect(ui->rrDock, &RulerDock::fontChanged, ruler->rr, &RulerModel::setFont);
	connect(ui->c1Dock, &RulerDock::fontChanged, ruler->c1, &RulerModel::setFont);
	connect(ui->crDock, &RulerDock::fontChanged, ruler->cr, &RulerModel::setFont);
	connect(ui->c2Dock, &RulerDock::fontChanged, ruler->c2, &RulerModel::setFont);
	connect(ui->prDock, &RulerDock::fontChanged, ruler->pr, &RulerModel::setFont);
	connect(ui->mlDock, &RulerDock::fontChanged, ruler->mr, &RulerModel::setFont);

	//connect(ui->lrDock->toggleText, &QCheckBox::toggled, [this]() { ruler->lr->setVisible(!ui->lrDock->toggleText->isChecked()); });
	//connect(ui->rrDock->toggleText, &QCheckBox::toggled, [this]() { ruler->rr->setVisible(!ui->rrDock->toggleText->isChecked()); });
	//connect(ui->crDock->toggleText, &QCheckBox::toggled, [this]() { ruler->c1->setVisible(!ui->crDock->toggleText->isChecked()); });
	//connect(ui->c2Dock->toggleText, &QCheckBox::toggled, [this]() { ruler->c2->setVisible(!ui->c2Dock->toggleText->isChecked()); });
	//connect(ui->prDock->toggleText, &QCheckBox::toggled, [this]() { ruler->pr->setVisible(!ui->prDock->toggleText->isChecked()); });
	//connect(ui->mlDock->toggleText, &QCheckBox::toggled, [this]() { ruler->mr->setVisible(!ui->mlDock->toggleText->isChecked()); });

	ruler->mapLR->addMapping(ui->lrDock->toggleText, 0);
	ruler->mapRR->addMapping(ui->rrDock->toggleText, 0);
	ruler->mapC1->addMapping(ui->c1Dock->toggleText, 0);
	ruler->mapCR->addMapping(ui->crDock->toggleText, 0);
	ruler->mapC2->addMapping(ui->c2Dock->toggleText, 0);
	ruler->mapPR->addMapping(ui->prDock->toggleText, 0);
	ruler->mapMR->addMapping(ui->mlDock->toggleText, 0);
}

void Logic::initScale() {
	scale = new ScaleLogic(this);

	auto setProfile = [this](int w, int h) { 
		scale->setRealMod(1.0*w / scene->width(), 1.0*h / scene->height()); 
	};
	connect(profile, &ProfileLogic::currentProfileChanged, setProfile);
	setProfile(profile->model->at(profile->mapper->currentIndex()).width, profile->model->at(profile->mapper->currentIndex()).height);

	connect(ui->scaleDock, &ScaleDock::checked_changed, scale, &ScaleLogic::showScale);
	connect(ui->scaleDock, &ScaleDock::hideScale, scale, &ScaleLogic::hideScale);
	connect(ui->scaleDock->length, &QComboBox::currentTextChanged, scale, &ScaleLogic::setLength);
	connect(ui->scaleDock->unit, SIGNAL(currentIndexChanged(int)), scale, SLOT(setUnit(int)));
	connect(ui->scaleDock->style_, SIGNAL(currentIndexChanged(int)), scale, SLOT(setStyle(int)));
	connect(ui->scaleDock->lineWeight, SIGNAL(valueChanged(int)), scale, SLOT(setPenWidth(int)));
	connect(ui->scaleDock->Font, &QFontComboBox::currentFontChanged, scale, &ScaleLogic::setFont);
	connect(ui->scaleDock->fontSize, &FontSizeCombo::fontSizeChanged, scale, &ScaleLogic::setFontSize);
	connect(ui->scaleDock->useBackground, &QCheckBox::stateChanged, scale, &ScaleLogic::setHasBackground);
	connect(ui->scaleDock, &ScaleDock::horizontal_checked, scale, &ScaleLogic::updateHScale);
	connect(ui->scaleDock, &ScaleDock::vertical_checked, scale, &ScaleLogic::updateVScale);

	scale->hideScale();
}

void Logic::initLegend() {
	legend = new LegendLogic(this, marker->model);
	connect(ui->legendDock->legendButton, &QPushButton::clicked, [this]{ legend->updateModel(); undo->addItem(legend->myGroup); });
	connect(ui->legendDock->Font, &QFontComboBox::currentFontChanged, legend, &LegendLogic::setFont);
	connect(ui->legendDock->fontSize, &FontSizeCombo::fontSizeChanged, legend, &LegendLogic::setFontSize);
	connect(ui->legendDock->penWidth, SIGNAL(valueChanged(int)), legend, SLOT(setPenWidth(int)));
	connect(ui->legendDock->Size, SIGNAL(valueChanged(int)), legend, SLOT(setSize(int)));
}

void Logic::viewRulerContextMenu(const QPoint& pos) {
	using namespace States;
	QMenu* menu = new QMenu(ui);
	if (RulerState::Polygon == ruler->state || RulerState::Path == ruler->state) {
		menu->addAction(actionFinish);
		if (scene->polyClick() < 3)
			actionFinish->setEnabled(false);
		else
			actionFinish->setEnabled(true);
		menu->exec(QCursor::pos());
	}
}

void Logic::selectAllItem() {
	for (auto item : scene->items())
		item->setSelected(true);
}

void Logic::removeSelected() {
	QList<QGraphicsItem*> items;
	for (auto item : scene->items()) {
		if (item->isSelected() && !item->parentItem()) items.append(item);
	}
	undo->removeItems(items);
}

#include "BlendItem.h"
void Logic::startBlendWizard() {
	using namespace blend;
	auto blend = new BlendWizard(myImageName, ui);
	int accept = blend->exec();
	if (accept == QWizard::Accepted) {
		QGraphicsItem* anchor = blend->getAnchor();
		QGraphicsItem* thumb = blend->getThumb();
		SimpleTextItem* text = blend->getText();
		anchor->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
		thumb->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
		text->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
		QList<QGraphicsItem*> items;
		items.append(anchor);
		items.append(thumb);
		items.append(text);
		undo->addItems(items);
	}
}