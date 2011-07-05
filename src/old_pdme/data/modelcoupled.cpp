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

#include <QDebug>
#include "modelcoupled.h"
#include "gpxeditionscene.h"

modelCoupled *modelCoupled::duplicate()
{
	modelCoupled *r = new modelCoupled();
	r->name = name;
	r->type = type;
	//r->inPorts = inPorts;
	r->priority = priority;
	//r->outPorts = outPorts;
	r->desc = desc;
	r->father = father;
	r->owner = owner;
	foreach(modelParameter * p, params)
	    r->params.append(p->duplicate());
	foreach(modelChild * c, childs)
	    r->childs.append(c->duplicate());
	foreach(modelPoint * p, points)
	    r->points.append(p->duplicate());
	foreach(modelLine * l, lines)
	    r->lines.append(l->duplicate());
	//foreach(modelPort * p, lsPorts)
	//    r->lsPorts.append(p->duplicate());
	foreach(modelPort * p, lsInPorts)
	    r->lsInPorts.append(p->duplicate());
	foreach(modelPort * p, lsOutPorts)
	    r->lsOutPorts.append(p->duplicate());

	foreach(modelConnection * c, lsIC)
	    r->lsIC.append(c->duplicate());

	foreach(modelConnection * c, lsEIC)
	    r->lsEIC.append(c->duplicate());

	foreach(modelConnection * c, lsEOC)
	    r->lsEOC.append(c->duplicate());
	r->graphics = graphics->duplicate();
	return r;

}

modelChild *modelChild::duplicate()
{
	modelChild *r = new modelChild();
	r->childType = childType;
	if (childType == ATOMIC) {
		r->atomic = atomic->duplicate();
		r->coupled = NULL;
	} else {
		r->coupled = coupled->duplicate();
		r->atomic = NULL;
	}
	return r;
}

int modelChild::getPriority()
{
	if (this->childType == ATOMIC) {
		return this->atomic->priority;
	} else {
		return this->coupled->priority;
	}
}

void modelChild::setPriority(int p)
{
	if (this->childType == ATOMIC) {
		this->atomic->priority = p;
	} else {
		this->coupled->priority = p;
	}
}

modelChild::~modelChild()
{
	if (childType == ATOMIC) {
		//qDebug("Releasing atomic %s", qPrintable(atomic->name));
		delete atomic;
	} else {
		//qDebug("Releasing coupled %s", qPrintable(coupled->name));
		delete coupled;
	}

}

modelPort *modelPort::duplicate()
{
	modelPort *p = new modelPort();
	p->name = name;
	p->coupledPort = coupledPort;
	p->type = type;
	p->desc = desc;
	p->direction = direction;
	p->x = x;
	p->y = y;
	p->dimension = dimension;
	qDebug("Direction %d", p->direction);
	return p;
}

modelCoupled::~modelCoupled()
{
	foreach(modelParameter * p, params)
	delete p;
	foreach(modelChild * c, childs)
	delete c;
	foreach(modelPoint * p, points)
	delete p;
	foreach(modelLine * l, lines)
	delete l;
	//foreach(modelPort * p, lsPorts)
	//delete p;
	foreach(modelPort * p, lsInPorts)
	delete p;
	foreach(modelPort * p, lsOutPorts)
	delete p;
	foreach(modelConnection * c, lsIC)
	delete c;
	foreach(modelConnection * c, lsEIC)
	delete c;
	foreach(modelConnection * c, lsEOC)
	delete c;
	/*if ( owner != NULL){
	   GpxCoupled *c = qgraphicsitem_cast<GpxCoupled *>(owner);
	   if (c->getOpenEditionScene() != NULL){
	   GpxEditionScene *es = c->getOpenEditionScene();
	   qDebug("setea la escena de %s",qPrintable(c->getName()));
	   es->setDeleted();
	   }
	   }else {
	   qDebug("No tiene owner!!!");
	   } */
}

void modelCoupled::removeInport(int i)
{
	if (father==NULL) return;
	//inPorts--;
	father->removeConnectionsToInport(this,i);
}

void modelCoupled::removeOutport(int i)
{
	if (father==NULL) return;
	//outPorts--;
	father->removeConnectionsToOutport(this,i);
}

