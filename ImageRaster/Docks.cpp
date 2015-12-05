#include "stdafx.h"
#include "Docks.h"
#include "RasterSettings.h"

QPixmap tryOpenPixmap(const QString& fn)
{
	QPixmap myImage = QPixmap{ fn };
	if (myImage.isNull())
		myImage = QPixmap(fn, "JPG");
	if (myImage.isNull())
		myImage = QPixmap(fn, "BMP");
	if (myImage.isNull())
		myImage = QPixmap(fn, "PNG");
	return myImage;
}

//MarkerDock
MarkerDock::MarkerDock(QWidget* parent) :
	QDockWidget(parent),
	Ui::MarkerDock()
{
	Ui::MarkerDock::setupUi(this);
	modeGroup = new QButtonGroup(this);
	modeGroup->addButton(arrow, 0);
	modeGroup->addButton(rect_, 1);
	modeGroup->addButton(circle, 2);
	modeGroup->setExclusive(true);
	connect(label, &QLineEdit::textChanged, this, &MarkerDock::dataChanged);
	connect(color1, SIGNAL(currentIndexChanged(int)), this, SIGNAL(dataChanged()));
	connect(color2, SIGNAL(currentIndexChanged(int)), this, SIGNAL(dataChanged()));
	connect(weight, SIGNAL(valueChanged(int)), this, SIGNAL(dataChanged()));
	connect(Font, &QFontComboBox::currentFontChanged, this, &MarkerDock::fontChanged);
	connect(fontSize, &FontSizeCombo::fontSizeChanged, this, &MarkerDock::dataChanged);
	connect(checkBox, &QCheckBox::stateChanged, this, &MarkerDock::dataChanged);
	connect(useBackground, &QCheckBox::stateChanged, this, &MarkerDock::dataChanged);
	connect(Size, SIGNAL(valueChanged(int)), this, SIGNAL(dataChanged()));
	connect(bw, &QPushButton::clicked, this, &MarkerDock::applyPreset);
	connect(wb, &QPushButton::clicked, this, &MarkerDock::applyPreset);
	connect(fuchsia, &QPushButton::clicked, this, &MarkerDock::applyPreset);
	connect(crimson, &QPushButton::clicked, this, &MarkerDock::applyPreset);
	connect(cyan, &QPushButton::clicked, this, &MarkerDock::applyPreset);
	connect(green, &QPushButton::clicked, this, &MarkerDock::applyPreset);
	connect(mono, &QPushButton::clicked, this, &MarkerDock::applyPreset);
	connect(lime, &QPushButton::clicked, this, &MarkerDock::applyPreset);

	connect(modeGroup, SIGNAL(buttonClicked(int)), this, SIGNAL(MarkerChanged(int)));

	save << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this);
	load << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this) << new QAction("Default", this);
	for (int i=0; i<save.size(); ++i) {
		save.at(i)->setData(i);
		connect(save.at(i), &QAction::triggered, this, &MarkerDock::saveTriggered);
	}
	for (int i=0; i<load.size(); ++i) {
		load.at(i)->setData(i);
		connect(load.at(i), &QAction::triggered, this, &MarkerDock::loadTriggered);
	}

	QString arg = QString("MarkerL");
	auto s = Settings(arg);
	color1->setColor(s.color1());
	color2->setColor(s.color2());
	weight->setValue(s.penWidth());
	Font->setCurrentFont(s.font());
	fontSize->setFontSize(s.fontSize());
	Size->setValue(s.size());
	useBackground->setChecked(s.hasBackground());
}

MarkerDock::~MarkerDock() {
	if (view->model()) {
		QString arg = QString("MarkerL");
		auto s = Settings(arg);
		s.setColor1(color1->color());
		s.setColor2(color2->color());
		s.setPenWidth(weight->value());
		s.setFont(Font->currentFont());
		s.setFontSize(fontSize->fontSize());
		s.setSize(Size->value());
		s.setHasBackground(useBackground->isChecked());
	}
}

void MarkerDock::applyPreset() {
	QString preset = qobject_cast<QPushButton*>(sender())->objectName();
	if ("bw" == preset) {
		color1->setColor(Qt::black);
		color2->setColor(Qt::white);
	}
	else if ("wb" == preset) {
		color1->setColor(Qt::white);
		color2->setColor(Qt::black);
	}
	else if ("fuchsia" == preset) {
		color1->setColor(Qt::magenta);
		color2->setColor(Qt::black);
	}
	else if ("cyan" == preset) {
		color1->setColor(Qt::cyan);
		color2->setColor(Qt::black);
	}
	else if ("crimson" == preset) {
		color1->setColor(Qt::red);
		color2->setColor(Qt::black);
	}
	else if ("green" == preset) {
		color1->setColor(QColor("green"));
		color2->setColor(Qt::black);
	}
	else if ("mono" == preset) { 
		color1->setColor(Qt::magenta);
		color2->setColor(Qt::transparent);
	}
	else if ("lime" == preset) {
		color1->setColor(QColor("lime"));
		color2->setColor(Qt::transparent);
	}
}

