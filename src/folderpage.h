/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef FOLDERPAGE_H
#define FOLDERPAGE_H

#include <MApplicationPage>
#include <MLinearLayoutPolicy>

#include <qmailstore.h>
#include <qmailaccount.h>

#include "folderitem.h"
#include "accountitem.h"

class FolderPage : public MApplicationPage
{
    Q_OBJECT

public:
    FolderPage(QGraphicsItem *parent = NULL);
    virtual ~FolderPage();
    virtual void createContent();

signals:
    void listMessagesByMailId(QMailFolderId folderId);
    void composeMail();
    void emailSettingsTriggered();

private slots:
    void accountsAdded(const QMailAccountIdList &);
    void accountsRemoved(const QMailAccountIdList &);
    void accountsUpdated(const QMailAccountIdList &);
    void folderSelected(QMailFolderId  folderId);
    void composeActionTriggered();
    void refreshActionTriggered();
    void toggleAccountSection(const QMailAccountId &);
    void onFolderContentsModified (const QMailFolderIdList &);
    void onFoldersAdded (const QMailFolderIdList &);
    void onFoldersRemoved (const QMailFolderIdList &);
    void onAccountSyncCompleted();

private:
    void createActions();
    void getEmailAccounts();
    void insertAccount (const QMailAccountId &);

private:
    MLayout *m_layout;
    MLinearLayoutPolicy *m_policy;

    QMailAccountIdList m_mailAccounts;
    QMailAccountIdList m_retrievalAccounts;
    QMap<QMailAccountId, AccountItem *> m_accountItems;
    QMap<QMailAccountId, QList<FolderItem *> > m_folderItems;
    QMap<QMailAccountId, bool> m_accountFolderVisible;
};

#endif // FOLDERPAGE_H
