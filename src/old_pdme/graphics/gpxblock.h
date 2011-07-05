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

#ifndef GPXBLOCK_H
#define GPXBLOCK_H

#include <QGraphicsPixmapItem>
#include <QList>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QWidget>
#include <QObject>
#include <QGraphicsItem>

#include "gpxconnectionpoint.h"
#include "parser.h"
#include "gpxtextbox.h"

class QPixmap;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QPolygonF;
class QGraphicsSimpleTextItem;

class PowerDialogs;

/*! \brief This class defines PowerDevs graphic item interface.
*
*   This class inherits from GraphicItem and defines the items needed to 
*   graphically represent DEVS models. These items are then used by DockScene
*   that inherits from QGraphicsScene. 
*		
*		\todo
*					- updatePortsPosition:
*						-# No esta actualizando bien el tamaño de los graficos, cuando se agregan mas de 5 puertos.
*					- removePort:
*						-# No hace bien el remove, esta sacando el inport y el uotport. Pero no se pueden sacar por separado, ni se actualiza el grafico.
*					- Ver la clase QClipBoard para hacer las operaciones de copiar, pegar, mover, etc....
*					- Ver si se puede hacer la herencia multiple en Atomic, para que pueda emitir señales. ##
*/

#define GPXBLOCK_TYPE 15
class GpxBlock :public QObject, public QGraphicsItem {
 Q_OBJECT public:
	// Ver que define este tipo. 
	enum { Type = UserType + GPXBLOCK_TYPE };
	/*! Enumeration defined to represent different types of graphic items */
	enum BlockType { SimpleAtomic,/**< Graphic item to represent an Atomic model. */
		InPort,								 /**< Graphic item to represent an Inport for a Coupled model. */
		OutPort,							  /**< Graphic item to represent an OutPort for a Coupled model. */
		Coupled /**< Graphic item to represent a Coupled model. */
	};

	/*! Creates a new Atomic, given the following parameters: */
	/*!     atomicType: The type of the Atomic, defined in #AtomicType. */
	/*! parent:             The parent is used for Coupled models. */
	/*! scene:                      The scene represents the context where the item is inserted. */
	/*! data:                               The data depend on the type of the Atomic. If it's a SimpleAtomic, we obtain data from a modelAtomic, if it's a Coupled, we obtain data                                                                  from a modelCoupled. For Inports and Outports, is initialized to NULL. */
	/*! portNumber: Falta.... */
	 GpxBlock(QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);

	/*! Destroys the Atomic. */
	~GpxBlock();

	/*! Removes the Port from the Atomic. No esta funcionando, hay que modificarlo. */
	void removePort(GpxConnectionPoint * port,
			GpxConnectionPoint::ConnectionPointType pt);
	/*! Removes all Ports from the Atomic. */
	void removePorts();
	/*! Returns the type of the Atomic. */
	virtual BlockType blockType() = 0;
	/*! Returns the Polygon of the Atomic. */
	QPolygonF polygon() const {
		return myPolygon;
	}
	/*! Adds a Port to the Atomic. */
	    void addPort(GpxConnectionPoint::ConnectionPointType);
	/*! Returns the Port Number of the Atomic. */
	//int myNumber();
	/*! Returns the image of the Atomic. */
	QPixmap image();
	/*! Returns the type of the Atomic. */
	int type() const {
		return Type;
	}
	/*! Updates the position of ports in the Atomic. 
	   Compares the height of the Atomic with the number of ports multiplied by 8 and updates it.
	 */ void updatePortsPositions();
	/*! Returns a #QPainterPath, which contains the graph of the model, complete ... */
	QPainterPath shape() const;
	/*! Rotate the graphic of atomic, side effect, changes the position and direction of the ports of entry, exit and text. */
	void rotate();
	/*! Updates the background color of the Atomic */
	void setBackgroundColor(const QColor & color);
	// Temporal, falta definir como hacer el, update!!!.
	QColor getBackgroundColor() {
		return myBackgroundColor;
	};
	/*! Returns the name of the Atomic. */
	QString getName() {
		return name;
	};
	/*! Updates the Atomic port number. */
	//void setPortNumber(int number);
	//int  getPortNumber() { return portNumber; };
	/*! Updates the Atomic polygon size. */
	void setSize(qreal w, qreal h);
	void resizeTo(QPointF);
	void startResizing(int corner) {
		resizeCorner = corner;
	}
	/*! Returns the size of the Atomic. */
	QPointF getSize() {
		return QPointF(getWidth(), getHeight());
	};
	/*! Retuns the width of the Atomic, including the size of the TextBox. */
	qreal getWidth() {
		return ((myText->boundingRect().width() >
			 boundingRect().width())? myText->boundingRect().width()
			: boundingRect().width());
	};
	/*! Retuns the height of the Atomic, including the size of the TextBox . */
	qreal getHeight() {
		return myText->boundingRect().height() +
		    boundingRect().height();
	};

