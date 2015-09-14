#include "stdafx.h"
#include "BlendItem.h"

#include <QQmlContext>

#include "RasterSettings.h"

using namespace blend;

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

//class BlendScene
BlendScene::BlendScene(QObject* parent)
	: QGraphicsScene{ parent }, m_thumbnail{ 10, 10 }, base{ 10, 10 },
	lastThumbPos{ QPointF{ 50, 50 } }
{
	myText = new SimpleTextItem("");
	myText->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	myText->setPos(lastAnchorPos + QPointF(0, 138));
	myText->setZValue(10000);
	addItem(myText);
}

BlendScene::~BlendScene() {

}

void BlendScene::setThumbnail(const QString& file, const QRect& crop)
{
	auto im = QImage{ file }.copy(crop);
	m_thumbnail = QPixmap::fromImage(im);
}

QPixmap BlendScene::thumbnail() const
{
	return m_thumbnail;
}

void BlendScene::setBackground(const QString& file)
{
	for (auto item : items())
	{
		if (item->type() == QGraphicsPixmapItem::Type)
			removeItem(item);
	}
	base = QPixmap{ file };
	auto bg = addPixmap(base);
	setSceneRect(base.rect());
	bg->setZValue(-1000);
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
	ClipRect* frame_thumb = new ClipRect(m_thumbnail, QRectF(lastThumbPos, QSizeF(size, size)));
	frame_thumb->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	addItem(frame_thumb);
	return frame_thumb;
}

ClipCircle* BlendScene::addClipCircle(int size) {
	ClipCircle* frame_thumb = new ClipCircle(m_thumbnail, QRectF(lastThumbPos, QSizeF(size, size)));
	frame_thumb->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	addItem(frame_thumb);
	return frame_thumb;
}

FitRect* BlendScene::addFitRect(int width, int height) {
	FitRect* frame_thumb = new FitRect(m_thumbnail, QRectF(lastThumbPos, QSizeF(width, height)));
	frame_thumb->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
	addItem(frame_thumb);
	return frame_thumb;
}

