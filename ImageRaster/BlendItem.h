#pragma once
#include <QQuickWidget>

#include "GraphicsItem.h"
#include "ui_StylePage.h"

namespace blend {
	class FrameRect :
		public RectItem
	{
		QPixmap myPix;
	public:
		FrameRect(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent = 0);
		~FrameRect() { }

		virtual void setPix(const QPixmap& pix) = 0;
		QPixmap pix() const { return myPix; }
	};

	class FrameCircle :
		public EllipseItem
	{
		QPixmap myPix;
	public:
		FrameCircle(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent = 0);
		~FrameCircle() { }

		virtual void setPix(const QPixmap& pix) = 0;
		QPixmap pix() const { return myPix; }
	};

	class ClipRect :
		public FrameRect
	{
	public:
		ClipRect(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent = 0);
		~ClipRect() { }

		enum  { Type = UserType + 15 };
		int type() const { return Type; }

		void setPix(const QPixmap& pix);
	};

	class ClipCircle :
		public FrameCircle
	{
	public:
		ClipCircle(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent = 0);
		~ClipCircle() { }

		enum  { Type = UserType + 16 };
		int type() const { return Type; }

		void setPix(const QPixmap& pix);
	};

	class FitRect :
		public FrameRect
	{
	public:
		FitRect(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent = 0);
		~FitRect() { }

		enum  { Type = UserType + 17 };
		int type() const { return Type; }

		void setPix(const QPixmap& pix);
	};

	class FitCircle :
		public FrameCircle
	{
	public:
		FitCircle(const QPixmap& pix, const QRectF& r, QGraphicsItem* parent = 0);
		~FitCircle() { }

		enum  { Type = UserType + 18 };
		int type() const { return Type; }

		void setPix(const QPixmap& pix);
	};

	class BlendScene :
		public QGraphicsScene
	{
		Q_OBJECT
		bool visible;
		int myPenWidth;
		QColor myColor1, myColor2;
		QFont myFont;
		int myFontSize;
		bool myHasBackground;

		QPixmap m_thumbnail, base;
		QPointF lastAnchorPos, lastThumbPos;
		SimpleTextItem *myText;
	public:
		BlendScene(QObject* parent = nullptr);
		~BlendScene();

		void setThumbnail(const QString& file, const QRect& crop);
		QPixmap thumbnail() const;
		void setBackground(const QString& file);
		QPixmap background() const;

		RectItem* addRectItem(int width, int height);
		EllipseItem* addCircleItem(int width, int height);
		ClipRect* addClipRect(int size);
		ClipCircle* addClipCircle(int size);
		FitRect* addFitRect(int width, int height);
		FitCircle* addFitCircle(int width, int height);

		QPointF lastAnchorTL() const { return lastAnchorPos; }
		QPointF lastThumbTL() const { return lastThumbPos; }
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
		QColor color2() const { return myColor2; }
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

	class CropWidget : public QQuickWidget
	{
		Q_OBJECT
		Q_PROPERTY(QUrl image READ image WRITE setImage NOTIFY imageChanged)
		Q_PROPERTY(QRect cropArea READ cropArea WRITE setCropArea NOTIFY cropAreaChanged)
	public:
		CropWidget(QWidget* parent = nullptr);

		QUrl image() const;
		void setImage(const QUrl& file);

		QRect cropArea() const;
		void setCropArea(const QRect& rect);

	signals:
		void imageChanged(const QUrl&);
		void cropAreaChanged(const QRect&);

	private:
		QUrl m_image;
		QRect m_crop;
	};

	class BlendIntroPage : public QWizardPage
	{
		Q_OBJECT
	public:
		BlendIntroPage(QWidget* parent = nullptr);
	};

	class OpenImagePage : public QWizardPage
	{
		Q_OBJECT
		QLineEdit* m_dir;
		QPushButton* m_browse;
	public:
		OpenImagePage(QWidget* parent = nullptr);

		void cleanupPage();
		bool validatePage();
	};

	class CropPage : public QWizardPage
	{
		Q_OBJECT
		CropWidget* m_crop;
	public:
		CropPage(QWidget* parent = nullptr);

		void initializePage();
	};

	class StylePage : public QWizardPage, public Ui::StylePage
	{
		Q_OBJECT
		Q_PROPERTY(QString background READ background WRITE setBackground NOTIFY backgroundChanged)
		QString m_background;
		BlendScene* m_scene;
		QList<QAction*> save;
		QList<QAction*> load;

		void saveProfile(int index);
		void loadProfile(int index);
	public:
		StylePage(const QString& background, QWidget* parent = nullptr);
		~StylePage();

		void setBackground(const QString& source);
		QString background() const;

		enum StyleType {
			Circle, Box, Ellipse, Rectangle
		};
		void applyFormat();

		BlendScene* scene();
		BlendScene* scene() const;

		void initializePage();

	protected:
		void contextMenuEvent(QContextMenuEvent* event);

	public slots:
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

	signals:
		void backgroundChanged(const QString&);
	};

	class BlendWizard : public QWizard
	{
		Q_OBJECT
		BlendIntroPage m_intro;
		OpenImagePage m_open;
		CropPage m_crop;
		StylePage m_style;
	public:
		BlendWizard(const QString& source, QWidget* parent = 0);

		enum PageStatus {
			Intro, OpenImage, Crop, Style
		};

		QGraphicsItem* getAnchor();
		QGraphicsItem* getThumb();
		SimpleTextItem* getText();
		QPointF anchorPos() const;
		QPointF thumbPos() const;
		QPointF textPos() const;

	public slots:

	};
}	//namespace blend