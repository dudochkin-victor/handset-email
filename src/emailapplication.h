/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef EMAILAPPLICATION_H
#define EMAILAPPLICATION_H

#include <MApplication>
#include <MApplicationWindow>

#include <qmailfolder.h>

#include "folderpage.h"
#include "composepage.h"
#include "accountsetuppage.h"
#include "accountinfo.h"
#include "emailsettingspage.h"

class EmailApplication : public MApplication
{
    Q_OBJECT

public:
    EmailApplication(int &argc, char **argv);
    static EmailApplication *instance();

    void addEmailAccount(AccountInfo *);
    void deleteEmailAccount(AccountInfo *);
    QList<AccountInfo *> emailAccounts () { return m_accountList; }


public slots:
    void showFolderPage();
    void showComposePage(QString recipient="", QStringList attachments=QStringList());
    void showAccountSetupPage();
    void showMessageListPage(QMailFolderId);
    void showConversationPage(QMailMessageId);
    void showManualAccountEditPage(AccountInfo *);
    void showReviewAccountPage(AccountInfo *);
    void showEmailSettingsPage();
    void showEditExistingAccountPage(QMailAccountId);

    void composeReply(QMailMessageId);
    void composeReplyAll(QMailMessageId);
    void composeForward(QMailMessageId);

private:
    MApplicationWindow *m_Window;
    FolderPage         *m_folderPage;
    ComposePage        *m_composePage;
    EmailSettingsPage   *m_emailSettingsPage;
    MApplicationPage   *m_accountSetupPage;
    QList<AccountInfo *> m_accountList;
};

#endif // EMAILAPPLICATION_H
