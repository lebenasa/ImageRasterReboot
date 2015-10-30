#ifndef IMAGERASTER_H
#define IMAGERASTER_H

#include <QtWidgets/QMainWindow>
#include "ui_imageraster.h"
#include "Docks.h"
#include "Enums.h"

class ActionGroup:
	public QObject
{
	Q_OBJECT
	QList<QPair<QAction*, int>> actions;
	int index;
public:
	ActionGroup(QObject* parent=0):
		QObject(parent), index(-1)	{ }
	~ActionGroup() { }

	void addAction(QAction* action, int number);
	void addAction(QAction* action);
	void clear() { actions.clear(); }

	int currentIndex() { return index; }
public slots:
	void triggerAt(int number);
signals:
	void actionTriggered(int);
};

class Logic;
class ImageRaster : public QMainWindow
{
	Q_OBJECT
	friend Logic;
	Logic* logic;

	Ui::ImageRasterClass ui;
	void initModes();
	ActionGroup* modes;
	void uncheckMode();
	int appState;
	int rulerState;

	MarkerDock* markerDock;
	RulerTools* rulerTools;
	ProfileDock* profileDock;
	QComboBox *profileCombo;
	QComboBox *modCombo;
	ScaleDock* scaleDock;
	LegendDock* legendDock;

	QList<QAction*> recents;

public:
	ImageRaster(QWidget *parent = 0);
	~ImageRaster();

	RulerDock *lrDock, *rrDock, *crDock, *c2Dock, *prDock, *mlDock, *c1Dock;
	void createRecents();
	void iopen(const QString& fname);

	QLabel* help;
	QLabel* zoom;
protected:
	void closeEvent(QCloseEvent* event);
public slots:
	void setAppState(int state);
	void openRecent();
	void on_actionOpenImage_triggered();
	bool on_actionSaveImage_triggered();
	void viewContextMenu(const QPoint& pos);
signals:
	void rulerContextMenu(const QPoint&);
};

#endif // IMAGERASTER_H
