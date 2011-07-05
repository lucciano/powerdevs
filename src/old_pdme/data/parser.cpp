/****************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**		       Universidad Nacional de Rosario - Argentina.
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

#include "parser.h"

bool writeParameters(int depth, QList < modelParameter * >p);
bool writeGraphics(int depth, modelGraphics * g, bool forCoupled);
bool writeCoupled(int depth, modelCoupled * c);

int lineNumber;
QFile *fd;
QString getLine()
{
	lineNumber++;
	if (fd->atEnd()) {
		printf("ERROR: End of file reached\n");
		return QString("");
	}
	// Chequae fin de archivo!!!
	return QString(fd->readLine()).trimmed();
}

qint64 writeLine(QString s, int tab)
{
	return fd->write(TAB(tab) + s.toAscii() + "\n");
}

int checkEqual(QString s, QString t)
{
	if (s != t) {
		printf("Error: Expecting: '%s' found: '%s' at line %d\n",
		       QSTR(t), QSTR(s), lineNumber);
		return 0;
	}
	return 1;
}

void skipSection()
{
	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return;
	do {
		//printf("SKIP: %s\n",strLine.toAscii().constData());
		strLine = getLine();
	} while (strLine != TOKCBRACE);
}

QString getValue(QString s, QString t)
{
	if (!s.startsWith(t + " " + TOKEQUAL)) {
		printf("Error: Expecting: '%s' found: '%s' at line %d\n",
		       QSTR(t), QSTR(s), lineNumber);
		return NULL;
	}
	return s.mid(s.indexOf(TOKEQUAL) + 1).trimmed();
}

modelLine *parseLine()
{

	modelLine *ret = new modelLine();

	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;

	strLine = getLine();
	QString source = getValue(strLine, TOKSOURCE);

	strLine = getLine();
	QString sink = getValue(strLine, TOKSINK);

	strLine = getLine();
	QString pointxstr = getValue(strLine, TOKPOINTX);
	QStringList pointXls =
	    pointxstr.split(TOKCOLON, QString::SkipEmptyParts);
	QStringList::iterator i;
	QList < int >pointX;
	for (i = pointXls.begin(); i != pointXls.end(); ++i)
		pointX.append(i->toInt());
	ret->pointX = pointX;

	strLine = getLine();
	QString pointystr = getValue(strLine, TOKPOINTY);
	QStringList pointYls =
	    pointystr.split(TOKCOLON, QString::SkipEmptyParts);
	QList < int >pointY;
	for (i = pointYls.begin(); i != pointYls.end(); ++i)
		pointY.append(i->toInt());
	ret->pointY = pointY;

	strLine = getLine();
	if (!checkEqual(strLine, TOKCBRACE))
		return NULL;

	QStringList src = source.split(TOKCOLON, QString::SkipEmptyParts);
	ret->sourceType = src.takeFirst().trimmed();
	QList < int >sources;
	for (i = src.begin(); i != src.end(); ++i)
		sources.append(i->toInt());
	ret->sources = sources;

	src = sink.split(TOKCOLON, QString::SkipEmptyParts);
	ret->sinkType = src.takeFirst().trimmed();
	QList < int >sinks;
	for (i = src.begin(); i != src.end(); ++i)
		sinks.append(i->toInt());
	ret->sinks = sinks;
	return ret;

}

modelPoint *parsePoint()
{

	modelPoint *ret = new modelPoint();
	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;

	strLine = getLine();
	QString connectedlines = getValue(strLine, TOKCONECTEDLINES);

	strLine = getLine();
	QString connectedextrems = getValue(strLine, TOKCONECTEDEXTREMS);

	strLine = getLine();
	QStringList lposition = getValue(strLine, TOKPOSITION).split(TOKCOLON);
	ret->x = lposition.first().toInt();
	ret->y = lposition.at(1).toInt();

	strLine = getLine();
	if (!checkEqual(strLine, TOKCBRACE))
		return NULL;

	ret->extrems =
	    connectedextrems.split(TOKCOLON, QString::SkipEmptyParts);
	QStringList t = connectedlines.split(TOKCOLON, QString::SkipEmptyParts);
	QStringList::iterator i;
	QList < int >ls;
	for (i = t.begin(); i != t.end(); ++i)
		ls.append(i->toInt());
	ret->lines = ls;
	return ret;
}

QList < modelParameter * >parseParameters()
{
	bool ok;
	QList < modelParameter * >ret;
	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return QList < modelParameter * >();
	QString par;
	QString name, value, desc, type;
	QStringList lsn, lsv, lss;
	strLine = getLine();
	while (strLine != TOKCBRACE) {
		lsn << strLine.left(strLine.indexOf(TOKEQUAL))
		    << strLine.mid(strLine.indexOf(TOKEQUAL) + 2);
		name = lsn.first().trimmed();
		lsv = lsn.last().split(TOKCOLON);
		type = lsv.takeFirst().trimmed();
		value = lsv.takeFirst().trimmed();
		desc = lsv.takeFirst().trimmed();
		modelParameter *p = new modelParameter();
		p->name = name;
		if (type == TOKVAL) {
			p->type = VAL;
			p->dValue = value.toDouble(&ok);
			//printf("Parameter '%s' de tipo val valor %g\n",QSTR(name),p->dValue);
		}
		if (type == TOKSTR) {
			p->type = STR;
			p->strValue = value;
			//printf("Parameter '%s' de tipo str valor '%s'\n",QSTR(name),QSTR(p->strValue));
		}
		if (type == TOKLST) {
			QStringList qs =
			    value.split(TOKPERC, QString::SkipEmptyParts);
			p->type = LST;
			p->lsValue = qs.takeFirst().trimmed().toInt(&ok);
			p->lsValues = qs;
			//printf("Parameter '%s' de tipo lst valor '%d'\n",QSTR(name),p->lsValue);
			/*
			   QStringList::iterator i;
			   for (i=qs.begin();i!=qs.end();i++) {
			   QString t=*i;
			   printf("\t%s\n",QSTR(t));
			   }
			 */
		}
		p->desc = desc;
		ret.append(p);
		strLine = getLine();
		lsn.clear();
	}
	return ret;
}

