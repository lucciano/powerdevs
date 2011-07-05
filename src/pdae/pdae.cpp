/***************************************************************************
**
**  Copyright (C) 2009 Facultad de Ciencia Exactas Ingeniería y Agrimensura
**       	       Universidad Nacional de Rosario - Argentina.
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
#include <QtGlobal>
#include <QSettings>

#include "pdae.h"
void msg(QString s)
{
	QMessageBox msgBox;
	msgBox.setText(s);
	msgBox.setWindowTitle("PowerDEVS");
	msgBox.exec();
} void PDAE::closeComp(bool v) 
{
	if (!v) {
		removeDockWidget(compilerOut);
		compilerOut = NULL;
	}
}


//! [0]
 PDAE::PDAE(QWidget * parent, QString file) :QMainWindow(parent) 
{
	setupFileMenu();
	setupToolsMenu();
	setupEditor();
	setWindowIcon(QIcon(":/pd.ico"));
	setMinimumWidth(800);
	setMinimumHeight(500);
	setCentralWidget(qhboxWidget);
	setWindowTitle(tr("PowerDEVS Atomic Editor"));
	if (file != "") {
		firsttime = false;
		openFile(file);
	} else {
		firsttime = true;
		newFile();
	}
	compilerOut = NULL;
	setWindowState(Qt::WindowMaximized);
}

void PDAE::closeEvent(QCloseEvent * event) 
{
	if (dirty) {
		QMessageBox msgBox;
		msgBox.setWindowTitle("Save file");
		msgBox.setText("Save changes?");
		msgBox.addButton(QMessageBox::Yes);
		msgBox.addButton(QMessageBox::No);
		msgBox.addButton(QMessageBox::Cancel);
		int result = msgBox.exec();
		if (result == QMessageBox::Cancel) {
			event->ignore();
			return;
		}
		if (result == QMessageBox::Yes)
			saveFileToCpp(false);
	}
	event->accept();
}

void PDAE::saveFileToCpp(bool saveas)
{
	if (name == "")
		saveas = true;
	QString newname;
	if (saveas) {
		newname =
		    QFileDialog::getSaveFileName(this, tr("Save model"),
						 QCoreApplication::
						 applicationDirPath() +
						 "/../atomics",
						 "C++ Files (*.cpp)");
	}
	if (saveas && newname == "")
		return;
	if (saveas)
		name = newname;
	name = name.replace("\\", "/");
	if (!name.endsWith(".cpp"))
		name = name + QString(".cpp");
	QString modelName =
	    name.mid(name.lastIndexOf("/") + 1,
		     name.lastIndexOf(".") - name.lastIndexOf("/") - 1);
	QString cppFileName =
	    name.right(name.indexOf("atomics", 0, Qt::CaseInsensitive) >=
		       0 ? name.length() - name.indexOf("atomics", 0,
							 Qt::CaseInsensitive) -
		       8 : name.length());
	QString hFile = name.left(name.lastIndexOf(".")) + ".h";
	QFile hf(hFile);
	QList < QString >::iterator i;
	hf.open(QIODevice::WriteOnly | QIODevice::Text);
	for (i = libs.begin(); i != libs.end(); ++i)
		hf.write(QString("//LIB:" + *i + "\n").toAscii());
	for (i = flags.begin(); i != flags.end(); ++i)
		hf.write(QString("//FLAGS:" + *i + "\n").toAscii());
	for (i = cpps.begin(); i != cpps.end(); ++i) {
		QString cpp;
		if (i->indexOf("atomics", 0, Qt::CaseInsensitive) >= 0)
			cpp =
			    i->right(i->length() -
				     i->indexOf("atomics", 0,
						Qt::CaseInsensitive) - 8);
		
		else
			cpp = *i;
		hf.write(QString("//CPP:" + cpp + "\n").toAscii());
	}
	for (i = headersDirs.begin(); i != headersDirs.end(); ++i)
		hf.write(QString("//HeadersDir:" + *i + "\n").toAscii());
	hf.write(("//CPP:" + cppFileName + "\n").toAscii());
	hf.write(("#if !defined " + modelName + "_h\n").toAscii());
	hf.write(("#define " + modelName + "_h\n").toAscii());
	hf.write("\n");
	hf.write("#include \"simulator.h\"\n");
	hf.write("#include \"event.h\"\n");
	hf.write("#include \"stdarg.h\"\n");
	hf.write("\n");
	for (i = headers.begin(); i != headers.end(); ++i)
		hf.write(QString("#include \"" + *i + "\"\n").toAscii());
	hf.write("\n");
	hf.write("\n");
	hf.write(QString("class " + modelName + ": public Simulator { \n").
		  toAscii());
	hf.write(editorState->toPlainText().toAscii());
	hf.write("\n");
	hf.write("public:\n");
	hf.write(QString
		  ("\t" + modelName +
		   "(const char *n): Simulator(n) {};\n").toAscii());
	hf.write("\tvoid init(double, ...);\n");
	hf.write("\tdouble ta(double t);\n");
	hf.write("\tvoid dint(double);\n");
	hf.write("\tvoid dext(Event , double );\n");
	hf.write("\tEvent lambda(double);\n");
	hf.write("\tvoid exit();\n");
	hf.write("};\n");
	hf.write("#endif\n");
	hf.close();
	QFile cf(name);
	cf.open(QIODevice::WriteOnly | QIODevice::Text);
	cf.write(("#include \"" + modelName + ".h\"\n").toAscii());
	cf.write(("void " + modelName + "::init(double t,...) {\n").toAscii());
	cf.write(editorInit->toPlainText().toAscii());
	cf.write("\n}\n");
	cf.write(("double " + modelName + "::ta(double t) {\n").toAscii());
	cf.write(editorTa->toPlainText().toAscii());
	cf.write("\n}\n");
	cf.write(("void " + modelName + "::dint(double t) {\n").toAscii());
	cf.write(editorDInt->toPlainText().toAscii());
	cf.write("\n}\n");
	cf.write(("void " + modelName + "::dext(Event x, double t) {\n").
		    toAscii());
	cf.write(editorDExt->toPlainText().toAscii());
	cf.write("\n}\n");
	cf.write(("Event " + modelName + "::lambda(double t) {\n").toAscii());
	cf.write(editorLambda->toPlainText().toAscii());
	cf.write("\n}\n");
	cf.write(("void " + modelName + "::exit() {\n").toAscii());
	cf.write(editorExit->toPlainText().toAscii());
	cf.write("\n}\n");
	cf.close();
	dirty = false;
	setWindowTitle("PowerDEVS Atomic Editor - " + modelName);
}

void PDAE::newFile() 
{
	if (dirty && !firsttime) {
		QMessageBox msgBox;
		msgBox.setWindowTitle("Save file");
		msgBox.setText("Save changes?");
		msgBox.addButton(QMessageBox::Yes);
		msgBox.addButton(QMessageBox::No);
		msgBox.addButton(QMessageBox::Cancel);
		int result = msgBox.exec();
		if (result == QMessageBox::Cancel)
			return;
		if (result == QMessageBox::Yes)
			saveFileToCpp(false);
	}
	firsttime = false;
	editorInit->
	    setPlainText
	    ("//The 'parameters' variable contains the parameters transferred from the editor.\nva_list parameters;\nva_start(parameters,t);\n//To get a parameter: %Name% = va_arg(parameters,%Type%)\n//where:\n//      %Name% is the parameter name\n//	%Type% is the parameter type\n");
	editorTa->setPlainText("//This function returns a double.\n");
	editorDInt->setPlainText("");
	editorDExt->
	    setPlainText
	    ("//The input event is in the 'x' variable.\n//where:\n//     'x.value' is the value (pointer to void)\n//     'x.port' is the port number\n//     'e' is the time elapsed since last transition\n");
	editorLambda->
	    setPlainText
	    ("//This function returns an Event:\n//     Event(%&Value%, %NroPort%)\n//where:\n//     %&Value% points to the variable which contains the value.\n//     %NroPort% is the port number (from 0 to n-1)\n");
	editorExit->
	    setPlainText("//Code executed at the end of the simulation.\n");
	editorState->
	    setPlainText
	    ("// Declare the state,\n// output variables\n// and parameters\n");
	editorInit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	editorTa->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	editorDInt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	editorDExt->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	editorLambda->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	editorExit->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	editorState->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	headers.clear();
	libs.clear();
	headersDirs.clear();
	cpps.clear();
	flags.clear();
	dirty = false;
	name = "";
	setWindowTitle(tr("PowerDEVS Atomic Editor"));
}

void PDAE::saveFile() 
{
	saveFileToCpp(false);
} void PDAE::saveAsFile() 
{
	saveFileToCpp(true);
} void PDAE::checkFile() 
{
	if (compilerOut != NULL) {
		removeDockWidget(compilerOut);
		delete compilerOut;
		compilerOut = NULL;
	}
	if (dirty) {
		saveFileToCpp(false);
	}
	if (name != "") {
		QProcess q;
		QString destFile = QString(name);
		QString path = QCoreApplication::applicationDirPath();
		QStringList args;
		destFile = destFile.mid(destFile.lastIndexOf("/") + 1);
		destFile.replace(".cpp", ".o");
		destFile = path + "/../build/objs/" + destFile;
		args << "-c" << "-I" + path + "/../engine/" << "-I" + path + "/../atomics/" <<name << "-o" << destFile;
		QStringList::iterator i;
		
    qDebug() << args;
#ifdef Q_OS_LINUX
		    q.start("/usr/bin/g++", args);
		
#else	/*  */
		    q.start("gcc/bin/gcc.exe", args);
