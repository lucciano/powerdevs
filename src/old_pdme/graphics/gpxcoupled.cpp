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

#include "gpxcoupled.h"
#include "powergui.h"
#include "modelcoupled.h"
#include <QtSvg>

 GpxCoupled::GpxCoupled(QGraphicsItem * parent, QGraphicsScene * scene, modelCoupled * data):GpxBlock(parent,
	 scene)
{
	int inPorts = 0, outPorts = 0;
	coupledData = data;
	inPorts = coupledData->lsInPorts.count();
	outPorts = coupledData->lsOutPorts.count();
	width = coupledData->graphics->width * TWIPSTOPIXEL;
	height = coupledData->graphics->height * TWIPSTOPIXEL;
	myImage = coupledData->graphics->icon;
	myBackgroundColor = GpxBlock::intToColor(coupledData->graphics->color);
	initDirection(coupledData->graphics->direction);
	initName(coupledData->name);
	myEditionScene = NULL;
	for (int i = 0; i < inPorts; i++)
		GpxBlock::addPort(GpxConnectionPoint::InPort);
	for (int i = 0; i < outPorts; i++)
		GpxBlock::addPort(GpxConnectionPoint::OutPort);
	myPolygon << QPointF(-(width / 2), (height / 2)) << QPointF((width / 2),
								    (height /
								     2))
	    << QPointF((width / 2), -(height / 2)) << QPointF(-(width / 2),
							      -(height / 2))
	    << QPointF(-(width / 2), (height / 2));
	updateTextPosition();
	setPos(coupledData->graphics->x * TWIPSTOPIXEL + (width / 2),
	       coupledData->graphics->y * TWIPSTOPIXEL + (height / 2));
	myPriority = data->priority;
	openIn = -1;
	coupledData->owner = this;
}

GpxCoupled::~GpxCoupled()
{
	coupledData->owner = NULL;
//              delete coupledData;
}

void GpxCoupled::resizeTo(QPointF p)
{
	GpxBlock::resizeTo(p);
	coupledData->graphics->width = width / TWIPSTOPIXEL;
	coupledData->graphics->height = height / TWIPSTOPIXEL;
	emit dirtyChanged();
}

void GpxCoupled::paint(QPainter * painter, const QStyleOptionGraphicsItem *,
		       QWidget *)
{
	painter->
	    setPen(QPen
		   (Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->setBrush(myBackgroundColor);
	painter->drawPolygon(myPolygon);
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
	if (!myImage.isEmpty()) {
		if (myImage.endsWith(".svg")) {
			QSvgRenderer *svgr = new QSvgRenderer(myImage,this);
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

void GpxCoupled::rotate()
{
	GpxBlock::rotate();
	coupledData->graphics->direction = myDirection;
	emit dirtyChanged();
}

void GpxCoupled::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{

}

void GpxCoupled::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
	emit editParameters(this);
}

void GpxCoupled::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	if (event->button() == Qt::RightButton && mayBeDrag == true) {
		qDebug("Context");
		emit contextMenu(this, event->screenPos());
		mayBeDrag = false;
	} else {
		QGraphicsItem::mouseReleaseEvent(event);
	}

}

QVariant GpxCoupled::itemChange(GraphicsItemChange change,
				const QVariant & value)
{
	if (change == ItemPositionHasChanged && scene()) {
		coupledData->graphics->x =
		    (pos().x() - width / 2) / TWIPSTOPIXEL;
		coupledData->graphics->y =
		    (pos().y() - height / 2) / TWIPSTOPIXEL;
		emit dirtyChanged();
	}
	return GpxBlock::itemChange(change, value);
}

void GpxCoupled::addPort(GpxConnectionPoint::ConnectionPointType pt)
{
	GpxBlock::addPort(pt);
	/*if (pt == GpxConnectionPoint::InPort)
		coupledData->inPorts++;
	else
		coupledData->outPorts++;*/
}

void GpxCoupled::removePorts()
{
	GpxBlock::removePorts();
	/*coupledData->inPorts = 0;
	coupledData->outPorts = 0;*/
}

void GpxCoupled::removePort(GpxConnectionPoint * port,
			    GpxConnectionPoint::ConnectionPointType pt)
{
	GpxBlock::removePort(port, pt);
	/*if (pt == GpxConnectionPoint::InPort)
		coupledData->inPorts--;
	else
		coupledData->outPorts--;*/
}

void GpxCoupled::setBackgroundColor(const QColor & color)
{
	GpxBlock::setBackgroundColor(color);
	coupledData->graphics->color = GpxBlock::colorToInt(color);
}
	
void GpxCoupled::setIcon(QString icon)
{
	GpxBlock::setIcon(icon);
	coupledData->graphics->icon = icon;
}
