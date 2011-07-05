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

#include "gpxconnectionpoint.h"
#include "gpxedge.h"

GpxConnectionPoint::GpxConnectionPoint(ConnectionPointType portType,
				       Direction direction,
				       QGraphicsItem * parent,
				       QGraphicsScene * scene)
 : QGraphicsItem(parent, scene)
{
	myPortType = portType;
	myDirection = direction;

	switch (myPortType) {
	case InPort:
	case OutPort:
		myPolygon << QPointF(-4, 4) << QPointF(0, 0) << QPointF(-4, -4)
		    << QPointF(-4, 0) << QPointF(-6, 0) << QPoint(-4, 0);
		break;
	case NoConnection:
		myPolygon << QPoint(-6, 6) << QPoint(6, 6) << QPoint(6,
								     -6) <<
		    QPoint(-6, -6)
		    << QPoint(-6, 6);
		break;
	case Node:
		myPolygon << QPoint(-3, 3) << QPoint(3, 3) << QPoint(3,
								     -3) <<
		    QPoint(-3, -3)
		    << QPoint(-3, 3);
		break;
	default:
		break;
	}

	if (myPortType == Node || myPortType == NoConnection) {
		setFlag(QGraphicsItem::ItemIsMovable, true);
		setFlag(QGraphicsItem::ItemIsSelectable, true);
	}
//      setPolygon(myPolygon);  
	else
		setFlag(QGraphicsItem::ItemIsSelectable, false);
}

void GpxConnectionPoint::removeEdge(GpxEdge * edge)
{
	int index = edges.indexOf(edge);

	if (index != -1)
		edges.removeAt(index);

	if (myPortType == Node && edges.size() == 2) {
		GpxEdge *edge0, *edge1;
		edge0 = edges.at(0);
		edge1 = edges.at(1);
		if (edge0->sourcePort() == this) {	// Edge0  leaves point.
			if (edge1->sourcePort() == this) {	// Edge1 leaves point.
				for (int i = edge1->polygon().size() - 1;
				     i >= 0; i--) {
					edge0->preAddPoint(edge1->polygon().
							   at(i));
				}
				edge0->setSourcePort(edge1->destPort());
				edge1->destPort()->addEdge(edge0);
				edge1->removeConnections();
				scene()->removeItem(edge1);
				scene()->removeItem(this);

			} else {	// Edge1 reaches point.
				for (int i = edge1->polygon().size() - 1;
				     i >= 0; i--) {
					edge0->preAddPoint(edge1->polygon().
							   at(i));
				}
				edge0->setSourcePort(edge1->sourcePort());
				edge1->sourcePort()->addEdge(edge0);
				edge1->removeConnections();
				scene()->removeItem(edge1);
				scene()->removeItem(this);
			}

		} else {	// Edge0 reaches point. 
			if (edge1->sourcePort() == this) {	// Edge1 leaves point.
				for (int i = 0; i < edge1->polygon().size();
				     i++) {
					edge0->addPoint(edge1->polygon().at(i));
				}
				edge0->setDestPort(edge1->destPort());
				edge1->destPort()->addEdge(edge0);
				edge1->removeConnections();
				scene()->removeItem(edge1);
				scene()->removeItem(this);

			} else {	// Edge1 reaches point.
				for (int i = edge1->polygon().size() - 1;
				     i >= 0; i--) {
					edge0->addPoint(edge1->polygon().at(i));
				}
				edge0->setDestPort(edge1->sourcePort());
				edge1->sourcePort()->addEdge(edge0);
				edge1->removeConnections();
				scene()->removeItem(edge1);
				scene()->removeItem(this);
			}
		}
		qDebug("Simplificar");
	}
}

void GpxConnectionPoint::removeEdges()
{
	foreach(GpxEdge * edge, edges) {
		if (edge->sourcePort() == this) {
			edge->destPort()->removeEdge(edge);
		}

		if (edge->destPort() == this) {
			edge->sourcePort()->removeEdge(edge);
		}
		//edge->destPort()->removeEdge(edge);
		// ¿Qué está sucediendo con esta instrucción? ¿Es un puntero a null?
		//qDebug("%d %d",scene(),edge);
		scene()->removeItem(edge);
		//delete edge;
	}
}

void GpxConnectionPoint::addEdge(GpxEdge * edge)
{
	edges.append(edge);
}

QVariant GpxConnectionPoint::itemChange(GraphicsItemChange change,
					const QVariant & value)
{
	if (change == QGraphicsItem::ItemPositionHasChanged) {
		foreach(GpxEdge * edge, edges) {
			edge->updatePosition();
		}
	}

	return value;
}