#endif	/*  */
		    q.waitForFinished();
		QByteArray errLog = q.readAllStandardError();
		if (errLog.isEmpty()) {
			msg("The compiler did not find any warnings");
		} else {
			if (compilerOut == NULL) {
				compilerOut =
				    new QDockWidget(tr("Compiler Output"),
						    this);
				
				    //compilerOut->setFeatures(QDockWidget::NoDockWidgetFeatures);
				    compilerOut->
				    setAllowedAreas(Qt::BottomDockWidgetArea);
				QObject::connect(compilerOut,
						  SIGNAL(visibilityChanged
							 (bool)), this,
						  SLOT(closeComp(bool)));
				QTextEdit * te = new QTextEdit(compilerOut);
				te->setReadOnly(true);
				te->setPlainText(QString::fromUtf8(errLog));
				te->setMinimumWidth(this->width());
				te->setMinimumHeight(200);
				compilerOut->setWidget(te);
				addDockWidget(Qt::BottomDockWidgetArea,
					       compilerOut);
			}
		}
	}
}

void PDAE::configureFile() 
{
	ConfigWindow w(this, &headers, &libs, &headersDirs, &cpps, &flags);
	w.exec();
	textChange();
} QString bodyOf(QString f, QByteArray b)
{
	int start, end, braces, p, inLine = 0, inBlock = 0, i;
	start = b.indexOf("{", b.indexOf("::" + f + "(")) + 2;
	if (start < 0)
		return "";
	braces = 1;
	for (i = start; i < b.count(); i++) {
		if ((b.at(i) == '/') && (i + 1 < b.count()) && (b.at(i + 1) == '/')) {	// Start of a line comment
			i += 2;
			while (b.at(i) != '\n') {
				i++;	// Skip line comment       
			}
			i++;
		}
		if ((b.at(i) == '/') && (i + 1 < b.count()) && (b.at(i + 1) == '*')) {	// Start of a block comment
			i += 2;
			while ((b.at(i) != '*') && (i + 1 < b.count())
				&& (b.at(i + 1) != '/')) {
				i++;	// Skip block comment      
			}
			i++;
		}
		if (b.at(i) == '{')
			braces++;
		if (b.at(i) == '}')
			braces--;
		if (!braces) {
			if (b.at(i - 1) == '\n')
				i--;
			break;
		}
	}
	if (!braces)
		return b.mid(start, i - start);
	
	else
		return "";
}