void MarkerDock::contextMenuEvent(QContextMenuEvent* event) {
	QMenu menu(this);
	QMenu* sv = menu.addMenu(tr("&Save"));
	QMenu* ld = menu.addMenu(tr("&Load"));
	sv->addActions(save);
	ld->addActions(load);
	menu.exec(event->globalPos());
	event->accept();
}

void MarkerDock::saveProfile(int index) {
	if (index > 4) return;
	QString arg = QString("Marker%1").arg(index);
	auto s = Settings(arg);
	s.setColor1(color1->color());
	s.setColor2(color2->color());
	s.setPenWidth(weight->value());
	s.setFont(Font->currentFont());
	s.setFontSize(fontSize->fontSize());
	s.setSize(Size->value());
	s.setHasBackground(useBackground->isChecked());
}

void MarkerDock::loadProfile(int index) {
	if (index > 5) return;
	QString arg = QString("Marker%1").arg((index==5) ? "" : QString::number(index));
	auto s = Settings(arg);
	color1->setColor(s.color1());
	color2->setColor(s.color2());
	weight->setValue(s.penWidth());
	Font->setCurrentFont(s.font());
	fontSize->setFontSize(s.fontSize());
	Size->setValue(s.size());
	useBackground->setChecked(s.hasBackground());
}

void MarkerDock::saveTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	saveProfile(i);
}

void MarkerDock::loadTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	loadProfile(i);
}

void MarkerDock::setMode(int mode) {
	auto btn = modeGroup->button(mode);
	btn->setChecked(true);
}

QFont MarkerDock::font() const
{
	return Font->currentFont();
}

void MarkerDock::setFont(const QFont& f)
{
	Font->setCurrentFont(f);
	emit fontChanged(f);
}

//RulerDock
RulerDock::RulerDock(int ruler_type, QWidget* parent) :
	QDockWidget(parent),
	Ui::RulerDock(),
	myType(ruler_type)
{
	Ui::RulerDock::setupUi(this);
	//connect(text, &QPlainTextEdit::textChanged, this, &RulerDock::dataChanged);
	connect(color1, &ColorEditor::colorChanged, this,  &RulerDock::dataChanged);
	connect(color2, &ColorEditor::colorChanged, this,  &RulerDock::dataChanged);
	connect(penWidth, SIGNAL(valueChanged(int)), this, SIGNAL(dataChanged()));
	connect(Font, &QFontComboBox::currentFontChanged, this, &RulerDock::fontChanged);
	connect(fontSize, &FontSizeCombo::fontSizeChanged, this, &RulerDock::dataChanged);
	connect(useBackground, &QCheckBox::stateChanged, this, &RulerDock::dataChanged);
	connect(toggleText, &QCheckBox::stateChanged, this, &RulerDock::dataChanged);
	connect(bw, &QPushButton::clicked, this, &RulerDock::applyPreset);
	connect(wb, &QPushButton::clicked, this, &RulerDock::applyPreset);
	connect(fuchsia, &QPushButton::clicked, this, &RulerDock::applyPreset);
	connect(crimson, &QPushButton::clicked, this, &RulerDock::applyPreset);
	connect(cyan, &QPushButton::clicked, this, &RulerDock::applyPreset);
	connect(green, &QPushButton::clicked, this, &RulerDock::applyPreset);
	connect(mono, &QPushButton::clicked, this, &RulerDock::applyPreset);
	connect(lime, &QPushButton::clicked, this, &RulerDock::applyPreset);

	connect(Default, &QPushButton::clicked, this, &RulerDock::defaultText);

	save << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this);
	load << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this) << new QAction("Default", this);
	for (int i=0; i<save.size(); ++i) {
		save.at(i)->setData(i);
		connect(save.at(i), &QAction::triggered, this, &RulerDock::saveTriggered);
	}
	for (int i=0; i<load.size(); ++i) {
		load.at(i)->setData(i);
		connect(load.at(i), &QAction::triggered, this, &RulerDock::loadTriggered);
	}

	widgets.append(color1);
	widgets.append(color2);
	widgets.append(penWidth);
	widgets.append(Font);
	widgets.append(fontSize);
	widgets.append(useBackground);
	widgets.append(text);
	widgets.append(data1);
	widgets.append(data2);
	widgets.append(data3);

	auto hide2 = [this] { num2->hide(); data2->hide(); };
	auto hide3 = [this] { num3->hide(); data3->hide(); };

	switch (ruler_type) {
	case 0:
		num1->setText("Length");
		hide2(); hide3();
		break;
	case 1:
		num1->setText("Width");
		num2->setText("Height");
		num3->setText("Area");
		break;
	case 2:
	case 6:
		num1->setText("Diameter");
		num2->setText("Area");
		hide3();
		break;
	case 3:
		num1->setText("Distance");
		hide2(); hide3();
		break;
	case 4:
		num1->setText("Area");
		num2->setText("Perimeter");
		hide3();
		break;
	case 5:
		num1->setText("Path");
		hide2(); hide3();
		break;
	}

	QString arg = QString("Ruler%1L").arg(myType);
	auto s = Settings(arg);
	color1->setColor(s.color1());
	color2->setColor(s.color2());
	penWidth->setValue(s.penWidth());
	Font->setCurrentFont(s.font());
	fontSize->setFontSize(s.fontSize());
	useBackground->setChecked(s.hasBackground());
}

