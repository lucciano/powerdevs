/****************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**										 Universidad Nacional de Rosario - Argentina.
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

#ifndef GPXATOMIC_H
#define GPXATOMIC_H
#include <QPainter>

#include "gpxblock.h"

class GpxAtomic:public GpxBlock {
 Q_OBJECT public:
	GpxAtomic(QGraphicsItem * parent, QGraphicsScene * scene,
		  modelAtomic *);
	~GpxAtomic();
	BlockType blockType() {
		return SimpleAtomic;
	};
	void rotate();
	void resizeTo(QPointF p);
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
		   QWidget * widget = 0);
	void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	QList < modelParameter * >getParams() {
		return atomicData->params;
	};
	QString getDescription() {
		return atomicData->desc;
	};
	modelAtomic *getData() {
		return atomicData;
	};
	QString getPath() {
		return atomicData->path;
	};
	int getPriority() {
		return myPriority;
	};
	void setPriority(int i) {
		myPriority = i;
	};
	void setNoInteractive() {
		myText->setTextInteractionFlags(Qt::NoTextInteraction);
		forUseInLibrary = true;
		setAcceptHoverEvents(true);
	};
	void setName(QString name) {
		GpxBlock::setName(name);
		atomicData->name = name;
		emit dirtyChanged();
	};
	QString desc() {
		return atomicData->desc;
	};
	void removePort(GpxConnectionPoint * port,
			GpxConnectionPoint::ConnectionPointType pt);
	void removePorts();
	void addPort(GpxConnectionPoint::ConnectionPointType);
	void setBackgroundColor(const QColor & color);
	void setIcon(QString icon);

 signals:
	void editParameters(GpxBlock *);
	void contextMenu(GpxBlock *, QPoint);
	void showStatusBarHelp(QString);
	void dirtyChanged();
 protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
	QVariant itemChange(GraphicsItemChange change, const QVariant & value);
 private:
	modelAtomic * atomicData;
	int myPriority;
};
#endif
