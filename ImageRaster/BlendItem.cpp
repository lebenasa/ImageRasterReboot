#include "stdafx.h"
#include "BlendItem.h"
#include "RasterSettings.h"

//class FrameRect
FrameRect::FrameRect(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent):
	RectItem(r, parent)
{
	setPix(pix);
}

void FrameRect::setPix(const QPixmap& pix) {
	myPix = pix;
}

//class FrameCircle
FrameCircle::FrameCircle(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent):
	EllipseItem(r, parent)
{
	setPix(pix);
}

void FrameCircle::setPix(const QPixmap& pix) {
	myPix = pix;
}

//class ClipRect
ClipRect::ClipRect(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent):
	FrameRect(pix, QRectF(r.topLeft(), QSizeF(r.width(), r.width())), parent)
{
	setPix(pix);
}

void ClipRect::setPix(const QPixmap& pix) {
	FrameRect::setPix(pix);
	QImage img = pix.toImage();
	//Crop middle part of the image to form square image:
	int w = img.width();
	int h = img.height();
	int s = (w > h) ? h : w;
	QPoint offset = (w > h) ? QPoint((w-s)/2, 0) : QPoint(0, (h-s)/2);
	QRect r = QRect(offset, QSize(s, s));
	QImage cropped = img.copy(r);
	//Scale it, and set it as brush:
	QImage scaled = cropped.scaled(rect().size().toSize(),
		Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	QBrush pix_brush = QBrush(scaled);
	QPointF currentTL = sceneTransform().map(boundingRect().topLeft());
	pix_brush.setTransform(QTransform::fromTranslate(currentTL.x(), currentTL.y()));
	setBrush(pix_brush);
}

//class ClipCircle
ClipCircle::ClipCircle(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent):
	FrameCircle(pix, QRectF(r.topLeft(), QSizeF(r.width(), r.width())), parent)
{
	setPix(pix);
}

void ClipCircle::setPix(const QPixmap& pix) {
	FrameCircle::setPix(pix);
	QImage img = pix.toImage();
	//Crop middle part of the image to form square image:
	int w = img.width();
	int h = img.height();
	int s = (w > h) ? h : w;
	QPoint offset = (w > h) ? QPoint((w-s)/2, 0) : QPoint(0, (h-s)/2);
	QRect r = QRect(offset, QSize(s, s));
	QImage cropped = img.copy(r);
	//Scale it, and set it as brush:
	QImage scaled = cropped.scaled(rect().size().toSize(),
		Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	QBrush pix_brush = QBrush(scaled);
	QPointF currentTL = sceneTransform().map(boundingRect().topLeft());
	pix_brush.setTransform(QTransform::fromTranslate(currentTL.x(), currentTL.y()));
	setBrush(pix_brush);
}

//class FitRect
FitRect::FitRect(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent):
	FrameRect(pix, r, parent)
{
	setPix(pix);
}

void FitRect::setPix(const QPixmap& pix) {
	FrameRect::setPix(pix);
	QImage img = pix.toImage();
	//Scale it (strecthed), and set it as brush:
	QImage scaled = img.scaled(rect().size().toSize(),
		Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	QBrush pix_brush = QBrush(scaled);
	QPointF currentTL = sceneTransform().map(boundingRect().topLeft());
	pix_brush.setTransform(QTransform::fromTranslate(currentTL.x(), currentTL.y()));
	setBrush(pix_brush);
}

//class FitCircle
FitCircle::FitCircle(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent):
	FrameCircle(pix, r, parent)
{
	setPix(pix);
}

void FitCircle::setPix(const QPixmap& pix) {
	FrameCircle::setPix(pix);
	QImage img = pix.toImage();
	//Scale it (strecthed), and set it as brush:
	QImage scaled = img.scaled(rect().size().toSize(),
		Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	QBrush pix_brush = QBrush(scaled);
	QPointF currentTL = sceneTransform().map(boundingRect().topLeft());
	pix_brush.setTransform(QTransform::fromTranslate(currentTL.x(), currentTL.y()));
	setBrush(pix_brush);
}

//class CropScene
CropScene::CropScene(const QString& img, QObject* parent) :
	QGraphicsScene(parent)
{
	myPix = QPixmap(img);
	pix_item = addPixmap(myPix);
	pix_item->setAcceptHoverEvents(true);
	setSceneRect(myPix.rect());
	crop_ok = false;
	isDragged = false;
	zoom = 1.0;
}

void CropScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	if (event->button() == Qt::LeftButton && !crop_ok) {
		p1 = event->scenePos();
		isDragged = true;
	}
	QGraphicsScene::mousePressEvent(event);
}

void CropScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	if (!crop_ok && isDragged) {
		p2 = event->scenePos();
		QList<QGraphicsItem*> is;
		for (auto p : items())
			if (p == tmpRect)
				is.append(p);
		for (auto p : is)
			delete p;
		QPen pen = QPen(QBrush(Qt::black), 1, Qt::DashLine);
		tmpRect = new QGraphicsRectItem(QRectF(p1, p2).normalized());
		tmpRect->setPen(pen);
		addItem(tmpRect);
	}
	QGraphicsScene::mouseMoveEvent(event);
}

void CropScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	if (!crop_ok && isDragged) {
		p2 = event->scenePos();
		bound = QRectF(p1, p2).normalized();
		QList<QGraphicsItem*> is;
		for (auto p : items())
			if (p == tmpRect)
				is.append(p);
		for (auto p : is)
			delete p;
		QPen pen = QPen(QBrush(Qt::black), 1, Qt::DashLine);
		tmpRect = new QGraphicsRectItem(QRectF(p1, p2).normalized());
		tmpRect->setPen(pen);
		addItem(tmpRect);
		crop_ok = true;
		isDragged = false;
		QImage img = myPix.toImage();
		QPixmap cropped = QPixmap::fromImage(img.copy(bound.toRect()));
		removeItem(pix_item);
		croppedItem = addPixmap(cropped);
		croppedItem->setAcceptHoverEvents(true);
		setSceneRect(croppedItem->boundingRect());
		delete tmpRect;
	}
	else if (event->button() == Qt::RightButton && crop_ok) {
		crop_ok = false;
		delete croppedItem;
		pix_item = addPixmap(myPix);
		pix_item->setAcceptHoverEvents(true);
		setSceneRect(pix_item->boundingRect());
	}
	QGraphicsScene::mouseReleaseEvent(event);
}

QPixmap CropScene::cropped() {
	QImage img = myPix.toImage();
	cropPix = QPixmap::fromImage(img.copy(bound.toRect()));
	return cropPix;
}

//class BlendScene
BlendScene::BlendScene(const QString& source, const QPixmap& thumb, QObject* parent) :
	QGraphicsScene(parent)
{
	thumbnail = thumb;
	base = QPixmap(source);
	QGraphicsPixmapItem* bg = addPixmap(base);
	setSceneRect(base.rect());
	//WidgetSettings s(this);
	lastThumbPos = QPointF(50, 50);
	//lastAnchorPos = QPointF(s.ThumbSize+50, s.ThumbSize+50);
	//addCircleItem(s.AnchorSize, s.AnchorSize);
	//addClipCircle(s.ThumbSize);
	myText = new SimpleTextItem("");
	myText->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	//myText->setFont(s.Blend.Font);
	myText->setPos(lastAnchorPos + QPointF(0, 138));
	myText->setZValue(10000);
	addItem(myText);
}

BlendScene::~BlendScene() {

}

RectItem* BlendScene::addRectItem(int width, int height) {
	RectItem* frame_anchor = new RectItem(QRectF(lastAnchorPos, QSizeF(width, height)));
	frame_anchor->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	addItem(frame_anchor);
	return frame_anchor;
}

EllipseItem* BlendScene::addCircleItem(int width, int height) {
	EllipseItem* frame_anchor = new EllipseItem(QRectF(lastAnchorPos, QSizeF(width, height)));
	frame_anchor->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	addItem(frame_anchor);
	return frame_anchor;
}

ClipRect* BlendScene::addClipRect(int size) {
	ClipRect* frame_thumb = new ClipRect(thumbnail, QRectF(lastThumbPos, QSizeF(size, size)));
	frame_thumb->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	addItem(frame_thumb);
	return frame_thumb;
}

ClipCircle* BlendScene::addClipCircle(int size) {
	ClipCircle* frame_thumb = new ClipCircle(thumbnail, QRectF(lastThumbPos, QSizeF(size, size)));
	frame_thumb->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	addItem(frame_thumb);
	return frame_thumb;
}

FitRect* BlendScene::addFitRect(int width, int height) {
	FitRect* frame_thumb = new FitRect(thumbnail, QRectF(lastThumbPos, QSizeF(width, height)));
	frame_thumb->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	addItem(frame_thumb);
	return frame_thumb;
}

FitCircle* BlendScene::addFitCircle(int width , int height) {
	FitCircle* frame_thumb = new FitCircle(thumbnail, QRectF(lastThumbPos, QSizeF(width, height)));
	frame_thumb->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	addItem(frame_thumb);
	return frame_thumb;
}

void BlendScene::removeAnchor() {
	QList<QGraphicsItem*> pool;
	for (auto item : items()) {
		if (RectItem::Type == item->type() || EllipseItem::Type == item->type()) {
			lastAnchorPos = item->sceneTransform().map(item->boundingRect().topLeft());
			pool.append(item);
		}
	}
	for (auto i : pool)
		delete i;
}

void BlendScene::removeThumb() {
	QList<QGraphicsItem*> pool;
	for (auto item : items()) {
		if (ClipRect::Type == item->type() || ClipCircle::Type == item->type() ||
			FitRect::Type == item->type() || FitCircle::Type == item->type()) {
			lastThumbPos = item->sceneTransform().map(item->boundingRect().topLeft());
			pool.append(item);
		}
	}
	for (auto i : pool)
		delete i;
}

QPointF BlendScene::AnchorPos() const {
	for (auto item : items()) {
		if (RectItem::Type == item->type() || EllipseItem::Type == item->type()) {
			return item->sceneTransform().map(item->pos());
		}
	}
	return QPointF();
}

QPointF BlendScene::ThumbPos() const {
	for (auto item : items()) {
		if (ClipRect::Type == item->type() || ClipCircle::Type == item->type() ||
			FitRect::Type == item->type() || FitCircle::Type == item->type()) {
			return item->sceneTransform().map(item->pos());
		}
	}
	return QPointF();
}

QGraphicsItem* BlendScene::getAnchor() {
	for (auto item : items()) {
		if (RectItem::Type == item->type() || EllipseItem::Type == item->type()) {
			return item;
		}
	}
	return nullptr;
}

QGraphicsItem* BlendScene::getThumb() {
	for (auto item : items()) {
		if (ClipRect::Type == item->type() || ClipCircle::Type == item->type() ||
			FitRect::Type == item->type() || FitCircle::Type == item->type()) {
			return item;
		}
	}
	return nullptr;
}

void BlendScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsScene::mousePressEvent(event);
}

void BlendScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsScene::mouseMoveEvent(event);
}

void BlendScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	QGraphicsScene::mouseReleaseEvent(event);
}

QPointF BlendScene::lastTextTL() const { return myText->pos(); }

QPointF BlendScene::TextPos() const { return myText->pos(); }

//class BlendWizard
BlendWizard::BlendWizard(const QString& source, QWidget* parent) :
	QWizard(parent),
	Ui::BlendWizard()
{
	Ui::BlendWizard::setupUi(this);
	//QMetaObject::connectSlotsByName(this);
	base = source;
	
	save << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this);
	load << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this) << new QAction("Default", this);
	for (int i=0; i<save.size(); ++i) {
		save.at(i)->setData(i);
		connect(save.at(i), &QAction::triggered, this, &BlendWizard::saveTriggered);
	}
	for (int i=0; i<load.size(); ++i) {
		load.at(i)->setData(i);
		connect(load.at(i), &QAction::triggered, this, &BlendWizard::loadTriggered);
	}
}

BlendWizard::~BlendWizard() {
	QString arg = QString("BlendL");
	auto s = Settings(arg);
	s.setColor1(color1->color());
	s.setColor2(color2->color());
	s.setPenWidth(lineweight->value());
	s.setFont(Font->currentFont());
	s.setFontSize(fontSize->fontSize());
	s.setCustom("_ThumbSize", thumbSize->value());
	s.setCustom("_AnchorSize", anchorSize->value());
}