modelGraphics *parseGraphics(bool forCoupled)
{

	modelGraphics *ret = new modelGraphics();
	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;

	strLine = getLine();
	QString position = getValue(strLine, TOKPOSITION);
	QStringList slpos = position.split(TOKCOLON, QString::SkipEmptyParts);
	ret->x = slpos.first().toInt();
	ret->y = slpos.at(1).toInt();

	strLine = getLine();
	QString dimension = getValue(strLine, TOKDIMENSION);
	QStringList sldim = dimension.split(TOKCOLON, QString::SkipEmptyParts);
	ret->width = sldim.first().toInt();
	ret->height = sldim.at(1).toInt();

	strLine = getLine();
	QString direction = getValue(strLine, TOKDIRECTION);
	QStringList l;
	l << TOKRIGHT << TOKDOWN << TOKLEFT << TOKUP;
	//qDebug("Direction '%s' indexof %d",QSTR(direction),l.indexOf(direction.trimmed()));
	ret->direction = l.indexOf(direction.trimmed());

	strLine = getLine();
	ret->color = getValue(strLine, TOKCOLOR).toInt();

	strLine = getLine();
	ret->icon = getValue(strLine, TOKICON);

	if (forCoupled) {

		strLine = getLine();
		// Parse window data
	}
	strLine = getLine();
	if (!checkEqual(strLine, TOKCBRACE))
		return NULL;

	return ret;
};

