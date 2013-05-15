/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <MApplicationWindow>
#include <qmailstore.h>

#include "emailapplication.h"
#include "emaildbusadaptor.h"

int main(int argc, char** argv)
{
    EmailApplication *app = new EmailApplication(argc, argv);

    new EmailDBusAdaptor(app);
    QDBusConnection::sessionBus().registerObject("/", app);
    QDBusConnection::sessionBus().registerService("com.meego.email");

    QMailAccountKey emailKey(QMailAccountKey::messageType(QMailMessage::Email));
    QMailAccountIdList accountList = QMailStore::instance()->queryAccounts(emailKey);

    if (accountList.size() > 0)
        app->showFolderPage();
    else
        app->showAccountSetupPage();

    return app->exec();
}