FitCircle* BlendScene::addFitCircle(int width , int height) {
	FitCircle* frame_thumb = new FitCircle(m_thumbnail, QRectF(lastThumbPos, QSizeF(width, height)));
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

CropWidget::CropWidget(QWidget* parent)
	: QQuickWidget{ parent }, m_image{ "qrc:///Resources/SmartRaster.png" }
{
	connect(this, &QQuickWidget::statusChanged,
	[this](QQuickWidget::Status stat){ 
		qDebug() << stat; 
		if (stat == QQuickWidget::Error)
		{
			for (auto err : errors())
				qDebug() << err;
		}
	});	

	setResizeMode(QQuickWidget::SizeRootObjectToView);

	auto ctx = this->rootContext();
	ctx->setContextProperty("widget", this);
	setSource(QUrl{ "qrc:///CropWidget.qml" });
}

QUrl CropWidget::image() const
{
	return m_image;
}

void CropWidget::setImage(const QUrl& file)
{
	if (m_image != file)
	{
		m_image = file;
		emit imageChanged(file);
	}
}

QRect CropWidget::cropArea() const
{
	return m_crop;
}

void CropWidget::setCropArea(const QRect& rect)
{
	if (m_crop != rect)
	{
		m_crop = rect;
		emit cropAreaChanged(rect);
	}
}

BlendIntroPage::BlendIntroPage(QWidget* parent)
	: QWizardPage{ parent }
{
	setTitle(tr("Thumbnail Wizard"));
	setSubTitle(tr("Thumbnail is useful to provide magnification or to emphasize a particular part. To begin, press Next"));
}

OpenImagePage::OpenImagePage(QWidget* parent)
	: QWizardPage{ parent }, m_dir{ new QLineEdit(this) }, m_browse{ new QPushButton(this) }
{
	setTitle(tr("Select Image for Thumbnail"));
	QHBoxLayout* hl = new QHBoxLayout{ this };
	setLayout(hl);
	layout()->addWidget(m_dir);
	layout()->addWidget(m_browse);

	m_dir->setPlaceholderText(tr("Image file (*.jpg, *.png, *.bmp, etc.)"));
	m_browse->setText(tr("Browse"));

	registerField("imagePath*", m_dir);

	connect(m_browse, &QPushButton::clicked, [this](){
		auto path = QFileDialog::getOpenFileName(this, tr("Select Image"), "", "Image Files (*.png *.jpg *.bmp *.gif)");
		if (!path.isEmpty()) m_dir->setText(path);
	});
}

void OpenImagePage::cleanupPage()
{
	m_dir->setText("");
}

bool OpenImagePage::validatePage()
{
	QImage im{ m_dir->text() };
	if (im.isNull())
	{
		QMessageBox::critical(this, tr("Invalid Image"), 
			tr("Unfortunately, the application was unable to read the image. Please choose different image."));
		return false;
	}
	return true;
}

CropPage::CropPage(QWidget* parent)
	: QWizardPage{ parent }, m_crop{ new CropWidget(this) }
{
	setTitle(tr("Pick Thumbnail Area"));
	setSubTitle(
		tr("Select part of the image that will become thumbnail.") 
		);

	setLayout(new QHBoxLayout(this));
	layout()->addWidget(m_crop);
	registerField("cropArea", m_crop, "cropArea", SIGNAL(cropAreaChanged));
	m_crop->show();
}

void CropPage::initializePage()
{
	auto path = field("imagePath").toString();
	QFileInfo file{ path };
	auto url = QUrl::fromLocalFile(file.absoluteFilePath());
	m_crop->setImage(url);
}

StylePage::StylePage(const QString& background, QWidget* parent)
	: QWizardPage{ parent }, m_background{ background }, m_scene{ new BlendScene(this) }
{
	Ui::StylePage::setupUi(this);
	setTitle(tr("Thumbnail Size and Style"));
	setSubTitle(tr("Thumbnail size and style can be modified here. ") +
		tr("After closing this wizard, thumbnail can be moved but it's style and size cannot change."));

	view2->setDragMode(QGraphicsView::RubberBandDrag);
	view2->setScene(m_scene);
	
	save << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this);
	load << new QAction("Profile 1", this) << new QAction("Profile 2", this)
		<< new QAction("Profile 3", this) << new QAction("Profile 4", this)
		<< new QAction("Profile 5", this) << new QAction("Default", this);
	for (int i=0; i<save.size(); ++i) {
		save.at(i)->setData(i);
		connect(save.at(i), &QAction::triggered, this, &StylePage::saveTriggered);
	}
	for (int i=0; i<load.size(); ++i) {
		load.at(i)->setData(i);
		connect(load.at(i), &QAction::triggered, this, &StylePage::loadTriggered);
	}

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
}

StylePage::~StylePage()
{
	QString arg = "BlendL";
	auto s = Settings(arg);
	s.setColor1(color1->color());
	s.setColor2(color2->color());
	s.setPenWidth(lineweight->value());
	s.setFont(Font->currentFont());
	s.setFontSize(fontSize->fontSize());
	s.setCustom("_ThumbSize", thumbSize->value());
	s.setCustom("_AnchorSize", anchorSize->value());
}

void StylePage::setBackground(const QString& source)
{
	if (m_background != source)
	{
		m_background = source;
		emit backgroundChanged(source);
	}
}

QString StylePage::background() const
{
	return m_background;
}

BlendScene* StylePage::scene()
{
	return m_scene;
}

BlendScene* StylePage::scene() const
{
	return m_scene;
}

void StylePage::contextMenuEvent(QContextMenuEvent* event) {
	QMenu menu(this);
	QMenu* sv = menu.addMenu(tr("&Save"));
	QMenu* ld = menu.addMenu(tr("&Load"));
	sv->addActions(save);
	ld->addActions(load);
	menu.exec(event->globalPos());
	event->accept();
}

void StylePage::initializePage()
{
	m_scene->setBackground(background());
	m_scene->setThumbnail(field("imagePath").toString(), field("cropArea").toRect());
	on_styleCombo_currentIndexChanged(styleCombo->currentIndex());
	view2->zoomToActual();
}

