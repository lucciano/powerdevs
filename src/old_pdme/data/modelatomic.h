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

#ifndef MODELATOMIC_H
#define MODELATOMIC_H

#include <QString>
#include <QList>
#include "modelparameter.h"
#include "modelcoupled.h"

class modelGraphics {
 public:
	int x;
	int y;
	int width;
	int height;
	int direction;
	int color;
	QString icon;
	modelGraphics *duplicate() {
		modelGraphics *g = new modelGraphics();
		 g->x = x;
		 g->y = y;
		 g->width = width;
		 g->height = height;
		 g->direction = direction;
		 g->color = color;
		 g->icon = icon;
		 return g;
}};

class modelAtomic {
 public:
	int inPorts;
	int outPorts;
	int priority;
	QString name;
	QString path;
	QString desc;
	 QList < modelParameter * >params;
	class modelCoupled *father;
	modelGraphics *graphics;
	modelAtomic *duplicate() {
		modelAtomic *d = new modelAtomic();
		 d->inPorts = inPorts;
		 d->outPorts = outPorts;
		 d->name = name;
		 d->path = path;
		 d->desc = desc;
		 d->priority = priority;
		 d->father = father;
		 foreach(modelParameter * p, params) {
			d->params.append(p->duplicate());
		} d->graphics = graphics->duplicate();
		return d;
	};
	~modelAtomic() {
		delete graphics;
		foreach(modelParameter * p, params)
		delete p;
	}
};
#endif
