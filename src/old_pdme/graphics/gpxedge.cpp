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

#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>

#include "gpxedge.h"
#include "gpxconnectionpoint.h"

GpxEdge::GpxEdge(GpxConnectionPoint * sourcePort, GpxConnectionPoint * destPort, modelLine *m)
{
	source = sourcePort;
	dest = destPort;
	// I suppose that the error is due to the lack of scene of this item.
	// source->addEdge(this);
	// dest->addEdge(this);
	sourcePoint = mapFromItem(source, source->getConnectionPoint());
	destPoint = mapFromItem(dest, dest->getConnectionPoint());
	movingSegment = -1;

	QPointF middlePoint;
	switch (source->direction()) {
	case GpxConnectionPoint::Right:
	case GpxConnectionPoint::Left:
		middlePoint.setX(destPoint.x());
		middlePoint.setY(sourcePoint.y());
		break;
	case GpxConnectionPoint::Up:
	case GpxConnectionPoint::Down:
		middlePoint.setX(sourcePoint.x());
		middlePoint.setY(destPoint.y());
	default:
		break;
	}
	myPolygon << sourcePoint << middlePoint << destPoint;

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	ml=m;
}

GpxConnectionPoint *GpxEdge::sourcePort() const
{
	return source;
}

void GpxEdge::setSourcePort(GpxConnectionPoint * port)
{
	source = port;
	updatePosition();
}

GpxConnectionPoint *GpxEdge::destPort() const
{
	return dest;
}

void GpxEdge::setDestPort(GpxConnectionPoint * port)
{
	dest = port;
	updatePosition();
}

/*! \brief Update of the position of the edge. 
 *
 *  In first place, there is a mapping from the position of the new position of the ports in the scene
 *  to the edge coordinate system. If the source port position has changed, actualization of the first tree points
 *  of the polygon is in order. If it is the destiny point, then the tree last points should be actualized.
 */
void GpxEdge::updatePosition()
{
	if (!source || !dest)
		return;
	// Necessary to avoid redrawing problems of the scene.
	prepareGeometryChange();

	sourcePoint = mapFromItem(source, source->getConnectionPoint());
	destPoint = mapFromItem(dest, dest->getConnectionPoint());

	if (sourcePoint != myPolygon.at(0) && myPolygon.size() > 1) {
		GpxConnectionPoint::Direction direction =
		    findDirection(myPolygon.at(1), myPolygon.at(0));
		myPolygon.replace(0, sourcePoint);
		QPointF middlePoint;
		switch (direction) {
		case GpxConnectionPoint::Right:
		case GpxConnectionPoint::Left:
			middlePoint.setX(myPolygon.at(2).x());
			middlePoint.setY(sourcePoint.y());
			break;
		case GpxConnectionPoint::Up:
		case GpxConnectionPoint::Down:
			middlePoint.setX(sourcePoint.x());
			middlePoint.setY(myPolygon.at(2).y());
		default:
			break;
		}
		myPolygon.replace(1, middlePoint);
	}
	if (destPoint != myPolygon.last() && myPolygon.size() > 1) {
		GpxConnectionPoint::Direction direction =
		    findDirection(myPolygon.at(myPolygon.size() - 3),
				  myPolygon.at(myPolygon.size() - 2));
		QPointF middlePoint;
		myPolygon.replace(myPolygon.size() - 1, destPoint);
		switch (direction) {
		case GpxConnectionPoint::Right:
		case GpxConnectionPoint::Left:
			middlePoint.setX(destPoint.x());
			middlePoint.setY(myPolygon.at(myPolygon.size() - 3).
					 y());
			break;
		case GpxConnectionPoint::Up:
		case GpxConnectionPoint::Down:
			middlePoint.setX(myPolygon.at(myPolygon.size() - 3).
					 x());
			middlePoint.setY(destPoint.y());
		default:
			break;
		}
		myPolygon.replace(myPolygon.size() - 2, middlePoint);
	}
/*	if (source->portType() == Port::InPort) {
		sourcePoint.setX(sourcePoint.x() - 10);
	}
	if (dest->portType() == Port::InPort) {
		destPoint.setX(destPoint.x() - 10);
	}

	//qDebug() << "Source x: " << (source->mapToParent(source->scenePos())).x() << " Dest y: " << (source->mapToParent(source->scenePos())).y() << "\n";
	//qDebug() << "Dest x: " << dest->pos().x() << " Dest y: " << dest->pos().y() << "\n";*/
}

