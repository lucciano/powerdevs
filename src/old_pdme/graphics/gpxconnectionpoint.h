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


/*! \brief Class Port.
 *
 * The class represents a PowerDevs graphic port.
*/
#ifndef GPXCONNECTIONPOINT_H
#define GPXCONNECTIONPOINT_H

#include <QGraphicsPixmapItem>
#include <QList>

class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QPolygonF;

class GpxBlock;
class GpxEdge;
#define GPXCONNECTIONPOINT_TYPE 20
class GpxConnectionPoint:public QGraphicsItem {
 public:
	/*!< Enum value. */
	enum { Type = UserType + GPXCONNECTIONPOINT_TYPE };
	/*!  This enumeration defines the type of port. */
	enum ConnectionPointType { InPort,	/*!< Enum value for the sink port. */
		OutPort,	/*!< Enum value for the source port. */
		NoConnection,	/*!< Enum value for the cross. */
		Node /*!< Enum value for the node. */
	};
	/*! This enumeration defines the direction of the port. */
	enum Direction { Right, Down, Left, Up };

	 GpxConnectionPoint(ConnectionPointType portType, Direction direction,
			    QGraphicsItem * parent = 0, QGraphicsScene * scene =
			    0);
	void rotate();
	void removeEdge(GpxEdge * edge);
	void removeEdges();
	ConnectionPointType connectionPointType() const {
		return myPortType;
	} QPolygonF polygon() const {
		return myPolygon;
	} void addEdge(GpxEdge * edge);
	GpxEdge *getEdgeAt(int index);
	QPixmap image() const;
	int type() const {
		return Type;
	} Direction direction() const {
		return myDirection;
	} QPainterPath shape() const;
	void updatePosition();
	bool isConnected();
	int numberOfEdges();

	QPointF getConnectionPoint();
	QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	void paint(QPainter * painter, const QStyleOptionGraphicsItem *,
		   QWidget *);
	QRectF boundingRect() const;

 private:
	 ConnectionPointType myPortType;
	QPolygonF myPolygon;
	Direction myDirection;
	 QList < GpxEdge * >edges;
};

GpxConnectionPoint::Direction findDirection(const QPointF middlePoint,
					    const QPointF endPoint);
#endif
