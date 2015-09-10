#include "stdafx.h"
#include "imageraster.h"
#include "RasterSettings.h"
#include "Logic.h"
#include "GraphicsScene.h"

void ActionGroup::addAction(QAction* action, int number) {
	actions.append(QPair<QAction*, int>(action, number));
	connect(action, &QAction::triggered, [=]() { triggerAt(number); });
	index = (index == -1) ? 0 : index;
}

void ActionGroup::addAction(QAction* action) {
	auto number = (index == -1) ? 0 : actions.size();
	actions.append(QPair<QAction*, int>(action, number));
	connect(action, &QAction::triggered, [=]() { triggerAt(number); });
	index = (index == -1) ? 0 : index;
}

void ActionGroup::triggerAt(int number) {
	index = number;
	for (auto p = actions.begin(); p != actions.end(); ++p) {
		auto pair = *p;
		auto action = pair.first;
		auto num = pair.second;
		if (num == index)
			action->setChecked(true);
		else
			action->setChecked(false);
	}
	emit actionTriggered(index);
}

ImageRaster::ImageRaster(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	QSettings settings("Miconos", "ImageRaster", this);
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("windowState").toByteArray());
	markerDock = new MarkerDock(this);
	addDockWidget(Qt::RightDockWidgetArea, markerDock);
	markerDock->hide();
	rulerTools = new RulerTools(this);
	addDockWidget(Qt::RightDockWidgetArea, rulerTools);
	rulerTools->circle->hide();
	rulerTools->circle2->hide();
	rulerTools->hide();
	lrDock = new RulerDock(0, this);
	addDockWidget(Qt::RightDockWidgetArea, lrDock);
	lrDock->hide();
	rrDock = new RulerDock(1, this);
	addDockWidget(Qt::RightDockWidgetArea, rrDock);
	rrDock->hide();
	crDock = new RulerDock(2, this);
	addDockWidget(Qt::RightDockWidgetArea, crDock);
	crDock->hide();
	c2Dock = new RulerDock(3, this);
	addDockWidget(Qt::RightDockWidgetArea, c2Dock);
	c2Dock->hide();
	prDock = new RulerDock(4, this);
	addDockWidget(Qt::RightDockWidgetArea, prDock);
	prDock->hide();
	mlDock = new RulerDock(5, this);
	addDockWidget(Qt::RightDockWidgetArea, mlDock);
	mlDock->hide();
	profileDock = new ProfileDock(this);
	addDockWidget(Qt::RightDockWidgetArea, profileDock);
	profileDock->hide();
	scaleDock = new ScaleDock(this);
	addDockWidget(Qt::RightDockWidgetArea, scaleDock);
	scaleDock->hide();
	legendDock = new LegendDock(this);
	addDockWidget(Qt::RightDockWidgetArea, legendDock);
	legendDock->hide();

	profileCombo = new QComboBox(ui.mainToolBar);
	ui.mainToolBar->addSeparator();
	ui.mainToolBar->addWidget(new QLabel("Profile: ", ui.mainToolBar));
	ui.mainToolBar->addWidget(profileCombo);
	modCombo = new QComboBox(ui.mainToolBar);
	modCombo->addItem(QString::fromLatin1("µm"), Qt::DisplayRole);
	modCombo->addItem("mm", Qt::DisplayRole);
	//modCombo->addItem("cm", Qt::DisplayRole);
	//modCombo->addItem("dm", Qt::DisplayRole);
	//modCombo->addItem("m", Qt::DisplayRole);
	ui.mainToolBar->addSeparator();
	ui.mainToolBar->addWidget(new QLabel("Unit:  ", ui.mainToolBar));
	ui.mainToolBar->addWidget(modCombo);

	initModes();
	createRecents();

	//Init Editor Factory:
	QItemEditorFactory *factory = new QItemEditorFactory;
	QItemEditorCreatorBase *colorListCreator =
		new QStandardItemEditorCreator<ColorEditor>();
	QItemEditorCreatorBase *stringCreator =
		new QStandardItemEditorCreator<QLineEdit>();
	QItemEditorCreatorBase *labelCreator =
		new QStandardItemEditorCreator<LabelDelegate>();
	QItemEditorCreatorBase *fontSizeCreator =
		new QStandardItemEditorCreator<FontSizeCombo>();
	factory->registerEditor(QVariant::Color, colorListCreator);
	factory->registerEditor(QVariant::String, stringCreator);
	factory->registerEditor(QVariant::Int, labelCreator);
	factory->registerEditor(QVariant::Int, fontSizeCreator);
	QItemEditorFactory::setDefaultFactory(factory);

	help = new QLabel();
	zoom = new QLabel();
	statusBar()->addPermanentWidget(help);
	statusBar()->addPermanentWidget(zoom);

	logic = new Logic(this);
	ui.toolBar->setEnabled(false);
}

