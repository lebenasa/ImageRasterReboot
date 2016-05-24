#pragma once

#include "ui_MarkerDock.h"
class MarkerDock :
	public QDockWidget,
	public Ui::MarkerDock
{
	Q_OBJECT
	QButtonGroup* modeGroup;
	QList<QAction*> save;
	QList<QAction*> load;
public:
	MarkerDock(QWidget* parent=0);
	~MarkerDock();
	void saveProfile(int index);
	void loadProfile(int index);

	QFont font() const;
	void setFont(const QFont& f);

protected:
	void contextMenuEvent(QContextMenuEvent* event);

signals:
	void dataChanged();
	void MarkerChanged(int);
	void State(int);	//Change MarkerState without calling setType() on model
	void fontChanged(const QFont&);

public slots:
	void applyPreset();
	void saveTriggered();
	void loadTriggered();
	void setMode(int mode);
};

#include "ui_RulerDock.h"
class RulerDock :
	public QDockWidget,
	public Ui::RulerDock
{
	Q_OBJECT
	QList<QAction*> save;
	QList<QAction*> load;
	int myType;
public:
	RulerDock(int ruler_type, QWidget* parent=0);
	~RulerDock();
	void saveProfile(int index);
	void loadProfile(int index);

	QList<QWidget*> widgets;

	QFont font() const;
	void setFont(const QFont& f);

protected:
	void contextMenuEvent(QContextMenuEvent* event);

signals:
	void dataChanged();
	void defaultText();
	void fontChanged(const QFont&);

public slots:
	void applyPreset();
	void saveTriggered();
	void loadTriggered();
	void emitData() { emit dataChanged(); }
};

#include "ui_RulerTools.h"
class RulerTools:
	public QDockWidget,
	public Ui::RulerTools
{
	Q_OBJECT
	QButtonGroup* rulers;
    QPushButton* circle2;
public:
	RulerTools(QWidget* parent=0);
	~RulerTools();

	int currentRuller() const { return rulers->checkedId(); }

public slots:
	void setRuler(int);
signals:
	void RulerChanged(int);
};

#include "ui_ProfileDock.h"
class ProfileDock:
	public QDockWidget,
	public Ui::ProfileDock
{
	Q_OBJECT
public:
	ProfileDock(QWidget* parent=0);
	~ProfileDock() { }
};

#include "ui_ScaleDock.h"
class ScaleDock :
	public QDockWidget,
	public Ui::ScaleDock
{
	Q_OBJECT
	QAction* saveSettingsAct;
	QAction* loadDefaultAct;
	QList<QCheckBox*> myCheckBox;
	QList<QAction*> save;
	QList<QAction*> load;
public:
	ScaleDock(QWidget* parent=0);
	~ScaleDock();
	enum Checked {
		TL, T, TR, L, C, R, BL, B, BR, None
	} h, v;
	void saveProfile(int index);
	void loadProfile(int index);

protected:
	void contextMenuEvent(QContextMenuEvent* event);

public slots:
	void checkState();
	void sendState();
	void on_rem_clicked();
	void saveTriggered();
	void loadTriggered();
signals:
	void checked_changed(int, int);
	void vertical_checked(int);
	void horizontal_checked(int);
	void hideScale();
};

#include "ui_LegendDock.h"
class LegendDock :
	public QDockWidget,
	public Ui::LegendDock	
{
	Q_OBJECT
	QList<QAction*> save;
	QList<QAction*> load;
public:
	LegendDock(QWidget* parent=0);
	~LegendDock();
	void saveProfile(int index);
	void loadProfile(int index);

protected:
	void contextMenuEvent(QContextMenuEvent* event);
	
public slots:
	void saveTriggered();
	void loadTriggered();
};

#include "qgraphicsscene.h"
class CalibrationScene:
	public QGraphicsScene
{
	Q_OBJECT
	QPointF p1, p2;
	QGraphicsLineItem *tmpLine1, *tmpLine2;
	QGraphicsLineItem *tmpLine;
	QGraphicsRectItem *tmpRect;
	QGraphicsPixmapItem *myPix;
	int pixelLength;
	bool isCalibrated;
public:
	CalibrationScene(int mode, QObject* parent=0);
	enum ClickStep {
		Click1, Click2, ClickIdle
	} clickStep;
	enum MoveStep {
		Move0, Move1, MoveIdle
	} moveStep;
	enum Mode {
		X, Y
	} mode;
	void resetState();
	void setImage(const QPixmap& pix);

	int PixelLength() const { return pixelLength; }
	bool IsCalibrated() const { return isCalibrated; }

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

signals:
	void distanceChanged(int);
};

#include "ui_CalibrationWizard.h"
class SimpleScale;
class CalibrationWizard:
	public QWizard,
	public Ui::CalibrationWizard
{
	Q_OBJECT
	QString xImgPath, yImgPath, imgSrc, reviewImgPath;
	CalibrationScene *xScene, *yScene;
	double xResult, yResult;
	QGraphicsScene *reviewScene;
	bool calibrateY;
	SimpleScale *xScale, *yScale;
	void addScale();
	QPixmap xPix;
	QPixmap yPix;
public:
	CalibrationWizard(const QString& srcImg, QWidget *parent=0);

	enum PageStatus {
		XOpen, XCalibrate, YIntro, YOpen, YCalibrate, Review, Finish
	} pageStatus;
	bool validateCurrentPage();
	int nextId() const;

	int XResult() const { return xResult; }
	int YResult() const { return yResult; }

public slots:
	void on_xBrowseImage_clicked();
	void on_yBrowseImage_clicked();
	void on_cmdCalibrateY_clicked();
	void on_cmdReview_clicked();
	void hideNextButton(int);

private:
	bool isImageExist(const QString& file);
	void initReview(const QString& image);

	void initView(GraphicsView*);
};