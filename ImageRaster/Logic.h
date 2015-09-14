#pragma once
#include "qobject.h"
#include "imageraster.h"

class GraphicsScene;
class MarkerModel;
class MarkerContainer;
class MarkerItem;
class Logic;

class MarkerLogic:
	public QObject
{
	Q_OBJECT
	friend class UndoLogic;
	Logic* logic;
	QDataWidgetMapper* mapper;
public:
	MarkerLogic(Logic* logic);
	~MarkerLogic() { }

	MarkerModel* model;
	QDataWidgetMapper* map() { return mapper; }

	int count() const;
	MarkerContainer* at(int i);
public slots:
	void addMarkerContainer();
	void insertMarkerContainer(MarkerContainer*);
	void removeMarkerContainer(int row);
	void addMarker(MarkerItem*);
	void popMarker(int row);
	void setType(int type);
	int currentType() const;
signals:
	void indexChanged();
};

#include "RasterSettings.h"
class ProfileLogic:
	public QObject
{
	Q_OBJECT
	friend Logic;
	AppSettings settings;
	ProfileModel* model;
	QDataWidgetMapper* mapper;
	QTimer* profileSync;
	Logic* logic;
public:
	ProfileLogic(Logic* logic);

public slots:
	void appendProfile();
	void removeProfile();
	void calibrateProfile();
	void resetProfile();
signals:
	void currentProfileChanged(int width, int height);
};

#include "RulerItem.h"
class RulerLogic:
	public QObject
{
	Q_OBJECT
	Logic* logic;
public:
	int state;
	LineRulerModel* lr;
	RectRulerModel* rr;
	CircleRulerModel* cr;
	Circle2RulerModel* c2;
	PolyRulerModel* pr;
	PathRulerModel* mr;

	QDataWidgetMapper* mapLR;
	QDataWidgetMapper* mapRR;
	QDataWidgetMapper* mapCR;
	QDataWidgetMapper* mapC2;
	QDataWidgetMapper* mapPR;
	QDataWidgetMapper* mapMR;

	RulerModel* modelAt(int RulerItemType);

	RulerLogic(Logic* logic);
	~RulerLogic() { }
public slots:
	void setState(int rulerState) { state = rulerState; }
	void setRealMod(double rw, double rh);
	void setUnit(int type);
signals:
};

#include "ScaleItem.h"
class ScaleLogic:
	public QObject
{
	Q_OBJECT
	int myPenWidth;
	QFont myFont;
	int myFontSize;
	int scaleType;
	double realWidth, realHeight;
	Logic* logic;
	GraphicsScene* scene;
	SimpleScale* hs1, *vs1;
	BarScale* hs2, *vs2;
	RulerScale* hs3, *vs3;
	QLineF hLine(int h), vLine(int v);
	Unit unit;
public:
	ScaleLogic(Logic* logic);
	~ScaleLogic();

	QList<ScaleItem*> vs, hs;
	int hI, vI;
public slots:
	void showScale(int h, int v);
	void updateHScale(int h);
	void updateVScale(int v);
	void updateStyle();
	void setPenWidth(int width);
	void setLength(const QString& txt);
	void setFont(const QFont& font);
	void setFontSize(int size);
	void setHasBackground(int state);
	void setStyle(int style);
	void setRealMod(double w, double h);
	void setUnit(int Unit);
	void hideScale();
};

#include "LegendItem.h"
class LegendLogic:
	public QObject
{
	Q_OBJECT
	Logic* logic;
	MarkerModel* model;
public:
	LegendLogic(Logic*, MarkerModel*);
	~LegendLogic();
	LegendGroup* myGroup;
public slots:
	void updateModel() { myGroup->updateModel(); }
	void setPenWidth(int width) { myGroup->setPenWidth(width); }
	void setSize(int size) { myGroup->setSize(size); }
	void setFont(const QFont& font) { myGroup->setFont(font); }
	void setFontSize(int size) { myGroup->setFontSize(size); }
};

#include "UndoCommand.h"
class UndoLogic:
	public QObject
{
	Q_OBJECT
	Logic* logic;
	GraphicsScene* scene;
	MarkerLogic* markerLogic;
	RulerLogic* ruler;
	QUndoStack* stack;

public:
	UndoLogic(Logic* logic);
	~UndoLogic() { }
	QAction* createUndoAction() { return stack->createUndoAction(this); }
	QAction* createRedoAction() { return stack->createRedoAction(this); }
public slots:
	void addMarkerContainer();
	void removeMarkerContainer(int row);
	void addMarker(MarkerItem*);
	void removeMarker(MarkerItem*);
	void addLR(LineRuler*);
	void addRR(RectRuler*);
	void addCR(const QPointF& p1, const QPointF& p2, const QPointF& p3);
	void addC2(const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4, const QPointF& p5, const QPointF& p6);
	void addPR(PolyRuler*);
	void addMR(PathRuler*);
	void removeLR(LineRuler*);
	void removeRR(RectRuler*);
	void removeCR(CircleRuler*);
	void removeC2(Circle2Ruler*);
	void removePR(PolyRuler*);
	void removeMR(PathRuler*);
	void removeRuler(QGraphicsItem* item);
	void addItem(QGraphicsItem*);
	void removeItem(QGraphicsItem*);
	void addItems(QList<QGraphicsItem*> items);
	void removeItems(QList<QGraphicsItem*> items);
};

class Logic :
	public QObject
{
	Q_OBJECT
	friend ImageRaster;
	friend ProfileLogic;
	friend UndoLogic;
	GraphicsScene* scene;

	bool myHasImage;
	QString myImageName;
	QPixmap myImage;

	MarkerLogic* marker;
	void initMarkers();

	UndoLogic *undo;
	void initUndo();

	ProfileLogic* profile;
	void initProfile();

	RulerLogic* ruler;
	void initRuler();
	QAction* actionFinish;

	ScaleLogic* scale;
	void initScale();

	LegendLogic* legend;
	void initLegend();
public:
	Logic(ImageRaster* ui);
	~Logic();

	ImageRaster* ui;
	GraphicsScene* initScene(const QString& fname);
	GraphicsScene* getScene() { return scene; }
	bool hasImage() const { return myHasImage; }
	QString imgName() const { return myImageName; }

	void saveImage(const QString& fname);
	void saveProfile();
public slots:
	void viewRulerContextMenu(const QPoint& pos);
	void startBlendWizard();
	void selectAllItem();
	void removeSelected();
signals:

};
