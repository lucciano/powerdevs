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
//#include <QGraphicsObject>

#include "gpxblock.h"
#include "gpxconnectionpoint.h"
#include "gpxedge.h"
#include "gpxtextbox.h"
#include "parser.h"
#include "powergui.h"
#include "powerdialogs.h"

 GpxBlock::GpxBlock(QGraphicsItem * parent, QGraphicsScene * scene):QGraphicsItem(parent,scene)
{
	//myBackgroundColor = Qt::white;
	mustRotateImage = false;
	forUseInLibrary = false;
	myText = new GpxTextBox(this, scene);
	myDirection = 0;
	mayBeDrag = false;
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
}

GpxBlock::~GpxBlock()
{
	delete myText;

}

void GpxBlock::removePort(GpxConnectionPoint * p,
			  GpxConnectionPoint::ConnectionPointType pt)
{
	int index;
	GpxConnectionPoint *tmp;
	if (pt == GpxConnectionPoint::InPort) {
		for (int i=0;i<p->numberOfEdges();i++) {
			GpxEdge *edge = p->getEdgeAt(i);
			if (edge != NULL && edge->destPort()->connectionPointType() == GpxConnectionPoint::NoConnection) {
				scene()->removeItem(edge->destPort());
			}
		}
		index = inPorts.indexOf(p);
		if (index != -1)
			tmp = inPorts.takeAt(index);
		p->removeEdges();
		delete tmp;
	} else {
		for (int i=0;i<p->numberOfEdges();i++) {
			GpxEdge *edge = p->getEdgeAt(i);
			if (edge != NULL && edge->destPort()->connectionPointType() == GpxConnectionPoint::NoConnection) {
				scene()->removeItem(edge->destPort());
			}
		}
		index = outPorts.indexOf(p);
		tmp = outPorts.takeAt(index);
		p->removeEdges();
		delete tmp;
	}
	updatePortsPositions();
}

void GpxBlock::removePorts()
{
	foreach(GpxConnectionPoint * port, inPorts) {
		for (int i=0;i<port->numberOfEdges();i++) {
			GpxEdge *edge = port->getEdgeAt(i);
			if (edge != NULL && edge->destPort()->connectionPointType() == GpxConnectionPoint::NoConnection)
				scene()->removeItem(edge->destPort());
		}
		port->removeEdges();
		delete port;
	}
	foreach(GpxConnectionPoint * port, outPorts) {
		for (int i=0;i<port->numberOfEdges();i++) {
			GpxEdge *edge = port->getEdgeAt(i);
			if (edge != NULL
		    	&& edge->destPort()->connectionPointType() ==
			    GpxConnectionPoint::NoConnection)
				scene()->removeItem(edge->destPort());
		}
		port->removeEdges();
		delete port;
	}
}

void GpxBlock::addPort(GpxConnectionPoint::ConnectionPointType pt)
{
	if (pt == GpxConnectionPoint::InPort) {
		GpxConnectionPoint *p =
		    new GpxConnectionPoint(GpxConnectionPoint::InPort,
					   GpxConnectionPoint::Left, NULL,
					   scene());
		inPorts.append(p);
		qDebug("Agregando un puerto a %x la lista ahora tiene %d", this, inPorts.size());
		p->setParentItem(this);
	} else {
		GpxConnectionPoint *p =
		    new GpxConnectionPoint(GpxConnectionPoint::OutPort,
					   GpxConnectionPoint::Right, NULL,
					   scene());
		outPorts.append(p);
		p->setParentItem(this);
	}
	updatePortsPositions();
}

QPixmap GpxBlock::image()
{
	QPixmap pixmap(width + 10, height + 10);
	pixmap.fill(Qt::transparent);
	QPainter painter(&pixmap);
	painter.setPen(QPen(Qt::black, 8));
	painter.translate((width + 10) / 2, (height + 10) / 2);
	painter.setBrush(myBackgroundColor);
	paint(&painter, NULL, NULL);
	qreal block = height / inPorts.size();
	int i = 0;
	foreach(GpxConnectionPoint * p, inPorts) {
		painter.translate(-(width / 2),
				  (block / 2) + i * block - height / 2);
		p->paint(&painter, NULL, NULL);
		i++;
	}
	i = 0;
	block = height / outPorts.size();
	foreach(GpxConnectionPoint * p, outPorts) {
		painter.translate((width / 2 + 22),
				  (block / 2) + i * block - height / 2);
		p->paint(&painter, NULL, NULL);
		i++;
	}
	return pixmap;
}