RulerDock::~RulerDock() {
}

void RulerDock::applyPreset() {
	QString preset = qobject_cast<QPushButton*>(sender())->objectName();
	if ("bw" == preset) {
		color1->setColor(Qt::black);
		color2->setColor(Qt::white);
	}
	else if ("wb" == preset) {
		color1->setColor(Qt::white);
		color2->setColor(Qt::black);
	}
	else if ("fuchsia" == preset) {
		color1->setColor(Qt::magenta);
		color2->setColor(Qt::black);
	}
	else if ("cyan" == preset) {
		color1->setColor(Qt::cyan);
		color2->setColor(Qt::black);
	}
	else if ("crimson" == preset) {
		color1->setColor(Qt::red);
		color2->setColor(Qt::black);
	}
	else if ("green" == preset) {
		color1->setColor(QColor("green"));
		color2->setColor(Qt::black);
	}
	else if ("mono" == preset) { 
		color1->setColor(Qt::magenta);
		color2->setColor(Qt::transparent);
	}
	else if ("lime" == preset) {
		color1->setColor(QColor("lime"));
		color2->setColor(Qt::transparent);
	}
}

void RulerDock::contextMenuEvent(QContextMenuEvent* event) {
	QMenu menu(this);
	QMenu* sv = menu.addMenu(tr("&Save"));
	QMenu* ld = menu.addMenu(tr("&Load"));
	sv->addActions(save);
	ld->addActions(load);
	menu.exec(event->globalPos());
	event->accept();
}

void RulerDock::saveProfile(int index) {
	if (index > 4) return;
	QString arg = QString("Ruler%1%2").arg(myType).arg(index);
	auto s = Settings(arg);
	s.setColor1(color1->color());
	s.setColor2(color2->color());
	s.setPenWidth(penWidth->value());
	s.setFont(Font->currentFont());
	s.setFontSize(fontSize->fontSize());
	s.setHasBackground(useBackground->isChecked());
}

void RulerDock::loadProfile(int index) {
	if (index > 5) return;
	QString arg = QString("Ruler%1%2").arg(myType).arg((index==5) ? "" : QString::number(index));
	auto s = Settings(arg);
	color1->setColor(s.color1());
	color2->setColor(s.color2());
	penWidth->setValue(s.penWidth());
	Font->setCurrentFont(s.font());
	fontSize->setFontSize(s.fontSize());
	useBackground->setChecked(s.hasBackground());
}

void RulerDock::saveTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	saveProfile(i);
}

void RulerDock::loadTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	loadProfile(i);
}

QFont RulerDock::font() const
{
	return Font->currentFont();
}

void RulerDock::setFont(const QFont& f)
{
	Font->setCurrentFont(f);
	emit fontChanged(f);
}

RulerTools::RulerTools(QWidget* parent):
	QDockWidget(parent),
	Ui::RulerTools()
{
	Ui::RulerTools::setupUi(this);
	rulers = new QButtonGroup(this);
	rulers->addButton(line, 0);
	rulers->addButton(rectangle, 1);
	rulers->addButton(circle, 2);
	rulers->addButton(circle2, 3);
	rulers->addButton(polygon, 4);
	rulers->addButton(multiline, 5);
	rulers->addButton(circle3, 6);
	rulers->setExclusive(true);
	connect(rulers, SIGNAL(buttonClicked(int)), this, SIGNAL(RulerChanged(int)));
}

RulerTools::~RulerTools() {
}

