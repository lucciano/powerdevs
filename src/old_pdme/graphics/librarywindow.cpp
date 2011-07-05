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

#include "QToolBox"
#include "QDockWidget"
#include "QGraphicsView"
#include "QGraphicsScene"
#include "QGraphicsItem"
#include "librarywindow.h"
#include "gpxblock.h"

 LibraryWindow::LibraryWindow(QString title, QWidget * parent):QDockWidget(title,
	    parent)
{
	setFeatures(DockWidgetMovable | DockWidgetFloatable);

}

void LibraryWindow::resizeEvent(QResizeEvent * e)
{
	QToolBox *toolBox = (QToolBox *) widget();
	QGraphicsView *v;
	QGraphicsScene *s;
	qreal libWidth = width() - 20;
	for (int i = 0; i < toolBox->count(); i++) {
		v = (QGraphicsView *) toolBox->widget(i);
		s = v->scene();
		qreal x = 0, y = 0, maxHeight = 0;
		foreach(QGraphicsItem * a, s->items()) {
			GpxBlock *atomic = qgraphicsitem_cast < GpxBlock * >(a);
			if (atomic == NULL)
				continue;
			qreal w = atomic->getWidth();
			if (x + w + 10 > libWidth) {
				x = 0;
				y += maxHeight;
				maxHeight = 0;
			}
			atomic->setPos(x + w / 2, y + 20);
			x += w;
			maxHeight =
			    (atomic->getHeight() >
			     maxHeight ? atomic->getHeight() : maxHeight);
		}
		s->setSceneRect(-10, -10, x + 10, y + 10);
	}

}
