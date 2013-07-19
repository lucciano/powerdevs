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

#include <QApplication>
#include <QSplashScreen>
#include <QTimer>
#include <QDir>
#ifdef Q_WS_WIN
#include <QSysInfo>
#include <QProcess>
#endif

#include <powergui.h>

QSplashScreen *splash;

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
 	QPixmap pixmap(":images/splash.png");
  splash = new QSplashScreen(pixmap);
  splash->show();
  QDir d;
  d.setCurrent(QCoreApplication::applicationDirPath ());
	PowerGui pg;
  //pg.show();
	pg.setWindowState( Qt::WindowMaximized);
#ifdef Q_WS_WIN
  /* Check for application experience */
  if (SysInfo::windowsVersion()==QSysInfo::WV_WINDOWS7) {
    QProcess ae;
    ae.start("sc query AeLookupSvc");
    if (ae.waitForFinished()==true) {
      QString out(ae.readAllStandardOutput());
      QStringList values=out.split("\n");
      foreach (QString s, values) {
        if (s.contains("
      }
    }

  }
  
#endif
  if (argc>1)
  {
    QString file(argv[1]);
    if (file.endsWith(".pdm"))
      pg.openFile(file);
  }
  QTimer::singleShot(1000, splash, SLOT(close()));
  QTimer::singleShot(1100, &pg, SLOT(show()));
	return app.exec();
}