bool BlendWizard::validateCurrentPage() {
	if (Intro == currentId()) {
	}
	else if (OpenImg == currentId()) {
		if (!QFile::exists(browse->text())) {
			QMessageBox::warning(this, tr("Image Not Found"),
			tr("Unable to find image. Please browse for image again."), QMessageBox::Ok);
			return false;
		}
		else {
			thumbFile = browse->text();
			crop = new CropScene(thumbFile, this);
			view1->setScene(crop);
			view1->zoomIn(); view1->zoomIn(); view1->zoomIn(); view1->zoomIn(); view1->zoomIn(); 
			view1->zoomIn(); view1->zoomIn(); view1->zoomIn(); view1->zoomIn(); view1->zoomIn(); 
			QImage im = QImage(thumbFile);
		}
	}
	else if (Crop == currentId()) {
		crop_pix = crop->cropped();
		blend = new BlendScene(base, crop_pix, this);
		view2->setDragMode(QGraphicsView::RubberBandDrag);
		view2->setScene(blend);
		QString arg = QString("BlendL");
		auto s = Settings(arg);
		color1->setColor(s.color1());
		color2->setColor(s.color2());
		lineweight->setValue(s.penWidth());
		Font->setCurrentFont(s.font());
		fontSize->setFontSize(s.fontSize());
		thumbSize->setValue(s.custom("_ThumbSize", 300).toInt());
		anchorSize->setValue(s.custom("_AnchorSize", 64).toInt());
		text->setPlainText("Text");
		QImage im = QImage(base);
	}
	else if (StyleEdit == currentId()) {
	}
	return QWizard::validateCurrentPage();
}

