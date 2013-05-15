/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include "mailmanager.h"

#include <qmailstore.h>
#include <qmailaccountkey.h>

MailManager *MailManager::m_instance = 0;

MailManager *MailManager::instance()
{
    if (!m_instance)
        m_instance = new MailManager();

    return m_instance;
}

MailManager::MailManager(QObject *parent)
    : QObject(parent),
    m_retrieving(false),
    m_transmitting(false),
    m_exporting(false),
    m_cancelling(false),
    m_retrievalAction(new QMailRetrievalAction(this)),
    m_storageAction(new QMailStorageAction(this)),
    m_transmitAction(new QMailTransmitAction(this)),
    m_exportAction(new QMailRetrievalAction(this))
{
    connect(m_retrievalAction, SIGNAL(progressChanged(uint, uint)),
            this, SLOT(progressChanged(uint,uint)));
    connect(m_retrievalAction, SIGNAL(activityChanged(QMailServiceAction::Activity)),
            this, SLOT(activityChanged(QMailServiceAction::Activity)));

    connect(m_transmitAction, SIGNAL(progressChanged(uint, uint)),
            this, SLOT(progressChanged(uint,uint)));
    connect(m_transmitAction, SIGNAL(activityChanged(QMailServiceAction::Activity)),
            this, SLOT(activityChanged(QMailServiceAction::Activity)));

    connect(m_exportAction, SIGNAL(activityChanged(QMailServiceAction::Activity)),
            this, SLOT(exportActivityChanged(QMailServiceAction::Activity)));

    // Set the default interval as 10 secs
    m_exportTimer.setInterval(10000);
    connect(&m_exportTimer, SIGNAL(timeout()), this, SLOT(exportAccounts()));
}

MailManager::~MailManager()
{
    delete m_retrievalAction;
    delete m_storageAction;
    delete m_transmitAction;
    delete m_exportAction;
}


void MailManager::retrieveMoreMessages(const QMailFolderId &folderId)
{
    if (!isSynchronizing() && folderId.isValid())
    {
        QMailFolder folder(folderId);

        QMailMessageKey countKey(QMailMessageKey::parentFolderId(folderId));
        countKey &= ~QMailMessageKey::status(QMailMessage::Temporary);
        int retrievedMinimum = QMailStore::instance()->countMessages(countKey);
        retrievedMinimum += 30;
        m_retrieving = true;
        m_cancelling = false;
        m_retrievalAction->retrieveMessageList(folder.parentAccountId(), folderId, retrievedMinimum);
    }
}

void MailManager::retrieveMessages(const QMailAccountId &id)
{
    if (!isSynchronizing() && id.isValid()) {
        m_cancelling = false;
        m_retrieving = true;
        m_retrievalAction->retrieveMessageList(id, QMailFolderId(), 30);
    }
}

void MailManager::synchronize(const QMailAccountId &id)
{
    if (!isSynchronizing() && id.isValid()) {
        m_cancelling = false;
        m_retrieving = true;
        m_retrievalAction->retrieveAll(id);
    }
}

void MailManager::sendMessages(const QMailAccountId &id)
{
    if (!isSynchronizing() && id.isValid()) {
        m_cancelling = false;
        m_transmitting = true;
        m_transmitAction->transmitMessages(id);
    }
}

void MailManager::renameFolder(const QMailFolderId &id, const QString &name)
{
    Q_ASSERT(!name.isEmpty());
    Q_ASSERT(id.isValid());

    m_storageAction->onlineRenameFolder(id, name);
}

void MailManager::deleteFolder(const QMailFolderId &id)
{
    Q_ASSERT(id.isValid());

    m_storageAction->onlineDeleteFolder(id);
}

QMailAccountIdList MailManager::accountIdList() const
{
    return QMailStore::instance()->queryAccounts(
            QMailAccountKey::status(QMailAccount::Enabled, QMailDataComparator::Includes),
            QMailAccountSortKey::name());
}

void MailManager::createFolder(const QString &name,
        const QMailAccountId &accountId, const QMailFolderId &parentId)
{
    Q_ASSERT(!name.isEmpty());
    Q_ASSERT(accountId.isValid());

    m_storageAction->onlineCreateFolder(name, accountId, parentId);
}

void MailManager::accountsSync()
{
    if (!isSynchronizing()) {
        // First we need to ensure that account lists are empty
        m_retrieveAccounts.clear();
        m_transmitAccounts.clear();

        // Get keys to avoid errors
        QMailAccountKey enabledKey(QMailAccountKey::status(QMailAccount::Enabled, QMailDataComparator::Includes));
        QMailAccountKey retrieveKey(QMailAccountKey::status(QMailAccount::CanRetrieve, QMailDataComparator::Includes));
        QMailAccountKey transmitKey(QMailAccountKey::status(QMailAccount::CanTransmit, QMailDataComparator::Includes));

        // Query accounts by capabilities
        m_retrieveAccounts = QMailStore::instance()->queryAccounts(enabledKey & retrieveKey);
        m_transmitAccounts = QMailStore::instance()->queryAccounts(enabledKey & transmitKey);
    }

    // Trigger accounts retrieving first
    if (!m_retrieveAccounts.isEmpty())
        synchronize(m_retrieveAccounts.takeFirst());
}