QRectF GpxEdge::boundingRect() const
{
	if (!source || !dest)
		return QRectF();

//      qreal penWidth =  1;
	qreal extra = 50;
	QRectF rectangle = myPolygon.boundingRect();
	return rectangle.adjusted(-extra, -extra, extra, extra);
}

void GpxEdge::paint(QPainter * painter, const QStyleOptionGraphicsItem *,
		    QWidget *)
{
	if (!source || !dest)
		return;

	//Draw the line itself
	painter->
	    setPen(QPen
		   (Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	if (movingSegment == -1)
		simplifyEdge();

	painter->drawPolyline(myPolygon);

	//qDebug() << "Source x: " << sourcePoint.x() << " Dest y: " << sourcePoint.y() << "\n";
	//qDebug() << "Dest x: " << destPoint.x() << " Dest y: " << destPoint.y() << "\n";

	painter->
	    setPen(QPen
		   (Qt::red, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

	// If the edge is selected, it makes its points visible.
	if (isSelected()) {
		for (int i = 0; i < myPolygon.size() - 1; i++) {
			QPointF point = myPolygon.at(i);
			painter->drawPoint(point);
		}
	}
}

void GpxEdge::addPoint(QPointF point)
{
	myPolygon << point;
}

void GpxEdge::preAddPoint(QPointF point)
{
	myPolygon.prepend(point);
}

QPainterPath GpxEdge::shape() const
{
	QPainterPath path;
	for (int i = 0; i < myPolygon.size() - 1; i++) {
		QPointF init = myPolygon.at(i), end = myPolygon.at(i + 1);
		if (init.x() == end.x())
			path.
			    addRect(QRectF
				    (init.x() - 3, init.y(), 6,
				     end.y() - init.y()).normalized());
		else
			path.
			    addRect(QRectF
				    (init.x(), init.y() - 3, end.x() - init.x(),
				     6).normalized());
	}
	return path;
}

/*! If a point is redundant, it must be eliminated from the polygon.
 */
void GpxEdge::simplifyEdge()
{
	if (myPolygon.size() > 3) {
		for (int i = 0; i < myPolygon.size() - 2; i++) {
			if (myPolygon.at(i).x() == myPolygon.at(i + 2).x() ||
			    myPolygon.at(i).y() == myPolygon.at(i + 2).y())
				myPolygon.remove(i + 1);
		}
	}
}

QVariant GpxEdge::itemChange(GraphicsItemChange change, const QVariant & value)
{
	if (change == QGraphicsItem::ItemPositionHasChanged)
		//      updatePosition();

		QGraphicsItem::itemChange(change, value);
	return value;
}

void GpxEdge::endMovingEdge()
{
	movingSegment = -1;
}

void GpxEdge::startMovingEdgeAt(QPointF point)
{
	QPointF newPoint = mapFromScene(point);
	for (int i = 0; i < myPolygon.size() - 1; i++) {
		QPointF init = myPolygon.at(i), end = myPolygon.at(i + 1);
		QRectF rectangle;
		if (init.x() == end.x())
			rectangle =
			    QRectF(init.x() - 3, init.y(), 6,
				   end.y() - init.y())
			    .normalized();
		else
			rectangle =
			    QRectF(init.x(), init.y() - 3, end.x() - init.x(),
				   6)
			    .normalized();
		if (rectangle.contains(newPoint)) {
			movingSegment = i;
			break;
		}
	}
}

void GpxEdge::moveEdgeAt(QPointF point)
{
	QPointF newPoint = mapFromScene(point);

	if (movingSegment < 0)
		return;

	QPointF firstPoint = myPolygon.at(movingSegment),
	    secondPoint = myPolygon.at(movingSegment + 1);
	if (firstPoint.x() == secondPoint.x()) {	// Vertical segment.
		firstPoint.setX(newPoint.x());
		secondPoint.setX(newPoint.x());
	} else {		// Horizontal segment.
		firstPoint.setY(newPoint.y());
		secondPoint.setY(newPoint.y());
	}
	myPolygon.replace(movingSegment, firstPoint);
	myPolygon.replace(movingSegment + 1, secondPoint);
	updatePosition();
}

GpxConnectionPoint::Direction GpxEdge::segmentDirectionAt(QPointF point)
{
	QPointF newPoint = mapFromScene(point);
	for (int i = 0; i < myPolygon.size() - 1; i++) {
		QPointF init = myPolygon.at(i), end = myPolygon.at(i + 1);
		QRectF rectangle;
		if (init.x() == end.x())
			rectangle =
			    QRect(init.x() - 3, init.y(), 6, end.y() - init.y())
			    .normalized();
		else
			rectangle =
			    QRectF(init.x(), init.y() - 3, end.x() - init.x(),
				   6)
			    .normalized();
		if (rectangle.contains(newPoint))
			return (init.x() ==
				end.
				x()? GpxConnectionPoint::
				Down : GpxConnectionPoint::Left);
	}
	return GpxConnectionPoint::Left;
}

void GpxEdge::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	setSelected(!isSelected());
	ungrabMouse();
}

QPointF GpxEdge::breakAt(QPointF point, QPolygonF & initPolygon,
			 QPolygonF & endPolygon)
{
	QPointF newPoint = mapFromScene(point), ret;
	bool found = false;
	bool vertical = false;

	for (int i = 0; i < myPolygon.size() - 1; i++) {
		QPointF init = myPolygon.at(i), end = myPolygon.at(i + 1);
		QRectF rectangle;
		if (init.x() == end.x()) {
			rectangle =
			    QRectF(init.x() - 3, init.y(), 6,
				   end.y() - init.y())
			    .normalized();
			vertical = true;
		} else {
			rectangle =
			    QRectF(init.x(), init.y() - 3, end.x() - init.x(),
				   6)
			    .normalized();
			vertical = false;
		}
		if (rectangle.contains(newPoint)) {
			found = true;
			initPolygon << myPolygon.at(i);
			if (vertical) {
				ret.setX(init.x());
				ret.setY(newPoint.y());
			} else {
				ret.setY(init.y());
				ret.setX(newPoint.x());
			}
		} else if (!found)
			initPolygon << myPolygon.at(i);
		else
			endPolygon << myPolygon.at(i);
	}
	endPolygon << myPolygon.at(myPolygon.size() - 1);

	return mapToScene(ret);
}

void GpxEdge::setMiddlePoint()
{
	QPointF middlePoint;
	switch (source->direction()) {
	case GpxConnectionPoint::Right:
	case GpxConnectionPoint::Left:
		middlePoint.setX(destPoint.x());
		middlePoint.setY(sourcePoint.y());
		break;
	case GpxConnectionPoint::Up:
	case GpxConnectionPoint::Down:
		middlePoint.setX(sourcePoint.x());
		middlePoint.setY(destPoint.y());
	default:
		break;
	}
	myPolygon.replace(1, middlePoint);
	myPolygon << destPoint;
}

void GpxEdge::removeConnections()
{
	source->removeEdge(this);
	if (source->connectionPointType() == GpxConnectionPoint::NoConnection
	    && source->numberOfEdges() == 0) {
		scene()->removeItem(source);
		qDebug("Borrando una crush");
		delete source;
	}
	dest->removeEdge(this);
	if (dest->connectionPointType() == GpxConnectionPoint::NoConnection
	    && dest->numberOfEdges() == 0) {
		scene()->removeItem(dest);
		qDebug("Borrando una crush");
		delete dest;
	}
}