void BlendWizard::contextMenuEvent(QContextMenuEvent* event) {
	if (currentId() == StyleEdit) {
		QMenu menu(this);
		QMenu* sv = menu.addMenu(tr("&Save"));
		QMenu* ld = menu.addMenu(tr("&Load"));
		sv->addActions(save);
		ld->addActions(load);
		menu.exec(event->globalPos());
		event->accept();
	}
	else event->ignore();
}

void BlendWizard::saveProfile(int index) {
	if (index > 4) return;
	QString arg = QString("Blend%1").arg(index);
	auto s = Settings(arg);
	s.setColor1(color1->color());
	s.setColor2(color2->color());
	s.setPenWidth(lineweight->value());
	s.setFont(Font->currentFont());
	s.setFontSize(fontSize->fontSize());
	s.setCustom("_ThumbSize", thumbSize->value());
	s.setCustom("_AnchorSize", anchorSize->value());
}

void BlendWizard::loadProfile(int index) {
	if (index > 5) return;
	QString arg = QString("Blend%1").arg((index==5) ? "" : QString::number(index));
	auto s = Settings(arg);
	color1->setColor(s.color1());
	color2->setColor(s.color2());
	lineweight->setValue(s.penWidth());
	Font->setCurrentFont(s.font());
	fontSize->setFontSize(s.fontSize());
	thumbSize->setValue(s.custom("_ThumbSize", 300).toInt());
	anchorSize->setValue(s.custom("_AnchorSize", 64).toInt());
}

void BlendWizard::saveTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	saveProfile(i);
}

void BlendWizard::loadTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	loadProfile(i);
}


void BlendWizard::on_browseButton_clicked() {
	QFileDialog openFileDg(this);
	openFileDg.setFileMode(QFileDialog::ExistingFile);
	openFileDg.setNameFilter("Image Files (*.png; *.jpg; *.bmp);;Portable Network Graphics File (*.png);;JPEG File-Format (*.jpg);;Bitmap File (*.bmp)");
	QString imgName;
	if (openFileDg.exec())
		imgName = openFileDg.selectedFiles().at(0);
	browse->setText(imgName);
}

void BlendWizard::on_styleCombo_currentIndexChanged(int i) {
	blend->removeAnchor();
	blend->removeThumb();
	if (Circle == i) {
		blend->addClipCircle(thumbSize->value());
		blend->addCircleItem(anchorSize->value(), anchorSize->value());
	}
	else if (Box == i) {
		blend->addClipRect(thumbSize->value());
		blend->addRectItem(anchorSize->value(), anchorSize->value());
	}
	else if (Ellipse == i) {
		double ratio = 1.0 * crop_pix.width() / crop_pix.height();
		blend->addFitCircle(thumbSize->value(), thumbSize->value() / ratio);
		blend->addCircleItem(anchorSize->value(), anchorSize->value() / ratio);
	}
	else if (Rectangle == i) {
		double ratio = 1.0 * crop_pix.width() / crop_pix.height();
		blend->addFitRect(thumbSize->value(), thumbSize->value() / ratio);
		blend->addRectItem(anchorSize->value(), anchorSize->value() / ratio);
	}
	applyFormat();
}

void BlendWizard::applyFormat() {
	on_lineweight_valueChanged(lineweight->value());
}

void BlendWizard::on_color1_currentIndexChanged() {
	blend->setColor1(color1->color());
}

void BlendWizard::on_color2_currentIndexChanged() {
	blend->setColor2(color2->color());
}

void BlendWizard::on_lineweight_valueChanged(int i) {
	blend->setPenWidth(i);
}

void BlendWizard::on_thumbSize_valueChanged(int) {
	on_styleCombo_currentIndexChanged(styleCombo->currentIndex());
}

void BlendWizard::on_anchorSize_valueChanged(int) {
	on_styleCombo_currentIndexChanged(styleCombo->currentIndex());
}

void BlendWizard::on_text_textChanged() {
	blend->setText(text->toPlainText());
}

void BlendWizard::on_Font_currentFontChanged(const QFont & font) {
	blend->setFont(font);
}

void BlendWizard::on_fontSize_fontSizeChanged(int size) {
	blend->setFontSize(size);
}

void BlendWizard::accept() {
	QWizard::accept();
}