void RulerTools::setRuler(int state) {
	rulers->button(state)->setChecked(true);
}

//Calibration
ProfileDock::ProfileDock(QWidget* parent):
	QDockWidget(parent), Ui::ProfileDock() 
{ 
	Ui::ProfileDock::setupUi(this);
	label->hide();
	label_2->hide();
	X->hide();
	Y->hide();
}

//Scale
ScaleDock::ScaleDock(QWidget* parent) :
	QDockWidget(parent),
	Ui::ScaleDock()
{
	Ui::ScaleDock::setupUi(this);
	h = BL;
	v = BL;
	frame_5->setEnabled(false);
	length->setEnabled(false);
	unit->setEnabled(false);
	style_->setEnabled(false);
	Font->setEnabled(false);
	fontSize->setEnabled(false);
	useBackground->setEnabled(false);
	color->setVisible(false);
	lineWeight->setEnabled(false);
	//color->setColor(Qt::black);
	connect(horizontalGroup, SIGNAL(buttonClicked(int)), this, SLOT(checkState()));
	connect(verticalGroup, SIGNAL(buttonClicked(int)), this, SLOT(checkState()));

	save << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this);
	load << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this) << new QAction("Default", this);
	for (int i=0; i<save.size(); ++i) {
		save.at(i)->setData(i);
		connect(save.at(i), &QAction::triggered, this, &ScaleDock::saveTriggered);
	}
	for (int i=0; i<load.size(); ++i) {
		load.at(i)->setData(i);
		connect(load.at(i), &QAction::triggered, this, &ScaleDock::loadTriggered);
	}
	QString arg = QString("ScaleL");
	auto s = Settings(arg);
	lineWeight->setValue(s.penWidth());
	Font->setCurrentFont(s.font());
	fontSize->setFontSize(s.fontSize());
	useBackground->setChecked(s.hasBackground());
	color->setColor(s.color1());
}

ScaleDock::~ScaleDock() {
	QString arg = QString("ScaleL");
	auto s = Settings(arg);
	s.setPenWidth(lineWeight->value());
	s.setFont(Font->currentFont());
	s.setFontSize(fontSize->fontSize());
	s.setHasBackground(useBackground->isChecked());
	s.setColor1(color->color());
}

void ScaleDock::checkState() {
	if (QRadioButton *b = qobject_cast<QRadioButton*>(qobject_cast<QButtonGroup*>(sender())->checkedButton())) {
		QString n = b->objectName();
		if (n.contains("_2")) {
			//vertical:
			if (n.contains("topleft"))
				v = TL;
			else if (n.contains("topright"))
				v = TR;
			else if (n.contains("center"))
				v = C;
			else if (n.contains("bottomleft"))
				v = BL;
			else if (n.contains("bottomright"))
				v = BR;
			else if (n.contains("left"))
				v = L;
			else if (n.contains("top"))
				v = T;
			else if (n.contains("right"))
				v = R;
			else if (n.contains("bottom"))
				v = B;
			emit vertical_checked(v);
		}
		else {
			//horizontal:
			if (n.contains("topleft"))
				h = TL;
			else if (n.contains("topright"))
				h = TR;
			else if (n.contains("center"))
				h = C;
			else if (n.contains("bottomleft"))
				h = BL;
			else if (n.contains("bottomright"))
				h = BR;
			else if (n.contains("left"))
				h = L;
			else if (n.contains("top"))
				h = T;
			else if (n.contains("right"))
				h = R;
			else if (n.contains("bottom"))
				h = B;
			emit horizontal_checked(h);
		}
	}
}

void ScaleDock::sendState() {
	emit checked_changed(h, v);
}

void ScaleDock::on_rem_clicked() {
	if (rem->isChecked()) {
		rem->setText("Disable Scale");
		frame_5->setEnabled(true);
		length->setEnabled(true);
		unit->setEnabled(true);
		style_->setEnabled(true);
		Font->setEnabled(true);
		fontSize->setEnabled(true);
		useBackground->setEnabled(true);
		color->setEnabled(true);
		lineWeight->setEnabled(true);
		sendState();
	}
	else {
		rem->setText("Enable Scale");
		frame_5->setEnabled(false);
		length->setEnabled(false);
		unit->setEnabled(false);
		style_->setEnabled(false);
		Font->setEnabled(false);
		fontSize->setEnabled(false);
		useBackground->setEnabled(false);
		color->setEnabled(false);
		lineWeight->setEnabled(false);
		emit hideScale();
	}
}

void ScaleDock::contextMenuEvent(QContextMenuEvent* event) {
	QMenu menu(this);
	QMenu* sv = menu.addMenu(tr("&Save"));
	QMenu* ld = menu.addMenu(tr("&Load"));
	sv->addActions(save);
	ld->addActions(load);
	menu.exec(event->globalPos());
	event->accept();
}

