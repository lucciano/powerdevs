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

#ifndef PARAMETER_H
#define PARAMETER_H

#include <QString>
#include <QStringList>

typedef enum { STR, VAL, LST, SCILAB } parameterType;

class modelParameter {
 public:
	QString name;
	parameterType type;
	double dValue;
	QString strValue;
	int lsValue;
	QStringList lsValues;
	QString desc;
	QString typeToString() {
		if (type == STR)
			return "STRING";
		if (type == VAL)
			return "VALUE";
		if (type == LST)
			return "LIST";
		if (type == SCILAB)
			return "SCILAB";
		return "";
	} void setTypeFromString(QString strType) {
		if (strType == "STRING")
			type = STR;
		if (strType == "VALUE")
			type = VAL;
		if (strType == "LIST")
			type = LST;
		if (strType == "SCILAB")
			type = SCILAB;
	}

	modelParameter *duplicate() {
		modelParameter *p = new modelParameter();
		p->name = name;
		p->type = type;
		p->dValue = dValue;
		p->lsValue = lsValue;
		p->strValue = strValue;
		p->lsValues = lsValues;
		p->desc = desc;
		return p;
	}
};
#endif
