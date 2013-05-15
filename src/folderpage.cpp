/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <MAction>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MContentItem>
#include <qmailstore.h>
#include <qmailfolder.h>
#include <qmailaccountkey.h>
#include <qmailserviceaction.h>

#include "folderpage.h"
#include "folderitem.h"
#include "mailmanager.h"
#include "accountitem.h"
#include "emailapplication.h"

FolderPage::FolderPage(QGraphicsItem *parent)
    : MApplicationPage(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setPannable(true);
    m_mailAccounts.clear();
    m_retrievalAccounts.clear();

    createActions();
    //% "Mail"
    setTitle(qtTrId("xx_page_title"));
}

FolderPage::~FolderPage()
{
}

void FolderPage::createContent()
{
    m_layout = new MLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setAnimation(NULL);

    m_policy = new MLinearLayoutPolicy(m_layout, Qt::Vertical);
    m_policy->setObjectName("VerticalPolicy");
    m_policy->setSpacing(1);

    // get All exsiting email accaounts
    getEmailAccounts();
    
    connect (QMailStore::instance(), SIGNAL(accountsAdded(const QMailAccountIdList &)), this,
             SLOT(accountsAdded (const QMailAccountIdList &)));
    connect (QMailStore::instance(), SIGNAL(accountsRemoved(const QMailAccountIdList &)), this,
             SLOT(accountsRemoved(const QMailAccountIdList &)));
    connect (QMailStore::instance(), SIGNAL(accountsUpdated(const QMailAccountIdList &)), this,
             SLOT(accountsUpdated(const QMailAccountIdList &)));
    connect (QMailStore::instance(), SIGNAL(folderContentsModified(const QMailFolderIdList &)), this,
             SLOT(onFolderContentsModified(const QMailFolderIdList &)));
    connect (QMailStore::instance(), SIGNAL(foldersAdded(const QMailFolderIdList &)), this,
             SLOT(onFoldersAdded(const QMailFolderIdList &)));
    connect (QMailStore::instance(), SIGNAL(foldersRemoved(const QMailFolderIdList &)), this,
             SLOT(onFoldersRemoved(const QMailFolderIdList &)));

    m_retrievalAccounts = m_mailAccounts;
    connect (MailManager::instance(), SIGNAL(syncCompleted()), this, SLOT(onAccountSyncCompleted()));
    MailManager::instance()->accountsSync();
    

    foreach (const QMailAccountId &id, m_mailAccounts)
        insertAccount(id);

    centralWidget()->setLayout(m_layout);
}

void FolderPage::insertAccount (const QMailAccountId &id)
{
    AccountItem *accountItem = new AccountItem (id);
    m_policy->addItem (accountItem);
    connect (accountItem, SIGNAL(clicked(const QMailAccountId &)), this, 
                 SLOT(toggleAccountSection(const QMailAccountId &)));
    m_accountItems[id] = accountItem;

    // Get all the folders belonging to this email account
    QMailFolderKey key = QMailFolderKey::parentAccountId(id);
    const QMailFolderIdList folders = QMailStore::instance()->queryFolders(key);

    QList<FolderItem *> folderItems;
    folderItems.clear();
    foreach (const QMailFolderId id, folders)
    {
        // Get this folder
        QMailFolder folder = QMailStore::instance()->folder(id);

        FolderItem *folderItem = new FolderItem (id);
        m_policy->addItem (folderItem);
        folderItems << folderItem;

        connect (folderItem, SIGNAL(clicked(QMailFolderId)), this,
                 SLOT(folderSelected(QMailFolderId)));
    }
    m_folderItems[id] = folderItems;
    m_accountFolderVisible[id] = true;
}

void FolderPage::getEmailAccounts()
{

    QMailAccountKey emailKey(QMailAccountKey::messageType(QMailMessage::Email));
    m_mailAccounts = QMailStore::instance()->queryAccounts(emailKey);

}

