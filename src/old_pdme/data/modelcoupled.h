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

#ifndef COUPLED_H
#define COUPLED_H

#include "modelatomic.h"
#include "modelparameter.h"
class GpxBlock;

typedef enum { INPORT, OUTPORT } typePort;

typedef enum { ATOMIC, COUPLED } modelType;
class modelPoint {
 public:
	QList < int >lines;
	QStringList extrems;
	int x, y;
	bool valid;
	modelPoint *duplicate() {
		modelPoint *ret = new modelPoint();
		 ret->lines = lines;
		 ret->x = x;
		 ret->y = y;
		 ret->valid = valid;
		 return ret;
}};

class modelLine {
 public:
	QString sourceType;
	QList < int >sources;
	QString sinkType;
	 QList < int >sinks;
	 QList < int >pointX;
	 QList < int >pointY;
	modelLine *duplicate() {
		modelLine *r = new modelLine();
		 r->sourceType = sourceType;
		 r->sources = sources;
		 r->sinkType = sinkType;
		 r->sinks = sinks;
		 r->pointX = pointX;
		 r->pointY = pointY;
		 return r;
}};

class modelConnection {
 public:
	int childSource;
	int sourcePort;
	int childSink;
	int sinkPort;
	modelConnection *duplicate() {
		modelConnection *r = new modelConnection();
		 r->childSource = childSource;
		 r->sourcePort = sourcePort;
		 r->childSink = childSink;
		 r->sinkPort = sinkPort;
		 return r;
}};

class modelPort {
 public:
	~modelPort() {
		//qDebug("Releasing port %s", qPrintable(name));
	};
	QString name;
	int coupledPort;
	typePort type;
	QString desc;
	int direction;
	int x, y;
	int dimension;
	GpxBlock *owner;
	modelPort *duplicate();
};

class modelChild;

class modelCoupled {
 public:
	~modelCoupled();
	QString name;
	QString type;
	//int inPorts;
	//int outPorts;
	int priority;
	QString desc;
	modelCoupled *father;
	QList < modelParameter * >params;
	QList < modelChild * >childs;
	QList < modelPoint * >points;
	QList < modelLine * >lines;
	//QList < modelPort * >lsPorts;
	QList < modelPort * >lsInPorts;
	QList < modelPort * >lsOutPorts;
	QList < modelConnection * >lsIC;
	QList < modelConnection * >lsEIC;
	QList < modelConnection * >lsEOC;
	modelGraphics *graphics;
	GpxBlock *owner;
	modelCoupled *duplicate();
	void removeInport(int i);
	void removeOutport(int i);
	void removeConnectionsToInport(modelCoupled *,int);
	void removeConnectionsToOutport(modelCoupled *,int);
	void swap(int index);
};
class modelChild {
 public:
	modelType childType;
	class modelAtomic *atomic;
	class modelCoupled *coupled;
	modelChild *duplicate();
	int getPriority();
	void setPriority(int p);
	~modelChild();
};

#endif
