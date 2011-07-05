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

#include "gpxoutport.h"
#include "powergui.h"
#include "modelcoupled.h"

 GpxOutport::GpxOutport(QGraphicsItem * parent, QGraphicsScene * scene, modelPort * data):GpxBlock(parent,
	 scene)
{
	portData = data;
	width = portData->dimension * TWIPSTOPIXEL;
	height = portData->dimension * TWIPSTOPIXEL;
	//setDirection(portData->direction);
	myBackgroundColor = Qt::white;
	myDirection = portData->direction;
	initDirection(myDirection);
	initName(portData->name);
	initPort(GpxConnectionPoint::InPort);
	QString str;
	portData->owner = this;
	str.setNum(portData->coupledPort);
	portNumber = portData->coupledPort;
	myPortNumber = new QGraphicsSimpleTextItem(str, this);
	myPortNumber->setFlags(QGraphicsItem::ItemIgnoresTransformations);
	myPortNumber->setFlags(QGraphicsItem::ItemIgnoresTransformations);
	myPortNumber->setZValue(zValue() - 1);
	myPortNumber->setPos(-(myPortNumber->boundingRect().width() / 2),
			     -(myPortNumber->boundingRect().height() / 2));
	myPolygon << QPointF(-(width / 2), (height / 2)) << QPointF((width / 2),
								    (height /
								     2))
	    << QPointF((width / 2), -(height / 2)) << QPointF(-(width / 2),
							      -(height / 2))
	    << QPointF(-(width / 2), (height / 2));
	updateTextPosition();
	setPos(portData->x * TWIPSTOPIXEL + (width / 2),
	       portData->y * TWIPSTOPIXEL + (height / 2));
}

GpxOutport::~GpxOutport()
{
//              delete portData;
}

void GpxOutport::resizeTo(QPointF p)
{
	p = mapFromScene(p);
	switch (resizeCorner) {
	case 1:
		width = -p.x() + width / 2;
		height = -p.y() + width / 2;
		break;
	case 2:
		width = p.x() + width / 2;
		height = -p.y() + width / 2;
		break;
	case 3:
		width = p.x() + width / 2;
		height = p.y() + width / 2;
		break;
	case 4:
		width = -p.x() + width / 2;
		height = p.y() + width / 2;
		break;

	}
	qreal m = (width < height ? width : height);
	width = height = m;
	if (width < 20) {
		width = 20;
	}
	if (height < 20) {
		height = 20;
	}
	if (height < inPorts.size() * 12) {
		height = inPorts.size() * 12;
	}
	if (height < outPorts.size() * 12) {
		height = outPorts.size() * 12;
	}
	myPolygon.clear();
	myPolygon << QPointF(-(width / 2), (height / 2)) << QPointF((width / 2),
								    (height /
								     2))
	    << QPointF((width / 2), -(height / 2)) << QPointF(-(width / 2),
							      -(height / 2))
	    << QPointF(-(width / 2), (height / 2));
	updatePortsPositions();
	prepareGeometryChange();
	update();
	updateTextPosition();
	portData->dimension = width / TWIPSTOPIXEL;
	emit dirtyChanged();
}

void GpxOutport::paint(QPainter * painter, const QStyleOptionGraphicsItem *,
		       QWidget *)
{
	painter->
	    setPen(QPen
		   (Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->setBrush(myBackgroundColor);
	painter->drawEllipse(myPolygon.boundingRect());
	if (isSelected()) {
		painter->
		    setPen(QPen
			   (Qt::red, 2, Qt::SolidLine, Qt::RoundCap,
			    Qt::MiterJoin));
		painter->drawLine(QPointF(-(width / 2 + 2), -(height / 2 - 2)),
				  QPointF(-(width / 2 + 2), -(height / 2 + 2)));
		painter->drawLine(QPointF(-(width / 2 + 2), -(height / 2 + 2)),
				  QPointF(-(width / 2 - 2), -(height / 2 + 2)));

		painter->drawLine(QPointF(-(width / 2 + 2), (height / 2 - 2)),
				  QPointF(-(width / 2 + 2), (height / 2 + 2)));
		painter->drawLine(QPointF(-(width / 2 + 2), (height / 2 + 2)),
				  QPointF(-(width / 2 - 2), (height / 2 + 2)));

		painter->drawLine(QPointF((width / 2 + 2), -(height / 2 - 2)),
				  QPointF((width / 2 + 2), -(height / 2 + 2)));
		painter->drawLine(QPointF((width / 2 + 2), -(height / 2 + 2)),
				  QPointF((width / 2 - 2), -(height / 2 + 2)));

		painter->drawLine(QPointF((width / 2 + 2), (height / 2 - 2)),
				  QPointF((width / 2 + 2), (height / 2 + 2)));

		painter->drawLine(QPointF((width / 2 + 2), (height / 2 + 2)),
				  QPointF((width / 2 - 2), (height / 2 + 2)));
	}
}

void GpxOutport::rotate()
{
	GpxBlock::rotate();
	portData->direction = myDirection;
	updateTextPosition();
	emit dirtyChanged();
}

void GpxOutport::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{

}

void GpxOutport::updateTextPosition()
{
	GpxBlock::updateTextPosition();
	switch (myDirection) {
	case 0:
		if (myPortNumber != NULL)
			myPortNumber->
			    setPos(-(myPortNumber->boundingRect().width() / 2),
				   -(myPortNumber->boundingRect().height() /
				     2));
		break;
	case 1:
		if (myPortNumber != NULL)
			myPortNumber->
			    setPos(-(myPortNumber->boundingRect().height() / 2),
				   +(myPortNumber->boundingRect().width() / 2));
		break;
	case 2:
		if (myPortNumber != NULL)
			myPortNumber->
			    setPos(+(myPortNumber->boundingRect().width() / 2),
				   +(myPortNumber->boundingRect().height() /
				     2));
		break;
	case 3:
		if (myPortNumber != NULL)
			myPortNumber->
			    setPos(+(myPortNumber->boundingRect().height() / 2),
				   -(myPortNumber->boundingRect().width() / 2));
	}
}

QVariant GpxOutport::itemChange(GraphicsItemChange change,
				const QVariant & value)
{
	if (change == ItemPositionHasChanged && scene()) {
		portData->x = (pos().x() - width / 2) / TWIPSTOPIXEL;
		portData->y = (pos().y() - height / 2) / TWIPSTOPIXEL;
		emit dirtyChanged();
	}
	return GpxBlock::itemChange(change, value);
}