void FolderPage::createActions()
{
    // "Write new mail" action item
    //% "Write new mail"
    MAction *action = new MAction(qtTrId("xx_write_new_mail"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(composeActionTriggered()));
  
    // "Settings" action item
    //% "Settings"
    action = new MAction(qtTrId("xx_settings"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SIGNAL(emailSettingsTriggered()));

    // "Refresh mail" action item
    //% "Refresh mail"
    action = new MAction(qtTrId("xx_refresh_mail"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(refreshActionTriggered()));

}

void FolderPage::folderSelected(QMailFolderId folderId)
{
    emit listMessagesByMailId (folderId);
}

void FolderPage::composeActionTriggered()
{
    emit composeMail();
}

void FolderPage::refreshActionTriggered()
{
    MailManager::instance()->accountsSync();
}

void FolderPage::accountsAdded(const QMailAccountIdList &ids)
{

    foreach (QMailAccountId id, ids)
    {
        m_mailAccounts.append(id);
        insertAccount (id);
    }

    m_retrievalAccounts = m_mailAccounts;
    MailManager::instance()->accountsSync();
}

void FolderPage::accountsUpdated(const QMailAccountIdList &ids)
{
    foreach (QMailAccountId id, ids)
    {
        AccountItem *accountItem = m_accountItems[id];
        if (!accountItem)
           return;
        m_policy->removeItem(accountItem);
        QList<FolderItem *>folderItems = m_folderItems[id];
        foreach (FolderItem *item, folderItems)
            m_policy->removeItem(item);

        insertAccount(id);
    }

}

void FolderPage::accountsRemoved(const QMailAccountIdList &ids)
{
    foreach (QMailAccountId id, ids)
    {
        AccountItem *accountItem = m_accountItems[id];
        if (!accountItem)
           return;
        m_policy->removeItem(accountItem);
        QList<FolderItem *>folderItems = m_folderItems[id];
        foreach (FolderItem *item, folderItems)
            m_policy->removeItem(item);
        m_mailAccounts.removeOne(id);
        m_folderItems.remove(id);
        m_accountFolderVisible.remove(id);
        m_accountItems.remove(id);
    }
}

void FolderPage::toggleAccountSection(const QMailAccountId &id)
{
    bool visible = m_accountFolderVisible[id];
    if (visible)
    {
        m_accountFolderVisible[id] = false;
        QList<FolderItem *>folderItems = m_folderItems[id];
        foreach (FolderItem *item, folderItems)
            m_policy->removeItem(item);
    }
    else
    {
        m_accountFolderVisible[id] = true;
        QList<FolderItem *>folderItems = m_folderItems[id];
        AccountItem *accountItem = m_accountItems[id];
        int index = m_policy->indexOf (accountItem);
        foreach (FolderItem *item, folderItems)
            m_policy->insertItem(++index, item);
    }
}

void FolderPage::onFolderContentsModified(const QMailFolderIdList &folderIds)
{
    // folder contents has been changed
    foreach (QMailFolderId folderId, folderIds)
    {
        bool found = false;
        foreach (QMailAccountId id, m_mailAccounts)
        {
            QList<FolderItem *>folderItems = m_folderItems[id];
            foreach (FolderItem *item, folderItems)
            {
                if (item->folderId() == folderId)
                {
                    item->updateMessagesCount();
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
    }
}

void FolderPage::onFoldersAdded(const QMailFolderIdList & folderIds)
{
    //  Lets take the first folder and determine which mail account it belongs to
    QMailFolder folder(folderIds[0]);
    QMailAccountId account = folder.parentAccountId();
    QMailAccount mailAccount(account);

    // Make sure message server downloaded some messages for the account
    MailManager::instance()->retrieveMessages(account);

    int index = m_policy->indexOf(m_accountItems[account]);
    if (m_folderItems[account].size() > 0)
    {
        index = m_policy->indexOf(m_folderItems[account].last());
    }
    
    index++;

    bool showFolder = m_accountFolderVisible[account];
    foreach (const QMailFolderId id, folderIds)
    {
        // Get this folder
        QMailFolder folder = QMailStore::instance()->folder(id);

        FolderItem *folderItem = new FolderItem (id);
        m_folderItems[account] << folderItem;

        if (showFolder)
        {
            m_policy->insertItem (index, folderItem);
            index++;
        }
        connect (folderItem, SIGNAL(clicked(QMailFolderId)), this,
                 SLOT(folderSelected(QMailFolderId)));
    }
}

void FolderPage::onFoldersRemoved(const QMailFolderIdList & folderIds)
{
    Q_UNUSED(folderIds);
    foreach (QMailAccountId account, m_mailAccounts)
    {
        QMailFolderKey key = QMailFolderKey::parentAccountId(account);
        const QMailFolderIdList folderIds = QMailStore::instance()->queryFolders(key);

        bool folderVisible = m_accountFolderVisible[account];
        QList<FolderItem *> folderItems = m_folderItems[account];
        foreach (FolderItem *item, folderItems)
        {
            bool found = false;
            foreach (const QMailFolderId id, folderIds)
            {
                if (id == item->folderId())
                {
                    found = true;
                    break;
                }
            }
            if (found == false)
            {
                if (folderVisible)
                    m_policy->removeItem(item);
                folderItems.removeOne(item);
                delete item;
            }
        }
        m_folderItems[account] = folderItems;
    }
}

void FolderPage::onAccountSyncCompleted()
{
    if (m_retrievalAccounts.size() > 0)
        // Make sure to populate the folders with some minimum amout of messages.
        MailManager::instance()->retrieveMessages(m_retrievalAccounts.takeFirst());
}