modelAtomic *parseAtomic()
{
	int inPorts, outPorts;
	bool ok;
	modelGraphics *graphics;
	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;

	strLine = getLine();
	QString name = getValue(strLine, TOKNAME);

	strLine = getLine();
	QString ports = getValue(strLine, TOKPORTS);
	QStringList slports = ports.split(TOKCOLON, QString::SkipEmptyParts);
	inPorts = slports.first().toInt(&ok);
	outPorts = slports.last().toInt(&ok);
	//printf("Atomic %s has %d inports and %d outports\n",name.toAscii().constData(),inPorts,outPorts);

	strLine = getLine();
	QString path = getValue(strLine, TOKPATH);

	strLine = getLine();
	QString desc = getValue(strLine, TOKDESCRIPTION);

	strLine = getLine();
	if (!checkEqual(strLine, TOKGRAPHIC))
		return NULL;
	graphics = parseGraphics(false);

	strLine = getLine();
	if (!checkEqual(strLine, TOKPARAMETERS))
		return NULL;
	QList < modelParameter * >params = parseParameters();

	strLine = getLine();
	if (!checkEqual(strLine, TOKCBRACE))
		return NULL;

	modelAtomic *ret = new modelAtomic();
	ret->graphics = graphics;
	ret->inPorts = inPorts;
	ret->outPorts = outPorts;
	ret->name = name;
	ret->path = path;
	ret->desc = desc;
	ret->params = params;
	return ret;
}

modelPort *parseInport()
{

	modelPort *ret = new modelPort();
	ret->type = INPORT;
	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;

	strLine = getLine();
	QString name = getValue(strLine, TOKNAME);

	strLine = getLine();
	QString coupledport = getValue(strLine, TOKCOUPLEDPORT);

	strLine = getLine();
	QString description = getValue(strLine, TOKDESCRIPTION);

	strLine = getLine();
	if (!checkEqual(strLine, TOKGRAPHIC))
		return NULL;

	strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;

	strLine = getLine();
	QString position = getValue(strLine, TOKPOSITION);
	ret->x = position.split(";").first().toInt();
	ret->y = position.split(";").last().toInt();

	strLine = getLine();
	QString dimension = getValue(strLine, TOKDIMENSION);
	ret->dimension = dimension.toInt();
	//qDebug("Dimension %d",ret->dimension);

	strLine = getLine();
	QString direction = getValue(strLine, TOKDIRECTION);
	QStringList l;
	l << TOKRIGHT << TOKDOWN << TOKLEFT << TOKUP;
	ret->direction = l.indexOf(direction.trimmed());

	strLine = getLine();
	if (!checkEqual(strLine, TOKCBRACE))
		return NULL;

	strLine = getLine();
	if (!checkEqual(strLine, TOKCBRACE))
		return NULL;
	ret->desc = description;
	ret->name = name;
	ret->coupledPort = coupledport.trimmed().toInt();
	return ret;
}

modelPort *parseOutport()
{
	modelPort *ret = new modelPort();

	ret->type = OUTPORT;
	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;

	strLine = getLine();
	QString name = getValue(strLine, TOKNAME);

	strLine = getLine();
	QString coupledport = getValue(strLine, TOKCOUPLEDPORT);

	strLine = getLine();
	QString description = getValue(strLine, TOKDESCRIPTION);

	strLine = getLine();
	if (!checkEqual(strLine, TOKGRAPHIC))
		return NULL;

	strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;

	strLine = getLine();
	QString position = getValue(strLine, TOKPOSITION);
	ret->x = position.split(";").first().toInt();
	ret->y = position.split(";").last().toInt();

	strLine = getLine();
	QString dimension = getValue(strLine, TOKDIMENSION);
	ret->dimension = dimension.toInt();

	strLine = getLine();
	QString direction = getValue(strLine, TOKDIRECTION);
	QStringList l;
	l << TOKRIGHT << TOKDOWN << TOKLEFT << TOKUP;
	ret->direction = l.indexOf(direction.trimmed());

	strLine = getLine();
	if (!checkEqual(strLine, TOKCBRACE))
		return NULL;

	strLine = getLine();
	if (!checkEqual(strLine, TOKCBRACE))
		return NULL;
	ret->name = name;
	ret->desc = description;
	ret->coupledPort = coupledport.trimmed().toInt();
	return ret;
}