void MailManager::cancelSync()
{
    if (!isSynchronizing())
        return;

    m_cancelling = true;
    m_transmitAction->cancelOperation();
    m_retrievalAction->cancelOperation();
}

bool MailManager::isSynchronizing() const
{
    return (m_retrieving || m_transmitting);
}

void MailManager::progressChanged(uint value, uint total)
{
    qDebug() << "progress " << value << " of " << total;
}

void MailManager::activityChanged(QMailServiceAction::Activity  activity)
{
    QMailServiceAction *action = static_cast<QMailServiceAction*>(sender());
    const QMailServiceAction::Status status(action->status());

    switch (activity) {
        case QMailServiceAction::Failed:

            // don't try to synchronise extra accounts if the user cancelled the sync
            if (m_cancelling) {
                m_retrieving = false;
                m_transmitting = false;
                emit error(status.accountId, status.text, status.errorCode);
                return;
            } else {
                // Report the error
                emit error(status.accountId, status.text, status.errorCode);
            }
            // fallthrough; if this wasn't a fatal error, try sync again
        case QMailServiceAction::Successful:
            if (action == m_retrievalAction) {
                m_retrieving = false;
                if (m_retrieveAccounts.isEmpty()) {
                    emit retrievalCompleted();
                    // If there's no account to retrieve let's start sending msgs
                    if (!m_transmitAccounts.isEmpty())
                        sendMessages(m_transmitAccounts.takeFirst());
                }
                else
                    synchronize(m_retrieveAccounts.takeFirst());
            }
            else if (action == m_transmitAction) {
                m_transmitting = false;
                if (m_transmitAccounts.isEmpty())
                    emit sendCompleted();
                else
                    sendMessages(m_transmitAccounts.takeFirst());
            }

            if (!isSynchronizing())
                emit syncCompleted();

        default:
            qDebug() << "Activity State Changed:" << activity;
            break;
    }
}

void MailManager::moveMessages(const QMailMessageIdList &ids,
        const QMailFolderId &dest)
{
    Q_ASSERT(dest.isValid());

    QMailMessageMetaData message(ids.first());

    if (message.parentFolderId() != dest)
        m_storageAction->onlineMoveMessages(ids, dest);
}

void MailManager::deleteMessages(const QMailMessageIdList &ids)
{
    Q_ASSERT(!ids.empty());

    m_storageAction->deleteMessages(ids);
    emit messagesDeleted(ids);
}

void MailManager::flagMessages(const QMailMessageIdList &ids, quint64 setMask,
        quint64 unsetMask)
{
    Q_ASSERT(!ids.empty());

    m_storageAction->flagMessages(ids, setMask, unsetMask);
}

void MailManager::exportAccountChanges(const QMailAccountId id)
{
    qDebug() << "Exporting accounts changes";

    // Only add the account to the list if it's not
    // already there.
    if (id.isValid() &&
        !m_exportAccounts.contains(id)) {

        // Add account to the list
        m_exportAccounts.append(id);

        // Start the timer!
        if (!m_exportTimer.isActive() && !m_exporting)
            m_exportTimer.start();
    }

}

void MailManager::exportAccounts()
{
    qDebug() << "Exporting accounts to the server";

    // Mark as exporting if it's not
    if (!m_exporting) {
        m_exporting = true;
        m_exportTimer.stop();
    }

    // Export message updates
    if (!m_exportAccounts.isEmpty())
        m_exportAction->exportUpdates(m_exportAccounts.first());
}

void MailManager::exportActivityChanged(QMailServiceAction::Activity  activity)
{
    qDebug() << "exportActivityChanged " << activity;

    QMailServiceAction *action = static_cast<QMailServiceAction*>(sender());
    const QMailServiceAction::Status status(action->status());

    if (action && m_exporting) {
        if (activity == QMailServiceAction::Successful) {
            m_exporting = false;
            m_exportAccounts.removeFirst();

            // Check if there's more accounts to export
            if (!m_exportAccounts.isEmpty())
                exportAccounts();
        }
    }
}

uint MailManager::folderUnreadCount (const QMailFolderId &id)
{
    QMailMessageKey parentFolderKey(QMailMessageKey::parentFolderId(id));
    QMailMessageKey unreadKey(QMailMessageKey::status(QMailMessage::Read, QMailDataComparator::Excludes));
    return (QMailStore::instance()->countMessages(parentFolderKey & unreadKey));
}

uint MailManager::folderTotalCount (const QMailFolderId &id)
{
    QMailMessageKey parentFolderKey(QMailMessageKey::parentFolderId(id));
    return (QMailStore::instance()->countMessages(parentFolderKey));
}

QString MailManager::getRecentEmailTimeStamp (const QString &senderEmail)
{
    QMailMessageKey senderKey(QMailMessageKey::sender(senderEmail, QMailDataComparator::Includes));
    QMailMessageSortKey sortKey(QMailMessageSortKey::receptionTimeStamp(Qt::DescendingOrder));
    QMailMessageIdList ids = QMailStore::instance()->queryMessages(senderKey, sortKey, 1);
    if (ids.size() == 0)
    {
        return QString("");
    }

    QMailMessage message(ids[0]);
    QDateTime timeStamp = message.receivedDate().toLocalTime();
    return (timeStamp.toString("hh:mm dd MM yyyy"));
}
