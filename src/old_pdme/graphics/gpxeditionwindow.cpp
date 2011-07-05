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

#include <QMessageBox>
#include <QCloseEvent>
#include <QGraphicsView>
#include <QLabel>
#include "gpxeditionwindow.h"
#include "gpxeditionscene.h"
#include "powergui.h"
#include "parser.h"
#include "gpxinport.h"
#include "gpxoutport.h"

GpxEditionWindow::GpxEditionWindow(modelCoupled * d)
{
	setTabsClosable(true);
	addTab(d);
	setTabPosition(QTabWidget::South);
	dirty = false;
	setWindowTitle(d->name + (dirty ? " *" : ""));
	connect(this, SIGNAL(tabCloseRequested(int)), this,
		SLOT(closeTab(int)));
}

GpxEditionWindow::~GpxEditionWindow()
{
	for (int i = 0; i < count(); i++) {
		QGraphicsView *gv = (QGraphicsView *) widget(i);
		GpxEditionScene *es = (GpxEditionScene *) gv->scene();
		es->clear();
		delete gv;
		delete es;
	}
}

void GpxEditionWindow::closeEvent(QCloseEvent * event)
{
	GpxEditionScene *mc =
	    (GpxEditionScene *) ((QGraphicsView *) widget(0))->scene();
	PowerGui *p = (PowerGui *) mainWindow;
	if (isDirty()) {
		int ret = p->saveModel(this, mc->getData());
		if (ret == -1) {
			event->ignore();
			return;
		}
	}
	qDebug("Cerrando modelo");
	event->accept();
	emit closeEditionWindow(this);
}

int GpxEditionWindow::addTab(modelCoupled * d, GpxCoupled * gc)
{
	GpxEditionScene *ges = new GpxEditionScene(this, d);
	QGraphicsView *gv = new QGraphicsView(ges);
	if (gc != NULL){
		ges->setOwner(gc);
		foreach(modelChild *mch, gc->getData()->childs){
			if(mch->childType == COUPLED){
				int scenes = count();
				for (int i = scenes - 1; i >= 0; i--) {
					QGraphicsView *gv = (QGraphicsView *) widget(i);
					GpxEditionScene *es = (GpxEditionScene *) gv->scene();
					if (es->getFather() == mch->coupled->father) {
						es->setOwner(qgraphicsitem_cast<GpxCoupled *>(mch->coupled->owner));
						break;
					}		 
				}
			}	
		}
	}	
	gv->setInteractive(true);
	gv->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	gv->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	//gv->setSize(gv->scene()->width() + 30, gv->scene()->height() + 30);
	if (count()) {
		QString name = d->name;
		modelCoupled *c = d->father;
		while (c != NULL) {
			name.prepend("\\");
			name.prepend(c->name);
			qDebug("Agregando %s en la lista", qPrintable(c->name));
			c = c->father;
		}
		connect(ges, SIGNAL(dirtyChanged(bool)), this,
			SLOT(dirtyChanged(bool)));
		int t = QTabWidget::addTab(gv, name);
		gc->setOpenEditionScene(ges);
		gc->setOpen(t);
		return t;
	} else {
		first = ges;
		connect(ges, SIGNAL(dirtyChanged(bool)), this,
			SLOT(dirtyChanged(bool)));
		int t = QTabWidget::addTab(gv, ges->getName());
		return t;

	}
}

void GpxEditionWindow::dirtyChanged(bool d)
{
	dirty = d;
	setWindowTitle(first->getName() + (dirty ? " *" : ""));
}

void GpxEditionWindow::closeTab(int index)
{
	if (index == 0) {
		for (int i = 0; i < count(); i++) {
			QGraphicsView *gv = (QGraphicsView *) widget(index);
			GpxEditionScene *es = (GpxEditionScene *) gv->scene();
			es->updateConnections();
		}
		close();
	} else {
		QGraphicsView *gv = (QGraphicsView *) widget(index);
		GpxEditionScene *es = (GpxEditionScene *) gv->scene();
		//if (es->isDeleted()) {
		//	es->clear();
		//	removeTab(index);
		//} else {
			es->updateConnections();
			es->closeScene();
			es->updateOwners();
			es->clear();
			removeTab(index);
		//}
	}
}

modelCoupled *GpxEditionWindow::getData()
{
	for (int i = 0; i < count(); i++) {
		QGraphicsView *gv = (QGraphicsView *) widget(i);
		GpxEditionScene *es = (GpxEditionScene *) gv->scene();
		es->updateConnections();
	}
	QGraphicsView *gv = (QGraphicsView *) widget(0);
	GpxEditionScene *es = (GpxEditionScene *) gv->scene();
	return es->getData();
}

void GpxEditionWindow::setName(QString n)
{
	QGraphicsView *gv = (QGraphicsView *) widget(0);
	GpxEditionScene *es = (GpxEditionScene *) gv->scene();
	es->coupledData->name = n;
	setTabText(0, n);

}

modelCoupled *GpxEditionWindow::getSelectedData()
{
	QGraphicsView *gv = (QGraphicsView *) currentWidget();
	GpxEditionScene *es = (GpxEditionScene *) gv->scene();
	return es->createModelDataFromSelection();
}

void GpxEditionWindow::removeBlock(GpxBlock * b)
{
	QGraphicsView *gv = (QGraphicsView *) currentWidget();
	GpxEditionScene *es = (GpxEditionScene *) gv->scene();
	if (b->blockType()==GpxBlock::Coupled) 
	{
	   GpxCoupled *c=qgraphicsitem_cast<GpxCoupled*>(b);
	   deleteScenes(c->getData());
	   /*if (c->openedIn()>0) {               
	   GpxEditionScene *es=c->getOpenEditionScene();
	   foreach (QGraphicsItem *i,es->items()) {
	   if (i->type()==GpxBlock::Type) {
	   es->removeBlock((GpxBlock*)i);
	   }
	   }
	   closeTab(c->openedIn());
	   }*/
	} 
	es->removeBlock(b);

}

void GpxEditionWindow::updateTabs(modelCoupled *mc)
{
	int scenes = count();
	for (int i = scenes - 1; i >= 0; i--) {
		QGraphicsView *gv = (QGraphicsView *) widget(i);
		GpxEditionScene *es = (GpxEditionScene *) gv->scene();
		if (es->getFather() == mc->father) {
			closeTab(i);
			break;
		} 
	}
}

void GpxEditionWindow::deleteScenes(modelCoupled *mc)
{
	foreach(modelChild *mch, mc->childs){
		if(mch->childType == COUPLED){
			deleteScenes(mch->coupled);
			updateTabs(mch->coupled);
		}
	}
	updateTabs(mc);
}


QUndoStack *GpxEditionWindow::getUndoStack() 
{
	 
   GpxEditionScene *es = (GpxEditionScene*)((QGraphicsView*)currentWidget())->scene();
   return es->getUndoStack();
}