void ScaleDock::saveProfile(int index) {
	if (index > 4) return;
	QString arg = QString("Scale%1").arg(index);
	auto s = Settings(arg);
	s.setColor1(color->color());
	s.setPenWidth(lineWeight->value());
	s.setFont(Font->currentFont());
	s.setFontSize(fontSize->fontSize());
	s.setHasBackground(useBackground->isChecked());
}

void ScaleDock::loadProfile(int index) {
	if (index > 5) return;
	QString arg = QString("Scale%1").arg((index==5) ? "" : QString::number(index));
	auto s = Settings(arg);
	color->setColor(s.color1());
	lineWeight->setValue(s.penWidth());
	Font->setCurrentFont(s.font());
	fontSize->setFontSize(s.fontSize());
	useBackground->setChecked(s.hasBackground());
}

void ScaleDock::saveTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	saveProfile(i);
}

void ScaleDock::loadTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	loadProfile(i);
}

//Legend
LegendDock::LegendDock(QWidget* parent) :
	QDockWidget(parent), Ui::LegendDock()
{
	Ui::LegendDock::setupUi(this);
	headerFont->hide();
	headerFontSize->hide();
	label_2->hide();
	frame_2->hide();
	
	save << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this);
	load << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this) << new QAction("Default", this);
	for (int i=0; i<save.size(); ++i) {
		save.at(i)->setData(i);
		connect(save.at(i), &QAction::triggered, this, &LegendDock::saveTriggered);
	}
	for (int i=0; i<load.size(); ++i) {
		load.at(i)->setData(i);
		connect(load.at(i), &QAction::triggered, this, &LegendDock::loadTriggered);
	}
	QString arg = QString("LegendL");
	auto s = Settings(arg);
	Font->setCurrentFont(s.font());
	fontSize->setFontSize(s.fontSize());
	headerFont->setCurrentFont(qvariant_cast<QFont>(s.custom("_Font2", QFont("Courier"))));
	headerFontSize->setFontSize(s.custom("_FontSize2", 40).toInt());
	Size->setValue(s.size());
	penWidth->setValue(s.penWidth());
}

LegendDock::~LegendDock() {
	QString arg = QString("LegendL");
	auto s = Settings(arg);
	s.setFont(Font->currentFont());
	s.setFontSize(fontSize->fontSize());
	s.setCustom("_Font2", headerFont->currentFont());
	s.setCustom("_FontSize2", headerFontSize->fontSize());
	s.setSize(Size->value());
	s.setPenWidth(penWidth->value());
}

void LegendDock::contextMenuEvent(QContextMenuEvent* event) {
	QMenu menu(this);
	QMenu* sv = menu.addMenu(tr("&Save"));
	QMenu* ld = menu.addMenu(tr("&Load"));
	sv->addActions(save);
	ld->addActions(load);
	menu.exec(event->globalPos());
	event->accept();
}

void LegendDock::saveProfile(int index) {
	if (index > 4) return;
	QString arg = QString("Legend%1").arg(index);
	auto s = Settings(arg);
	s.setFont(Font->currentFont());
	s.setFontSize(fontSize->fontSize());
	s.setCustom("_Font2", headerFont->currentFont());
	s.setCustom("_FontSize2", headerFontSize->fontSize());
	s.setSize(Size->value());
	s.setPenWidth(penWidth->value());
}

void LegendDock::loadProfile(int index) {
	if (index > 5) return;
	QString arg = QString("Legend%1").arg((index==5) ? "" : QString::number(index));
	auto s = Settings(arg);
	Font->setCurrentFont(s.font());
	fontSize->setFontSize(s.fontSize());
	headerFont->setCurrentFont(qvariant_cast<QFont>(s.custom("_Font2", QFont("Courier"))));
	headerFontSize->setFontSize(s.custom("_FontSize2", 40).toInt());
	Size->setValue(s.size());
	penWidth->setValue(s.penWidth());
}

void LegendDock::saveTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	saveProfile(i);
}

void LegendDock::loadTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	loadProfile(i);
}

//Calibration
CalibrationScene::CalibrationScene(int mode, QObject* parent) :
	QGraphicsScene(parent), mode((Mode)mode)
{
	clickStep = Click1;
	moveStep = Move0;
	QPen p(Qt::red, 2);
	tmpLine1 = new QGraphicsLineItem();
	tmpLine1->setPen(p);
	tmpLine2 = new QGraphicsLineItem();
	tmpLine2->setPen(p);
	QBrush b(QColor(255, 0, 0, 128));
	tmpRect = new QGraphicsRectItem();
	tmpRect->setPen(Qt::NoPen);
	tmpRect->setBrush(b);
	pixelLength = 0;
	isCalibrated = false;
}

void CalibrationScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	if (event->button() == Qt::RightButton) {
		resetState();
		return;
	}
	if (mode == X) {
		if (clickStep == Click1) {
			isCalibrated = false;
			p1 = event->scenePos();
			tmpLine1->setLine(p1.x(), 0, p1.x(), height());
			addItem(tmpLine1);
			clickStep = Click2;
			moveStep = Move1;
		}
		else if (clickStep == Click2) {
			p2 = event->scenePos();
			tmpLine2->setLine(p2.x(), 0, p2.x(), height());
			addItem(tmpLine2);
			clickStep = ClickIdle;
			moveStep = MoveIdle;
			pixelLength = abs(QLineF(p1, p2).dx());
			isCalibrated = true;
		}
	}
	else if (mode == Y) {
		if (clickStep == Click1) {
			isCalibrated = false;
			p1 = event->scenePos();
			tmpLine1->setLine(0, p1.y(), width(), p1.y());
			addItem(tmpLine1);
			clickStep = Click2;
			moveStep = Move1;
		}
		else if (clickStep == Click2) {
			p2 = event->scenePos();
			tmpLine2->setLine(0, p2.y(), width(), p2.y());
			addItem(tmpLine2);
			clickStep = ClickIdle;
			moveStep = MoveIdle;
			pixelLength = abs(QLineF(p1, p2).dy());
			isCalibrated = true;
		}
	}
}

void CalibrationScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	if (mode == X) {
		if (moveStep == Move0) {
			tmpLine1->setLine(event->scenePos().x(), 0, event->scenePos().x(), height());
			addItem(tmpLine1);
			tmpRect->setVisible(false);
		}
		else if (moveStep == Move1) {
			tmpLine2->setLine(event->scenePos().x(), 0, event->scenePos().x(), height());
			addItem(tmpLine2);
			tmpRect->setRect(QRectF(p1.x(), 0, QLineF(p1, event->scenePos()).dx(), height()).normalized());
			tmpRect->setVisible(true);
			addItem(tmpRect);
			emit distanceChanged(abs(QLineF(p1, event->scenePos()).dx()));
		}
	}
	else if (mode == Y) {
		if (moveStep == Move0) {
			tmpLine1->setLine(0, event->scenePos().y(), width(), event->scenePos().y());
			addItem(tmpLine1);
			tmpRect->setVisible(false);
		}
		else if (moveStep == Move1) {
			tmpLine2->setLine(0, event->scenePos().y(), width(), event->scenePos().y());
			addItem(tmpLine2);
			tmpRect->setRect(QRectF(0, p1.y(), width(), QLineF(p1, event->scenePos()).dy()).normalized());
			tmpRect->setVisible(true);
			addItem(tmpRect);
			emit distanceChanged(abs(QLineF(p1, event->scenePos()).dy()));
		}
	}
}

void CalibrationScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
}

void CalibrationScene::resetState() {
	clickStep = Click1;
	moveStep = Move0;
	pixelLength = 0;
	isCalibrated = false;
	for (auto item : items()) {
		if (item == tmpLine1 || item == tmpLine2 || item == tmpRect)
			removeItem(item);
	}
	emit distanceChanged(0);
}

void setImage(QGraphicsScene* scn, const QString& path)
{
	for (auto item : scn->items())
		if (item->type() == QGraphicsPixmapItem::Type) scn->removeItem(item);
	auto pix = tryOpenPixmap( path );
	auto myPix = scn->addPixmap(pix);
	scn->setSceneRect(pix.rect());
	myPix->setAcceptHoverEvents(true);
}

void CalibrationScene::setImage(const QPixmap& pix) {
	for (auto item : items())
		if (item == myPix) delete item;
	myPix = addPixmap(pix);
	setSceneRect(pix.rect());
	myPix->setAcceptHoverEvents(true);
}

