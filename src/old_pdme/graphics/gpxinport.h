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

#ifndef GPXINPORT_H
#define GPXINPORT_H
#include "gpxblock.h"
#include <QPainter>

class GpxInport:public GpxBlock {
 Q_OBJECT public:
	GpxInport(QGraphicsItem * parent, QGraphicsScene * scene, modelPort *);
	~GpxInport();
	BlockType blockType() {
		return InPort;
	};
	void rotate();
	void resizeTo(QPointF p);
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
		   QWidget * widget = 0);
	void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
	void setPortNumber(int i) {
		myPortNumber->setText(QString().setNum(i));
		update();
	};
	int getPortNumber() {
		return portNumber;
	};
	QString getDescription() {
		return portData->desc;
	};
	QList < modelParameter * >getParams() {
		return QList < modelParameter * >();
	};
	modelPort *getData() {
		return portData;
	};
	void setNoInteractive() {
		myText->setTextInteractionFlags(Qt::NoTextInteraction);
		forUseInLibrary = true;
		setAcceptHoverEvents(true);
	};
	void setName(QString name) {
		GpxBlock::setName(name);
		portData->name = name;
		emit dirtyChanged();
	};
 protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	void updateTextPosition();
 signals:
	void showStatusBarHelp(QString);
	void dirtyChanged();
 private:
	int portNumber;
	modelPort *portData;
	QGraphicsSimpleTextItem *myPortNumber;
};
#endif
