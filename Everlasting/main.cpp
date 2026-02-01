//////////////////////////////////////////////////////////////////////////
/// COPYRIGHT NOTICE
/// Copyright (c) 2020~2030, LiuTao
/// All rights reserved.
///
/// @file		main.cpp
/// @brief		Galaxy-Music Engine - main.cpp
/// @version	1.0
/// @author		LiuTao
/// @date		2020.12.10
//////////////////////////////////////////////////////////////////////////

#include "GMSystemManager.h"
#include "steam/steam_api.h"
#include <QTextCodec>
#include <QFileInfo>
#include <QtWidgets/QApplication>
#include <QFont>
#include <QIcon>
#include <QTranslator>
#include <iostream>

using namespace GM;

int main(int argc, char **argv)
{
	if (SteamAPI_RestartAppIfNecessary(4241180))
	{
		// if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the 
		// local Steam client and also launches this game again.

		// Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
		// removed steam_appid.txt from the game depot.

		return EXIT_FAILURE;
	}

	SteamErrMsg errMsg = { 0 };
	if (k_ESteamAPIInitResult_OK != SteamAPI_InitEx(&errMsg))
	{
		std::cout << "SteamAPI_InitEx failed: " << errMsg << std::endl;
		return EXIT_FAILURE;
	}

	// Ensure that the user has logged into Steam. This will always return true if the game is launched
	// from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
	// will return false.
	if (!SteamUser()->BLoggedOn())
	{
		std::cout << "Steam user is not logged in" << std::endl;
		return EXIT_FAILURE;
	}

	//自动适应高分辨率
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QTextCodec *xcodec = QTextCodec::codecForLocale();
	QString exeDir = xcodec->toUnicode(QByteArray(argv[0]));
	QString BKE_CURRENT_DIR = QFileInfo(exeDir).path();
	QStringList  libpath;
	libpath << BKE_CURRENT_DIR + QString::fromLocal8Bit("/plugins/platforms");
	libpath << BKE_CURRENT_DIR << BKE_CURRENT_DIR + QString::fromLocal8Bit("/plugins/imageformats");
	libpath << BKE_CURRENT_DIR + QString::fromLocal8Bit("/plugins");
	libpath << QApplication::libraryPaths();
	QApplication::setLibraryPaths(libpath);

	QApplication a(argc, argv);
	a.setWindowIcon(QIcon(":/Resources/GM_logo.png"));
	a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));

	QFont textFont = a.font();
	textFont.setFamily("Microsoft YaHei");
	textFont.setStyleStrategy(QFont::PreferAntialias);
	a.setFont(textFont);

	//加载Qt标准对话框的中文翻译文件
	QTranslator tran;
	tran.load(":/Resources/qt_zh_CN.qm");
	a.installTranslator(&tran);

	GM_SYSTEM_MANAGER.Init();
	return a.exec();
}