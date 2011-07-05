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
#include "gpxeditionscene.h"
#include "gpxedge.h"
#include "powergui.h"
#include "gpxatomic.h"
#include "gpxcoupled.h"
#include "gpxinport.h"
#include "gpxoutport.h"
#include "modelcoupled.h"
#include "powerdialogs.h"

GpxEditionScene::GpxEditionScene(QObject * parent, modelCoupled * data)
 : QGraphicsScene(parent)
{
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QString pathLib = settings.value("Path/libPath").toString();

	deleted = false;
	rubberBand = NULL;
	myMode = MoveItem;
	connection = NULL;
	textItem = 0;
	myItemColor = Qt::white;
	myTextColor = Qt::black;
	myLineColor = Qt::black;
	owner = NULL;
	PowerGui *powergui = (PowerGui *) mainWindow;
	if (data == NULL) {	// only use in the edition dialog to preview
		coupledData = new modelCoupled();
		coupledData->owner = NULL;
		coupledData->father = NULL;
		coupledData->type = TOKROOT;
		//coupledData->inPorts = 0;
		//coupledData->outPorts = 0;
		coupledData->desc = "";
		modelGraphics *mg = new modelGraphics();
		coupledData->graphics = mg;
		mg->x = mg->y = 0;
		mg->width = mg->height = 500;
		mg->direction = 0;
		mg->icon = "";
		mg->color = 15;

	} else {
		QList < GpxConnectionPoint * >points;
		QList < GpxBlock * >childs;
		QList < GpxBlock * >lsPorts;
		int priority = 0;
		coupledData = data;
		foreach(modelChild * c, coupledData->childs) {
			if (c->childType == ATOMIC) {
				QString icon = c->atomic->graphics->icon;
				if (icon == "None")
					icon = "";
				if (icon.startsWith("\%")) {
					icon =
					    pathLib + "/" +
					    icon.mid(1).replace("\%",
								"/").toLower();
				}
				c->atomic->graphics->icon = icon;
				c->atomic->priority = priority++;
				GpxAtomic *a = new GpxAtomic(NULL, this, c->atomic);
				GpxBlock *item = a;
				connect(item,
					SIGNAL(editParameters(GpxBlock *)),
					pDialogs,
					SLOT(blockParamDialog(GpxBlock *)));
				connect(item, SIGNAL(dragCopy(QPointF)),
					powergui, SLOT(dragCopy(QPointF)));
				connect(item,
					SIGNAL(contextMenu(GpxBlock *, QPoint)),
					powergui,
					SLOT(contextMenu(GpxBlock *, QPoint)));
				connect(item, SIGNAL(dirtyChanged()), powergui,
					SLOT(dirtyChanged()));

				childs << a;
			} else {
				QString icon = c->coupled->graphics->icon;
				if (icon == "None")
					icon = "";
				if (icon.startsWith("\%")) {
					icon =
					    pathLib + "/" +
					    icon.mid(1).replace("\%",
								"/").toLower();
				}
				c->coupled->graphics->icon = icon;
				c->coupled->priority = priority++;
				GpxCoupled *gpxc =
				    new GpxCoupled(NULL, this, c->coupled);
				GpxBlock *item = gpxc;
				connect(item,
					SIGNAL(editParameters(GpxBlock *)),
					pDialogs,
					SLOT(blockParamDialog(GpxBlock *)));
				connect(item,
					SIGNAL(contextMenu(GpxBlock *, QPoint)),
					powergui,
					SLOT(contextMenu(GpxBlock *, QPoint)));
				connect(item, SIGNAL(dirtyChanged()), powergui,
					SLOT(dirtyChanged()));
				connect(item, SIGNAL(dragCopy(QPointF)),
					powergui, SLOT(dragCopy(QPointF)));
				childs << gpxc;
				//for (int i=0;i< (c->coupled->graphics->direction);i++) a->rotate(-90);

			}
		}
		foreach(modelPoint * p, coupledData->points) {
			GpxConnectionPoint *port =
			    new GpxConnectionPoint(GpxConnectionPoint::Node,
						   GpxConnectionPoint::Left,
						   NULL, this);
			points << port;
			port->setPos(p->x * TWIPSTOPIXEL, p->y * TWIPSTOPIXEL);
		}
		foreach(modelPort * p, coupledData->lsInPorts) {
				GpxInport *ip = new GpxInport(NULL, this, p);
				connect(ip, SIGNAL(dirtyChanged()), powergui,
					SLOT(dirtyChanged()));
				connect(ip, SIGNAL(dragCopy()), powergui,
					SLOT(dragCopy()));
				ip->setPos(p->x * TWIPSTOPIXEL,
					   p->y * TWIPSTOPIXEL);
				lsPorts << ip;
		}
		foreach(modelPort * p, coupledData->lsOutPorts) {
				GpxOutport *op = new GpxOutport(NULL, this, p);
				connect(op, SIGNAL(dirtyChanged()), powergui,
					SLOT(dirtyChanged()));
				connect(op, SIGNAL(dragCopy()), powergui,
					SLOT(dragCopy()));
				op->setPos(p->x * TWIPSTOPIXEL,
					   p->y * TWIPSTOPIXEL);
				lsPorts << op;
		}
		foreach(modelLine * l, coupledData->lines) {
			GpxConnectionPoint *s = NULL, *d = NULL;

			if (l->sourceType == TOKCMP) {	// Source is a model
				if (l->sources.at(2) == 0) {	// OutPort
					s = childs.at(l->sources.at(0) -
						      1)->getOutportAt(l->
								       sources.
								       at(1) -
								       1);
				} else {	// InPort
					s = childs.at(l->sources.at(0) -
						      1)->getInportAt(l->
								      sources.
								      at(1) -
								      1);
				}

			}
			if (l->sourceType == TOKNOC) {	// Source is a cross
				s = new GpxConnectionPoint(GpxConnectionPoint::
							   NoConnection,
							   GpxConnectionPoint::
							   Left, NULL, this);
				s->setPos(l->pointX.first() * TWIPSTOPIXEL,
					  l->pointY.first() * TWIPSTOPIXEL);
			}
			if (l->sinkType == TOKNOC) {	// Sink is a cross
				d = new GpxConnectionPoint(GpxConnectionPoint::
							   NoConnection,
							   GpxConnectionPoint::
							   Left, NULL, this);
				d->setPos(l->pointX.last() * TWIPSTOPIXEL,
					  l->pointY.last() * TWIPSTOPIXEL);
			}

			if (l->sourceType == TOKPNT) {	// Source is a point
				s = points.at(l->sources.at(0) - 1);
			}
			if (l->sourceType == TOKPRT) {	// Source is a port
				GpxBlock *port =
				    lsPorts.at(l->sources.at(0) - 1);
				if (port->blockType() == GpxBlock::InPort)
					s = port->getOutportAt(0);
				else
					s = port->getInportAt(0);
			}

			if (l->sinkType == TOKCMP) {	// Sink is a model
				if (l->sinks.at(2) == 0) {	// OutPort
					d = childs.at(l->sinks.at(0) -
						      1)->getOutportAt(l->sinks.
								       at(1) -
								       1);
				} else {	// InPort
					d = childs.at(l->sinks.at(0) -
						      1)->getInportAt(l->sinks.
								      at(1) -
								      1);
				}
			}
			if (l->sinkType == TOKPNT) {	// Source is a point
				d = points.at(l->sinks.at(0) - 1);
			}
			if (l->sinkType == TOKPRT) {	// Sink is a port
				GpxBlock *port = lsPorts.at(l->sinks.at(0) - 1);
				if (port->blockType() == GpxBlock::InPort)
					d = port->getOutportAt(0);
				else
					d = port->getInportAt(0);
			}
			//qDebug("%s %s %x %x\n",QSTR(l->sourceType),QSTR(l->sinkType),s,d);
			if (s != NULL && d != NULL) {
				GpxEdge *e = new GpxEdge(s, d);
				QPolygonF p;
				int i = 0;
				foreach(int x, l->pointX) {
					p << QPointF(x * TWIPSTOPIXEL,
						     l->pointY.at(i) *
						     TWIPSTOPIXEL);
					i++;
				}
				if (l->pointX.size() > 2)
					e->setPolygon(p);
				s->addEdge(e);
				d->addEdge(e);
				addItem(e);
			} else {
			}
		}
	}
	connect(this, SIGNAL(selectionChanged()), powergui,
		SLOT(updateMenus()));
	connect(this, SIGNAL(contextMenu(QPoint)), powergui,
		SLOT(blankContextMenu(QPoint)));
	connect(this, SIGNAL(selectionChanged()), powergui,
		SLOT(updateMenus()));
	connect(this, SIGNAL(itemInserted(GpxBlock *)), powergui,
		SLOT(itemInserted(GpxBlock *)));
	connect(this, SIGNAL(itemSelected(QGraphicsItem *)), powergui,
		SLOT(itemSelected(QGraphicsItem *)));
	connect(this, SIGNAL(edgeInserted(GpxEdge *)), powergui,
		SLOT(edgeInserted(GpxEdge *)));
 	uStack = new QUndoStack(); 
	qDebug("Creating a new undo stack at %p",uStack);
	/*! \todo: Release the undo stack on the destructor */
}