ImageRaster::~ImageRaster()
{
}

void ImageRaster::initModes() {
	appState = 0;
	rulerState = 0;
	modes = new ActionGroup(this);
	modes->addAction(ui.actionSelect);
	modes->addAction(ui.actionCounter);
	modes->addAction(ui.actionMeasure);
	modes->addAction(ui.actionCalibrate);
	modes->addAction(ui.actionScale);
	modes->addAction(ui.actionLegend);
	connect(modes, &ActionGroup::actionTriggered, this, &ImageRaster::setAppState);
	connect(rulerTools, &RulerTools::RulerChanged, [this] { setAppState(States::Measure); });
	setAppState(appState);
}

void ImageRaster::setAppState(int state) {
	appState = state;
	markerDock->hide();
	rulerTools->hide();
	lrDock->hide();
	rrDock->hide();
	crDock->hide();
	c2Dock->hide();
	prDock->hide();
	mlDock->hide();
	profileDock->hide();
	scaleDock->hide();
	legendDock->hide();
	using namespace States;
	switch (appState) {
	case Select:
		break;
	case Marker:
		markerDock->show();
		break;
	case Measure:
		rulerTools->show();
		switch (rulerTools->currentRuller()) {
		case Line:
			lrDock->show();
			break;
		case RulerState::Rect:
			rrDock->show();
			break;
		case Circle:
			crDock->show();
			break;
		case Circle2:
			c2Dock->show();
			break;
		case RulerState::Polygon:
			prDock->show();
			break;
		case Path:
			mlDock->show();
			break;
		default:
			lrDock->show();
			break;
		}
		break;
	case Calibration:
		profileDock->show();
		break;
	case Scale:
		scaleDock->show();
		break;
	case Legend:
		legendDock->show();
		break;
	}
}

void ImageRaster::iopen(const QString& fname) {
	if (logic->hasImage()) {
		if (fname == logic->imgName()) { }
		else {
			ImageRaster *other = new ImageRaster;
			other->show();
			other->iopen(fname);
		}
    }
    else {
        auto scn = logic->initScene(fname);
		ui.graphicsView->setScene(scn);
		ui.graphicsView->setInteractive(true);
		ui.graphicsView->setCacheMode(QGraphicsView::CacheBackground);
		ui.graphicsView->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(ui.graphicsView, &QWidget::customContextMenuRequested, this, &ImageRaster::viewContextMenu);
		connect(ui.actionZoomIn, &QAction::triggered, ui.graphicsView, &GraphicsView::zoomIn);
		connect(ui.actionZoomOut, &QAction::triggered, ui.graphicsView, &GraphicsView::zoomOut);
		connect(ui.actionZoomToFit, &QAction::triggered, ui.graphicsView, &GraphicsView::zoomToFit);
		connect(ui.actionZoomToActual, &QAction::triggered, ui.graphicsView, &GraphicsView::zoomToActual);
		//connect(ui.graphicsView, &GraphicsView::zoomChanged, this, &ImageRaster::zoomLevelChanged);
		ui.toolBar->setEnabled(true);
    }
}

