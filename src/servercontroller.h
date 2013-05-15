/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef SERVERCONTROLLER_H
#define SERVERCONTROLLER_H

#include <QObject>
#include <QProcess>

class ServerController: public QObject
{
Q_OBJECT
public:
    /**
     * Singleton accessor
     */
    static ServerController * instance();

    /**
     * Destructor
     */
    ~ServerController();

    /**
     * Check if the messageserver is running or not
     * @returns true if the process is running, false otherwise.
     */
    bool isRunning() const;

    /**
     * Start the messageserver
     * @returns true on success
     */
    bool startServer();

Q_SIGNALS:
    /**
     * Send an error message
     */
    void errorMessage(const QString &);

private Q_SLOTS:
    /**
     * Slot to process any errors from the QProcess
     */
    void processError(QProcess::ProcessError);

private:
    ServerController(QObject *parent = 0);

    QProcess *m_serverProcess;
    static ServerController *m_instance;
};

#endif