bool writeAtomic(int depth, modelAtomic * a)
{

	writeLine(TOKATOMIC, depth);
	writeLine(TOKOBRACE, depth + 1);
	writeLine(TOKNAME + QString(" = ") + a->name, depth + 1);
	writeLine(TOKPORTS + QString(" = ") +
		  QString("%1 ; %2").arg(a->inPorts).arg(a->outPorts),
		  depth + 1);
	writeLine(TOKPATH + QString(" = ") + a->path, depth + 1);
	writeLine(TOKDESCRIPTION + QString(" = ") + a->desc, depth + 1);
	writeGraphics(depth + 1, a->graphics, false);
	writeParameters(depth + 1, a->params);
	writeLine(TOKCBRACE, depth + 1);
}

bool writeLines(int depth, modelLine * l)
{
	writeLine(TOKLINE, depth);
	writeLine(TOKOBRACE, depth + 1);
	QString src = " = ";
	src.append(l->sourceType + " ; ");
	QList < int >::iterator i;
	for (i = l->sources.begin(); i != l->sources.end(); i++) {
		src.append(QString("%1").arg(*i));
		if (i + 1 != l->sources.end())
			src.append(" ; ");
	}
	QString snk = " = ";
	snk.append(l->sinkType + " ; ");
	for (i = l->sinks.begin(); i != l->sinks.end(); i++) {
		snk.append(QString("%1").arg(*i));
		if (i + 1 != l->sinks.end())
			snk.append(" ; ");
	}
	QString px = " = ";
	for (i = l->pointX.begin(); i != l->pointX.end(); i++) {
		px.append(QString("%1").arg(*i));
		if (i + 1 != l->pointX.end())
			px.append(" ; ");
	}
	QString py = " = ";
	for (i = l->pointY.begin(); i != l->pointY.end(); i++) {
		py.append(QString("%1").arg(*i));
		if (i + 1 != l->pointY.end())
			py.append(" ; ");
	}
	writeLine(TOKSOURCE + src, depth + 1);
	writeLine(TOKSINK + snk, depth + 1);
	writeLine(TOKPOINTX + px, depth + 1);
	writeLine(TOKPOINTY + py, depth + 1);
	writeLine(TOKCBRACE, depth + 1);
}

bool writePoints(int depth, modelPoint * p)
{

	writeLine(TOKPOINT, depth);
	writeLine(TOKOBRACE, depth + 1);
	QString cl = " = ";
	foreach(int l, p->lines) {
		cl.append(QString("%1").arg(l));
		if (l != p->lines.last())
			cl.append(" ; ");
	}
	writeLine(TOKCONECTEDLINES + cl, depth + 1);
	writeLine(TOKCONECTEDEXTREMS +
		  QString(" = %1").arg(p->extrems.join(";")), depth + 1);
	writeLine(TOKPOSITION + QString(" = %1 ; %2 ").arg(p->x).arg(p->y),
		  depth + 1);
	writeLine(TOKCBRACE, depth + 1);
}

bool writePorts(int depth, modelPort * mp)
{
	QStringList l;
	l << TOKRIGHT << TOKDOWN << TOKLEFT << TOKUP;
	if (mp->type == INPORT)
		writeLine(TOKINPORT, depth);
	else
		writeLine(TOKOUTPORT, depth);
	writeLine(TOKOBRACE, depth + 1);
	writeLine(TOKNAME + QString(" = ") + mp->name, depth + 1);
	writeLine(TOKCOUPLEDPORT + QString(" = %1").arg(mp->coupledPort),
		  depth + 1);
	writeLine(TOKDESCRIPTION + QString(" = ") + mp->desc, depth + 1);
	writeLine(TOKGRAPHIC, depth + 1);
	writeLine(TOKOBRACE, depth + 2);
	writeLine(TOKPOSITION + QString(" = %1 ; %2").arg(mp->x).arg(mp->y),
		  depth + 2);
	writeLine(TOKDIMENSION + QString(" = %1").arg(mp->dimension),
		  depth + 2);
	writeLine(TOKDIRECTION + QString(" = ") + l.at(mp->direction),
		  depth + 2);
	writeLine(TOKCBRACE, depth + 2);
	writeLine(TOKCBRACE, depth + 1);
}