//CalibrationWizard
#include "ScaleItem.h"
CalibrationWizard::CalibrationWizard(const QString& srcImg, QWidget *parent):
	QWizard(parent), Ui::CalibrationWizard(), imgSrc(srcImg)
{
	Ui::CalibrationWizard::setupUi(this);
	setWizardStyle(QWizard::ModernStyle);
	xScene = new CalibrationScene(0, this);
	yScene = new CalibrationScene(1, this);
	xView->setScene(xScene);
	yView->setScene(yScene);
	reviewScene = new QGraphicsScene(this);
	QPixmap src = tryOpenPixmap(imgSrc);
	reviewScene->addPixmap(src);
	reviewScene->setSceneRect(src.rect());
	reviewView->setScene(reviewScene);
	reviewView->setInteractive(true);
	xResult = 0;
	yResult = 0;
	connect(xScene, SIGNAL(distanceChanged(int)), this->xPixel, SLOT(setNum(int)));
	connect(yScene, SIGNAL(distanceChanged(int)), this->yPixel, SLOT(setNum(int)));
	connect(this, &QWizard::currentIdChanged, this, &CalibrationWizard::hideNextButton);
	//setWindowState(Qt::WindowMaximized);

	//auto btn = button(QWizard::CustomButton1);
	//btn->setText("&Back");
	//connect(btn, SIGNAL(clicked()), this, SLOT(back()));
	//connect(this, &QWizard::currentIdChanged, [btn](int id) { btn->setEnabled(id != 0); });
}

void initScene(CalibrationScene* scn, const QString& image)
{
	scn->resetState();
	auto im = tryOpenPixmap(image);
	scn->setImage(im);
}

void CalibrationWizard::initView(GraphicsView* view)
{
	view->zoomToFit();
	view->zoomIn();
	view->zoomIn();
	view->zoomIn();
	view->zoomIn();
}

bool CalibrationWizard::isImageExist(const QString& file)
{
	if (QFile::exists(file)) return true;
	QMessageBox::warning(this, "Invalid Image File", "Please browse your drive for image to continue.");
	return false;
}

void CalibrationWizard::initReview(const QString& image)
{
	reviewImgPath = image;
	setImage(reviewScene, reviewImgPath);
	addScale();
	initView(reviewView);
}

bool CalibrationWizard::validateCurrentPage() {
	int pixelX, pixelY;
	QPixmap reviewPix;
	int tmpLength;
	switch (currentId()) {
	case PageStatus::XOpen:
		if (xUseImageFromScene->isChecked())
			xImgPath = imgSrc;
		else if (xUseImageFromDrive->isChecked()) {
			if (isImageExist(xImgDir->text()))
				xImgPath = xImgDir->text();
			else
				return false;
		}
		xPix = tryOpenPixmap(xImgPath);
		initScene(xScene, xImgPath);
		initView(xView);
		break;
	case PageStatus::XCalibrate:
		if (!xScene->IsCalibrated()) {
			QMessageBox::warning(this, "Invalid pixel length", "Please pick two points from the scene.");
			return false;
		}
		else if (xLength->value() == 0) {
			QMessageBox::warning(this, "Invalid pixel length", "Please insert real width between two points.");
			return false;
		}
		else {
			pixelX = xScene->PixelLength();
			tmpLength = ("mm" == xUnit->currentText()) ? xLength->value() * 1000 : xLength->value();
			xResult = 1.0 * xPix.width() / pixelX * tmpLength;
			yResult = 1.0 * xPix.height() / xPix.width() * xResult;
		}
		break;
	case PageStatus::YIntro:
		initReview(xImgPath);
		break;
	case PageStatus::YOpen:
		if (yUseImageFromScene->isChecked())
			yImgPath = imgSrc;
		else if (yUseImageFromX->isChecked())
			yImgPath = xImgPath;
		else if (yUseImageFromDrive->isChecked()) {
			if (isImageExist(yImgDir->text()))
				yImgPath = yImgDir->text();
			else return false;
		}
		yPix = tryOpenPixmap(yImgPath);
		initScene(yScene, yImgPath);
		initView(yView);
		break;
	case PageStatus::YCalibrate:
		if (!yScene->IsCalibrated()) {
			QMessageBox::warning(this, "Invalid pixel length", "Please pick two points from the scene.");
			return false;
		}
		else if (yLength->value() == 0) {
			QMessageBox::warning(this, "Invalid pixel length", "Please insert real width between two points.");
			return false;
		}
		else {
			pixelY = yScene->PixelLength();
			tmpLength = ("mm" == yUnit->currentText()) ? yLength->value() * 1000 : yLength->value();
			yResult = 1.0 * yPix.height() / pixelY * tmpLength;
		}

		initReview(yImgPath);
		break;
	case PageStatus::Review:
		break;
	case PageStatus::Finish:
		break;
	}
	return QWizard::validateCurrentPage();
}

int CalibrationWizard::nextId() const {
	switch (currentId()) {
	case XOpen:
		break;
	case XCalibrate:
		break;
	case YIntro:
		if (!calibrateY) return Review;
		else return QWizard::nextId();
		break;
	case YOpen:
		break;
	case YCalibrate:
		break;
	case Review:
		break;
	case Finish:
		break;
	}
	return QWizard::nextId();
}

