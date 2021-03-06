/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2013 - 2014 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "ProgressBarWidget.h"
#include "../../../core/Utils.h"
#include "../../../ui/WebWidget.h"

#include <QtGui/QPalette>
#include <QtWidgets/QHBoxLayout>

namespace Otter
{

ProgressBarWidget::ProgressBarWidget(WebWidget *webWidget, QWidget *parent) : QFrame(parent),
	m_webWidget(webWidget),
	m_progressBar(new QProgressBar(this)),
	m_elementsLabel(new QLabel(this)),
	m_totalLabel(new QLabel(this)),
	m_speedLabel(new QLabel(this)),
	m_elapsedLabel(new QLabel(this)),
	m_messageLabel(new QLabel(this)),
	m_time(NULL)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->addWidget(m_progressBar);
	layout->addWidget(m_elementsLabel);
	layout->addWidget(m_totalLabel);
	layout->addWidget(m_speedLabel);
	layout->addWidget(m_elapsedLabel);
	layout->addWidget(m_messageLabel);
	layout->setContentsMargins(0, 0, 0, 0);

	QPalette palette = m_elementsLabel->palette();
	palette.setColor(QPalette::Background, palette.color(QPalette::Base));

	setPalette(palette);
	setFrameStyle(QFrame::StyledPanel);
	setLineWidth(1);

	palette.setColor(QPalette::Background, palette.color(QPalette::AlternateBase));

	m_progressBar->setFixedWidth(150);
	m_progressBar->setFormat(tr("Document: %p%"));
	m_elementsLabel->setFixedWidth(150);
	m_totalLabel->setFixedWidth(150);
	m_totalLabel->setAutoFillBackground(true);
	m_totalLabel->setPalette(palette);
	m_speedLabel->setFixedWidth(150);
	m_elapsedLabel->setFixedWidth(150);
	m_elapsedLabel->setAutoFillBackground(true);
	m_elapsedLabel->setPalette(palette);

	setAutoFillBackground(true);
	setLoading(webWidget->isLoading());

	connect(webWidget, SIGNAL(loadProgress(int)), m_progressBar, SLOT(setValue(int)));
	connect(webWidget, SIGNAL(loadStatusChanged(int,int,qint64,qint64,qint64)), this, SLOT(updateLoadStatus(int,int,qint64,qint64,qint64)));
	connect(webWidget, SIGNAL(loadingChanged(bool)), this, SLOT(setLoading(bool)));
}

void ProgressBarWidget::timerEvent(QTimerEvent *event)
{
	if (m_time)
	{
		int seconds = (m_time->elapsed() / 1000);
		int minutes = (seconds / 60);

		seconds = (seconds - (minutes * 60));

		m_elapsedLabel->setText(tr("Time: %1").arg(QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QLatin1Char('0'))));
	}
	else
	{
		m_elapsedLabel->setText(QString());
	}

	if (!m_webWidget->isLoading())
	{
		killTimer(event->timerId());
	}
}

void ProgressBarWidget::updateLoadStatus(int finishedRequests, int startedReuests, qint64 bytesReceived, qint64 bytesTotal, qint64 speed)
{
	Q_UNUSED(bytesTotal)

	m_elementsLabel->setText(tr("Elements: %1/%2").arg(finishedRequests).arg(startedReuests));
	m_totalLabel->setText(tr("Total: %1").arg(Utils::formatUnit(bytesReceived, false, 1)));
	m_speedLabel->setText(tr("Speed: %1").arg(Utils::formatUnit(speed, true, 1)));
}

void ProgressBarWidget::setLoading(bool loading)
{
	if (loading)
	{
		m_progressBar->setValue(0);
		m_elapsedLabel->setText(tr("Time: %1").arg(QLatin1String("0:00")));

		updateLoadStatus(0, 0, 0, 0, 0);
		show();

		m_time = new QTime();
		m_time->start();

		startTimer(1000);
	}
	else
	{
		if (m_time)
		{
			delete m_time;

			m_time = NULL;
		}

		hide();
	}
}

}
