/***************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**		  Universidad Nacional de Rosario - Argentina.
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
#include <QDebug>
#include "codegenerator.h"
#include "modelcoupled.h"

#define TAB(N) QSTR(QString("").fill(' ',2*N))
#define CHILDNAME(chld) ((chld)->childType==ATOMIC ? chld->atomic->name.trimmed() : chld->coupled->name.trimmed())

QSet < QString > includes;
QSet < QString > objects;
QStringList constructor;
QStringList state;
QStringList makefile;
int currentCoupled;
QFile *fdPds;

QString getRelativePath(QString p)
{
	return p.trimmed().replace("\\", "/").toLower();
}

QString getBaseFilename(QString s)
{
	return getRelativePath(s.left(s.lastIndexOf(".")));
}

QString getClassName(QString p)
{
	QString l = p.left(p.indexOf("."));
	int slash = p.lastIndexOf("/") + 1;
	int bslash = p.lastIndexOf("\\") + 1;
	int m = (slash > bslash ? slash : bslash);
	return l.mid(m);
}

void printConnection(modelLine * i, modelCoupled * c)
{
	printf("Connection from ");
	if ((i)->sourceType == TOKCMP) {
		printf("%s(%d) (%s:%d) to ",
		       QSTR(CHILDNAME(c->childs.at((i)->sources.first() - 1))),
		       (i)->sources.first(),
		       ((i)->sources.at(2) != 0 ? "INPUT" : "OUTPUT"),
		       (i)->sources.at(1) - 1);
	}
	if ((i)->sourceType == TOKPRT) {
		printf("%s (%s:%d) to ", "Port",
		       (c->lsPorts.at(i->sources.at(0) - 1)->type ==
			INPORT ? "INPUT" : "OUTPUT"),
		       c->lsPorts.at(i->sources.at(0) - 1)->coupledPort - 1);
	}
	if ((i)->sourceType == TOKPNT) {
		printf("Point %d to ", (i)->sources.at(0));
	}

	if ((i)->sinkType == TOKCMP) {
		printf("%s(%d) (%s:%d) to ",
		       QSTR(CHILDNAME(c->childs.at((i)->sinks.first() - 1))),
		       (i)->sinks.first(),
		       ((i)->sinks.at(2) != 0 ? "INPUT" : "OUTPUT"),
		       (i)->sinks.at(1) - 1);
	}
	if ((i)->sinkType == TOKPRT) {
		printf("%s (%s:%d) to ", "Port",
		       (c->lsPorts.at(i->sinks.at(0) - 1)->type ==
			INPORT ? "INPUT" : "OUTPUT"),
		       c->lsPorts.at(i->sinks.at(0) - 1)->coupledPort - 1);

	}
	if ((i)->sinkType == TOKPNT) {
		printf("Point %d", (i)->sinks.at(0));
	}
	printf("\n");
}

void computeConnections(modelCoupled * c)
{
	QList < modelLine * >::iterator i, o, d;
	modelPoint *p;
	modelPoint *n;
	QList < int >::iterator j, k;
	modelLine *lk, *lj;
	QList < modelLine * >l;
	int pointNumber = 1;
	while (c->points.size() != 0) {
		p = c->points.first();
		for (j = p->lines.begin(); j != p->lines.end(); j++) {
			for (k = j + 1; k != p->lines.end(); k++) {
				lj = c->lines.at(*j - 1);
				lk = c->lines.at(*k - 1);
				if (lj->sourceType == TOKPNT
				    && lj->sources.first() == pointNumber) {
					if (lk->sinkType == TOKPNT
					    && lk->sinks.first() ==
					    pointNumber) {
						modelLine *nl = new modelLine();
						nl->sourceType = lj->sinkType;
						nl->sources = lj->sinks;
						nl->sinkType = lk->sourceType;
						nl->sinks = lk->sources;
						c->lines.append(nl);
						if (nl->sinkType == TOKPNT) {
							int pointDest = nl->sinks.first();
							if (pointDest >
							    pointNumber) {
								n = c->points.  at(pointDest - pointNumber);
								n->lines.  append(c-> lines.  size ());
							}
						}
						if (nl->sourceType == TOKPNT) {
							int pointDest = nl->sources.first();
							if (pointDest > pointNumber) {
								n = c->points.  at(pointDest - pointNumber);
								n->lines.  append(c-> lines.  size ());
							}
						}
					}
				}
				if (lj->sourceType == TOKPNT
				    && lj->sources.first() == pointNumber) {
					if (lk->sourceType == TOKPNT
					    && lk->sources.first() ==
					    pointNumber) {
						modelLine *nl = new modelLine();
						nl->sourceType = lj->sinkType;
						nl->sources = lj->sinks;
						nl->sinkType = lk->sinkType;
						nl->sinks = lk->sinks;
						c->lines.append(nl);
						if (nl->sinkType == TOKPNT) {
							int pointDest = nl->sinks.first();
							if (pointDest > pointNumber) {
								n = c->points.at(pointDest - pointNumber);
								n->lines.  append(c-> lines.  size ());
							}
						}
						if (nl->sourceType == TOKPNT) {
							int pointDest = nl->sources.first();
							if (pointDest >
							    pointNumber) {
								n = c->points.at(pointDest - pointNumber);
								n->lines.append(c->lines.size());
							}
						}
					}
				}
				if (lj->sinkType == TOKPNT
				    && lj->sinks.first() == pointNumber) {
					if (lk->sourceType == TOKPNT
					    && lk->sources.first() ==
					    pointNumber) {
						modelLine *nl = new modelLine();
						nl->sourceType = lj->sourceType;
						nl->sources = lj->sources;
						nl->sinkType = lk->sinkType;
						nl->sinks = lk->sinks;
						c->lines.append(nl);
						if (nl->sinkType == TOKPNT) {
							int pointDest = nl->sinks.first();
							if (pointDest >
							    pointNumber) {
								n = c->points.at(pointDest - pointNumber);
								n->lines.append(c->lines.size ());
							}
						}
						if (nl->sourceType == TOKPNT) {
							int pointDest = nl->sources.first();
							if (pointDest >
							    pointNumber) {
								n = c->points.at(pointDest - pointNumber);
								n->lines.append(c->lines.size ());
							}
						}
					}
				}
				if (lj->sinkType == TOKPNT
				    && lj->sinks.first() == pointNumber) {
					if (lk->sinkType == TOKPNT
					    && lk->sinks.first() ==
					    pointNumber) {
						modelLine *nl = new modelLine();
						nl->sourceType = lj->sourceType;
						nl->sources = lj->sources;
						nl->sinkType = lk->sourceType;
						nl->sinks = lk->sources;
						c->lines.append(nl);
						if (nl->sinkType == TOKPNT) {
							int pointDest = nl->sinks.first();
							if (pointDest > pointNumber) {
								n = c->points.  at(pointDest - pointNumber);
								n->lines.  append(c-> lines.  size ());
							}
						}
						if (nl->sourceType == TOKPNT) {
							int pointDest = nl->sources.first();
							if (pointDest >
							    pointNumber) {
								n = c->points.  at(pointDest - pointNumber);
								n->lines.  append(c-> lines.  size ());
							}
						}
					}
				}
			}
		}
		pointNumber++;
		c->points.removeFirst();
	}
	QList < modelLine * >removedLines;
	for (i = c->lines.begin(); i != c->lines.end(); i++) {	// Remove repeated lines and check for feedbacks
		bool repeated = false;
		if (((*i)->sourceType == TOKCMP) &&
		    ((*i)->sinkType == TOKCMP) &&
		    ((*i)->sources.first() == (*i)->sinks.first())) {
			int in = (*i)->sources.at(2);
			int out = (*i)->sinks.at(2);
			if (in != out) {
				//((*i)->sources.at(2) != (*i)->sinks.at(2))  // Theres feedback
				int child = (*i)->sources.first() - 1;
				modelChild *mc = c->childs.at(child);
        char buff[1024];
				printf ("There is a direct feedback in the component '%s'\n", QSTR(mc->childType == ATOMIC ? mc->atomic->name : mc-> coupled->name));
				sprintf (buff,"There is a direct feedback in the component '%s'\n", QSTR(mc->childType == ATOMIC ? mc->atomic->name : mc-> coupled->name));
        QMessageBox::critical(NULL,"PowerDEVS",buff);
				exit(-1);
			}
		}

		for (d = i + 1; d != c->lines.end(); d++) {
			if (((*i)->sourceType == (*d)->sourceType) &&
			    ((*i)->sources == (*d)->sources) &&
			    ((*i)->sinkType == (*d)->sinkType) &&
			    ((*i)->sinks == (*d)->sinks)) {
				repeated = true;
			}
		}
		if ((*i)->sinkType == TOKCMP && (*i)->sourceType == TOKCMP
		    && ((*i)->sources.at(2) == (*i)->sinks.at(2))) {
			continue;
		}
		if ((*i)->sinkType == TOKPRT &&
		    (*i)->sourceType == TOKCMP &&
		    ((*i)->sources.at(2) != 0) &&
		    (c->lsPorts.at((*i)->sinks.first() - 1)->type != INPORT)) {
			continue;
		}
		if ((*i)->sourceType == TOKPRT &&
		    (*i)->sinkType == TOKCMP &&
		    ((*i)->sinks.at(2) != 0) &&
		    (c->lsPorts.at((*i)->sources.first() - 1)->type !=
		     INPORT)) {
			continue;
		}
		if ((*i)->sinkType == TOKPNT || (*i)->sourceType == TOKPNT) {
			continue;
		}
		if (!repeated)
			removedLines << (*i);
	}
	c->lines = removedLines;

	for (i = c->lines.begin(); i != c->lines.end(); i++) {
		if (((*i)->sourceType == TOKCMP && (*i)->sources.at(2) != 0) || ((*i)->sourceType == TOKPRT && c->lsPorts.at((*i)->sources.first() - 1)->type != INPORT)) {	// Swtich order
			QString t;
			QList < int >tt;
			t = (*i)->sourceType;
			tt = (*i)->sources;
			(*i)->sourceType = (*i)->sinkType;
			(*i)->sources = (*i)->sinks;
			(*i)->sinkType = t;
			(*i)->sinks = tt;
		}
	}
}

void processConnections(modelCoupled * c)
{
	QList < modelLine * >::iterator i;
	for (i = c->lines.begin(); i != c->lines.end(); i++) {
		if ((*i)->sinkType == TOKCMP && (*i)->sourceType == TOKCMP)	// IC Connection
		{
			if ((*i)->sources.at(2) == (*i)->sinks.at(2)) {
				continue;
			}
			modelConnection *con = new modelConnection();
			con->childSource = (*i)->sources.first() - 1;
			con->sourcePort = (*i)->sources.at(1) - 1;
			con->childSink = (*i)->sinks.first() - 1;
			con->sinkPort = (*i)->sinks.at(1) - 1;
			c->lsIC.append(con);
		}
		if ((*i)->sinkType == TOKCMP && (*i)->sourceType == TOKPRT)	// EIC
		{
			modelConnection *con = new modelConnection();
			con->childSource = 0;
			con->sourcePort =
			    c->lsPorts.at((*i)->sources.first() -
					  1)->coupledPort - 1;
			con->childSink = (*i)->sinks.first() - 1;
			con->sinkPort = (*i)->sinks.at(1) - 1;
			c->lsEIC.append(con);
		}
		if ((*i)->sinkType == TOKPRT && (*i)->sourceType == TOKCMP) {
			modelConnection *con = new modelConnection();
			con->childSource = 0;
			con->childSource = (*i)->sources.first() - 1;
			con->sourcePort = (*i)->sources.at(1) - 1;
			con->childSink = 0;
			con->sinkPort =
			    c->lsPorts.at((*i)->sinks.first() -
					  1)->coupledPort - 1;
			c->lsEOC.append(con);
		}
	}
}

modelParameter *getParameter(QString par, modelCoupled * c)
{
	QList < modelParameter * >::iterator j;
	//qDebug("Searching arg '%s' in %s\n",QSTR(par),QSTR(c->name));
	for (j = c->params.begin(); j != c->params.end(); j++) {
		//      qDebug("Comparing against '%s'\n",QSTR((*j)->name));
		if ((*j)->name.trimmed() == par.trimmed()) {
			if ((*j)->type == STR && (*j)->strValue.startsWith("%")) {
				return getParameter((*j)->strValue.mid(1),
						    c->father);
			}
			return (*j);
		}
	}
	printf("Parameter %s not found in model\n", QSTR(par));
	exit(-1);
}

QString getParameterString(QList < modelParameter * >params, modelAtomic * a)
{

	QString ret;
	QList < modelParameter * >::iterator i;
	for (i = params.begin(); i != params.end(); ++i) {

		//printf("Evaluating parameter %s of atomic %s\n", QSTR((*i)->strValue),QSTR(a->name));
		switch ((*i)->type) {
		case STR:
		case SCILAB:
			if ((*i)->strValue == "%Inputs") {
				ret += QString("%1").arg((double)a->inPorts, 0, 'e');
			} else if ((*i)->strValue == "%Outputs") {
				ret += QString("%1").arg((double)a->inPorts, 0, 'e');
			} else {
				if ((*i)->strValue.startsWith("%")) {
					QString par = (*i)->strValue.mid(1);
					modelParameter *j = getParameter(par, a->father);
					switch (j->type) {
					case STR:
					case SCILAB:
						ret += "\"" + j->strValue + "\"";
						break;
					case VAL:
						ret += QString("%1").arg(j->dValue, 0, 'e');
						break;
					case LST:
						ret += "\"" + (j->lsValues.  at(j->lsValue - 1)) + "\"";
						break;
					}
				} else {
					ret += "\"" + (*i)->strValue + "\"";
				}
			}
			break;
		case VAL:
			ret += QString("%1").arg((*i)->dValue, 0, 'e');
			break;
		case LST:
			//printf("Valor de lista %d %d\n",(*i)->lsValues.size(), (*i)->lsValue);
			ret +=
			    "\"" + (*i)->lsValues.at((*i)->lsValue - 1) + "\"";
			break;
		}
		if (i + 1 != params.end())
			ret += ",";
	}
	//printf("Returning '%s'\n",QSTR(ret));
	return ret;
}

QString getExtraDep(QString at)
{
	QString ret = "";
	QString path = QCoreApplication::applicationDirPath();
	QString file = getRelativePath(path + QString("/../atomics/") + at + ".h");
  //qDebug() << "Getting extra deps for "<< file;
  char buff[1024];

	QFile fd(file);
	if (fd.open(QIODevice::ReadOnly | QIODevice::Text)) {
    while (fd.readLine(buff,1024)>0) {
      QString line(buff);
      if (line.startsWith("//cpp:",Qt::CaseInsensitive)) {
        if (!line.contains(getClassName(at)))
          ret +=  " $(ATOMICS)/" + getBaseFilename(line.mid(6)) + ".cpp";
      }
      if (line.startsWith("#include",Qt::CaseInsensitive)) {
        QString header = line.mid(8).trimmed().mid(1);
        header = header.left(header.length()-1);
	      QFile hfile(getRelativePath(path + QString("/../atomics/") + header));
        if (hfile.exists())
        {
          ret +=  " $(ATOMICS)/" + header;
          //qDebug() << "Header->" << header;
        }
      }
		}
		fd.close();
	}

	return ret;

}
QSet < QString > getExtraObjs(QString at)
{

	QSet < QString > ret;
	QString path = QCoreApplication::applicationDirPath();
	QString file = getRelativePath(path + QString("/../atomics/") + at);

	QFile fd(file);
	if (fd.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QString line = QString(fd.readLine());
		while (!line.startsWith("#")) {
			line = getRelativePath(line);
			if (line.startsWith("//cpp:")
			    && !line.contains(getClassName(at))) {
				objects.insert(getBaseFilename(line.mid(6)) + ".o");
			}
			line = QString(fd.readLine());
		}
		fd.close();
	}

	return ret;
}

void processChilds(QList < modelChild * >childs, int depth)
{
	int idCoupled = currentCoupled;
	QList < modelChild * >::iterator i;
	int currentChild = 0;
	for (i = childs.begin(); i != childs.end(); ++i, currentChild++) {
		if ((*i)->childType == ATOMIC) {	// Atomic
			if ((*i)->atomic->path.isEmpty()) {
			QMessageBox msgBox;
			msgBox.setText (QString("Error: The atomic %1 has no implementation associated.").arg((*i)->atomic->name));
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.setWindowTitle("PowerDEVS");
			msgBox.exec();
			exit(-1);
			
			}
			includes.insert(getRelativePath ((*i)->atomic->path.toLower()));
			objects.insert(getBaseFilename((*i)->atomic->path) + ".o");
			getExtraObjs((*i)->atomic->path.toLower());
			constructor << QString("%1//D%2[%3] is  %4\n").
			    arg(TAB(depth)).arg(idCoupled).arg(currentChild).
			    arg((*i)->atomic->name.trimmed());
			constructor << QString("%1D%2[%3] = new %4(\"%5\");\n").
			    arg(TAB(depth)).arg(idCoupled).arg(currentChild).
			    arg(getClassName((*i)->atomic->path)).arg((*i)->
								      atomic->
								      name.
								      trimmed
								      ());

			if ((*i)->atomic->paramsString.trimmed() != "") {
				constructor <<
				    QString("%1D%2[%3]->init(0.0,%4);\n").
				    arg(TAB(depth)).arg(idCoupled).
				    arg(currentChild).arg((*i)->atomic->
							  paramsString);
			} else if ((*i)->atomic->params.size() != 0) {
				constructor <<
				    QString("%1D%2[%3]->init(0.0,%4);\n").
				    arg(TAB(depth)).arg(idCoupled).
				    arg(currentChild).arg(getParameterString
							  ((*i)->atomic->params,
							   (*i)->atomic));
			} else {
				constructor <<
				    QString("%1D%2[%3]->init(0.0);\n").
				    arg(TAB(depth)).arg(idCoupled).
				    arg(currentChild);
			}
		} else {	// Coupled
			int childCoupled = ++currentCoupled;
			constructor << QString("%1//D%2[%3] is %4\n").
			    arg(TAB(depth)).arg(idCoupled).arg(currentChild).
			    arg((*i)->coupled->name);
			//constructor<< QString("%1D%2[%3] = new Coupling();\n").arg(TAB(depth)).arg(currentCoupled).arg(currentChild);
			constructor <<
			    QString("%1D%2[%3] = new Coupling(\"%4\");\n").
			    arg(TAB(depth)).arg(idCoupled).arg(currentChild).
			    arg((*i)->coupled->name);
			constructor <<
			    QString
			    ("%1Simulator **D%2 = new Simulator* [%3];\n").
			    arg(TAB(depth + 1)).arg(childCoupled).arg((*i)->
								      coupled->
								      childs.
								      size());

			constructor <<
			    QString
			    ("%1Connection **EIC%2 = new Connection* [%3];\n").
			    arg(TAB(depth + 1)).arg(childCoupled).arg((*i)->
								      coupled->
								      lsEIC.
								      size());
			constructor <<
			    QString
			    ("%1Connection **EOC%2 = new Connection* [%3];\n").
			    arg(TAB(depth + 1)).arg(childCoupled).arg((*i)->
								      coupled->
								      lsEOC.
								      size());
			constructor <<
			    QString
			    ("%1Connection **IC%2 = new Connection* [%3];\n").
			    arg(TAB(depth + 1)).arg(childCoupled).arg((*i)->
								      coupled->
								      lsIC.
								      size());
			processChilds((*i)->coupled->childs, depth + 1);
			QList < modelConnection * >::iterator l;
			int count = 0;
			for (l = (*i)->coupled->lsEIC.begin();
			     l != (*i)->coupled->lsEIC.end(); l++, count++) {
				constructor <<
				    QString
				    ("%1EIC%2[%3] = new Connection();\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count);
				constructor <<
				    QString
				    ("%1EIC%2[%3]->setup(%4,%5,%6,%7);\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count).arg((*l)->childSource).arg((*l)->
									  sourcePort).arg
				    ((*l)->childSink).arg((*l)->sinkPort);
			};
			count = 0;
			for (l = (*i)->coupled->lsEOC.begin();
			     l != (*i)->coupled->lsEOC.end(); l++, count++) {
				constructor <<
				    QString
				    ("%1EOC%2[%3] = new Connection();\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count);
				constructor <<
				    QString
				    ("%1EOC%2[%3]->setup(%4,%5,%6,%7);\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count).arg((*l)->childSource).arg((*l)->
									  sourcePort).arg
				    ((*l)->childSink).arg((*l)->sinkPort);
			};
			count = 0;
			for (l = (*i)->coupled->lsIC.begin();
			     l != (*i)->coupled->lsIC.end(); l++, count++) {
				constructor <<
				    QString("%1IC%2[%3] = new Connection();\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count);
				constructor <<
				    QString
				    ("%1IC%2[%3]->setup(%4,%5,%6,%7);\n").
				    arg(TAB(depth + 1)).arg(childCoupled).
				    arg(count).arg((*l)->childSource).arg((*l)->
									  sourcePort).arg
				    ((*l)->childSink).arg((*l)->sinkPort);
			}
			constructor <<
			    QString
			    ("%1((Coupling*)D%2[%3])->setup(D%4,%5,IC%6,%7,EIC%8,%9,EOC%10,%11);\n").
			    arg(TAB(depth)).arg(idCoupled).arg(currentChild).arg
			    (childCoupled).arg((*i)->coupled->childs.
					       size()).arg(childCoupled).arg((*i)->coupled->lsIC.size()).arg(childCoupled).arg((*i)->coupled->lsEIC.size()).arg(childCoupled).arg((*i)->coupled->lsEOC.size());
			constructor <<
			    QString("%1((Coupling*)D%2[%3])->init(ti);\n").
			    arg(TAB(depth)).arg(idCoupled).arg(currentChild);
		}
	}

}

void generatePDS(modelCoupled * c, int depth)
{

//      qDebug("Generating Coupled %s",QSTR(c->name));
	computeConnections(c);
	processConnections(c);
//      qDebug("Connections ready %s",QSTR(c->name));

	char buff[100000];
	if (c->type.trimmed() == TOKROOT) {
		sprintf(buff, "Root-Coordinator\n");
		fdPds->write(buff, strlen(buff));
	} else {
		sprintf(buff, "%sCoordinator\n", TAB(depth));
		fdPds->write(buff, strlen(buff));
	}
	sprintf(buff, "%s{\n", TAB(depth + 1));
	fdPds->write(buff, strlen(buff));
	QList < modelChild * >::iterator i;
	for (i = c->childs.begin(); i != c->childs.end(); i++) {
		if ((*i)->childType == ATOMIC) {
			sprintf(buff, "%sSimulator\n", TAB(depth + 2));
			fdPds->write(buff, strlen(buff));
			sprintf(buff, "%s{\n", TAB(depth + 3));
			fdPds->write(buff, strlen(buff));
			sprintf(buff, "%sPath = %s\n", TAB(depth + 4),
				QSTR((*i)->atomic->path.toLower()));
			fdPds->write(buff, strlen(buff));
			sprintf(buff, "%sParameters = %s\n", TAB(depth + 4),
				QSTR(getParameterString
				     ((*i)->atomic->params, (*i)->atomic)));
			fdPds->write(buff, strlen(buff));
			sprintf(buff, "%s}\n", TAB(depth + 3));
			fdPds->write(buff, strlen(buff));
		} else {
			generatePDS((*i)->coupled, depth + 2);
		}
	}
	QList < modelConnection * >::iterator con;
	sprintf(buff, "%sEIC\n", TAB(depth + 2));
	fdPds->write(buff, strlen(buff));
	sprintf(buff, "%s{\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));
	for (con = c->lsEIC.begin(); con != c->lsEIC.end(); con++) {
		sprintf(buff, "%s(%d,%d);(%d,%d)\n", TAB(depth + 4),
			(*con)->childSource, (*con)->sourcePort,
			(*con)->childSink, (*con)->sinkPort);
		fdPds->write(buff, strlen(buff));
	}
	sprintf(buff, "%s}\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));

	sprintf(buff, "%sEOC\n", TAB(depth + 2));
	fdPds->write(buff, strlen(buff));
	sprintf(buff, "%s{\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));
	for (con = c->lsEOC.begin(); con != c->lsEOC.end(); con++) {
		sprintf(buff, "%s(%d,%d);(%d,%d)\n", TAB(depth + 4),
			(*con)->childSource, (*con)->sourcePort,
			(*con)->childSink, (*con)->sinkPort);
		fdPds->write(buff, strlen(buff));
	}
	sprintf(buff, "%s}\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));

	sprintf(buff, "%sIC\n", TAB(depth + 2));
	fdPds->write(buff, strlen(buff));
	sprintf(buff, "%s{\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));
	for (con = c->lsIC.begin(); con != c->lsIC.end(); con++) {
		sprintf(buff, "%s(%d,%d);(%d,%d)\n", TAB(depth + 4),
			(*con)->childSource, (*con)->sourcePort,
			(*con)->childSink, (*con)->sinkPort);
		fdPds->write(buff, strlen(buff));
	}
	sprintf(buff, "%s}\n", TAB(depth + 3));
	fdPds->write(buff, strlen(buff));

	sprintf(buff, "%s}\n", TAB(depth + 1));
	fdPds->write(buff, strlen(buff));

	//qDebug("Coupled %s finished",QSTR(c->name));
}

void generateModel(modelCoupled * c)
{
	currentCoupled = 0;
	constructor << QString("class Model: public RootSimulator {\n");
	constructor << QString("%1public:\n").arg(TAB(1));
	constructor <<
	    QString("%1Model (Time ti): RootSimulator((char*)\"%2\") {\n").
	    arg(TAB(2)).arg(c->name);

	constructor <<
	    QString("%1mainCoupling = new RootCoupling((char*)\"%2\");\n").
	    arg(TAB(3)).arg(c->name);
	constructor << QString("%1Simulator **D%2 = new Simulator* [%3];\n").
	    arg(TAB(3)).arg(currentCoupled).arg(c->childs.size());
	constructor << QString("%1Connection **IC%2 = new Connection* [%3];\n").
	    arg(TAB(3)).arg(currentCoupled).arg(c->lsIC.size());

	processChilds(c->childs, 3);
	QList < modelConnection * >::iterator l;
	int count = 0;
	for (l = c->lsIC.begin(); l != c->lsIC.end(); l++, count++) {
		constructor << QString("%1IC%2[%3] = new Connection();\n").
		    arg(TAB(3)).arg(0).arg(count);
		constructor << QString("%1IC%2[%3]->setup(%4,%5,%6,%7);\n").
		    arg(TAB(3)).arg(0).arg(count).arg((*l)->
						      childSource).arg((*l)->
								       sourcePort).arg
		    ((*l)->childSink).arg((*l)->sinkPort);
	}

	constructor << QString("%1mainCoupling->setup(D%2,%3,IC%4,%5);\n").
	    arg(TAB(3)).arg(0).arg(c->childs.size()).arg(0).arg(c->lsIC.size());
	//constructor<< QString("%1mainCoupling->init(ti);\n").arg(TAB(3));
	constructor <<
	    QString("%1((RootCoupling*)mainCoupling)->rootSim = this;\n").
	    arg(TAB(3));
	constructor << QString("%1}\n").arg(TAB(2));
	constructor << QString("%1};\n").arg(TAB(1));

	QStringList::iterator i;
	QSet < QString >::iterator j;
	includes << "engine.h";

	QString path = QCoreApplication::applicationDirPath();
	QFile *fd = new QFile(path + QString("/../build/model.h"));
	//QFile *fd = new QFile("./model.h");
	fd->open(QIODevice::WriteOnly | QIODevice::Text);

	for (j = includes.begin(); j != includes.end(); j++) {
		QString w = QSTR(QString("#include <%1>\n").arg(*j));
		fd->write(QSTR(w), w.size());
	}
	for (i = constructor.begin(); i != constructor.end(); i++) {
		fd->write(QSTR(*i), (*i).size());
	}
	fd->close();
	delete fd;

	fd = new QFile(path + "/../build/Makefile.include");
	fd->open(QIODevice::WriteOnly | QIODevice::Text);
	QString reqModel;
	QString reqModelPath;
	for (j = objects.begin(); j != objects.end(); j++) {
		QString w =
		    QString
		    ("$(BUILDOBJ)/%1.o: $(ATOMICS)/%2.cpp $(ATOMICS)/%3.h %4\n\t$(CXX) -c $(CXXFLAGS) $(INCLUDES) $(ATOMICS)/%5.cpp -o $(BUILDOBJ)/%6.o %7\n");
		QString basefile = getClassName((*j));
		reqModel = reqModel + QString(" %1.o ").arg(basefile);
		w = w.arg(basefile);
		basefile = getBaseFilename((*j));
		w = w.arg(basefile);
		w = w.arg(basefile);
		w = w.arg(getExtraDep(getBaseFilename((*j))));	// Extra cpp 
		w = w.arg(basefile);
		basefile = getClassName((*j));
		w = w.arg(basefile);
		w = w.arg("");	// Extra cpp
		reqModel = reqModel + QString(" %1.o ").arg(basefile);
		reqModelPath =
		    reqModelPath + QString(" $(BUILDOBJ)/%1.o ").arg(basefile);
		fd->write(QSTR(w), w.size());
	}
	//reqModelPath += " $(BUILDOBJ)/connection.o ";
	//reqModelPath += " $(BUILDOBJ)/coupling.o ";
	//reqModelPath += " $(BUILDOBJ)/event.o ";
	//reqModelPath += " $(BUILDOBJ)/simulator.o ";
	//reqModelPath += " $(BUILDOBJ)/root_simulator.o ";
	//reqModelPath += " $(BUILDOBJ)/root_coupling.o ";
	//reqModelPath += " $(BUILDOBJ)/pdevslib.o ";
	QString w =
	    QString
	    ("$(MODEL): $(SRCENGINE)/model.cpp $(BUILD)/model.h $(BUILDOBJ)/libsimpd.a %1 %2 \n\t$(CXX) $(CXXFLAGS) $(INCLUDES) %3 %4 $(SRCENGINE)/model.cpp $(LIBS) -o $(MODEL)\n");
	w = w.arg(reqModelPath);
	w = w.arg("");		// Extra objs y libs
	w = w.arg(reqModelPath);
	w = w.arg("");		// Extra objs y libs
	fd->write(QSTR(w), w.size());

	fd->close();

}

int generateCode(modelCoupled * c, QString pdsName, bool generateCCode,
		 bool genPDS)
{
	if (genPDS) {
		fdPds = new QFile(pdsName);
		fdPds->
		    open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::
			 Truncate);
		generatePDS(c, 0);
		fdPds->close();
	}
	if (generateCCode) {
		generateModel(c);
	}
	return 0;
}