void ImageRaster::createRecents() {
	ui.menuFile->removeAction(ui.actionExit);
	ui.menuFile->addSeparator();
	auto s = Settings("Global");
	auto names = s.custom("_RecentFiles", QVariant()).toStringList();
	for (auto action : recents)
		delete action;
	recents.clear();
	auto j = names.end();
	while (j != names.begin()) {
		--j;
		if (QFile::exists(*j))
		{
			auto recent = new QAction(*j, this);
			connect(recent, &QAction::triggered, this, &ImageRaster::openRecent);
			recents.append(recent);
			ui.menuFile->addAction(recent);
		}
	}
	ui.menuFile->addSeparator();
	ui.menuFile->addAction(ui.actionExit);
}

void ImageRaster::openRecent() {
	QAction* act = (QAction*)sender();
	QString name = act->text();
	createRecents();
	iopen(name);
}

void ImageRaster::closeEvent(QCloseEvent *event) {
	if (!logic->hasImage()) {
		event->accept();
		return;
	}
	QMessageBox msg;
	msg.setText(tr("Exit Prompt"));
	msg.setInformativeText(tr("Do you want to save before closing?"));
	msg.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msg.setDefaultButton(QMessageBox::Save);
	int ret = msg.exec();
	if (QMessageBox::Cancel == ret) {
		event->ignore();
		return;
	}
	logic->saveProfile();
	QSettings settings("Miconos", "ImageRaster");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("windowState", saveState());
	auto s = Settings("Global");
	s.setCustom("_LastProfile", profileCombo->currentIndex());
	if (QMessageBox::Save == ret) {
		if (!on_actionSaveImage_triggered()) {
			event->ignore();
			return;
		}
	}
	event->accept();
	QMainWindow::closeEvent(event);
}

void ImageRaster::on_actionOpenImage_triggered() {
	QFileDialog openFileDg(this);
	openFileDg.setFileMode(QFileDialog::ExistingFile);
	openFileDg.setNameFilter("Image Files (*.png; *.jpg; *.bmp);;Portable Network Graphics File (*.png);;\
		JPEG File-Format (*.jpg);;Bitmap File (*.bmp)");
	//openFileDg.setDirectory(QDir::home());
	//openFileDg.setViewMode(QFileDialog::Detail);
	QString imgName;
	if (openFileDg.exec())
		imgName = openFileDg.selectedFiles().at(0);
    if (imgName != "") {
		//Save recent image:
		auto s = Settings("Global");
		QStringList names = s.custom("_RecentFiles").toStringList();
		if (!names.contains(imgName)) {
			if (names.size() > 4) 
				names.pop_front();
			names.append(imgName);
		}
		s.setCustom("_RecentFiles", names);
		createRecents();
        iopen(imgName);
    }
}

bool ImageRaster::on_actionSaveImage_triggered() {
	if (!logic->hasImage()) return false;
	QString imgName = QFileDialog::getSaveFileName(this, "Save Edited Image",
		"", "Portable Network Graphics File (*.png);;JPEG File-Format (*.jpg);;Bitmap File (*.bmp)");
	if (imgName == "") return false;
	logic->saveImage(imgName);
	return true;
}

void ImageRaster::viewContextMenu(const QPoint& pos) {
	QMenu menu(this);
	using namespace States;
	if (appState == AppState::Measure) {
		emit rulerContextMenu(pos); 
		return;
	}
	menu.addAction(ui.actionSelectAll);
	menu.addAction(ui.actionDelete);
	menu.addSeparator();
	menu.addAction(ui.actionZoomIn);
	menu.addAction(ui.actionZoomOut);
	menu.addAction(ui.actionZoomToFit);
	menu.addAction(ui.actionZoomToActual);
	menu.exec(QCursor::pos());
}