void modelCoupled::removeConnectionsToInport(modelCoupled *child, int port)
{
	int c=0;
	foreach (modelChild *mc, childs) {
		if (mc->childType==COUPLED && mc->coupled==child)
			break;
		c++;
	}
	QList<modelLine *> deleteLines;
	QList<int> deleteLinesNumber;
        int ln=1;
	foreach(modelLine *ml, lines) {
		if ((ml->sourceType==TOKCMP && ml->sources.at(0)==c+1 && ml->sources.at(1)==port+1 && ml->sources.at(2)==1) ||
			(ml->sinkType==TOKCMP && ml->sinks.at(0)==c+1 && ml->sinks.at(1)==port+1 && ml->sinks.at(2)==1)) {
			deleteLines << ml;
			deleteLinesNumber << ln;
		}
		ln++;
	}
	int i=0;
	foreach(modelLine *ml, deleteLines) {
		int lineToDelete = deleteLinesNumber.at(i);
		QList <modelPoint *> pointsToDelete;
		foreach(modelPoint *mp, points) {
			for(int l=0;l<mp->lines.count();l++) {
				if (mp->lines.at(l)>lineToDelete) {
					mp->lines.replace(l,mp->lines.at(l)-1);
				} else if (mp->lines.at(l) == lineToDelete && mp->lines.size()==3) {
					qDebug("Simplifify!");
					int pointToSimplify=points.indexOf(mp);
					mp->lines.removeOne(lineToDelete);
					int l1=mp->lines.at(0)-1,l2=mp->lines.at(1)-1;
					if (lines.at(l1)->sourceType==TOKPNT && lines.at(l1)->sources.at(0)==pointToSimplify) { // L1 leaves the point to simplify --> Swap 
						swap(l1);
					}
					if (lines.at(l2)->sinkType==TOKPNT && lines.at(l2)->sinks.at(0)==pointToSimplify) { // L2 goes to the point to simplify --> Swap 
						swap(l2);
					}
		
					lines.at(l1)->pointX+= lines.at(l2)->pointX;		
					lines.at(l1)->pointY+= lines.at(l2)->pointY;		
					lines.at(l1)->sinkType=lines.at(l2)->sinkType;
					lines.at(l1)->sinks=lines.at(l2)->sinks;
                    lines.removeAt(l2);
                    // Refresh if the new line goes to a point
                    pointsToDelete << mp;
				}
			}
		}
        lines.removeOne(ml);
        foreach(modelPoint *mp, pointsToDelete)
			points.removeOne(mp);
		delete ml;
		i++;
	}
	qDebug() << deleteLines;
}

/*void modelCoupled::removeConnectionsToInport(modelCoupled *child, int port)
{
	int c=0;
	foreach (modelChild *mc, childs) {
		if (mc->childType==COUPLED && mc->coupled==child)
			break;
		c++;
	}
	modelLine deleteLine;
	int deleteLineNumber;
    int ln=1;
	foreach(modelLine *ml, lines) {
		if ((ml->sourceType==TOKCMP && ml->sources.at(0)==c+1 && ml->sources.at(1)==port+1 && ml->sources.at(2)==1) ||
			(ml->sinkType==TOKCMP && ml->sinks.at(0)==c+1 && ml->sinks.at(1)==port+1 && ml->sinks.at(2)==1)) {
			deleteLine = ml;
			deleteLineNumber = ln;
			break;
		}
		ln++;
	}
	int i=0;
	//foreach(modelLine *ml, deleteLines) {
		//int lineToDelete = deleteLinesNumber.at(i);
	int pointToDelete = 1;
	 Search the point to be deleted 
	for( pointToDelete = 1; pointToDelete < points.count(); pointToDelete++) {
		if(points.at(pointToDelete)->lines.contains(deleteLineNumber))
			
			for(int l=0;l<mp->lines.count();l++) {
				if (mp->lines.at(l)>lineToDelete) {
					mp->lines.replace(l,mp->lines.at(l)-1);
				} else if (mp->lines.at(l) == lineToDelete && mp->lines.size()==3) {
					qDebug("Simplifify!");
					int pointToSimplify=points.indexOf(mp);
					mp->lines.removeOne(lineToDelete);
					int l1=mp->lines.at(0)-1,l2=mp->lines.at(1)-1;
					if (lines.at(l1)->sourceType==TOKPNT && lines.at(l1)->sources.at(0)==pointToSimplify) { // L1 leaves the point to simplify --> Swap 
						swap(l1);
					}
					if (lines.at(l2)->sinkType==TOKPNT && lines.at(l2)->sinks.at(0)==pointToSimplify) { // L2 goes to the point to simplify --> Swap 
						swap(l2);
					}
		
					lines.at(l1)->pointX+= lines.at(l2)->pointX;		
					lines.at(l1)->pointY+= lines.at(l2)->pointY;		
					lines.at(l1)->sinkType=lines.at(l2)->sinkType;
					lines.at(l1)->sinks=lines.at(l2)->sinks;
                    lines.removeAt(l2);
                    // Refresh if the new line goes to a point
                    pointsToDelete << mp;
				}
			}
		}
        lines.removeOne(ml);
        foreach(modelPoint *mp, pointsToDelete)
			points.removeOne(mp);
		delete ml;
		i++;
	}
	qDebug() << deleteLines;
}*/

