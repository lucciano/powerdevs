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

#include "libraryscene.h"
#include "gpxatomic.h"
#include "gpxcoupled.h"
#include "gpxinport.h"
#include "gpxoutport.h"
#include "powergui.h"

 LibraryScene::LibraryScene(QObject * parent, modelCoupled * c):QGraphicsScene
    (parent)
{
	mc = c;
	QSettings settings(QCoreApplication::applicationDirPath() +
			   "/powerdevs.ini", QSettings::IniFormat);
	QString pathLib = settings.value("Path/libPath").toString();
	foreach(modelChild * child, c->childs) {
		if (child->childType == ATOMIC) {
			QString icon = child->atomic->graphics->icon;
			if (icon == "None")
				icon = "";
			if (icon.startsWith("\%")) {
				icon =
				    pathLib + "/" + icon.mid(1).replace("\%",
									"/").
				    toLower();
			}
			child->atomic->graphics->icon = icon;
			GpxAtomic *atomic =
			    new GpxAtomic(NULL, this, child->atomic);
			//connect(atomic,SIGNAL(showStatusBarHelp(QString *)),this,SLOT(setStatusBar(QString *)));
			atomic->setFlag(QGraphicsItem::ItemIsMovable, false);
			atomic->setNoInteractive();
		} else {
			QString icon = child->coupled->graphics->icon;
			if (icon == "None")
				icon = "";
			if (icon.startsWith("\%")) {
				icon =
				    pathLib + "/" + icon.mid(1).replace("\%",
									"/").
				    toLower();
			}
			child->coupled->graphics->icon = icon;
			GpxCoupled *coupled =
			    new GpxCoupled(NULL, this, child->coupled);
			coupled->setNoInteractive();
		}
	}
	foreach(modelPort * mpo, c->lsInPorts) {
			mpo->coupledPort -= 1;
			GpxInport *port = new GpxInport(NULL, this, mpo);
			port->setNoInteractive();
			port->setFlag(QGraphicsItem::ItemIsMovable, false);
	}
	foreach(modelPort * mpo, c->lsOutPorts) {
			mpo->coupledPort -= 1;
			GpxOutport *port = new GpxOutport(NULL, this, mpo);
			port->setNoInteractive();
			port->setFlag(QGraphicsItem::ItemIsMovable, false);
	}
	QString icon = c->graphics->icon;
	if (icon == "None")
		icon = "";
	if (icon.startsWith("\%")) {
		icon = pathLib + "/" + icon.mid(1).replace("\%", "/").toLower();
	}

}

LibraryScene::~LibraryScene()
{
	delete mc;
}

void LibraryScene::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	if (event->button() != Qt::LeftButton) {
		event->ignore();
		return;
	}

	QList < QGraphicsItem * >startItems = items(event->scenePos());
	if (!startItems.isEmpty() && startItems.first() != NULL &&
	    startItems.first()->type() == GpxBlock::Type) {

		GpxBlock *item =
		    qgraphicsitem_cast < GpxBlock * >(startItems.first());
		PowerGui *p = (PowerGui *) parent();
		QMimeData *mimeData = new QMimeData;
		if (item->blockType() == GpxBlock::SimpleAtomic) {
			p->fromLibAtomic =
			    (qgraphicsitem_cast <
			     GpxAtomic * >(item))->getData();
			mimeData->setText(QString("FromLibAtomic"));
		} else if (item->blockType() == GpxBlock::Coupled) {
			p->fromLibCoupled =
			    (qgraphicsitem_cast <
			     GpxCoupled * >(item))->getData();
			mimeData->setText(QString("FromLibCoupled"));
		} else if (item->blockType() == GpxBlock::InPort) {
			p->fromLibPort =
			    (qgraphicsitem_cast <
			     GpxInport * >(item))->getData();
			mimeData->setText(QString("FromLibInPort"));
		} else if (item->blockType() == GpxBlock::OutPort) {
			p->fromLibPort =
			    (qgraphicsitem_cast <
			     GpxOutport * >(item))->getData();
			mimeData->setText(QString("FromLibOutPort"));
		}
		QDrag *drag = new QDrag(event->widget());
		drag->setMimeData(mimeData);
		drag->setPixmap(item->image());
		drag->setHotSpot(QPoint(15, 30));
		drag->start();
	} else if (startItems.count() == 2
		   && startItems.value(1)->type() == GpxBlock::Type) {
		GpxBlock *item =
		    qgraphicsitem_cast < GpxBlock * >(startItems.value(1));
		PowerGui *p = (PowerGui *) parent();
		QMimeData *mimeData = new QMimeData;
		if (item->blockType() == GpxBlock::InPort) {
			p->fromLibPort =
			    (qgraphicsitem_cast <
			     GpxInport * >(item))->getData();
			mimeData->setText(QString("FromLibInPort"));
		} else if (item->blockType() == GpxBlock::OutPort) {
			p->fromLibPort =
			    (qgraphicsitem_cast <
			     GpxOutport * >(item))->getData();
			mimeData->setText(QString("FromLibOutPort"));
		}
		QDrag *drag = new QDrag(event->widget());
		drag->setMimeData(mimeData);
		drag->setPixmap(item->image());
		drag->setHotSpot(QPoint(15, 30));
		drag->start();
	}
}
