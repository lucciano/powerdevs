/****************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**  Universidad Nacional de Rosario - Argentina.
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

#include <stdio.h>
#include <stdlib.h>
#include <QProcess>
#include <QFile>
#include <QFSFileEngine>
#include <QMessageBox>
#include <QDebug>
#include <QPushButton>

#include "parser.h"
#include "modelcoupled.h"
#include "codegenerator.h"

void showLog();
int main(int argc, char **argv)
{

	QApplication app(argc, argv);
	if (argc < 2) {
		printf("Usage: pdppt [options] file\n");
		printf("	Options =  \n");
		printf("	           -r Only generate structure file\n");
		printf ("	           -m Generate structure and generate code\n");
		printf ("	           -x Generate structure, code and run\n");
		printf ("	           -s Generate structure, code and run silent\n");
		printf ("	file can be a model (pdm) or a model structure(pds)\n");
		return 0;
	}
	// Parse command line args
	QString filename;
	bool generateCCode = true;
	bool silent = false;
	bool runSimulation = false;
	if (argc == 2) {
		filename = QString(argv[1]);
	}
	if (argc == 3) {
		QString opt = QString(argv[1]);
		if (opt.trimmed() == "-r") {
			generateCCode = false;
		} else if (opt.trimmed() == "-x") {
			runSimulation = true;
		} else if (opt.trimmed() == "-m") {
			runSimulation = false;
		} else if (opt.trimmed() == "-s") {
			silent = true;
			runSimulation = true;
		} else {
			printf("Unrecongnized option %s\n", opt.toAscii().constData());
			return -1;
		}
		filename = QString(argv[2]);
	}
	// Hack to run under wine

	if (filename.contains(":\\powerdevs")) {
		filename = filename.mid(filename.indexOf("powerdevs") + 9).replace("\\", "/");
		filename = ".." + filename;
	}
	modelCoupled *c = NULL;
	if (filename.endsWith(".pdm", Qt::CaseInsensitive)) {
		c = parseModel(filename);
		generateCode(c, filename.replace(".pdm", ".pds", Qt::CaseInsensitive), generateCCode, true);
	} else if (filename.endsWith(".pds", Qt::CaseInsensitive)) {
		c = parsePDS(filename);
		generateCode(c, filename.replace(".pdm", ".pds", Qt::CaseInsensitive), generateCCode, false);
	} else {
		printf("File must be either a .pdm or .pds\n");
		return -1;
	}
		QString path = QCoreApplication::applicationDirPath();
		QFSFileEngine::setCurrentPath(path + "/../build");
		QProcess make;
#ifdef Q_OS_LINUX
		make.start("/usr/bin/make");
#else
                QStringList env = QProcess::systemEnvironment();
                env << "PATH=" + path + "/gcc/bin" ;
                make.setEnvironment(env);
		make.start("../bin/gcc/bin/make.exe");
#endif
		make.waitForFinished(-1);

    QByteArray log(make.readAllStandardError());
		if (make.exitCode() == 0) {
	    if (runSimulation) {
			  QFSFileEngine::setCurrentPath(path + "/../output");
			  QProcess pdif;
#ifdef RTAIOS
			  pdif.startDetached("/usr/bin/kdesudo", QStringList() << "../bin/pdif" << filename.left(filename.  lastIndexOf(".")) + ".stm");
#else
			  if (!silent) {
#ifdef Q_OS_LINUX
          bool pdifRunning = system("ps -e | grep pdif") == 0;
          qDebug() << "PDIF running = " << pdifRunning;
          if (pdifRunning) {
            system("killall pdif");
          }
#endif
				  pdif.startDetached("../bin/pdif", QStringList() << filename.  left(filename.lastIndexOf(".")) + ".stm");
			  } else {
				  QFile stm(filename.left(filename.lastIndexOf(".")) + ".stm");
				  //qDebug() << (filename.left(filename.lastIndexOf(".")) + ".stm");
				  double tf,ti;
				  bool ok=false;
				  if (stm.open(QIODevice::ReadOnly | QIODevice::Text)) {
					  QString s(stm.readAll());
					  QStringList ls = s.split('\n');
					  ti = ls.first().trimmed().toDouble();
					  tf = ls.at(1).trimmed().toDouble(&ok);
				  }
				  if (ok)
				  	pdif.startDetached("./model", QStringList() << "-tf" << QString("%1").arg(tf));
				  else
					  pdif.startDetached("../bin/pdif", QStringList() << filename.  left(filename.lastIndexOf(".")) + ".stm");
	      }
			}
#endif
		} else {
			QMessageBox msgBox;
			msgBox.setWindowState(Qt::WindowNoState);
      
			//msgBox.setDetailedText(QString:: fromUtf8(log));
      QPushButton viewLog("View Compilation Log");
  
      msgBox.addButton(&viewLog,QMessageBox::HelpRole);
      msgBox.addButton(QMessageBox::Ok);
			msgBox.setText ("Error: The compilation process has reported an error.");
			msgBox.setIcon(QMessageBox::Critical);
			msgBox.setWindowTitle("PowerDEVS");
			msgBox.show();
			msgBox.showNormal();

      QFile logfile(path + "/../output/compile.log");
      logfile.open(QIODevice::WriteOnly);
      logfile.write(log);
      logfile.close();
      msgBox.exec();
			if (msgBox.clickedButton()==&viewLog) {
#ifndef Q_OS_WIN32
        QProcess::startDetached("/usr/bin/xdg-open", QStringList() << "../output/compile.log");
#else
        QProcess::startDetached("notepad", QStringList() << "../output/compile.log");
#endif
      }
			return -1;
		}
    QFile logfile(path + "/../output/compile.log");
    logfile.open(QIODevice::WriteOnly);
    logfile.write(log);
    logfile.close();
  	return 0;
}

void showLog() {

}