QRectF GpxConnectionPoint::boundingRect() const
{
//      if(myPortType != Port::No_Conn)
	return myPolygon.boundingRect();
//      else return QRectF(QPointF(-10,10), QSizeF(30,30))
//                              .normalized();
}

void GpxConnectionPoint::paint(QPainter * painter,
			       const QStyleOptionGraphicsItem *, QWidget *)
{
	painter->
	    setPen(QPen
		   (Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	painter->setBrush(Qt::black);
	if (edges.empty()) {
		painter->drawLine(myPolygon.at(0), myPolygon.at(1));
		painter->drawLine(myPolygon.at(1), myPolygon.at(2));
	} else
		switch (myPortType) {
		case OutPort:
			painter->drawLine(QPointF(myPolygon.at(0).x(), 0),
					  myPolygon.at(1));
			break;
		case InPort:
			painter->drawPolygon(myPolygon);
			break;
		case NoConnection:
			//      painter->drawPolyline(myPolygon);
			painter->
			    setPen(QPen
				   (Qt::black, 2.6, Qt::SolidLine, Qt::RoundCap,
				    Qt::RoundJoin));
			painter->drawLine(QPointF(0, 6), QPointF(0, -6));
			painter->drawLine(QPointF(6, 0), QPointF(-6, 0));
			break;
		case Node:
			painter->drawEllipse(myPolygon.boundingRect());
			break;
		default:
			break;
		}
}

QPainterPath GpxConnectionPoint::shape() const
{
	QPainterPath path;
	path.addPolygon(myPolygon);
	return path;
}

void GpxConnectionPoint::updatePosition()
{
	prepareGeometryChange();
	qDebug("Updating position");
	foreach(GpxEdge * edge, edges) {
		edge->updatePosition();
	}
}

QPointF GpxConnectionPoint::getConnectionPoint()
{
	QPointF point;

	switch (myPortType) {

	case InPort:
		point = myPolygon.at(4);
		break;
	case OutPort:
		point = myPolygon.at(1);
		break;
	case NoConnection:
		//point = myPolygon.at(1);
	case Node:
		point = QPointF(0, 0);
		break;
	default:
		break;
	}
	return point;
}

void GpxConnectionPoint::rotate()
{
	int temp = myDirection + 1;
	temp %= 4;
	myDirection = (Direction) temp;
//      qDebug("Mi nueva pos es %d\n",(int)myDirection);
}

GpxEdge *GpxConnectionPoint::getEdgeAt(int index)
{
	if (!edges.isEmpty())
		return edges.at(index);
	else
		return NULL;
}

bool GpxConnectionPoint::isConnected()
{
	return edges.size() > 0;
}

int GpxConnectionPoint::numberOfEdges()
{
	return edges.size();
}

GpxConnectionPoint::Direction findDirection(const QPointF middlePoint,
					    const QPointF endPoint)
{
	if (middlePoint.x() > endPoint.x() && middlePoint.y() == endPoint.y())	//{
		//      qDebug("middlePoint.x(): %g, middlePoint.y(): %g, endPoint.x(): %g, endPoint.y(): %g.\n", middlePoint.x(), middlePoint.y(), endPoint.x(), endPoint.y()); 
		//      qDebug("Left.\n"); 
		return GpxConnectionPoint::Left;
	//      }

	else if (middlePoint.x() <= endPoint.x() && middlePoint.y() == endPoint.y())	//{ Decisión de diseño. ¿Qué hacer cuando ambos tienen la misma abscisa? De lo contrario no se grafica.
		//              qDebug("middlePoint.x(): %g, middlePoint.y(): %g, endPoint.x(): %g, endPoint.y(): %g.\n", middlePoint.x(), middlePoint.y(), endPoint.x(), endPoint.y()); 
		//              qDebug("Right.\n");
		return GpxConnectionPoint::Right;
	//              }

	else if (middlePoint.y() < endPoint.y())	//{ Leer sistema de coordenadas en la  escena.
		//              qDebug("middlePoint.x(): %g, middlePoint.y(): %g, endPoint.x(): %g, endPoint.y(): %g.\n", middlePoint.x(), middlePoint.y(), endPoint.x(), endPoint.y()); 
		//              qDebug("Down.\n");
		return GpxConnectionPoint::Down;
	//              }       

	else			// { 
		//      qDebug("middlePoint.x(): %g, middlePoint.y(): %g, endPoint.x(): %g, endPoint.y(): %g.\n", middlePoint.x(), middlePoint.y(), endPoint.x(), endPoint.y()); 
		//      qDebug("Up.\n");
		return GpxConnectionPoint::Up;
}
