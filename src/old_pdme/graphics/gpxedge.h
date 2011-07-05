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


/*! \brief GpxEdge class 
 *  
 *  The edge class implements a connection between the powerdevs elements.
*/
#ifndef GPXEDGE_H
#define GPXEDGE_H

#include <QGraphicsItem>

#include "gpxconnectionpoint.h"
#include "modelcoupled.h"
class QPolygonF;
class GpxConnectionPoint;

class GpxEdge:public QGraphicsItem {
 public:
	/*! The constructor of the class. It needs a source Port and a destiny Port.
	 */
	GpxEdge(GpxConnectionPoint * sourcePort, GpxConnectionPoint * destPort, modelLine *ml=0);

	/*! Method that returns the source port of the edge.
	 */
	GpxConnectionPoint *sourcePort() const;

	/*! Method to set the source port.
	 */
	void setSourcePort(GpxConnectionPoint * port);

	/*! Method that returns the destiny port of the edge.
	 */
	GpxConnectionPoint *destPort() const;

	/*! Method to set the destiny port of the edge.
	 */
	void setDestPort(GpxConnectionPoint * port);

	/*! Method to update the position of the edge. 
	 */
	void updatePosition();

	/*! Method to add a point to the polygon of the edge.
	 *      \sa myPolygon
	 */
	void addPoint(QPointF point);

	/*! Method to preppend a point in the polygon of the edge.
	 *      \sa myPolygon
	 */
	void preAddPoint(QPointF point);

	enum { Type = UserType + 2 };	/*!< Enum value. */

	/*! Returns the type of the edge.
	 *      For the identification of the object in modelchild\'s methods.
	 */
	int type() const {
		return Type;
	}
	/*! Method that returns the polygon of the edge.
	 */ QPolygonF polygon() const {
		return myPolygon;
	}
	/*! Method for set the polygon of the edge.
	 */ void setPolygon(QPolygonF polygon) {
		myPolygon = polygon;
		updatePosition();
	}
	/*! Method for the movement of the edge. It indicates the point where begins the movement.
	 *      \sa moveEdgeAt(QpointF point), endMovingEdge() .
	 */ void startMovingEdgeAt(QPointF point);

	/*! Method to  move the edge.
	 *      \sa startMovingEdgeAt(QPointF point), endMovingEdge().
	 */
	void moveEdgeAt(QPointF point);

	/*! Method that indicates the finish of the movement of the edge.
	 *      \sa startMovingEdgeAt(QPointF point), moveEdgeAt(QPointF point) .
	 */
	void endMovingEdge();

	/*! Method to break the polygon of the edge into two polygons.
	 */
	QPointF breakAt(QPointF point, QPolygonF & initPolygon,
			QPolygonF & endPolygon);

	/*! Returns the direction of the segment of the polygon that contains the point.
	 */
	GpxConnectionPoint::Direction segmentDirectionAt(QPointF point);

	/*! In the case of a polygon with two points, it's necessary to add a middle point.
	 */
	void setMiddlePoint();

	/*! Remove the ports of the edge.
	 */
	void removeConnections();

 protected:
	QRectF boundingRect()const;
	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option,
		   QWidget * widget);
	QPainterPath shape() const;
	QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	void mousePressEvent(QGraphicsSceneMouseEvent * event);

 private:
	/*! Private member, that eliminates unnecessary points of the polygon.
	 */
	modelLine * lineData;
	void simplifyEdge();
	GpxConnectionPoint *source, *dest;	/*! < Variables for the source and destiny ports. */
	QPolygonF myPolygon;	/*! < The polygon of the edge. */
	QPointF sourcePoint;
	QPointF destPoint;
	int movingSegment;	/*! < Variable to check whether the edge is moving or not. */
	modelLine *ml;
};

#endif
