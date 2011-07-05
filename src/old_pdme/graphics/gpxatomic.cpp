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

#include "gpxatomic.h"
#include "powergui.h"
#include "modelatomic.h"
#include <QtSvg>
#include "gpxconnectionpoint.h"

 GpxAtomic::GpxAtomic(QGraphicsItem * parent, QGraphicsScene * scene, modelAtomic * data):GpxBlock(parent,
	 scene)
{
	int inPorts = 0, outPorts = 0;
	atomicData = data;
	inPorts = atomicData->inPorts;
	outPorts = atomicData->outPorts;
	width = atomicData->graphics->width * TWIPSTOPIXEL;
	height = atomicData->graphics->height * TWIPSTOPIXEL;
	myImage = atomicData->graphics->icon;
	myBackgroundColor = GpxBlock::intToColor(atomicData->graphics->color);
	initDirection(atomicData->graphics->direction);
	initName(atomicData->name);
	for (int i = 0; i < inPorts; i++) {
		GpxBlock::addPort(GpxConnectionPoint::InPort);
	}
	for (int i = 0; i < outPorts; i++)
		GpxBlock::addPort(GpxConnectionPoint::OutPort);
	myPolygon << QPointF(-(width / 2), (height / 2)) << QPointF((width / 2),
								    (height /
								     2)) <<
	    QPointF((width / 2), -(height / 2)) << QPointF(-(width / 2),
							   -(height /
							     2)) <<
	    QPointF(-(width / 2), (height / 2));
	updateTextPosition();
	setPos(atomicData->graphics->x * TWIPSTOPIXEL + (width / 2),
	       atomicData->graphics->y * TWIPSTOPIXEL + (height / 2));
	myPriority = data->priority;
}

GpxAtomic::~GpxAtomic()
{
//              delete atomicData;
}

void
 GpxAtomic::resizeTo(QPointF p)
{
	GpxBlock::resizeTo(p);
	atomicData->graphics->width = width / TWIPSTOPIXEL;
	atomicData->graphics->height = height / TWIPSTOPIXEL;
	qDebug("Setting size to %d %d", atomicData->graphics->width,
	       atomicData->graphics->height);
	emit dirtyChanged();
}

void GpxAtomic::paint(QPainter * painter, const QStyleOptionGraphicsItem *,
		      QWidget *)
{
	painter->setPen(QPen (Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->setBrush(myBackgroundColor);
	painter->drawPolygon(myPolygon);
	if (isSelected()) {
		painter->setPen(QPen
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
	if (!myImage.isEmpty()) {
		if (myImage.endsWith(".svg")) {
			QSvgRenderer *svgr = new QSvgRenderer(myImage);
			QGraphicsSvgItem *svgImage = new QGraphicsSvgItem(myImage);
			svgImage->setSharedRenderer(svgr);
			if(!mustRotateImage)
				svgImage->rotate(myDirection * -90);
			svgImage->renderer()->render(painter,myPolygon.boundingRect().adjusted(1,1,-1,-1));
		} else {
			QImage image;
			image.load(myImage);
			if (!mustRotateImage)
			    image = image.transformed(QMatrix().rotate(myDirection * -90));
			painter->drawImage(myPolygon.boundingRect().adjusted(1, 1, -1, -1), image, image.rect());
		}
	}

}

void GpxAtomic::rotate()
{
	GpxBlock::rotate();
	atomicData->graphics->direction = myDirection;
	emit dirtyChanged();
}

void GpxAtomic::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{

}

void GpxAtomic::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
	emit editParameters(this);
	emit dirtyChanged();
}

void GpxAtomic::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	if (event->button() == Qt::RightButton && mayBeDrag == true) {
		qDebug("Context");
		emit contextMenu(this, event->screenPos());
		mayBeDrag = false;
	} else {
		QGraphicsItem::mouseReleaseEvent(event);
	}
}

QVariant GpxAtomic::itemChange(GraphicsItemChange change, const QVariant & value)
{
	if (change == ItemPositionHasChanged && scene()) {
		atomicData->graphics->x =
		    (pos().x() - width / 2) / TWIPSTOPIXEL;
		atomicData->graphics->y =
		    (pos().y() - height / 2) / TWIPSTOPIXEL;
		emit dirtyChanged();
	}
	return GpxBlock::itemChange(change, value);
}

void
 GpxAtomic::addPort(GpxConnectionPoint::ConnectionPointType pt)
{
	GpxBlock::addPort(pt);
	if (pt == GpxConnectionPoint::InPort) {
		atomicData->inPorts++;
	} else
		atomicData->outPorts++;
}

void GpxAtomic::removePorts()
{
	GpxBlock::removePorts();
	atomicData->inPorts = 0;
	atomicData->outPorts = 0;
}

void GpxAtomic::removePort(GpxConnectionPoint * port,
			   GpxConnectionPoint::ConnectionPointType pt)
{
	GpxBlock::removePort(port, pt);
	if (pt == GpxConnectionPoint::InPort)
		atomicData->inPorts--;
	else
		atomicData->outPorts--;
}

void GpxAtomic::setBackgroundColor(const QColor & color)
{
	GpxBlock::setBackgroundColor(color);
	atomicData->graphics->color = GpxBlock::colorToInt(color);
}

void GpxAtomic::setIcon(QString icon)
{
	GpxBlock::setIcon(icon);
	atomicData->graphics->icon = icon;
}
