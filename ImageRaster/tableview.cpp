#include "stdafx.h"
#include "tableview.h"

TableView::TableView(QWidget *parent)
	: QTableView(parent), menu{ new QMenu(this) }
{
	auto action = menu->addAction("Copy");
	connect(action, &QAction::triggered, this, &TableView::copy);
}

TableView::~TableView()
{

}

void TableView::copy()
{
	QModelIndexList cells = selectedIndexes();
	qSort(cells); // Necessary, otherwise they are in column order

	QString text;
	int currentRow = 0; // To determine when to insert newlines
	foreach(const QModelIndex& cell, cells) {
		if (text.length() == 0) {
			// First item
		}
		else if (cell.row() != currentRow) {
			// New row
			text += '\n';
		}
		else {
			// Next cell
			text += '\t';
		}
		currentRow = cell.row();
		text += cell.data().toString();
	}

	QApplication::clipboard()->setText(text);
}

void TableView::keyPressEvent(QKeyEvent* event)
{
	if (event->matches(QKeySequence::Copy))
	{
		copy();
		event->accept();
		return;
	}
	QTableView::keyPressEvent(event);
}

void TableView::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton)
	{
		menu->exec(event->globalPos());
		event->accept();
		return;
	}
	QTableView::mousePressEvent(event);
}