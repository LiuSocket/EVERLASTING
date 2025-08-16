#include "GMPlayKitWidget.h"
#include "../Engine/GMEngine.h"
#include <QKeyEvent>

using namespace GM;

CGMPlayKitWidget::CGMPlayKitWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setAttribute(Qt::WA_TranslucentBackground);

	connect(ui.playBtn, SIGNAL(clicked()), this, SLOT(_slotPlayOrPause()));
}

CGMPlayKitWidget::~CGMPlayKitWidget()
{}

void CGMPlayKitWidget::Update()
{
	const std::wstring wstrAudioName = GM_ENGINE.GetAudioName();
	QString strFileName = QString::fromStdWString(wstrAudioName);
	if ("" == strFileName || GM_ENGINE.IsAudioOver())
	{
		// ½«²¥·Å/ÔÝÍ£°´Å¥ÉèÖÃ³ÉÔÝÍ£×´Ì¬
		ui.playBtn->setChecked(false);
		return;
	}
}

void CGMPlayKitWidget::_slotPlayOrPause()
{
	if (ui.playBtn->isChecked())
	{
		GM_ENGINE.Play();
	}
	else
	{
		GM_ENGINE.Pause();
	}
}