	int getBlockHeight() {
		return height;
	};
	int getBlockWidth() {
		return width;
	};
	/*! Returns {1,2,3,4} if the point passed as an argument is at the corner of the Atomic */
	int isAtCorner(QPointF);
	/*! Returns the number of InPorts of the Atomic. */
	int numberOfInPorts() {
		return inPorts.size();
	}
	/*! Returns the numbeer of OutPorts of the Atomic. */
	int numberOfOutPorts() {
		return outPorts.size();
	}
	int getInportNumber(GpxConnectionPoint * gcp) {
		return inPorts.indexOf(gcp);
	};
	int getOutportNumber(GpxConnectionPoint * gcp) {
		return outPorts.indexOf(gcp);
	};
	/*! Returns the InPort that is in the position i in the list of InPorts of the Atomic. */
	GpxConnectionPoint *getInportAt(int i) {
                //qDebug("Looking for port %d in a total of %d of %x",i,inPorts.size(),(unsigned)this);
		return inPorts.at(i);
	};
	/*! Returns the OutPort that is in the position i in the list of OutPorts of the Atomic. */
	GpxConnectionPoint *getOutportAt(int i) {
		return outPorts.at(i);
	};
	/*! Contains data associated with the Atomic #modelCoupled, or NULL if not an Atomic::Coupled. To access the data consistently, 
	   consult first the #AtomicType of the Atomic. */
	//modelCoupled *coupledData;    
	/*! Contains data associated with the Atomic #modelAtomic, or NULL if not an Atomic::SimpleAtomic. To access the data consistently, 
	   consult first the #AtomicType of the Atomic. */
	//modelAtomic  *atomicData;     
	/*! Contains data associated with the Atomic #modelPort, or NULL if not an Atomic::InPort or Atomic::OutPort. To access the data consistently, 
	   consult first the #AtomicType of the Atomic. */
	//modelPort      *portData;
	/*! Returns the path of the icon of the Atomic. It's the same path that the image of the Atomic. */
	QString getIcon() {
		return myImage;
	};
	/*! Updates the Atomic icon. */
	void setIcon(QString icon);
	/*! Returns the direction of the Atomic. */
	int getDirection() {
		return myDirection;
	};
	/*! Falta ver que hace... */
	void setNoInteractive() {
		setFlag(QGraphicsItem::ItemIsMovable, false);
		myText->setTextInteractionFlags(Qt::NoTextInteraction);
		forUseInLibrary = true;
		setAcceptHoverEvents(true);
	};
	/*! This attribute contains the priority of the Atomic in the model. */
	//      int priority;
	virtual QList < modelParameter * >getParams() = 0;

	virtual QString getDescription() = 0;
	QColor intToColor(int i);
	int colorToInt(const QColor &color);

 signals:
	/*! This signal is emitted when changing the height of the Atomic to contain the number of added ports.
	   It is used in the Atomic editor (PowerDialogs) to update the value of width and height in the properties tab. */
	void heightChanged(int h);
	void showStatusBarHelp(QString *);
	void dragCopy(QPointF);

	public slots:
	    /*! Upates the name of the Atomic. */
	void setName(QString);
	/*! Changes Atomic direction, it calls rotate. */
	void setDirection(int d);
	/*! Updates the width of the Atomic. */
	void setWidth(int w);
	/*! Updates the height of the Atomic. */
	void setHeight(int h);

 protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant & value);
	//void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	QRectF boundingRect() const;
	//void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );              
	void mousePressEvent(QGraphicsSceneMouseEvent * event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
	void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
	void hoverEnterEvent(QGraphicsSceneHoverEvent * event);

 protected:
	void updateTextPosition();
	void initPort(GpxConnectionPoint::ConnectionPointType);
	void initName(QString namearg);
	void initDirection(int i);
	QPolygonF myPolygon;
	QColor myBackgroundColor;
	QList < GpxConnectionPoint * >inPorts, outPorts;
	GpxTextBox *myText;
	qreal width, height;
	int resizeCorner;
	int myDirection;
	QString name;
	QString myImage;
	bool mayBeDrag;
	bool mustRotateImage;
	bool forUseInLibrary;
};

#endif