void modelCoupled::swap(int index)
{
	modelLine *tempLine = lines.at(index)->duplicate();
	lines.at(index)->sinkType = tempLine->sourceType;
	lines.at(index)->sinks = tempLine->sources;
	lines.at(index)->sourceType = tempLine->sinkType;
	lines.at(index)->sources = tempLine->sinks;
	delete tempLine;
}

void modelCoupled::removeConnectionsToOutport(modelCoupled *child, int port)
{
	int c=0;
	foreach (modelChild *mc, childs) {
		if (mc->childType==COUPLED && mc->coupled==child)
			break;
		c++;
	}
	QList<modelLine *> deleteLines;
	QList<int> deleteLinesNumber;
        int ln=1;
	foreach(modelLine *ml, lines) {
		if ((ml->sourceType==TOKCMP && ml->sources.at(0)==c+1 && ml->sources.at(1)==port+1 && ml->sources.at(2)==1) ||
			(ml->sinkType==TOKCMP && ml->sinks.at(0)==c+1 && ml->sinks.at(1)==port+1 && ml->sinks.at(2)==1)) {
			deleteLines << ml;
			deleteLinesNumber <<ln;
		}
		ln++;
	}
	int i=0;
	foreach(modelLine *ml, deleteLines) {
		int lineToDelete = deleteLinesNumber.at(i);
		QList <modelPoint *> pointsToDelete;
		foreach(modelPoint *mp, points) {
			for(int l=0;l<mp->lines.count();l++) {
				if (mp->lines.at(l)>lineToDelete) {
					mp->lines.replace(l,mp->lines.at(l)-1);
				} else if (mp->lines.at(l) == lineToDelete && mp->lines.size()==3) {
					qDebug("Simplifify!");
					int pointToSimplify=points.indexOf(mp);
					mp->lines.removeOne(lineToDelete);
					int l1=mp->lines.at(0)-1,l2=mp->lines.at(1)-1;
					if (lines.at(l1)->sourceType==TOKPNT && lines.at(l1)->sources.at(0)==pointToSimplify) { // L1 leaves the point to simplify --> Swap 
						modelLine *tempLine = lines.at(l1)->duplicate();
						lines.at(l1)->sinkType = tempLine->sourceType;
						lines.at(l1)->sinks = tempLine->sources;
						lines.at(l1)->sourceType = tempLine->sinkType;
						lines.at(l1)->sources = tempLine->sinks;
						delete tempLine;
					}
					if (lines.at(l2)->sinkType==TOKPNT && lines.at(l2)->sinks.at(0)==pointToSimplify) { // L2 goes to the point to simplify --> Swap 
						modelLine *tempLine = lines.at(l2)->duplicate();
						lines.at(l2)->sinkType = tempLine->sourceType;
						lines.at(l2)->sinks = tempLine->sources;
						lines.at(l2)->sourceType = tempLine->sinkType;
						lines.at(l2)->sources = tempLine->sinks;
						delete tempLine;
					}
		
					lines.at(l1)->pointX+= lines.at(l2)->pointX;		
					lines.at(l1)->pointY+= lines.at(l2)->pointY;		
					lines.at(l1)->sinkType=lines.at(l2)->sinkType;
					lines.at(l1)->sinks=lines.at(l2)->sinks;
                                        lines.removeAt(l2);
                                        // Refresh if the new line goes to a point
                                        pointsToDelete << mp;
				}
			}
		}
                lines.removeOne(ml);
                foreach(modelPoint *mp, pointsToDelete)
			points.removeOne(mp);
		delete ml;
		i++;
	}
	qDebug() << deleteLines;
}