void CalibrationWizard::on_xBrowseImage_clicked() {
	QFileDialog openFileDg(this);
	openFileDg.setFileMode(QFileDialog::ExistingFile);
	openFileDg.setNameFilter("Image Files (*.png; *.jpg; *.bmp);;Portable Network Graphics File (*.png);;JPEG File-Format (*.jpg);;Bitmap File (*.bmp)");
	QString imgFile;
	if (openFileDg.exec()) {
		imgFile = openFileDg.selectedFiles().at(0);
		xImgDir->setText(imgFile);
	}
}

void CalibrationWizard::on_yBrowseImage_clicked() {
	QFileDialog openFileDg(this);
	openFileDg.setFileMode(QFileDialog::ExistingFile);
	openFileDg.setNameFilter("Image Files (*.png; *.jpg; *.bmp);;Portable Network Graphics File (*.png);;JPEG File-Format (*.jpg);;Bitmap File (*.bmp)");
	QString imgFile;
	if (openFileDg.exec()) {
		imgFile = openFileDg.selectedFiles().at(0);
		yImgDir->setText(imgFile);
	}
}

void CalibrationWizard::on_cmdCalibrateY_clicked() {
	calibrateY = true;
	next();
}

void CalibrationWizard::on_cmdReview_clicked() {
	calibrateY = false;
	next();
}

void CalibrationWizard::addScale() {
	double xLen = 100.0 * reviewScene->width() / xResult;
	double yLen = 100.0 * reviewScene->height() / yResult;
	double wa = 0.05*reviewScene->width();
	double wm = 0.5*reviewScene->width();
	double wb = 0.9*reviewScene->width();
	double ha = 0.05*reviewScene->height();
	double hm = 0.5*reviewScene->height();
	double hb = 0.9*reviewScene->height();

	int offset = 100;

	QLineF hTL = QLineF(QPointF(wa+offset, ha), QPointF(wa+xLen+offset, ha));
	QLineF hBL = QLineF(QPointF(wa+offset, hb), QPointF(wa+xLen+offset, hb));
	QLineF hT  = QLineF(QPointF(wm-0.5*xLen, ha), QPointF(wm+0.5*xLen, ha));
	QLineF hC  = QLineF(QPointF(wm-0.5*xLen, hm), QPointF(wm+0.5*xLen, hm));
	QLineF hB  = QLineF(QPointF(wm-0.5*xLen, hb), QPointF(wm+0.5*xLen, hb));
	QLineF hTR = QLineF(QPointF(wb-xLen-offset, ha), QPointF(wb-offset, ha));
	QLineF hBR = QLineF(QPointF(wb-xLen-offset, hb), QPointF(wb-offset, hb));

	QLineF vTL = QLineF(QPointF(wa, ha+offset), QPointF(wa, ha+yLen+offset));
	QLineF vTR = QLineF(QPointF(wb, ha+offset), QPointF(wb, ha+yLen+offset));
	QLineF vL  = QLineF(QPointF(wa, hm-0.5*yLen), QPointF(wa, hm+0.5*yLen));
	QLineF vC  = QLineF(QPointF(wm, hm-0.5*yLen), QPointF(wm, hm+0.5*yLen));
	QLineF vR  = QLineF(QPointF(wb, hm-0.5*yLen), QPointF(wb, hm+0.5*yLen));
	QLineF vBL = QLineF(QPointF(wa, hb-yLen-offset), QPointF(wa, hb-offset));
	QLineF vBR = QLineF(QPointF(wb, hb-yLen-offset), QPointF(wb, hb-offset));
	
	bool deleteOk = false;
	for (auto item : reviewScene->items())
		if (item == xScale) deleteOk = true;
	if (deleteOk) delete xScale;
	deleteOk = false;
	for (auto item : reviewScene->items())
		if (item == yScale) deleteOk = true;
	if (deleteOk) delete yScale;

	xScale = new SimpleScale(hBL);
	yScale = new SimpleScale(vBL);
	xScale->setFont(QFont("Arial"));
	yScale->setFont(QFont("Arial"));
	xScale->setFontSize(20);
	yScale->setFontSize(20);
	xScale->setText2("100 " + QString::fromLatin1("µm"));
	yScale->setText2("100 " + QString::fromLatin1("µm"));
	xScale->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	yScale->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	reviewScene->addItem(xScale);
	reviewScene->addItem(yScale);
}

void CalibrationWizard::hideNextButton(int index) {
	QAbstractButton *btn = this->button(QWizard::NextButton);
	if (index == YIntro)
		btn->hide();
	else 
		btn->show();
}