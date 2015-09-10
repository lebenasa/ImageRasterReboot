#pragma once
#include "qundostack.h"
#include <functional>

class BasicUndo:
	public QUndoCommand
{
	std::function<void()> Undo, Redo;
public:
	BasicUndo(std::function<void()> undo, std::function<void()> redo, QUndoCommand* parent=0):
		QUndoCommand(parent), Undo(undo), Redo(redo)
	{ }
	~BasicUndo() { }

	void undo() { Undo(); }
	void redo() { Redo(); }
};

template<class Item>
class ItemUndo:
	public QUndoCommand
{
	std::function<void(Item*)> Undo, Redo;
	Item* item;
public:
	ItemUndo(std::function<void(Item*)> undo, std::function<void(Item*)> redo, Item* item, QUndoCommand* parent=0):
		QUndoCommand(parent), Undo(undo), Redo(redo), item(item)
	{ }
	~ItemUndo() { }
	
	void undo() { Undo(item); }
	void redo() { Redo(item); }
};

template<class I, class M>
class ModelUndo:
	public QUndoCommand
{
	std::function<void(I*, M*)> Undo, Redo;
	I* item;
	M* model;
public:
	ModelUndo(std::function<void(I*, M*)> undo, std::function<void(I*, M*)> redo, I* item, M* model, QUndoCommand* parent=0):
		QUndoCommand(parent), Undo(undo), Redo(redo), item(item), model(model)
	{ }
	~ModelUndo() { }
	
	void undo() { Undo(item, model); }
	void redo() { Redo(item, model); }
};