void PDAE::openFile(const QString & path) 
{
	char buf[1024];
	QString fileName = path;
	if (fileName.isNull())
		fileName =
		    QFileDialog::getOpenFileName(this, tr("Open File"),
						 QCoreApplication::
						 applicationDirPath() +
						 "/../atomics",
						 "C++ Files (*.cpp)");
	if (!fileName.isEmpty()) {
		fileName = fileName.toLower();
		
		    // Hack to run under wine
		    if (fileName.
			contains("C:\\powerdevs\\bin\\", Qt::CaseInsensitive)) {
			fileName =
			    fileName.remove("C:\\powerdevs\\bin\\",
					    Qt::CaseInsensitive).replace("\\",
									 "/");
		}
		cpps.clear();
		libs.clear();
		headers.clear();
		headersDirs.clear();
		name = fileName.left(fileName.lastIndexOf(".")) + ".cpp";
		name.replace("\\", "/");
		QString modelName =
		    name.mid(name.lastIndexOf("/") + 1,
			     name.lastIndexOf(".") - name.lastIndexOf("/") - 1);
		QString hFile =
		    fileName.left(fileName.lastIndexOf(".")) + ".h";
		QString cFile =
		    fileName.left(fileName.lastIndexOf(".")) + ".cpp";
		QFile cf(cFile);
		QFile hf(hFile);
		QString s;
		QString acc = "";
		if (!hf.open(QIODevice::ReadOnly | QIODevice::Text)) {
			msg(QString(hFile + " The file could not be open").
			     toAscii().constData());
			exit(-1);
		}
		if (!cf.open(QIODevice::ReadOnly | QIODevice::Text)) {
			msg("The file could not be open");
			exit(-1);
		}
		
		do {		// Skip lines until the class declaration line (Parse CPP LIBS HEADERS INCLUDEs)
			if (!hf.readLine(buf, sizeof(buf))) {
				msg("The file could not be open");
				break;
			}
			s = QString(buf);
			if (s.contains("#include", Qt::CaseInsensitive)
			     && !s.contains("simulator.h", Qt::CaseInsensitive)
			     && !s.contains("event.h", Qt::CaseInsensitive)
			     && !s.contains("stdarg.h", Qt::CaseInsensitive)) {
				if (s.contains("<")) {
					acc =
					    s.mid(s.indexOf("<") + 1,
						  s.lastIndexOf(">") -
						  s.indexOf("<") - 1);
				} else {
					acc =
					    s.mid(s.indexOf("\"") + 1,
						  s.lastIndexOf("\"") -
						  s.indexOf("\"") - 1);
				}
				headers.append(acc);
			}
			if (s.contains("//CPP:")) {
				if (!s.mid(6).contains(modelName))
					cpps.append(s.mid(6));
			}
			if (s.contains("//LIB:")) {
				libs.append(s.mid(6));
			}
			if (s.contains("//FLAGS:")) {
				flags.append(s.mid(8));
			}
			if (s.contains("//HeadersDir:")) {
				headersDirs.append(s.mid(13));
			}
		} while (!s.contains("class") && !s.isEmpty());
		acc = "";
		
		do {		// Parse state declaration until the public keyword
			if (!hf.readLine(buf, sizeof(buf))) {
				msg("The file could not be open");
				break;
			};
			s = QString(buf);
			if (!s.contains("public"))
				acc = acc + s;
		} while (!s.contains("public") && !s.isEmpty());
		editorState->setPlainText(acc);
		hf.close();
		QByteArray b = cf.readAll();
		editorInit->setPlainText(bodyOf("init", b));
		editorTa->setPlainText(bodyOf("ta", b));
		editorDInt->setPlainText(bodyOf("dint", b));
		editorDExt->setPlainText(bodyOf("dext", b));
		editorLambda->setPlainText(bodyOf("lambda", b));
		editorExit->setPlainText(bodyOf("exit", b));
		cf.close();
		setWindowTitle("PowerDEVS Atomic Editor - " + modelName);
		dirty = false;
	}
}