bool writeSystem(int depth, QList < modelChild * >c, QList < modelPoint * >p,
		 QList < modelLine * >l, QList < modelPort * >lp)
{

	writeLine(TOKSYSTEM, depth);
	writeLine(TOKOBRACE, depth + 1);
	foreach(modelChild * mc, c)
	    if (mc->childType == ATOMIC)
		writeAtomic(depth + 1, mc->atomic);
	else
		writeCoupled(depth + 1, mc->coupled);
	foreach(modelPort * mp, lp)
	    writePorts(depth + 1, mp);
	foreach(modelPoint * mp, p)
	    writePoints(depth + 1, mp);
	foreach(modelLine * ml, l)
	    writeLines(depth + 1, ml);
	writeLine(TOKCBRACE, depth + 1);
}

bool writeParameters(int depth, QList < modelParameter * >p)
{

	writeLine(TOKPARAMETERS, depth);
	writeLine(TOKOBRACE, depth + 1);
	foreach(modelParameter * mp, p) {
		QString s;
		s = mp->name + QString(" = ") + (mp->type ==
						 STR ? TOKSTR : (mp->type ==
								 LST ? TOKLST :
								 TOKVAL));
		if (mp->type == STR)
			s = s.append("; " + mp->strValue + " ; ");
		if (mp->type == VAL)
			s = s.append("; " + QString("%1").arg(mp->dValue) +
				     " ; ");
		if (mp->type == LST) {
			s = s.append("; " + QString("%1").arg(mp->lsValue) +
				     "\%");
			foreach(QString st, mp->lsValues)
			    s = s.append(st + "\%");
			s = s.append(" ; ");
		}
		s = s.append(mp->desc);
		writeLine(s, depth + 1);
	}
	writeLine(TOKCBRACE, depth + 1);
}

bool writeGraphics(int depth, modelGraphics * g, bool forCoupled)
{

	QStringList l;
	l << TOKRIGHT << TOKDOWN << TOKLEFT << TOKUP;
	if (g->direction<0 || g->direction>3)
		g->direction=0;

	writeLine(TOKGRAPHIC, depth);
	writeLine(TOKOBRACE, depth + 1);
	writeLine(TOKPOSITION + QString(" = %1 ; %2").arg(g->x).arg(g->y),
		  depth + 1);
	writeLine(TOKDIMENSION +
		  QString(" = %1 ; %2").arg(g->width).arg(g->height),
		  depth + 1);
	writeLine(TOKDIRECTION + QString(" = ") + l.at(g->direction),
		  depth + 1);
	writeLine(TOKCOLOR + QString(" = %1").arg(g->color), depth + 1);
	writeLine(TOKICON + QString(" = ") +
		  (g->icon.isEmpty()? TOKNONE : g->icon), depth + 1);
	if (forCoupled) {	// TODO
		writeLine(TOKWINDOW + QString(" = 5000; 5000; 5000; 5000 "),
			  depth + 1);
	}
	writeLine(TOKCBRACE, depth + 1);
}