void StylePage::saveTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	saveProfile(i);
}

void StylePage::saveProfile(int index)
{
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

void StylePage::loadProfile(int index)
{
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

void StylePage::loadTriggered() {
	int i = ((QAction*)sender())->data().toInt();
	loadProfile(i);
}

void StylePage::on_styleCombo_currentIndexChanged(int i) {
	m_scene->removeAnchor();
	m_scene->removeThumb();
	if (Circle == i) {
		m_scene->addClipCircle(thumbSize->value());
		m_scene->addCircleItem(anchorSize->value(), anchorSize->value());
	}
	else if (Box == i) {
		m_scene->addClipRect(thumbSize->value());
		m_scene->addRectItem(anchorSize->value(), anchorSize->value());
	}
	else if (Ellipse == i) {
		double ratio = 1.0 * m_scene->thumbnail().width() / m_scene->thumbnail().height();
		m_scene->addFitCircle(thumbSize->value(), thumbSize->value() / ratio);
		m_scene->addCircleItem(anchorSize->value(), anchorSize->value() / ratio);
	}
	else if (Rectangle == i) {
		double ratio = 1.0 * m_scene->thumbnail().width() / m_scene->thumbnail().height();
		m_scene->addFitRect(thumbSize->value(), thumbSize->value() / ratio);
		m_scene->addRectItem(anchorSize->value(), anchorSize->value() / ratio);
	}
	applyFormat();
}

void StylePage::applyFormat() {
	on_lineweight_valueChanged(lineweight->value());
}

void StylePage::on_color1_currentIndexChanged() {
	m_scene->setColor1(color1->color());
}

void StylePage::on_color2_currentIndexChanged() {
	m_scene->setColor2(color2->color());
}

void StylePage::on_lineweight_valueChanged(int i) {
	m_scene->setPenWidth(i);
}

void StylePage::on_thumbSize_valueChanged(int) {
	on_styleCombo_currentIndexChanged(styleCombo->currentIndex());
}

void StylePage::on_anchorSize_valueChanged(int) {
	on_styleCombo_currentIndexChanged(styleCombo->currentIndex());
}

void StylePage::on_text_textChanged() {
	m_scene->setText(text->toPlainText());
}

void StylePage::on_Font_currentFontChanged(const QFont & font) {
	m_scene->setFont(font);
}

void StylePage::on_fontSize_fontSizeChanged(int size) {
	m_scene->setFontSize(size);
}

//class BlendWizard
BlendWizard::BlendWizard(const QString& source, QWidget* parent)
	: QWizard{ parent, Qt::Dialog }, m_intro{ this }, m_open{ this }, m_crop{ this }, 
	m_style{ source, this }
{
	auto scr = QApplication::primaryScreen()->size();
	setGeometry((scr.width() - 800) / 2, (scr.height() - 600) / 2, 800, 600);

	setPage(Intro, &m_intro);
	setPage(OpenImage, &m_open);
	setPage(Crop, &m_crop);
	setPage(Style, &m_style);

	setWizardStyle(QWizard::ModernStyle);
	//setOption(QWizard::HaveCustomButton1, true);
	//setButtonText(QWizard::CustomButton1, "&Back");

	//connect(this, &QWizard::customButtonClicked, 
	//	[this](int which){ if (which == QWizard::CustomButton1) back(); });
	//
	//connect(this, &QWizard::currentIdChanged, 
	//	[this](int id) { button(CustomButton1)->setEnabled(id != Intro); });
}

QGraphicsItem* BlendWizard::getAnchor()
{
	return m_style.scene()->getAnchor();
}

QGraphicsItem* BlendWizard::getThumb()
{
	return m_style.scene()->getThumb();
}

SimpleTextItem* BlendWizard::getText()
{
	return m_style.scene()->getText();
}

QPointF BlendWizard::anchorPos() const
{
	return m_style.scene()->AnchorPos();
}

QPointF BlendWizard::thumbPos() const
{
	return m_style.scene()->ThumbPos();
}

QPointF BlendWizard::textPos() const
{
	return m_style.scene()->TextPos();
}