QVariant getSetting(QString key, QVariant def)
{
  QSettings settings(QCoreApplication::applicationDirPath() + "/powerdevs.ini", QSettings::IniFormat);
  return settings.value(key,def);
}


void setSetting(QString key,QVariant value)
{
  QSettings settings(QCoreApplication::applicationDirPath() + "/powerdevs.ini", QSettings::IniFormat);
  settings.setValue(key,value);
}

//! [1]
void PDAE::setupEditor() 
{
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
  double fontSize = getSetting("pdae/fontsize",QVariant(10.0)).toDouble();
  setSetting("pdae/fontsize",fontSize);
	font.setPointSize(fontSize);
	qhboxLay = new QHBoxLayout;
	qvboxLay = new QVBoxLayout;
	tabWidget = new QTabWidget;

	editorState = new QTextEdit;
	editorInit = new QTextEdit;
	editorTa = new QTextEdit;
	editorDInt = new QTextEdit;
	editorDExt = new QTextEdit;
	editorLambda = new QTextEdit;
	editorExit = new QTextEdit;

	qhboxWidget = new QWidget;
	qvboxWidget = new QWidget;
	qlabel = new QLabel("State variables and parameters");
	editorState->setFont(font);
	editorInit->setFont(font);
	editorTa->setFont(font);
	editorDInt->setFont(font);
	editorDExt->setFont(font);
	editorLambda->setFont(font);
	editorExit->setFont(font);
	editorState->setTabStopWidth(20);
	editorInit->setTabStopWidth(20);
	editorTa->setTabStopWidth(20);
	editorDInt->setTabStopWidth(20);
	editorDExt->setTabStopWidth(20);
	editorLambda->setTabStopWidth(20);
	editorExit->setTabStopWidth(20);
	editorState->setWordWrapMode(QTextOption::NoWrap);
	editorInit->setWordWrapMode(QTextOption::NoWrap);
	editorTa->setWordWrapMode(QTextOption::NoWrap);
	editorDInt->setWordWrapMode(QTextOption::NoWrap);
	editorDExt->setWordWrapMode(QTextOption::NoWrap);
	editorLambda->setWordWrapMode(QTextOption::NoWrap);
	editorExit->setWordWrapMode(QTextOption::NoWrap);
	tabWidget->addTab(editorInit, "Init ");
	tabWidget->addTab(editorTa, "Time advance ");
	tabWidget->addTab(editorDInt, "Internal transition ");
	tabWidget->addTab(editorDExt, "External transition ");
	tabWidget->addTab(editorLambda, "Output ");
	tabWidget->addTab(editorExit, "Exit ");
	highlighter = new Highlighter(editorState->document());
	highlighter = new Highlighter(editorInit->document());
	highlighter = new Highlighter(editorTa->document());
	highlighter = new Highlighter(editorDInt->document());
	highlighter = new Highlighter(editorDExt->document());
	highlighter = new Highlighter(editorLambda->document());
	highlighter = new Highlighter(editorExit->document());
	qvboxLay->addWidget(qlabel);
	qvboxLay->addWidget(editorState);
	qvboxWidget->setLayout(qvboxLay);
	qvboxWidget->setMaximumWidth(200);
	qhboxLay->addWidget(qvboxWidget);
	qhboxLay->addWidget(tabWidget);
	qhboxWidget->setLayout(qhboxLay);
	QObject::connect(editorState, SIGNAL(textChanged()), this,
			   SLOT(textChange()));
	QObject::connect(editorInit, SIGNAL(textChanged()), this,
			  SLOT(textChange()));
	QObject::connect(editorTa, SIGNAL(textChanged()), this,
			  SLOT(textChange()));
	QObject::connect(editorDInt, SIGNAL(textChanged()), this,
			  SLOT(textChange()));
	QObject::connect(editorDExt, SIGNAL(textChanged()), this,
			  SLOT(textChange()));
	QObject::connect(editorLambda, SIGNAL(textChanged()), this,
			  SLOT(textChange()));
	QObject::connect(editorExit, SIGNAL(textChanged()), this,
			  SLOT(textChange()));

  /*
	editorState->setFontPointSize(fontSize);
	editorInit->setFontPointSize(fontSize);
	editorTa ->setFontPointSize(fontSize);
	editorDInt->setFontPointSize(fontSize);
	editorDExt->setFontPointSize(fontSize);
	editorLambda->setFontPointSize(fontSize);
	editorExit->setFontPointSize(fontSize);
  */

} 

