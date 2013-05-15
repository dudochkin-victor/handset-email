/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef EMAILDBUSADAPTOR_H
#define EMAILDBUSADAPTOR_H

#include <QtDBus/QtDBus>
#include <QDBusAbstractAdaptor>
#include "emailapplication.h"

class EmailDBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.meego.email")

public:
    EmailDBusAdaptor(EmailApplication *app);

public slots:
    void showComposePage(QString recipient="", QStringList attachments=QStringList());
    Q_SCRIPTABLE QString getRecentEmailTimeStamp (const QString &emailAddress);

private:
    EmailApplication *mApp;

};

#endif // EMAILDBUSADAPTOR_H