QVariant GpxBlock::itemChange(GraphicsItemChange change, const QVariant & value)
{
	if (change == QGraphicsItem::ItemPositionHasChanged) {
		foreach(GpxConnectionPoint * port, inPorts) {
			qDebug("Updating ports");
			port->updatePosition();
		}
		foreach(GpxConnectionPoint * port, outPorts) {
			port->updatePosition();
		}
		prepareGeometryChange();
	}
	return value;
}

QRectF GpxBlock::boundingRect() const 
{
	QRectF br = myPolygon.boundingRect();
	return br.adjusted(-10 + (myDirection == 3 ? -15 : 0),
			   -10 + (myDirection == 2 ? -15 : 0),
			   10 + (myDirection == 1 ? 15 : 0),
			   10 + (myDirection == 0 ? 15 : 0));
}

void GpxBlock::updateTextPosition()
{
	QPointF pos;
	switch (myDirection) {
	case 0:
		pos.setX(-(myText->boundingRect().width() / 2));
		pos.setY(height / 2 + 3);
		break;
	case 1:
		pos.setY((myText->boundingRect().width() / 2));
		pos.setX(width / 2 + 5);
		break;
	case 2:
		pos.setX((myText->boundingRect().width() / 2));
		pos.setY(-height / 2 - 5);
		break;
	case 3:
		pos.setY(-(myText->boundingRect().width() / 2));
		pos.setX(-width / 2 - 5);
	}
	//qDebug("Usando direccion %d",myDirection);
	myText->setPos(pos);
}

void GpxBlock::setDirection(int d)
{
	QRectF old = scene()->sceneRect();
	while (d != myDirection)
		rotate();
	scene()->setSceneRect(old);
	return;
}

void GpxBlock::initDirection(int d)
{
	while (d != myDirection)
		rotate();
}

void GpxBlock::rotate()
{
	QGraphicsItem::rotate(90);
	foreach(GpxConnectionPoint * port, inPorts) {
		port->rotate();
		port->updatePosition();
	}
	foreach(GpxConnectionPoint * port, outPorts) {
		port->rotate();
		port->updatePosition();
	}
	qDebug("Old direction %d", myDirection);
	myDirection = (myDirection + 1) % 4;
	qDebug("New directoin %d", myDirection);
	prepareGeometryChange();
	update();
	updateTextPosition();
}

QPainterPath GpxBlock::shape() const 
{
	QPainterPath path;
	path.addPolygon(myPolygon);
	return path;
}

void GpxBlock::setBackgroundColor(const QColor & color)
{
	QList < QColor > colors;
	colors << Qt::white << Qt::black << Qt::red << Qt::darkRed << Qt::
	    green << Qt::darkGreen << Qt::blue << Qt::darkBlue << Qt::
	    cyan << Qt::darkCyan << Qt::magenta << Qt::darkMagenta << Qt::
	    yellow << Qt::darkYellow << Qt::gray << Qt::darkGray << Qt::
	    lightGray;
	myBackgroundColor = color;
	prepareGeometryChange();
	update();

}

void GpxBlock::setName(QString namearg)
{
	name = QString(namearg);
	myText->setPlainText(name);
	updateTextPosition();
}

void GpxBlock::setIcon(QString icon)
{
	myImage = icon;
	update();
	return;
}

int GpxBlock::isAtCorner(QPointF m)
{
	m = mapFromScene(m);
	double offset = 6;
	if (abs(-(width / 2) - m.x()) < offset
	    && abs(-(height / 2) - m.y()) < offset)
		return 1;
	if (abs((width / 2) - m.x()) < offset
	    && abs(-(height / 2) - m.y()) < offset)
		return 2;
	if (abs((width / 2) - m.x()) < offset
	    && abs((height / 2) - m.y()) < offset)
		return 3;
	if (abs(-(width / 2) - m.x()) < offset
	    && abs((height / 2) - m.y()) < offset)
		return 4;
	return 0;
}