bool writeCoupled(int depth, modelCoupled * c)
{
	writeLine(TOKCOUPLED, depth);
	writeLine(TOKOBRACE, depth + 1);
	writeLine(TOKTYPE + QString(" = ") + c->type, depth + 1);
	writeLine(TOKNAME + QString(" = ") + c->name, depth + 1);
	writeLine(TOKPORTS + QString(" = ") +
		  QString("%1; %2").arg(c->lsInPorts.count()).arg(c->lsOutPorts.count()),
		  depth + 1);
	writeLine(TOKDESCRIPTION + QString(" = ") + c->desc, depth + 1);
	writeGraphics(depth + 1, c->graphics, true);
	writeParameters(depth + 1, c->params);
	writeSystem(depth + 1, c->childs, c->points, c->lines, c->lsInPorts+c->lsOutPorts);
	writeLine(TOKCBRACE, depth + 1);

	return true;

}

modelCoupled *parseCoupled()
{
	bool ok;
	//int inPorts, outPorts;
	modelCoupled *ret = new modelCoupled();
	modelGraphics *graphics;
	QList < modelChild * >childs;
	QList < modelPoint * >points;
	QList < modelLine * >lines;
	//QList < modelPort * >lsPorts;
	QList < modelPort * >lsInPorts;
	QList < modelPort * >lsOutPorts;

	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;
	strLine = getLine();
	QString type = getValue(strLine, TOKTYPE);
	if (type == TOKROOT)
		ret->father = NULL;
	strLine = getLine();
	QString name = getValue(strLine, TOKNAME);
	//printf("Parsing a coupled named:%s of type %s\n", QSTR(name),QSTR(type));

	strLine = getLine();
	QString ports = getValue(strLine, TOKPORTS);
	//QStringList slports = ports.split(TOKCOLON, QString::SkipEmptyParts);
	//inPorts = slports.first().toInt(&ok);
	//outPorts = slports.last().toInt(&ok);

	strLine = getLine();
	QString desc = getValue(strLine, TOKDESCRIPTION);

	strLine = getLine();
	if (!checkEqual(strLine, TOKGRAPHIC))
		return NULL;
	graphics = parseGraphics(true);

	strLine = getLine();
	if (!checkEqual(strLine, TOKPARAMETERS))
		return NULL;
	QList < modelParameter * >params = parseParameters();

	strLine = getLine();
	if (!checkEqual(strLine, TOKSYSTEM))
		return NULL;
	strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;
	do {
		strLine = getLine();
		if (strLine == TOKATOMIC) {
			modelChild *c = new modelChild();
			c->childType = ATOMIC;
			c->atomic = parseAtomic();
			c->atomic->father = ret;
			childs.append(c);
		} else if (strLine == TOKCOUPLED) {
			modelChild *c = new modelChild();
			c->childType = COUPLED;
			c->coupled = parseCoupled();
			c->coupled->father = ret;
			childs.append(c);
		} else if (strLine == TOKINPORT) {
			lsInPorts.append(parseInport());
		} else if (strLine == TOKOUTPORT) {
			lsOutPorts.append(parseOutport());
		} else if (strLine == TOKPOINT) {
			points.append(parsePoint());
		} else if (strLine == TOKLINE) {
			lines.append(parseLine());
		} else if (strLine != TOKCBRACE) {	// Error
			return NULL;
		}
	} while (strLine != TOKCBRACE);
	strLine = getLine();
	if (!checkEqual(strLine, TOKCBRACE))
		return NULL;

	ret->name = name;
	ret->desc = desc;
	ret->graphics = graphics;
	ret->type = type;
	//ret->lsPorts = lsPorts;
	ret->lsInPorts = lsInPorts;
	ret->lsOutPorts = lsOutPorts;
	ret->params = params;
	ret->childs = childs;
	ret->points = points;
	ret->lines = lines;
	//ret->inPorts = inPorts;
	//ret->outPorts = outPorts;
	ret->owner = NULL;

	return ret;
}

modelCoupled *parseModel(QString filename)
{
	fd = new QFile(filename);
	if (!fd->open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug("Error opening file %s\n",
		       filename.toAscii().constData());
		return NULL;
	}
	QString strLine = getLine();
	if (!checkEqual(strLine, TOKCOUPLED))
		return NULL;
	modelCoupled *r = parseCoupled();
	fd->close();
	delete fd;
	return r;
}

