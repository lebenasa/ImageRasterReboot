#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

class TableView : public QTableView
{
	Q_OBJECT
	QMenu* menu;
public:
	TableView(QWidget *parent);
	~TableView();

public slots:
	void copy();

protected:
	void keyPressEvent(QKeyEvent* event);
	void mousePressEvent(QMouseEvent* event);
};

#endif // TABLEVIEW_H