void GpxBlock::resizeTo(QPointF p)
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
}

void GpxBlock::setSize(qreal w, qreal h)
{
	width = w;
	height = h;
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
	return;
}

void GpxBlock::setWidth(int w)
{
	setSize(w, height);
	return;
}

void GpxBlock::setHeight(int h)
{
	setSize(width, h);
	return;
}

void GpxBlock::updatePortsPositions()
{
	int i;
	GpxConnectionPoint *p;
	if (height < inPorts.size() * 8) {
		height = inPorts.size() * 8;
		setSize(width, height);
		emit(heightChanged((int)height));
	}
	if (height < outPorts.size() * 8) {
		height = outPorts.size() * 8;
		setSize(width, height);
		emit(heightChanged((int)height));
	}
	qreal block = height / inPorts.size();
	for (i = 0; i < inPorts.size(); i++) {
		p = inPorts.at(i);
		p->setPos(-(width / 2 + 3),
			  (block / 2) + i * block - height / 2);
	}
	block = height / outPorts.size();
	for (i = 0; i < outPorts.size(); i++) {
		p = outPorts.at(i);
		p->setPos((width / 2 + 6),
			  (block / 2) + i * block - height / 2);
	}
}

void GpxBlock::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	if (event->button() == Qt::RightButton)
		mayBeDrag = true;
	QGraphicsItem::mousePressEvent(event);

}

void GpxBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	if (event->button() == Qt::RightButton && mayBeDrag == true) {
		qDebug("Context");
		mayBeDrag = false;
	} else {
		QGraphicsItem::mouseReleaseEvent(event);
	}

}

void GpxBlock::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
	if (mayBeDrag) {
		qDebug("COPIAR");
		mayBeDrag = false;
		setSelected(true);
		emit dragCopy(event->scenePos());
	} else {
		QGraphicsItem::mouseMoveEvent(event);
		foreach(GpxConnectionPoint * p,inPorts)
			p->updatePosition();
		foreach(GpxConnectionPoint * p,outPorts)
			p->updatePosition();
	}
}

void GpxBlock::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	QString *s = new QString("");
	emit showStatusBarHelp(s);
	delete s;
}

void GpxBlock::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	QString *s = new QString(getDescription());
	qDebug("Emitiendo señal");
	emit showStatusBarHelp(s);
	delete s;
};

void GpxBlock::initName(QString namearg)
{
	name = QString(namearg);
	myText->setPlainText(name);
	updateTextPosition();
}

void GpxBlock::initPort(GpxConnectionPoint::ConnectionPointType pt)
{
	if (pt == GpxConnectionPoint::InPort) {
		GpxConnectionPoint *p =
		    new GpxConnectionPoint(GpxConnectionPoint::InPort,
					   GpxConnectionPoint::Left, NULL,
					   scene());
		inPorts.append(p);
		p->setParentItem(this);
	} else {
		GpxConnectionPoint *p =
		    new GpxConnectionPoint(GpxConnectionPoint::OutPort,
					   GpxConnectionPoint::Right, NULL,
					   scene());
		outPorts.append(p);
		p->setParentItem(this);
	}
	updatePortsPositions();
}

QColor GpxBlock::intToColor(int c)
{
	/* Falta ver como acomodar la lista, porque sobra un color */
	QList < QColor > colors;
	colors << Qt::black << Qt::darkBlue << Qt::darkGreen << Qt::
	    darkCyan << Qt::darkRed << Qt::darkMagenta << Qt::darkYellow << Qt::
	    gray << Qt::darkGray << Qt::blue << Qt::green << Qt::cyan << Qt::
	    red << Qt::lightGray << Qt::darkGreen <<Qt::white;
	return colors.at(c);
}

int GpxBlock::colorToInt(const QColor &c)
{
	QList < QColor > colors;
	colors << Qt::black << Qt::darkBlue << Qt::darkGreen << Qt::
	    darkCyan << Qt::darkRed << Qt::darkMagenta << Qt::darkYellow << Qt::
	    gray << Qt::darkGray << Qt::blue << Qt::green << Qt::cyan << Qt::
	    red << Qt::lightGray << Qt::white;
	return colors.indexOf(c);
}