void PDAE::textChange()
{
	if (!dirty && name != "") {
		setWindowTitle(windowTitle() + "*");
	}
	dirty = true;
  
}

void PDAE::setupFileMenu() 
{
	QMenu * fileMenu = new QMenu(tr("&File"), this);
	menuBar()->addMenu(fileMenu);
	fileMenu->addAction(tr("&New"), this, SLOT(newFile()),
			      QKeySequence(tr("Ctrl+N", "File|New")));
	fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()),
			     QKeySequence(tr("Ctrl+O", "File|Open")));
	fileMenu->addAction(tr("&Save"), this, SLOT(saveFile()),
			     QKeySequence(tr("Ctrl+S", "File|Save")));
	fileMenu->addAction(tr("&Save as..."), this, SLOT(saveAsFile()),
			     QKeySequence(tr("Ctrl+A", "File|Save as")));
	fileMenu->addAction(tr("E&xit"), this, SLOT(close()),
			     QKeySequence(tr("Ctrl+Q", "File|Exit")));
} void PDAE::setupToolsMenu() 
{
	QMenu * fileMenu = new QMenu(tr("&Tools"), this);
	menuBar()->addMenu(fileMenu);
	fileMenu->addAction(tr("&Check"), this, SLOT(checkFile()),
			     QKeySequence(tr("Ctrl+K", "Tools|Check")));
	fileMenu->addAction(tr("Confi&gure"), this, SLOT(configureFile()));
} void PDAE::setupHelpMenu() 
{
	QMenu * helpMenu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(helpMenu);
	helpMenu->addAction(tr("&About"), this, SLOT(about()));
	helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
} void PDAE::keyReleaseEvent(QKeyEvent * event) 
{
	if (event->nativeScanCode() == 117 && event->nativeModifiers() & 4) {
		tabWidget->setCurrentIndex((tabWidget->currentIndex() + 1) %
					    tabWidget->count());
	} else if (event->nativeScanCode() == 112
		    && event->nativeModifiers() & 4) {
		tabWidget->setCurrentIndex(tabWidget->currentIndex() >
					    0 ? tabWidget->currentIndex() -
					    1 : tabWidget->count() - 1);
	} else
		QWidget::keyReleaseEvent(event);
}


