/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "servercontroller.h"

#include <qmailnamespace.h>

#include <QtDebug>

ServerController *ServerController::m_instance = 0;

ServerController *ServerController::instance()
{
    if (!m_instance)
        m_instance = new ServerController();

    return m_instance;
}

ServerController::~ServerController()
{
    if (m_serverProcess) {
        qDebug() << "Shutting down messageserver child process..";
        // TODO: not sure what to do here, we should probably either kill the process safely, or
        // leave it running?
        //m_serverProcess->waitForFinished();
        delete m_serverProcess; m_serverProcess = 0;
    }
}

bool ServerController::isRunning() const
{
    const QString lockfile = "messageserver-instance.lock";
    const int lockid = QMail::fileLock(lockfile);
    if (lockid == -1)
        return true;

    QMail::fileUnlock(lockid);

    return false;
}

bool ServerController::startServer()
{
    if (m_serverProcess)
        delete m_serverProcess;

    m_serverProcess = new QProcess(this);
    connect(m_serverProcess, SIGNAL(error(QProcess::ProcessError)),
        this, SLOT(processError(QProcess::ProcessError)));

#ifdef Q_OS_WIN
    const QString servername("/messageserver.exe");
#else
    const QString servername("messageserver");
#endif

    qDebug() << "starting messageserver " << QMail::messageServerPath() + servername;
    m_serverProcess->start(QMail::messageServerPath() + servername);
    return m_serverProcess->waitForStarted();
}

void ServerController::processError(QProcess::ProcessError error)
{
    //% "The Message server child process encountered an error (%1)."
    const QString errorMsg = QString(qtTrId("xx_message_server_error").arg(static_cast<int>(error)));

    emit errorMessage(errorMsg.toLatin1());
}

ServerController::ServerController(QObject *parent)
    :QObject(parent),
    m_serverProcess(0)
{

}
