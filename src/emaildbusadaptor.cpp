/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "emaildbusadaptor.h"
#include "mailmanager.h"

EmailDBusAdaptor::EmailDBusAdaptor(EmailApplication *app) :
    QDBusAbstractAdaptor(app), mApp(app) 
{}

void EmailDBusAdaptor::showComposePage(QString recipient, QStringList attachments)
{
    mApp->showComposePage(recipient, attachments);
}

QString EmailDBusAdaptor::getRecentEmailTimeStamp(const QString& emailAddress)
{
    return MailManager::instance()->getRecentEmailTimeStamp(emailAddress);
}
