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

#ifndef GPXEDITIONSCENE_H
#define GPXEDITIONSCENE_H

#include <QGraphicsScene>
#include <QRubberBand>
#include "gpxblock.h"
#include "gpxtextbox.h"
#include "gpxconnection.h"
#include "gpxeditionwindow.h"
#include "gpxconnectionpoint.h"

class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QFont;
class QGraphicsTextItem;
class QColor;

class GpxEditionScene:public QGraphicsScene {
 Q_OBJECT public:
	enum Mode { InsertLine, InsertText, MoveItem, MoveLine, ResizeItem };

	 GpxEditionScene(QObject * parent, modelCoupled * d = 0);
	void setItemColor(const QColor & color);
	void deleteItem(GpxBlock * blockToDelete);
	bool existsName(QString n, GpxBlock * a);
	 QList < modelChild * >getChilds();
	void setMode(Mode mode);
	void removeBlock(GpxBlock *);
	void closeScene();
	void setOwner(GpxCoupled * c);
	GpxCoupled *getOwner() {
		return owner;
	};
	modelCoupled *createModelDataFromSelection();
	modelCoupled *getData() {
		updateConnections();
		return coupledData;
	};
	QString getName() {
		return coupledData->name;
	};
	modelCoupled *getFather() {
		return coupledData->father;
		//return coupledData;
	}
	modelCoupled *coupledData;
	void updateConnections();
	void paste(modelCoupled *, qreal offx = 30, qreal offy = 30);
	void setDeleted() {
		deleted = true;
	};
	bool isDeleted() {
		return deleted;
	};
	void updateOwners();
	QUndoStack *getUndoStack() { return uStack; };

	public slots:void editorLostFocus(GpxTextBox * item);

 signals:
	void itemInserted(GpxBlock * item);
	void textInserted(QGraphicsTextItem * item);
	void itemSelected(QGraphicsItem * item);
	void itemDeleted(QGraphicsItem * item);
	void edgeInserted(GpxEdge * edge);
	void dirtyChanged(bool dirty);
	void contextMenu(QPoint pos);

 protected:
	void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
	void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
	void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
	void dropEvent(QGraphicsSceneDragDropEvent * event);

 private:
	bool isItemChange(int type);
	void setDeletedScenes(modelCoupled * c);
	QString findName(QString);
	int maxInportNumber();
	int maxOutportNumber();
	int getMaxPriority();
	GpxCoupled *owner;
	QMenu *myItemMenu;
	Mode myMode;
	bool leftButtonDown;
	QPointF startPoint;
	GpxConnection *connection;
	GpxBlock *atomic;
	QFont myFont;
	GpxTextBox *textItem;
	QColor myTextColor;
	QColor myItemColor;
	QColor myLineColor;
	GpxEdge *movingEdge;
	//bool dirty;
	QGraphicsPolygonItem *rubberBand;
	QPointF origin;
	bool deleted;
	QUndoStack *uStack;
};

#endif
