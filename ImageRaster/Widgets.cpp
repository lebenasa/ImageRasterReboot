#include "stdafx.h"
#include "Widgets.h"

ColorEditor::ColorEditor(QWidget* widget) :
	QComboBox(widget)
{
	populateList();
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetColorChanged(int)));
}

QColor ColorEditor::color() const {
	return qvariant_cast<QColor>(itemData(currentIndex(), Qt::DecorationRole));
}

void ColorEditor::setColor(QColor c) {
	setCurrentIndex(findData(c, int(Qt::DecorationRole)));
}

void ColorEditor::widgetColorChanged(int) {
	emit colorChanged(color());
}

void ColorEditor::populateList() {
	QStringList colorNames = QColor::colorNames();
	for (int i=0; i<colorNames.size(); ++i) {
		QColor color(colorNames[i]);
		insertItem(i, colorNames[i]);
		setItemData(i, color, Qt::DecorationRole);
	}
}