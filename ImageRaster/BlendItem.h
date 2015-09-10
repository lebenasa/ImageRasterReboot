#pragma once
#include "GraphicsItem.h"

class FrameRect:
	public RectItem
{
	QPixmap myPix;
public:
	FrameRect(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent=0);
	~FrameRect() { }

	virtual void setPix(const QPixmap& pix) = 0;
	QPixmap pix() const { return myPix; }
};

class FrameCircle:
	public EllipseItem
{
	QPixmap myPix;
public:
	FrameCircle(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent=0);
	~FrameCircle() { }

	virtual void setPix(const QPixmap& pix) = 0;
	QPixmap pix() const { return myPix; }
};

class ClipRect:
	public FrameRect
{
public:
	ClipRect(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent=0);
	~ClipRect() { }

	enum  { Type = UserType + 15 };
	int type() const { return Type; }

	void setPix(const QPixmap& pix);
};

class ClipCircle:
	public FrameCircle
{
public:
	ClipCircle(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent=0);
	~ClipCircle() { }

	enum  { Type = UserType + 16 };
	int type() const { return Type; }

	void setPix(const QPixmap& pix);
};

class FitRect:
	public FrameRect
{
public:
	FitRect(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent=0);
	~FitRect() { }

	enum  { Type = UserType + 17 };
	int type() const { return Type; }

	void setPix(const QPixmap& pix);
};

class FitCircle:
	public FrameCircle
{
public:
	FitCircle(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent=0);
	~FitCircle() { }

	enum  { Type = UserType + 18 };
	int type() const { return Type; }

	void setPix(const QPixmap& pix);
};

class CropScene:
	public QGraphicsScene
{
	Q_OBJECT
	QPixmap myPix, cropPix;
	QGraphicsRectItem *tmpRect;
	QGraphicsPixmapItem* pix_item, *croppedItem;
	QPointF p1, p2;
	bool isDragged;
	bool crop_ok;
	double zoom;
	QRectF bound;
public:
	CropScene(const QString& img, QObject* parent=0);
	~CropScene(void) { }

	QPixmap cropped();
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

class BlendScene:
	public QGraphicsScene
{
	bool visible;
	int myPenWidth;
	QColor myColor1, myColor2;
	QFont myFont;
	int myFontSize;
	bool myHasBackground;

	QPixmap thumbnail, base;
	QPointF lastAnchorPos, lastThumbPos;
	SimpleTextItem *myText;
public:
	BlendScene(const QString& source, const QPixmap& thumb, QObject* parent=0);
	~BlendScene();

	RectItem* addRectItem(int width, int height);
	EllipseItem* addCircleItem(int width, int height);
	ClipRect* addClipRect(int size);
	ClipCircle* addClipCircle(int size);
	FitRect* addFitRect(int width, int height);
	FitCircle* addFitCircle(int width , int height);

	QPointF lastAnchorTL() const { return lastAnchorPos; }
	QPointF lastThumbTL() const { return lastThumbPos;  }
	QPointF lastTextTL() const;

	QPointF AnchorPos() const;
	QPointF ThumbPos() const;
	QPointF TextPos() const;
	QGraphicsItem* getAnchor();
	QGraphicsItem* getThumb();
	SimpleTextItem* getText() { return myText; }

	void removeAnchor();
	void removeThumb();

	void setPenColor(const QColor& shape, const QColor& outline) {
		myColor1 = shape;
		myColor2 = outline;
		for (auto i : items()) {
			if (auto item = dynamic_cast<RectItem*>(i))
				item->setPenColor(shape, outline);
			else if (auto item = dynamic_cast<EllipseItem*>(i))
				item->setPenColor(shape, outline);
		}
	}
	void setColor1(const QColor& shape_color) { setPenColor(shape_color, color1()); }
	QColor color1() const { return myColor1; }
	void setColor2(const QColor& outline_color) { setPenColor(color1(), outline_color); }
	QColor color2() const {	return myColor2; }
	void setPenWidth(int width) {
		myPenWidth = width;
		for (auto i : items()) {
			if (auto item = dynamic_cast<RectItem*>(i))
				item->setPenWidth(width);
			else if (auto item = dynamic_cast<EllipseItem*>(i))
				item->setPenWidth(width);
		}
	}
	int penWidth() const { return myPenWidth; }

	void setFont(const QFont& font) { myText->setFont(font); }
	QFont font() const { return myText->font(); }
	void setFontSize(int size) { myText->setFontSize(size); }
	int fontSize() const { return myText->fontSize(); }
	void setHasBackground(bool show) { myText->setHasBackground(show); }
	bool hasBackground() const { return myText->hasBackground(); }

	void setText(const QString& txt) { myText->setText(txt); }
	QString text() const { return myText->text(); }

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#include "ui_BlendWizard.h"
class BlendWizard:
	public QWizard,
	public Ui::BlendWizard
{
	Q_OBJECT
	QString thumbFile;
	CropScene* crop;
	QPixmap crop_pix;
	BlendScene* blend;
	QString base;
	QList<QAction*> save;
	QList<QAction*> load;
public:
	BlendWizard(const QString& source, QWidget* parent=0);
	~BlendWizard();

	enum PageStatus {
		Intro, OpenImg, Crop, StyleEdit
	} page_status;
	enum StyleType {
		Circle, Box, Ellipse, Rectangle
	};
	bool validateCurrentPage();
	void applyFormat();

	QGraphicsItem* getAnchor() { return blend->getAnchor(); }
	QGraphicsItem* getThumb() { return blend->getThumb(); }
	SimpleTextItem* getText() { return blend->getText(); }
	QPointF AnchorPos() const { return blend->AnchorPos(); }
	QPointF ThumbPos() const { return blend->ThumbPos(); }
	QPointF TextPos() const { return blend->TextPos(); }

	void accept();
		
	void saveProfile(int index);
	void loadProfile(int index);
protected:
	void contextMenuEvent(QContextMenuEvent* event);

public slots:
	void on_browseButton_clicked();
	void on_styleCombo_currentIndexChanged(int);
	void on_color1_currentIndexChanged();
	void on_color2_currentIndexChanged();
	void on_lineweight_valueChanged(int);
	void on_thumbSize_valueChanged(int);
	void on_anchorSize_valueChanged(int);
	void on_text_textChanged();
	void on_Font_currentFontChanged(const QFont & font);
	void on_fontSize_fontSizeChanged(int);
	void saveTriggered();
	void loadTriggered();
};