void GpxEditionScene::setOwner(GpxCoupled * c)
{
	owner = c;
	if(c != NULL)
		coupledData = c->getData();
}

bool GpxEditionScene::existsName(QString n, GpxBlock * a)
{
	foreach(QGraphicsItem * item, items()) {
		if (item->type() == GpxBlock::Type) {
			GpxBlock *atomic =
			    qgraphicsitem_cast < GpxBlock * >(item);
			if (n == atomic->getName() && a != atomic) {
				return true;
			}
		}

	}
	return false;
}

void GpxEditionScene::setItemColor(const QColor & color)
{
	myItemColor = color;
	if (isItemChange(GpxBlock::Type)) {
		GpxBlock *item =
		    qgraphicsitem_cast < GpxBlock * >(selectedItems().first());
		item->setBackgroundColor(myItemColor);
	}
}

void GpxEditionScene::setMode(Mode mode)
{
	myMode = mode;
}

void GpxEditionScene::editorLostFocus(GpxTextBox * item)
{
	QTextCursor cursor = item->textCursor();
	cursor.clearSelection();
	item->setTextCursor(cursor);

	if (item->toPlainText().isEmpty()) {
		removeItem(item);
		item->deleteLater();
	}
}

void GpxEditionScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	// For test purpouses only. Not the best place for this variable.
	if (mouseEvent->button() == Qt::LeftButton) {
		if (!(items(mouseEvent->scenePos()).isEmpty()) && items(mouseEvent->scenePos()).first()->type() == GpxConnectionPoint::Type) {	// Creating a new edge
			myMode = InsertLine;
		}
		if (!(items(mouseEvent->scenePos()).isEmpty()) && items(mouseEvent->scenePos()).first()->type() == GpxBlock::Type) {	// Re-sizeing an item
			GpxBlock *a =
			    qgraphicsitem_cast <
			    GpxBlock * >(items(mouseEvent->scenePos()).first());
			QPointF s = a->getSize();
			int corner = a->isAtCorner(mouseEvent->scenePos());
			if (corner > 0) {
				myMode = ResizeItem;
				a->startResizing(corner);
				//qDebug("Resizing");
				emit dirtyChanged(true);
				atomic = a;
			}
		}
		if (!(items(mouseEvent->scenePos()).isEmpty()) && items(mouseEvent->scenePos()).first()->type() == GpxEdge::Type) {	// Moving a line
			myMode = MoveLine;
			emit dirtyChanged(true);
		}
		switch (myMode) {
		case InsertLine:	// Start drawing a line
			emit dirtyChanged(true);
			if (!(items(mouseEvent->scenePos()).isEmpty()) &&
			    items(mouseEvent->scenePos()).first()->type() ==
			    GpxConnectionPoint::Type) {
				GpxConnectionPoint *p =
				    (GpxConnectionPoint *) items(mouseEvent->
								 scenePos()).
				    first();
				connection =
				    new GpxConnection(p->
						      mapToScene(p->
								 getConnectionPoint
								 ()),
						      mouseEvent->scenePos(),
						      p);
				connection->setPen(QPen(myLineColor, 2));
				addItem(connection);
			}
			break;
			/*
			   case InsertText: // Add a comment
			   emit dirtyChanged(true);     
			   textItem = new GpxTextBox();
			   textItem->setFont(myFont);
			   textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
			   textItem->setZValue(1000.0);
			   connect(textItem, SIGNAL(lostFocus(GpxTextBox *)),
			   this, SLOT(editorLostFocus(GpxTextBox *)));
			   connect(textItem, SIGNAL(selectedChange(QGraphicsItem *)),
			   this, SIGNAL(itemSelected(QGraphicsItem *)));
			   addItem(textItem);
			   textItem->setDefaultTextColor(myTextColor);
			   textItem->setPos(mouseEvent->scenePos());
			   emit textInserted(textItem);
			 */
		case MoveLine:	// Moving a line
			emit dirtyChanged(true);
			if (!(items(mouseEvent->scenePos()).isEmpty()) &&
			    items(mouseEvent->scenePos()).first()->type() ==
			    GpxEdge::Type) {
				//     qDebug("Línea tocada.\n");
				QPointF pos = mouseEvent->scenePos();
				movingEdge =
				    qgraphicsitem_cast <
				    GpxEdge *
				    >(items(mouseEvent->scenePos()).first());
				movingEdge->startMovingEdgeAt(pos);
			}
			break;
		default:
			qWarning("Handle switch");
		}
		if (items(mouseEvent->scenePos()).isEmpty()) {	// Selection 
			origin = mouseEvent->scenePos();
			if (!rubberBand)
				rubberBand =
				    new
				    QGraphicsPolygonItem(QPolygonF
							 (QRectF
							  (origin, QSizeF())),
							 NULL, this);
			rubberBand->
			    setPen(QPen
				   (QBrush(Qt::SolidPattern), 1, Qt::DashLine));

		}
	} else if (mouseEvent->button() == Qt::RightButton) {
		if (items(mouseEvent->scenePos()).isEmpty())
			emit contextMenu(mouseEvent->screenPos());
	}
	QGraphicsScene::mousePressEvent(mouseEvent);
}

void GpxEditionScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	switch (myMode) {
	case InsertLine:
		if (connection != NULL) {
			GpxConnection *temp;
			if (!(items(mouseEvent->scenePos()).isEmpty())
			    && items(mouseEvent->scenePos()).first()->type() ==
			    GpxConnectionPoint::Type) {
				GpxConnectionPoint *p =
				    (GpxConnectionPoint *) items(mouseEvent->
								 scenePos()).
				    first();
				temp =
				    new GpxConnection(connection->
						      getInitPoint(),
						      p->mapToScene(p->
								    getConnectionPoint
								    ()),
						      connection->
						      getInitPort());
				emit dirtyChanged(true);
				//qDebug("soltá\n");
			} else {
				temp =
				    new GpxConnection(connection->
						      getInitPoint(),
						      mouseEvent->scenePos(),
						      connection->
						      getInitPort());
			}
			removeItem(connection);
			delete connection;
			connection = temp;
			addItem(connection);
		};
		break;
	case MoveItem:
		QGraphicsScene::mouseMoveEvent(mouseEvent);
		break;

	case MoveLine:
		movingEdge->moveEdgeAt(mouseEvent->scenePos());
		break;
	case ResizeItem:
		if (atomic != NULL) {
			emit dirtyChanged(true);
			if (atomic->blockType() == GpxBlock::SimpleAtomic)
				((GpxAtomic *) atomic)->resizeTo(mouseEvent->
								 scenePos());
			if (atomic->blockType() == GpxBlock::Coupled)
				((GpxCoupled *) atomic)->resizeTo(mouseEvent->
								  scenePos());
			if (atomic->blockType() == GpxBlock::InPort)
				((GpxInport *) atomic)->resizeTo(mouseEvent->
								 scenePos());
			if (atomic->blockType() == GpxBlock::OutPort)
				((GpxOutport *) atomic)->resizeTo(mouseEvent->
								  scenePos());

		}
		break;
	default:
		qFatal("Handle switch GpxEditionScene::mouseMoveEvent");
	}

	QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	if (!(items(mouseEvent->scenePos()).isEmpty()) &&
	    items(mouseEvent->scenePos()).first()->type() ==
	    GpxConnectionPoint::Type) {
		QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
	} else if (myMode == InsertLine && !(items(mouseEvent->scenePos()).isEmpty())) {	// && 

		if (items(mouseEvent->scenePos()).size() > 1
		    && items(mouseEvent->scenePos()).at(1)->type() ==
		    GpxEdge::Type) {
			QApplication::
			    setOverrideCursor(QCursor(Qt::CrossCursor));
		}
	} else if (!(items(mouseEvent->scenePos()).isEmpty()) &&
		   items(mouseEvent->scenePos()).first()->type() ==
		   GpxEdge::Type) {
		GpxEdge *edge =
		    qgraphicsitem_cast <
		    GpxEdge * >(items(mouseEvent->scenePos()).first());
		if (edge->segmentDirectionAt(mouseEvent->scenePos()) ==
		    GpxConnectionPoint::Down) {
			QApplication::
			    setOverrideCursor(QCursor(Qt::SizeHorCursor));
		} else {
			QApplication::
			    setOverrideCursor(QCursor(Qt::SizeVerCursor));
		}
	} else if (!(items(mouseEvent->scenePos()).isEmpty()) &&
		   items(mouseEvent->scenePos()).first()->type() ==
		   GpxBlock::Type) {
		GpxBlock *a =
		    qgraphicsitem_cast <
		    GpxBlock * >(items(mouseEvent->scenePos()).first());
		int corner = a->isAtCorner(mouseEvent->scenePos());
		if (corner == 1 || corner == 3) {
			if (a->getDirection() % 2 == 0)
				QApplication::
				    setOverrideCursor(QCursor
						      (Qt::SizeFDiagCursor));
			else
				QApplication::
				    setOverrideCursor(QCursor
						      (Qt::SizeBDiagCursor));
		} else if (corner == 2 || corner == 4) {
			if (a->getDirection() % 2 == 0)
				QApplication::
				    setOverrideCursor(QCursor
						      (Qt::SizeBDiagCursor));
			else
				QApplication::
				    setOverrideCursor(QCursor
						      (Qt::SizeFDiagCursor));
		}
	}
	if (rubberBand) {
		rubberBand->
		    setPolygon(QPolygonF
			       (QRectF(origin, mouseEvent->scenePos()).
				normalized()));
		QPainterPath qp;
		qp.addPolygon(QPolygonF
			      (QRectF(origin, mouseEvent->scenePos()).
			       normalized()));
		setSelectionArea(qp);
		QList < QGraphicsItem * >l;
		foreach(QGraphicsItem * i, selectedItems()) {
			if (i->type() == GpxTextBox::Type)
				l << i;
		}
		foreach(QGraphicsItem * i, l) {
			i->setSelected(false);
		}
	}
}

void GpxEditionScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
	switch (myMode) {
	case InsertLine:
		if (connection != NULL) {
			emit dirtyChanged(true);
			QList < QGraphicsItem * >startItems =
			    items(connection->getInitPoint());
			if (startItems.count()
			    && startItems.first() == connection)
				startItems.removeFirst();
			QList < QGraphicsItem * >endItems =
			    items(mouseEvent->scenePos());
			if (endItems.count() && endItems.first() == connection)
				endItems.removeFirst();
			if (endItems.count() > 0
			    && endItems.first()->type() ==
			    GpxConnectionPoint::Type
			    && connection->getInitPort() != endItems.first()) {
				GpxConnectionPoint *startItem =
				    connection->getInitPort();
				GpxConnectionPoint *endItem =
				    qgraphicsitem_cast <
				    GpxConnectionPoint * >(endItems.first());
				GpxEdge *newEdge;
				if (startItem->connectionPointType() ==
				    GpxConnectionPoint::NoConnection
				    && (endItem->connectionPointType() ==
					GpxConnectionPoint::OutPort
					|| endItem->connectionPointType() ==
					GpxConnectionPoint::InPort)) {
					GpxEdge *edge = startItem->getEdgeAt(0);
					GpxConnectionPoint *startPort =
					    edge->sourcePort();
					if (startPort->connectionPointType() !=
					    endItem->
					    connectionPointType()
					    /* && parentEndItem->atomicType() == Atomic::SimpleAtomic */
					    ) {
						edge->addPoint(connection->
							       getMiddlePoint
							       ());
						edge->addPoint(connection->
							       getEndPoint());
						edge->setDestPort(endItem);
						endItem->addEdge(edge);
						newEdge = edge;
						removeItem(startItem);
					}
				} else
				    if ((startItem->connectionPointType() ==
					 GpxConnectionPoint::OutPort
					 || startItem->connectionPointType() ==
					 GpxConnectionPoint::InPort)
					&& endItem->connectionPointType() ==
					GpxConnectionPoint::NoConnection) {
					GpxEdge *edge = endItem->getEdgeAt(0);
					GpxConnectionPoint *endPort =
					    edge->sourcePort();
					if (endPort->connectionPointType() !=
					    startItem->
					    connectionPointType()
					    /*&& parentStartItem->atomicType() == Atomic::SimpleAtomic */
					    ) {
						edge->addPoint(connection->
							       getMiddlePoint
							       ());
						edge->addPoint(connection->
							       getEndPoint());
						edge->setDestPort(startItem);
						startItem->addEdge(edge);
						newEdge = edge;
						removeItem(endItem);
					}
				} else
				    if ((startItem->connectionPointType() ==
					 GpxConnectionPoint::NoConnection
					 && endItem->connectionPointType() ==
					 GpxConnectionPoint::NoConnection)) {
					GpxEdge *startEdge =
					    startItem->getEdgeAt(0), *endEdge =
					    endItem->getEdgeAt(0);
					GpxConnectionPoint *startPort =
					    startEdge->sourcePort(), *endPort =
					    endEdge->sourcePort();
					GpxBlock *parentStartItem =
					    qgraphicsitem_cast <
					    GpxBlock *
					    >(startPort->parentItem());
					GpxBlock *parentEndItem =
					    qgraphicsitem_cast <
					    GpxBlock * >(endPort->parentItem());
					// I should check neither parentStartItem nor parentEndItem is NULL.
					if (startPort->connectionPointType() !=
					    endPort->connectionPointType() &&
					    // For the moment only In_ports and Out_GpxConnectionPoints. 
					    (parentStartItem->blockType() ==
					     GpxBlock::SimpleAtomic
					     || parentEndItem->blockType() ==
					     GpxBlock::SimpleAtomic)) {
						startEdge->addPoint(connection->
								    getMiddlePoint
								    ());
						for (int i =
						     endEdge->polygon().size() -
						     1; i >= 0; i--) {
							startEdge->
							    addPoint(endEdge->
								     polygon().
								     at(i));
						}
						startEdge->setDestPort(endPort);
						removeItem(endEdge);
						endPort->removeEdges();
						endPort->addEdge(startEdge);
						removeItem(startItem);
						removeItem(endItem);
					}
					// I think there\'s a mistake. A port may not have a parent. I.e. cross and node.
				} else
				    if ((startItem->connectionPointType() ==
					 GpxConnectionPoint::OutPort
					 || startItem->connectionPointType() ==
					 GpxConnectionPoint::InPort)
					&& (endItem->connectionPointType() ==
					    GpxConnectionPoint::OutPort
					    || endItem->connectionPointType() ==
					    GpxConnectionPoint::InPort)
					&& (startItem->connectionPointType() !=
					    endItem->connectionPointType())) {

					GpxBlock *parentStartItem =
					    qgraphicsitem_cast <
					    GpxBlock *
					    >(startItem->parentItem());
					GpxBlock *parentEndItem =
					    qgraphicsitem_cast <
					    GpxBlock * >(endItem->parentItem());

					if (parentStartItem->blockType() ==
					    GpxBlock::SimpleAtomic
					    || parentEndItem->blockType() ==
					    GpxBlock::SimpleAtomic) {
						newEdge =
						    new GpxEdge(startItem,
								endItem);
						startItem->addEdge(newEdge);
						endItem->addEdge(newEdge);
						newEdge->setZValue(-1000.0);
						addItem(newEdge);
					}
				} else if (startItem->connectionPointType() ==
					   GpxConnectionPoint::OutPort
					   && endItem->connectionPointType() ==
					   GpxConnectionPoint::Node) {
					GpxEdge *edge =
					    new GpxEdge(startItem, endItem);
					startItem->removeEdges();
					startItem->addEdge(edge);
					endItem->addEdge(edge);
					newEdge = edge;
				}
				emit edgeInserted(newEdge);
			} else if (endItems.count() > 0
				   && endItems.first()->type() ==
				   GpxEdge::Type) {
				GpxConnectionPoint *startConnectionPort =
				    connection->getInitPort();
				GpxEdge *breakEdge =
				    qgraphicsitem_cast <
				    GpxEdge * >(endItems.first()), *sourceEdge,
				    *destEdge, *newEdge;
				if (breakEdge !=
				    startConnectionPort->getEdgeAt(0)) {

					GpxConnectionPoint *newPort =
					    new
					    GpxConnectionPoint
					    (GpxConnectionPoint::Node,
					     GpxConnectionPoint::Left);
					GpxConnectionPoint *sourcePort =
					    breakEdge->sourcePort(), *destPort =
					    breakEdge->destPort();
					QPolygonF initPolygon, endPolygon;
					QPointF dot =
					    breakEdge->breakAt(mouseEvent->
							       scenePos(),
							       initPolygon,
							       endPolygon);
					newPort->setPos(dot);
					//qDebug("Mouse at (%g,%g) point at (%g,%g)\n",mouseEvent->scenePos().x(),mouseEvent->scenePos().y(),newGpxConnectionPoint->x(),newPort->y());
					initPolygon.append(newPort->pos());
					endPolygon.prepend(newPort->pos());

					if (startConnectionPort->
					    connectionPointType() ==
					    GpxConnectionPoint::NoConnection) {
						newEdge =
						    startConnectionPort->
						    getEdgeAt(0);
						newEdge->addPoint(connection->
								  getMiddlePoint
								  ());
						newEdge->addPoint(connection->
								  getEndPoint
								  ());
						newEdge->setDestPort(newPort);
						//startConnectionGpxConnectionPoint = newEdge->sourcePort();
						//startConnectionGpxConnectionPoint->addEdge(newEdge);
						removeItem(startConnectionPort);
					} else {
						newEdge =
						    new
						    GpxEdge(startConnectionPort,
							    newPort);
						startConnectionPort->
						    addEdge(newEdge);
					}
					//qDebug("El tipo de el sourceGpxConnectionPoint es :%d.\n", sourcePort->portType());
					sourceEdge =
					    new GpxEdge(sourcePort, newPort);
					destEdge =
					    new GpxEdge(newPort, destPort);
					//newEdge = new Edge(startConnectionGpxConnectionPoint, newPort);

					sourceEdge->setPolygon(initPolygon);
					if (initPolygon.size() < 3)
						sourceEdge->setMiddlePoint();

					destEdge->setPolygon(endPolygon);
					if (endPolygon.size() < 3)
						destEdge->setMiddlePoint();

					/*      for(int i=0; i < sourceEdge->polygon().size(); i++)
					   qDebug("Elemento del primer polígono, número: %d.\n", i);
					   for(int i=0; i < destEdge->polygon().size(); i++)
					   qDebug("Elemento del segundo polígono, número: %d.\n", i); */

					//sourceGpxConnectionPoint->removeEdge(breakEdge);
					//destGpxConnectionPoint->removeEdge(breakEdge); 
					/* Still not sure, but the delete of breakEdge before adding newEdge causes an extrage behavior. So I decided tu put remove 
					 *                              edge after removeEdge */

					sourcePort->addEdge(sourceEdge);
					sourcePort->removeEdge(breakEdge);
					destPort->addEdge(destEdge);
					destPort->removeEdge(breakEdge);
					newPort->addEdge(sourceEdge);
					newPort->addEdge(destEdge);
					newPort->addEdge(newEdge);
					removeItem(breakEdge);

					addItem(sourceEdge);
					addItem(destEdge);
					addItem(newPort);

					if (startConnectionPort->
					    connectionPointType() !=
					    GpxConnectionPoint::NoConnection) {
						addItem(newEdge);
					}
				} else {
					//qDebug("Creating a violent loop!!!!");
				}
				myMode = MoveItem;
			} else if (endItems.count() == 0) {
				GpxConnectionPoint *startItem =
				    connection->getInitPort();
				if (startItem->connectionPointType() !=
				    GpxConnectionPoint::Node) {
					GpxConnectionPoint *endItem =
					    new
					    GpxConnectionPoint
					    (GpxConnectionPoint::NoConnection,
					     findDirection(connection->
							   getMiddlePoint(),
							   connection->
							   getEndPoint()));
					//qDebug("Mi dirección es %d (Right, Down, Left, Up).\n", endItem->direction()); 
					endItem->setPos(connection->
							getEndPoint());
					addItem(endItem);
					GpxEdge *newEdge;
					if (startItem->connectionPointType() ==
					    GpxConnectionPoint::NoConnection) {
						GpxEdge *edge =
						    startItem->getEdgeAt(0);
						edge->addPoint(connection->
							       getMiddlePoint
							       ());
						edge->addPoint(connection->
							       getEndPoint());
						edge->setDestPort(endItem);
						newEdge = edge;
						removeItem(startItem);
					} else {
						newEdge =
						    new GpxEdge(startItem,
								endItem);
						startItem->addEdge(newEdge);
						newEdge->setZValue(-1000.0);
						addItem(newEdge);
					}
					endItem->addEdge(newEdge);
					emit edgeInserted(newEdge);
					myMode = MoveItem;
				}
			}
			removeItem(connection);
			delete connection;
			connection = NULL;
		}
		break;
	case MoveLine:
		emit dirtyChanged(true);
		movingEdge->endMovingEdge();
		movingEdge = NULL;
		myMode = MoveItem;
		break;
	case ResizeItem:
		emit dirtyChanged(true);
		myMode = MoveItem;
		break;
	default:
		qWarning("Handle switch");
	}
	if (rubberBand) {
		removeItem(rubberBand);
		delete rubberBand;
		rubberBand = NULL;

	}
	QRectF v = views().first()->sceneRect();
	QList < QRectF > l;
	l.append(v);
	emit changed(l);
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