modelAtomic *parseSimulator()
{

	modelAtomic *ret = new modelAtomic();

	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;

	strLine = getLine();
	ret->path = getValue(strLine, TOKPATH);

	strLine = getLine();
	//ret->paramsString=getValue(strLine, TOKPARAMETERS);

	strLine = getLine();
	if (!checkEqual(strLine, TOKCBRACE))
		return NULL;

	return ret;
}

QList < modelConnection * >parseConnections()
{
	QList < modelConnection * >ret;

	QString strLine = getLine();
	if (checkEqual(strLine, TOKOBRACE))
		return QList < modelConnection * >();

	strLine = getLine();
	while (strLine != TOKCBRACE) {
		QStringList lsLine = strLine.remove("(").remove(")").split(";");
		QStringList lsSource = lsLine.first().split(",");
		QStringList lsSink = lsLine.at(1).split(",");
		modelConnection *c = new modelConnection();
		c->childSource = lsSource.first().toInt();
		c->sourcePort = lsSource.at(1).toInt();
		c->childSink = lsSink.first().toInt();
		c->sinkPort = lsSink.at(1).toInt();
		ret.append(c);

		strLine = getLine();
	}

	if (!checkEqual(strLine, TOKCBRACE))
		return QList < modelConnection * >();
	return ret;
}

modelCoupled *parseCoordinator()
{
	modelCoupled *ret = new modelCoupled();

	QString strLine = getLine();
	if (!checkEqual(strLine, TOKOBRACE))
		return NULL;

	do {
		strLine = getLine();
		if (strLine.trimmed() == TOKSIMULATOR) {
			modelChild *c = new modelChild();
			c->childType = ATOMIC;
			c->atomic = parseSimulator();
			ret->childs.append(c);
		} else if (strLine.trimmed() == TOKCOORDINATOR) {
			modelChild *c = new modelChild();
			c->childType = COUPLED;
			c->coupled = parseCoordinator();
			ret->childs.append(c);
		} else if (strLine.trimmed() == TOKEIC) {
			QList < modelConnection * >l = parseConnections();
			ret->lsEIC = l;
		} else if (strLine.trimmed() == TOKEOC) {
			QList < modelConnection * >l = parseConnections();
			ret->lsEOC = l;
		} else if (strLine.trimmed() == TOKIC) {
			QList < modelConnection * >l = parseConnections();
			ret->lsIC = l;
		} else if (strLine != TOKCBRACE) {
			printf("Expecting {%s, %s, %s, %s, %s}: found %s\n",
			       TOKSIMULATOR, TOKCOORDINATOR, TOKEIC, TOKEOC,
			       TOKIC, QSTR(strLine));
			exit(-1);
		}
	} while (strLine != TOKCBRACE);

	return ret;
}

modelCoupled *parsePDS(QString filename)
{
	fd = new QFile(filename);
	if (!filename.endsWith(".pds", Qt::CaseInsensitive)) {
		printf("File must be either a .pdm or .pds\n");
		exit(-1);
	}
	if (!fd->open(QIODevice::ReadOnly | QIODevice::Text)) {
		printf("Error opening file %s\n",
		       filename.toAscii().constData());
		exit(-1);
	}
	QString strLine = getLine();
	if (!checkEqual(strLine, TOKROOTCOORD))
		return NULL;
	modelCoupled *r = parseCoordinator();
	fd->close();
	return r;
}

bool saveModelToFile(modelCoupled * c, QString filename)
{
	qDebug("Saving model to file %s", qPrintable(filename));
	fd = new QFile(filename);
	if (!fd->open(QIODevice::WriteOnly | QIODevice::Text)) {
		return false;
	}
	writeCoupled(0, c);
	fd->close();
	return true;
}
