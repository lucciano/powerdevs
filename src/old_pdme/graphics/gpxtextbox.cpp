/****************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**     Universidad Nacional de Rosario - Argentina.
**  Contact: PowerDEVS Information (kofman@fceia.unr.edu.ar, fbergero@fceia.unr.edu.ar)
**
**  This file is part of PowerDEVS.
**
**  PowerDEVS is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  PowerDEVS is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with PowerDEVS.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <QtGui>

#include "gpxtextbox.h"
#include "gpxeditionscene.h"
#include "powergui.h"
#include "gpxatomic.h"
#include "gpxcoupled.h"
#include "gpxinport.h"
#include "gpxoutport.h"

GpxTextBox::GpxTextBox(QGraphicsItem * parent, QGraphicsScene * scene)
 : QGraphicsTextItem(parent, scene)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
	setAcceptedMouseButtons(Qt::LeftButton);
	setTextInteractionFlags(Qt::NoTextInteraction);
	editing = false;
}

QVariant GpxTextBox::itemChange(GraphicsItemChange change,
				const QVariant & value)
{
	if (change == QGraphicsItem::ItemSelectedHasChanged && isSelected()) {
		if (textInteractionFlags() == Qt::NoTextInteraction) {
			if (parentItem() != NULL) {
				qDebug("Starting name edition");
				setTextInteractionFlags(Qt::
							TextEditorInteraction);
				//mainWindow->editingName();
				scene()->clearSelection();
			}
		}
	}
	return value;
}

void GpxTextBox::focusOutEvent(QFocusEvent * event)
{
	editing = false;
	//mainWindow->doneEditingName();
	qDebug("Focus out");
	setTextInteractionFlags(Qt::NoTextInteraction);
	if (parentItem() != NULL) {
		GpxBlock *a = qgraphicsitem_cast < GpxBlock * >(parentItem());
		GpxEditionScene *mc = (GpxEditionScene *) a->scene();
		qDebug("Setting name to %s", qPrintable(toPlainText()));
		if (toPlainText().trimmed().isEmpty()) {
			setPlainText(a->getName());
			return;
		}
		if (!mc->existsName(toPlainText(), a)) {
			if (a->blockType() == GpxBlock::SimpleAtomic)
				((GpxAtomic *) a)->setName(toPlainText());
			if (a->blockType() == GpxBlock::Coupled)
				((GpxCoupled *) a)->setName(toPlainText());
			if (a->blockType() == GpxBlock::InPort)
				((GpxInport *) a)->setName(toPlainText());
			if (a->blockType() == GpxBlock::OutPort)
				((GpxOutport *) a)->setName(toPlainText());
		} else {
			QMessageBox q(QMessageBox::Warning, "PowerDEVS",
				      QString("The name '%1' already exists").
				      arg(toPlainText()));
			q.exec();
			qDebug("MESAGEBOX REPEATED NAME");
			setPlainText(a->getName());
		}
	}
	if (event)
		QGraphicsTextItem::focusOutEvent(event);
	qDebug("Done editing");
}

/*
void GpxTextBox::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if(textInteractionFlags() & Qt::NoTextInteraction) {
		qDebug("Starting name edition");
		mainWindow->editingName();
		scene()->clearSelection();
		setTextInteractionFlags(Qt::TextEditorInteraction);
	}
	QGraphicsItem::mouseReleaseEvent(event);
}
*/
void GpxTextBox::keyPressEvent(QKeyEvent * event)
{
	/*mainWindow->editingName();
	   //scene()->clearSelection();
	   if (event->key()==Qt::Key_Return) 
	   focusOutEvent(NULL);
	   else */
	QGraphicsTextItem::keyPressEvent(event);

}

void GpxTextBox::paint(QPainter * painter,
		       const QStyleOptionGraphicsItem * option,
		       QWidget * widget)
{
	if (editing)
		QGraphicsTextItem::paint(painter, option, widget);
	else
		painter->drawText(QPointF(5, 10), toPlainText());
}

void GpxTextBox::focusInEvent(QFocusEvent * event)
{
	editing = true;
}