bool GpxEditionScene::isItemChange(int type)
{
	foreach(QGraphicsItem * item, selectedItems()) {
		if (item->type() == type)
			return true;
	}
	return false;
}

void GpxEditionScene::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
	QGraphicsScene::dragMoveEvent(event);
	emit dirtyChanged(true);
	//qDebug("%g %g\n", event->scenePos().x(),event->scenePos().y());
	event->accept();

}

void GpxEditionScene::dropEvent(QGraphicsSceneDragDropEvent * event)
{
	PowerGui *p = (PowerGui *) mainWindow;
	if (event->mimeData()->text() == "FromLibAtomic") {	// Dropping an atomic
		modelAtomic *a = p->fromLibAtomic->duplicate();
		a->name = findName(a->name);
		a->priority = coupledData->childs.count();
		GpxAtomic *item = new GpxAtomic(NULL, this, a);
		item->setPos(event->scenePos());
		a->graphics->x = item->x() / TWIPSTOPIXEL;
		a->graphics->y = item->y() / TWIPSTOPIXEL;
		modelChild *mc = new modelChild();
		mc->childType = ATOMIC;
		mc->atomic = a;
		coupledData->childs.append(mc);
		QGraphicsScene::dropEvent(event);
		emit itemInserted(item);
		emit dirtyChanged(true);
	} else if (event->mimeData()->text() == "FromLibInPort" || event->mimeData()->text() == "FromLibOutPort") {	// Dropping an outPort from the library
		emit dirtyChanged(true);
		modelPort *port = p->fromLibPort->duplicate();
		port->name = findName(port->name);
		if (event->mimeData()->text() == "FromLibInPort") {
			port->coupledPort = maxInportNumber();
			GpxInport *item = new GpxInport(NULL, this, port);
			item->setPos(event->scenePos());
			port->x = item->x() / TWIPSTOPIXEL;
			port->y = item->y() / TWIPSTOPIXEL;
			coupledData->lsInPorts.append(port);
			GpxBlock *gb = getData()->owner;
			QGraphicsScene::dropEvent(event);
			emit itemInserted(item);
		} else {
			port->coupledPort = maxOutportNumber();
			GpxOutport *item = new GpxOutport(NULL, this, port);
			item->setPos(event->scenePos());
			port->x = item->x() / TWIPSTOPIXEL;
			port->y = item->y() / TWIPSTOPIXEL;
			coupledData->lsOutPorts.append(port);
			GpxBlock *gb = getData()->owner;
			QGraphicsScene::dropEvent(event);
			emit itemInserted(item);
		}

	} else if (event->mimeData()->text() == "FromLibCoupled") {	// Dropping a coupled
		PowerGui *p = (PowerGui *) mainWindow;
		emit dirtyChanged(true);
		modelCoupled *c = p->fromLibCoupled->duplicate();
		c->name = findName(c->name);
		c->father = coupledData;
		c->priority = coupledData->childs.count();
		GpxCoupled *item = new GpxCoupled(NULL, this, c);
		item->setPos(event->scenePos());
		c->graphics->x = item->x() / TWIPSTOPIXEL;
		c->graphics->y = item->y() / TWIPSTOPIXEL;
		modelChild *mc = new modelChild();
		mc->childType = COUPLED;
		mc->coupled = c;
		coupledData->childs.append(mc);
		QGraphicsScene::dropEvent(event);
		emit itemInserted(item);
	} else if (event->mimeData()->text().startsWith("Drag")) {	// Dropping an outPort from the library
		PowerGui *powerGui = (PowerGui *) mainWindow;
		QStringList ls = event->mimeData()->text().split(" ");
		qreal x, y;
		x = ls.at(1).toFloat();
		y = ls.at(2).toFloat();
		qreal offx = event->scenePos().x() - x;
		qreal offy = event->scenePos().y() - y;
		paste(powerGui->fromDragCoupled, offx, offy);
		delete(powerGui->fromDragCoupled);
		powerGui->fromDragCoupled = NULL;
	} else {
		event->ignore();
	}
}

