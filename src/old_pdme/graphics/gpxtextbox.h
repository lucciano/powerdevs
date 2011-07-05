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

/*! \brief Class GpxTextBox.
 *
 * A text box for the Power Devs. It's used by the graphics elements
 * an also as stand alone graphic element.
*/
#ifndef GPXTEXTBOX_H
#define GPXTEXTBOX_H

#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
class QFocusEvent;
class QGraphicsItem;
class QGraphicsScene;
class QLineEdit;

class GpxTextBox:public QGraphicsTextItem {
 Q_OBJECT public:
	enum { Type = UserType + 3 };	/*!< Enum value. */

	/*! Constructor of the class. */
	 GpxTextBox(QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);

	/*! Method that returns the type of the class. */
	int type() const {
		return Type;
	} void paint(QPainter * painter,
		     const QStyleOptionGraphicsItem * option, QWidget * widget =
		     0);
	 signals:void lostFocus(GpxTextBox * item);
	void selectedChange(QGraphicsItem * item);

 protected:
	void focusInEvent(QFocusEvent * event);
	void focusOutEvent(QFocusEvent * event);
	void keyPressEvent(QKeyEvent * event);
	//void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	QVariant itemChange(GraphicsItemChange change, const QVariant & value);
 private:
	 bool editing;
};

#endif