void GpxEditionScene::deleteItem(GpxBlock * atomicToDelete)
{
	emit dirtyChanged(true);
	foreach(QGraphicsItem * item, items()) {
		if (item->type() == GpxBlock::Type) {
			if (atomic->blockType() == GpxBlock::InPort
			    && atomicToDelete->blockType() ==
			    GpxBlock::InPort) {
				GpxInport *ip =
				    qgraphicsitem_cast < GpxInport * >(item);
				GpxInport *ipToDelete =
				    qgraphicsitem_cast <
				    GpxInport * >(atomicToDelete);
				if (ip->getPortNumber() >
				    ipToDelete->getPortNumber())
					ip->setPortNumber(ip->getPortNumber() -
							  1);
			}
			if (atomic->blockType() == GpxBlock::OutPort
			    && atomicToDelete->blockType() ==
			    GpxBlock::OutPort) {
				GpxOutport *op =
				    qgraphicsitem_cast < GpxOutport * >(item);
				GpxOutport *opToDelete =
				    qgraphicsitem_cast <
				    GpxOutport * >(atomicToDelete);
				if (op->getPortNumber() >
				    opToDelete->getPortNumber())
					op->setPortNumber(op->getPortNumber() -
							  1);
			}
		}
	}
}

int GpxEditionScene::getMaxPriority()
{
	int priority = 0;
	foreach(QGraphicsItem * i, items()) {
		if (i->type() == GpxBlock::Type) {
			GpxBlock *b = qgraphicsitem_cast < GpxBlock * >(i);
			if (b->blockType() == GpxBlock::Coupled) {
				GpxCoupled *c = (GpxCoupled *) b;
				if (c->getPriority() >= priority)
					priority = c->getPriority() + 1;
			}
			if (b->blockType() == GpxBlock::SimpleAtomic) {
				GpxAtomic *a = (GpxAtomic *) b;
				if (a->getPriority() >= priority)
					priority = a->getPriority() + 1;
			}

		}
	}
	return priority;
}

QList < modelChild * >GpxEditionScene::getChilds()
{
	QList < modelChild * >l;
	foreach(modelChild * mc, coupledData->childs)
	    l << mc;
	/*foreach (QGraphicsItem *i,items()) {
	   if (i->type()==GpxBlock::Type) {
	   GpxBlock *a=qgraphicsitem_cast<GpxBlock *>(i);
	   if (a->blockType()==GpxBlock::Coupled) {
	   modelChild *mc=new modelChild();
	   mc->childType=COUPLED;
	   mc->coupled=((GpxCoupled*)a)->getData();
	   l << mc;
	   }
	   if (a->blockType()==GpxBlock::SimpleAtomic) {
	   modelChild *mc=new modelChild();
	   mc->childType=ATOMIC;
	   mc->atomic=((GpxAtomic*)a)->getData();
	   l << mc;
	   }
	   }
	   } */
	return l;
}

void GpxEditionScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *
					    mouseEvent)
{
	/*
	   if (items(mouseEvent->scenePos()).count()==0) {
	   GpxTextBox *textItem = new GpxTextBox();
	   textItem->setFont(myFont);
	   textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
	   textItem->setZValue(1000.0);
	   connect(textItem, SIGNAL(lostFocus(GpxTextBox *)),
	   this, SLOT(editorLostFocus(GpxTextBox *)));
	   connect(textItem, SIGNAL(selectedChange(QGraphicsItem *)),
	   this, SIGNAL(itemSelected(QGraphicsItem *)));
	   addItem(textItem);
	   textItem->setDefaultTextColor(myTextColor);
	   textItem->setFlag(QGraphicsItem::ItemIsMovable,true);
	   textItem->setPos(mouseEvent->scenePos());
	   qDebug("Add comment");
	   emit dirtyChanged(true);     
	   }
	 */
	QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void GpxEditionScene::setDeletedScenes(modelCoupled * c)
{
	qDebug("Cantidad de childs %d",c->childs.count());
	foreach(modelChild * mch, c->childs) {
		if (mch->childType == COUPLED) {
			if (mch->coupled->owner != NULL) { // In this case, theres an open Tab
				qDebug("Intenta borrar el coupled %s",qPrintable(mch->coupled->name));
				GpxCoupled *cb = qgraphicsitem_cast < GpxCoupled * >(mch->coupled->owner);
				if (cb->getOpenEditionScene() != NULL) {
					qDebug ("Setea como borrada la escena %s", qPrintable(cb->getName()));
					cb->getOpenEditionScene()->setDeleted();
				}
			}
			setDeletedScenes(mch->coupled);
		}
	}
}

void GpxEditionScene::removeBlock(GpxBlock * b)
{
	if (b->blockType() == GpxBlock::SimpleAtomic || b->blockType() == GpxBlock::Coupled) {
		int i = 0;
		foreach(modelChild * mc, coupledData->childs) {
			if (mc->childType == ATOMIC
			    && mc->atomic == ((GpxAtomic *) b)->getData())
				break;
			if (mc->childType == COUPLED
			    && mc->coupled == ((GpxCoupled *) b)->getData())
				break;
			i++;
		}
		modelChild *mc = coupledData->childs.takeAt(i);
		/*if (mc->childType == COUPLED) {
			modelCoupled *c = mc->coupled;
			setDeletedScenes(c);
			if (c->owner != NULL) {	// Creo que esto es redundante.
				GpxCoupled *cb = qgraphicsitem_cast < GpxCoupled * >(c->owner);
				if (cb->getOpenEditionScene() != NULL) {
					qDebug ("Setea como borrada la escena %s", qPrintable(cb->getName()));
					cb->getOpenEditionScene()->setDeleted();
				}
			}
		}*/
		delete mc;
		//((GpxEditionWindow *) parent())->updateTabs();
	} else if (b->blockType() == GpxBlock::InPort) {
		int i = 0;
		foreach(modelPort * p, coupledData->lsInPorts) {
				if (p->coupledPort >
				    ((GpxInport *) b)->getData()->coupledPort) {
					p->coupledPort--;
					((GpxInport *) p->owner)->
					    setPortNumber(p->coupledPort);
				}
		}
		foreach(modelPort * p, coupledData->lsInPorts) {
			if (p == ((GpxInport *) b)->getData())
				break;
			i++;
		}
	   GpxInport *gi=qgraphicsitem_cast<GpxInport*>(b);
	   GpxBlock *gb = getData()->owner;
	   if (gb!=NULL) {
		 GpxConnectionPoint *gcp = gb->getInportAt(i);
         gb->removePort(gcp, GpxConnectionPoint::InPort);
	   } else { // The father's scene is close so, delete the port in the modelCouled struct
	     getData()->removeInport(i);
      }
		modelPort *mp = coupledData->lsInPorts.takeAt(i);
	} else if (b->blockType() == GpxBlock::OutPort) {
		int i = 0;
		foreach(modelPort * p, coupledData->lsOutPorts) {
				if (p->coupledPort >
				    ((GpxOutport *) b)->getData()->
				    coupledPort) {
					p->coupledPort--;
					((GpxOutport *) p->owner)->
					    setPortNumber(p->coupledPort);
				}
		}
		foreach(modelPort * p, coupledData->lsOutPorts) {
			if (p == ((GpxOutport *) b)->getData())
				break;
			i++;
		}
	   GpxInport *gi=qgraphicsitem_cast<GpxInport*>(b);
	   GpxBlock *gb = getData()->owner;
	   if (gb!=NULL) {
		 GpxConnectionPoint *gcp = gb->getOutportAt(i);
         gb->removePort(gcp, GpxConnectionPoint::OutPort);
	   } else { // The father's scene is close so, delete the port in the modelCouled struct
	     getData()->removeOutport(i);
      }
		modelPort *mp = coupledData->lsOutPorts.takeAt(i);
		/*if (owner != NULL)
			owner->removePort(owner->getOutportAt(i), GpxConnectionPoint::OutPort);
		modelPort *mp = coupledData->lsPorts.takeAt(i);*/

	}
	QGraphicsScene::removeItem(b);
}

void GpxEditionScene::updateConnections()
{
	/* Cleanup */
	foreach(modelPoint * p, coupledData->points)
	delete p;
	coupledData->points.clear();
	foreach(modelLine * l, coupledData->lines)
	delete l;
	coupledData->lines.clear();
	//coupledData->lsPorts.clear();
	coupledData->lsInPorts.clear();
	coupledData->lsOutPorts.clear();
	QList < GpxBlock * >childs;
	QList < GpxConnectionPoint * >points;
	QList < modelPoint * >pointsData;
	QList < GpxBlock * >ports;
	foreach(QGraphicsItem * i, items()) {
		if (i->type() == GpxBlock::Type) {
			GpxBlock *b = qgraphicsitem_cast < GpxBlock * >(i);
			if (b->blockType() == GpxBlock::InPort) {
				GpxInport *gip =
				    qgraphicsitem_cast < GpxInport * >(b);
				coupledData->lsInPorts.append(gip->getData());
				ports.append(gip);
			} else if (b->blockType() == GpxBlock::OutPort) {
				GpxOutport *gop =
				    qgraphicsitem_cast < GpxOutport * >(b);
				coupledData->lsOutPorts.append(gop->getData());
				ports.append(gop);
			} else
				childs.append(b);
		} else if (i->type() == GpxConnectionPoint::Type) {
			GpxConnectionPoint *gcp =
			    qgraphicsitem_cast < GpxConnectionPoint * >(i);
			if (gcp->connectionPointType() ==
			    GpxConnectionPoint::Node) {
				modelPoint *mp = new modelPoint();
				mp->x = gcp->x() / TWIPSTOPIXEL;
				mp->y = gcp->y() / TWIPSTOPIXEL;
				coupledData->points.append(mp);
				pointsData.append(mp);
				points.append(gcp);
			}

		}
	}
	foreach(QGraphicsItem * i, items()) {
		if (i->type() == GpxEdge::Type) {
			GpxEdge *ge = qgraphicsitem_cast < GpxEdge * >(i);
			modelLine *ml = new modelLine();
			GpxConnectionPoint *s = ge->sourcePort();
			GpxConnectionPoint *d = ge->destPort();
			if (s->connectionPointType() ==
			    GpxConnectionPoint::NoConnection) {
				ml->sourceType = TOKNOC;
				ml->sources << 1 << -1 << 0;
			}
			if (d->connectionPointType() ==
			    GpxConnectionPoint::NoConnection) {
				ml->sinkType = TOKNOC;
				ml->sinks << 1 << -1 << 0;
			}
			if (s->connectionPointType() ==
			    GpxConnectionPoint::InPort) {
				GpxBlock *b =
				    qgraphicsitem_cast <
				    GpxBlock * >(s->parentItem());
				switch (b->blockType()) {
				case GpxBlock::SimpleAtomic:
				case GpxBlock::Coupled:
					ml->sourceType = TOKCMP;
					ml->sources << childs.indexOf(b) +
					    1 << b->getInportNumber(s) + 1 << 1;
					break;
				case GpxBlock::InPort:
				case GpxBlock::OutPort:
					ml->sourceType = TOKPRT;
					if (b->blockType() == GpxBlock::InPort)
						ml->sources << ports.
						    indexOf(b) + 1 << 0 << 0;
					else
						ml->sources << ports.
						    indexOf(b) + 1 << 0 << -1;
				}
			}
			if (d->connectionPointType() ==
			    GpxConnectionPoint::InPort) {
				GpxBlock *b =
				    qgraphicsitem_cast <
				    GpxBlock * >(d->parentItem());
				switch (b->blockType()) {
				case GpxBlock::SimpleAtomic:
				case GpxBlock::Coupled:
					ml->sinkType = TOKCMP;
					ml->sinks << childs.indexOf(b) +
					    1 << b->getInportNumber(d) + 1 << 1;
					break;
				case GpxBlock::InPort:
				case GpxBlock::OutPort:
					ml->sinkType = TOKPRT;
					if (b->blockType() == GpxBlock::InPort)
						ml->sinks << ports.indexOf(b) +
						    1 << 0 << 0;
					else
						ml->sinks << ports.indexOf(b) +
						    1 << 0 << -1;
				}
			}
			if (s->connectionPointType() ==
			    GpxConnectionPoint::OutPort) {
				GpxBlock *b =
				    qgraphicsitem_cast <
				    GpxBlock * >(s->parentItem());
				switch (b->blockType()) {
				case GpxBlock::SimpleAtomic:
				case GpxBlock::Coupled:
					ml->sourceType = TOKCMP;
					ml->sources << childs.indexOf(b) +
					    1 << b->getOutportNumber(s) +
					    1 << 0;
					break;
				case GpxBlock::InPort:
				case GpxBlock::OutPort:
					ml->sourceType = TOKPRT;
					if (b->blockType() == GpxBlock::InPort)
						ml->sources << ports.indexOf(b) + 1 << 0 << 0;
					else
						ml->sources << ports.
						    indexOf(b) + 1 << 0 << -1;
				}

			}
			if (d->connectionPointType() ==
			    GpxConnectionPoint::OutPort) {
				GpxBlock *b =
				    qgraphicsitem_cast <
				    GpxBlock * >(d->parentItem());
				switch (b->blockType()) {
				case GpxBlock::SimpleAtomic:
				case GpxBlock::Coupled:
					ml->sinkType = TOKCMP;
					ml->sinks << childs.indexOf(b) +
					    1 << b->getOutportNumber(d) +
					    1 << 0;
					break;
				case GpxBlock::InPort:
				case GpxBlock::OutPort:
					ml->sinkType = TOKPRT;
					if (b->blockType() == GpxBlock::InPort)
						ml->sinks << ports.indexOf(b) +
						    1 << 0 << 0;
					else
						ml->sinks << ports.indexOf(b) +
						    1 << 0 << -1;
				}
			}
			if (s->connectionPointType() ==
			    GpxConnectionPoint::Node) {
				ml->sourceType = TOKPNT;
				int point = points.indexOf(s);
				ml->sources << point + 1 << 0 << 0;
				modelPoint *mp = pointsData.at(point);
				mp->lines.append(coupledData->lines.size()+1);
				mp->extrems.append(TOKORG);
			}
			if (d->connectionPointType() ==
			    GpxConnectionPoint::Node) {
				ml->sinkType = TOKPNT;
				int point = points.indexOf(d);
				ml->sinks << point + 1 << 0 << 0;
				modelPoint *mp = pointsData.at(point);
				mp->lines.append(coupledData->lines.size()+1);
				mp->extrems.append(TOKDST);
			}
			QPolygonF poly = ge->polygon();
			for (int i = 0; i < poly.count(); i++) {
				QPointF p = poly.at(i);
				ml->pointX << (p.x() / TWIPSTOPIXEL);
				ml->pointY << (p.y() / TWIPSTOPIXEL);
			}
			coupledData->lines.append(ml);

		}
	}
}

modelCoupled *GpxEditionScene::createModelDataFromSelection()
{
	modelCoupled *ret = new modelCoupled();
	ret->owner = NULL;
	QList < GpxConnectionPoint * >points;
	QList < GpxBlock * >childs;
	QList < GpxBlock * >ports;
	QList < modelPoint * >pointsData;
	foreach(QGraphicsItem * i, selectedItems()) {
		if (i->type() == GpxBlock::Type) {
			GpxBlock *b = qgraphicsitem_cast < GpxBlock * >(i);
			if (b->blockType() == GpxBlock::SimpleAtomic) {
				modelChild *mc = new modelChild();
				mc->childType = ATOMIC;
				mc->atomic =
				    qgraphicsitem_cast <
				    GpxAtomic * >(b)->getData()->duplicate();
				childs << b;
				ret->childs.append(mc);
			} else if (b->blockType() == GpxBlock::Coupled) {
				modelChild *mc = new modelChild();
				mc->childType = COUPLED;
				mc->coupled =
				    qgraphicsitem_cast <
				    GpxCoupled * >(b)->getData()->duplicate();
				childs << b;
				ret->childs.append(mc);
			} else if (b->blockType() == GpxBlock::InPort) {
				ret->lsInPorts.append(qgraphicsitem_cast <
						    GpxInport *
						    >(b)->getData()->
						    duplicate());
				ports << b;
			} else if (b->blockType() == GpxBlock::OutPort) {
				ret->lsOutPorts.append(qgraphicsitem_cast <
						    GpxOutport *
						    >(b)->getData()->
						    duplicate());
				ports << b;
			}
		}
		if (i->type() == GpxConnectionPoint::Type) {
			GpxConnectionPoint *gcp = qgraphicsitem_cast<GpxConnectionPoint*>(i);
			bool allSelected=true;
			for (int j=0;j<gcp->numberOfEdges();j++) {
				allSelected &= gcp->getEdgeAt(j)->isSelected();
			}	
			if (allSelected) { // Copy this point
				modelPoint *mp = new modelPoint();
				mp->x = gcp->x() / TWIPSTOPIXEL;
				mp->y = gcp->y() / TWIPSTOPIXEL;
				ret->points.append(mp);
				pointsData.append(mp);
				points.append(gcp);
			}
		}
	}
	int lines = 1;
	foreach(QGraphicsItem * i, selectedItems()) {
		if (i->type() == GpxEdge::Type) {
			GpxEdge *e = qgraphicsitem_cast < GpxEdge * >(i);
			if (e->sourcePort()->connectionPointType() == GpxConnectionPoint::NoConnection || e->destPort()->connectionPointType() == GpxConnectionPoint::NoConnection) {	// do not copy connectos from/to a cross
				continue;
			}
			if ((e->sourcePort()->connectionPointType() ==
			     GpxConnectionPoint::InPort
			     || e->sourcePort()->connectionPointType() ==
			     GpxConnectionPoint::OutPort 
				 || e->sourcePort()->connectionPointType() ==
				 GpxConnectionPoint::Node )
			    && (e->destPort()->connectionPointType() ==
				GpxConnectionPoint::InPort
				|| e->destPort()->connectionPointType() ==
				GpxConnectionPoint::OutPort
				 || e->sourcePort()->connectionPointType() ==
				 GpxConnectionPoint::Node )){
				if (e->sourcePort()->connectionPointType()!=GpxConnectionPoint::Node &&
					!e->sourcePort()->parentItem()->isSelected()) continue;
				if (e->destPort()->connectionPointType()!=GpxConnectionPoint::Node &&
					!e->destPort()->parentItem()->isSelected()) continue;
				if (e->sourcePort()->connectionPointType()==GpxConnectionPoint::Node &&
					points.indexOf(e->sourcePort())<0) continue;
				if (e->destPort()->connectionPointType()==GpxConnectionPoint::Node &&
					points.indexOf(e->destPort())<0) continue;
				qDebug("Copiar la conexion");
				modelLine *ml = new modelLine();
				GpxConnectionPoint *s = e->sourcePort();
				GpxConnectionPoint *d = e->destPort();
				if (s->connectionPointType() ==
				    GpxConnectionPoint::InPort) {
					GpxBlock *b =
					    qgraphicsitem_cast <
					    GpxBlock * >(s->parentItem());
					switch (b->blockType()) {
					case GpxBlock::SimpleAtomic:
					case GpxBlock::Coupled:
						ml->sourceType = TOKCMP;
						ml->sources << childs.
						    indexOf(b) +
						    1 << b->getInportNumber(s) +
						    1 << 1;
						break;
					case GpxBlock::InPort:
					case GpxBlock::OutPort:
						ml->sourceType = TOKPRT;
						if (b->blockType() ==
						    GpxBlock::InPort)
							ml->sources << ports.
							    indexOf(b) +
							    1 << 0 << 0;
						else
							ml->sources << ports.
							    indexOf(b) +
							    1 << 0 << -1;
					}
				}
				if (d->connectionPointType() ==
				    GpxConnectionPoint::InPort) {
					GpxBlock *b =
					    qgraphicsitem_cast <
					    GpxBlock * >(d->parentItem());
					switch (b->blockType()) {
					case GpxBlock::SimpleAtomic:
					case GpxBlock::Coupled:
						ml->sinkType = TOKCMP;
						ml->sinks << childs.indexOf(b) +
						    1 << b->getInportNumber(d) +
						    1 << 1;
						break;
					case GpxBlock::InPort:
					case GpxBlock::OutPort:
						ml->sinkType = TOKPRT;
						if (b->blockType() ==
						    GpxBlock::InPort)
							ml->sinks << ports.
							    indexOf(b) +
							    1 << 0 << 0;
						else
							ml->sinks << ports.
							    indexOf(b) +
							    1 << 0 << -1;
					}
				}
				if (s->connectionPointType() ==
				    GpxConnectionPoint::OutPort) {
					GpxBlock *b =
					    qgraphicsitem_cast <
					    GpxBlock * >(s->parentItem());
					switch (b->blockType()) {
					case GpxBlock::SimpleAtomic:
					case GpxBlock::Coupled:
						ml->sourceType = TOKCMP;
						ml->sources << childs.
						    indexOf(b) +
						    1 << b->
						    getOutportNumber(s) +
						    1 << 0;
						break;
					case GpxBlock::InPort:
					case GpxBlock::OutPort:
						ml->sourceType = TOKPRT;
						if (b->blockType() ==
						    GpxBlock::InPort)
							ml->sources << ports.
							    indexOf(b) +
							    1 << 0 << 0;
						else
							ml->sources << ports.
							    indexOf(b) +
							    1 << 0 << -1;
					}

				}
				if (d->connectionPointType() ==
				    GpxConnectionPoint::OutPort) {
					GpxBlock *b =
					    qgraphicsitem_cast <
					    GpxBlock * >(d->parentItem());
					switch (b->blockType()) {
					case GpxBlock::SimpleAtomic:
					case GpxBlock::Coupled:
						ml->sinkType = TOKCMP;
						ml->sinks << childs.indexOf(b) +
						    1 << b->
						    getOutportNumber(d) +
						    1 << 0;
						break;
					case GpxBlock::InPort:
					case GpxBlock::OutPort:
						ml->sinkType = TOKPRT;
						if (b->blockType() ==
						    GpxBlock::InPort)
							ml->sinks << ports.
							    indexOf(b) +
							    1 << 0 << 0;
						else
							ml->sinks << ports.
							    indexOf(b) +
							    1 << 0 << -1;
					}
					if (s->connectionPointType() ==
					    GpxConnectionPoint::Node) {
						ml->sourceType = TOKPNT;
						int point = points.indexOf(s);
						ml->sources << point + 1 << 0 << 0;
						modelPoint *mp = pointsData.at(point);
						mp->lines.append(ret->lines.size()+1);
						mp->extrems.append(TOKORG);
					}
					if (d->connectionPointType() ==
					    GpxConnectionPoint::Node) {
						ml->sinkType = TOKPNT;
						int point = points.indexOf(d);
						ml->sinks << point + 1 << 0 << 0;
						modelPoint *mp = pointsData.at(point);
						mp->lines.append(ret->lines.size()+1);
						mp->extrems.append(TOKDST);
					}
				}
				QPolygonF poly = e->polygon();
				for (int i = 0; i < poly.count(); i++) {
					QPointF p = poly.at(i);
					ml->pointX << (p.x() / TWIPSTOPIXEL);
					ml->pointY << (p.y() / TWIPSTOPIXEL);
				}
				ret->lines.append(ml);
			}
		}
	}
	ret->owner = NULL;
	return ret;
}

void GpxEditionScene::paste(modelCoupled * data, qreal offx, qreal offy)
{
	QList < GpxConnectionPoint * >points;
	QList < GpxBlock * >childs;
	QList < GpxBlock * >lsPorts;
	clearSelection();
	foreach(modelChild * c, data->childs) {
		if (c->childType == ATOMIC) {
			c->atomic->graphics->y += offy / TWIPSTOPIXEL;
			c->atomic->graphics->x += offx / TWIPSTOPIXEL;
			modelAtomic *a = c->atomic->duplicate();
			a->name = findName(a->name);
			a->priority = coupledData->childs.count();
			GpxAtomic *item = new GpxAtomic(NULL, this, a);
			item->setSelected(true);
			item->
			    setPos(QPoint
				   (a->graphics->x * TWIPSTOPIXEL,
				    a->graphics->y * TWIPSTOPIXEL));
			a->graphics->x = item->x() / TWIPSTOPIXEL;
			a->graphics->y = item->y() / TWIPSTOPIXEL;
			modelChild *mc = new modelChild();
			mc->childType = ATOMIC;
			mc->atomic = a;
			coupledData->childs.append(mc);
			emit itemInserted(item);
			childs << item;
		} else {
			c->coupled->graphics->y += offy / TWIPSTOPIXEL;
			c->coupled->graphics->x += offx / TWIPSTOPIXEL;
			modelCoupled *newC = c->coupled->duplicate();
			newC->name = findName(newC->name);
			newC->father = coupledData;
			newC->priority = coupledData->childs.count();
			GpxCoupled *item = new GpxCoupled(NULL, this, newC);
			childs << item;
			item->
			    setPos(QPoint
				   (newC->graphics->x * TWIPSTOPIXEL,
				    newC->graphics->y * TWIPSTOPIXEL));
			newC->graphics->x = item->x() / TWIPSTOPIXEL;
			newC->graphics->y = item->y() / TWIPSTOPIXEL;
			modelChild *mc = new modelChild();
			mc->childType = COUPLED;
			mc->coupled = newC;
			coupledData->childs.append(mc);
			emit itemInserted(item);
			item->setSelected(true);
		}
	}
	foreach(modelPort * p, data->lsInPorts) {
			p->y += offy / TWIPSTOPIXEL;
			p->x += offx / TWIPSTOPIXEL;
			modelPort *port = p->duplicate();
			port->name = findName(port->name);
			port->coupledPort = maxInportNumber();
			GpxInport *item = new GpxInport(NULL, this, port);
			item->
			    setPos(QPoint
				   (port->x * TWIPSTOPIXEL,
				    port->y * TWIPSTOPIXEL));
			port->x = item->x() / TWIPSTOPIXEL;
			port->y = item->y() / TWIPSTOPIXEL;
			coupledData->lsInPorts.append(port);
			//coupledData->inPorts += 1;
			item->setSelected(true);
			emit itemInserted(item);
			lsPorts << item;
	}
	foreach(modelPort * p, data->lsOutPorts) {
			p->y += offy / TWIPSTOPIXEL;
			p->x += offx / TWIPSTOPIXEL;
			modelPort *port = p->duplicate();
			port->name = findName(port->name);
			port->coupledPort = maxOutportNumber();
			GpxOutport *item = new GpxOutport(NULL, this, port);
			item->
			    setPos(QPoint
				   (port->x * TWIPSTOPIXEL,
				    port->y * TWIPSTOPIXEL));
			item->setSelected(true);
			port->x = item->x() / TWIPSTOPIXEL;
			port->y = item->y() / TWIPSTOPIXEL;
			coupledData->lsOutPorts.append(port);
			//coupledData->outPorts += 1;
			emit itemInserted(item);
			lsPorts << item;
	}
	foreach(modelLine * l, data->lines) {
		GpxConnectionPoint *s = NULL, *d = NULL;
		if (l->sourceType == TOKCMP) {	// Source is a model
			if (l->sources.at(2) == 0) {	// OutPort
				s = childs.at(l->sources.at(0) -
					      1)->getOutportAt(l->sources.
							       at(1) - 1);
			} else {	// InPort
				s = childs.at(l->sources.at(0) -
					      1)->getInportAt(l->sources.at(1) -
							      1);
			}

		}
		if (l->sourceType == TOKNOC) {	// Source is a cross
			s = new GpxConnectionPoint(GpxConnectionPoint::
						   NoConnection,
						   GpxConnectionPoint::Left,
						   NULL, this);
			s->setPos(l->pointX.first() * TWIPSTOPIXEL,
				  l->pointY.first() * TWIPSTOPIXEL);
		}
		if (l->sinkType == TOKNOC) {	// Sink is a cross
			d = new GpxConnectionPoint(GpxConnectionPoint::
						   NoConnection,
						   GpxConnectionPoint::Left,
						   NULL, this);
			d->setPos(l->pointX.last() * TWIPSTOPIXEL,
				  l->pointY.last() * TWIPSTOPIXEL);
		}

		if (l->sourceType == TOKPNT) {	// Source is a point
			s = points.at(l->sources.at(0) - 1);
		}
		if (l->sourceType == TOKPRT) {	// Source is a port
			GpxBlock *port = lsPorts.at(l->sources.at(0) - 1);
			if (port->blockType() == GpxBlock::InPort)
				s = port->getOutportAt(0);
			else
				s = port->getInportAt(0);
		}
		if (l->sinkType == TOKCMP) {	// Sink is a model
			if (l->sinks.at(2) == 0) {	// OutPort
				d = childs.at(l->sinks.at(0) -
					      1)->getOutportAt(l->sinks.at(1) -
							       1);
			} else {	// InPort
				d = childs.at(l->sinks.at(0) -
					      1)->getInportAt(l->sinks.at(1) -
							      1);
			}
		}
		if (l->sinkType == TOKPNT) {	// Source is a point
			d = points.at(l->sinks.at(0) - 1);
		}
		if (l->sinkType == TOKPRT) {	// Sink is a port
			GpxBlock *port = lsPorts.at(l->sinks.at(0) - 1);
			if (port->blockType() == GpxBlock::InPort)
				d = port->getOutportAt(0);
			else
				d = port->getInportAt(0);
		}
		//qDebug("%s %s %x %x\n",QSTR(l->sourceType),QSTR(l->sinkType),s,d);
		if (s != NULL && d != NULL) {
			GpxEdge *e = new GpxEdge(s, d);
			QPolygonF p;
			int i = 0;
			foreach(int x, l->pointX) {
				x += offx / TWIPSTOPIXEL;
				p << QPointF(x * TWIPSTOPIXEL,
					     (l->pointY.at(i) +
					      offy) * TWIPSTOPIXEL);
				i++;
			}
			if (l->pointX.size() > 2)
				e->setPolygon(p);
			s->addEdge(e);
			d->addEdge(e);
			addItem(e);
		}
	}
	emit dirtyChanged(true);
}

QString GpxEditionScene::findName(QString proposedName)
{
	QStringList ls = proposedName.split(" ");
	bool ok;
	int number = 0;
	number = ls.last().toInt(&ok);
	if (ok) {
		ls.removeLast();
		proposedName = ls.join(" ");
	} else {
		number = 0;
	}
	bool forBreak;
	bool nameFound = false;
	proposedName.append(" %1");
	do {
		forBreak = false;
		foreach(QGraphicsItem * item, items()) {
			if (item->type() == GpxBlock::Type) {
				GpxBlock *b =
				    qgraphicsitem_cast < GpxBlock * >(item);
				if (proposedName.arg(number) == b->getName()) {
					number++;
					forBreak = true;
					break;
				}
			}
		}
		if (!forBreak) {
			nameFound = true;
		}
	} while (!nameFound && items().size());
	proposedName = proposedName.arg(number);
	return proposedName;
}

int GpxEditionScene::maxOutportNumber()
{
	//int c = 0;
	//foreach(modelPort * p, coupledData->lsPorts) {
	//	if (p->type == OUTPORT)
	//		c++;
	//}
	return coupledData->lsOutPorts.count();
}

int GpxEditionScene::maxInportNumber()
{
	/*int c = 0;
	foreach(modelPort * p, coupledData->lsPorts) {
		if (p->type == INPORT)
			c++;
	}*/
	return coupledData->lsInPorts.count();
}

void GpxEditionScene::closeScene()
{
	if(owner != NULL){
		owner->setOpen(-1);
		owner->doneEditing();
	}
}

void GpxEditionScene::updateOwners()
{
	foreach(modelChild *mc, coupledData->childs){
		if(mc->childType == COUPLED){	
			if(mc->coupled->owner != NULL){ // Creo que es redundante
				qDebug("Setea el owner a NULL para %s",qPrintable(mc->coupled->name));
				GpxCoupled *c = qgraphicsitem_cast<GpxCoupled *>(mc->coupled->owner);
				if(c->getOpenEditionScene() != NULL)
					c->getOpenEditionScene()->setOwner(NULL);
			}
		}
	}
	//qDebug("Setea su owner a NULL %s", qPrintable(owner->getName()));
	owner = NULL;
}
