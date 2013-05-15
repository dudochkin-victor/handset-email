/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MAILMANAGER_H
#define MAILMANAGER_H

#include <QObject>
#include <QTimer>

#include <qmailaccount.h>
#include <qmailserviceaction.h>

class QMailRetrievalAction;

class MailManager : public QObject
{
Q_OBJECT
Q_DISABLE_COPY(MailManager)

public:
    /**
     * Get singleton instance.
     */
    static MailManager *instance();

    ~MailManager();

    /** Determine whether a send/recieve is currently ongoing
      * @returns boolean true if an operation is ongoing, false otherwise
      */
    bool isSynchronizing() const;

signals:
    /** Signal emitted when recieve phase is complete.
      */
    void retrievalCompleted();

    /** Signal emitted when send phase is complete.
      */
    void sendCompleted();

    /** Signal emitted when the full synchronisation is complete.
      */
    void syncCompleted();

    /** Signal emitted when the full synchronisation is complete.
      */
    void messagesDeleted(QMailMessageIdList);
     
    /** Signal emitted when an error occurs during a send/recieve operation.
      * @param id The account ID the error occured on
      * @param msg The error message
      * @param code The error code
      *
      * (TODO: it might be an idea to directly expose the QMailServiceAction::Status?)
      */
    void error(const QMailAccountId &id, const QString &msg, int code);

public slots:
    /** Instructs MailManager to begin retrieving new messages for a given account.
      * @param id The account ID to retrieve messages for
      */
    void retrieveMessages(const QMailAccountId &id);

    /** Instructs MailManager to begin synchronize with the remote mail server for a given account.
      * @param id The account ID to retrieve messages for
      */
    void synchronize(const QMailAccountId &id);

    /** Instructs MailManager to begin retrieving more messages for a given folder.
      * @param folderId The folder ID to retrieve messages for
      */
    void retrieveMoreMessages(const QMailFolderId &id);

    /** Instructs MailManager to begin sending messages for a given account.
      * @param id The account ID to send messages for
      */
    void sendMessages(const QMailAccountId &id);

    /**
     * Slot to rename a folder
     * @param id folderid that is to be renamed
     * @param name new name to be given to folder, must not be empty
     */
    void renameFolder(const QMailFolderId &id, const QString &name);

    /**
     * Slot to delete a folder
     * This will also delete any subfolder and any messages
     * @param id the folder that is to be deleted
     */
    void deleteFolder(const QMailFolderId &id);

    /**
     * Slot to create a folder
     * @param name new folder's name
     * @param accountId the account the folder should be created in
     * @param parentId the containing folder if any
     *  (otherwise QMailFolderId() for a top-level folder)
     */
    void createFolder(const QString &name, const QMailAccountId &accountId,
        const QMailFolderId &parentId);

    /** Synchronise all accounts.
      */
    void accountsSync();

    /** Cancel a synchronisation operation.
      */
    void cancelSync();

    /**
     * Slot to move messages from a folder to another.
     * @param ids List of message ids to be moved.
     * @param dest Destination folder.
     */
    void moveMessages(const QMailMessageIdList &ids, const QMailFolderId &dest);

    /**
     * Slot to delete messages.
     * @param ids List of message ids to be deleted.
     */
    void deleteMessages(const QMailMessageIdList &ids);

    /**
     * Slot to flag messages
     * @param ids list of message ids to be flagged
     * @param setMask flag to add to messages
     * @param unsetMask flag to remove from messages
     */
    void flagMessages(const QMailMessageIdList &ids, quint64 setMask, quint64 unsetMask);

    /**
     * Slot to export message flags changes.
     * @param id The message account that will be synced.
     */
    void exportAccountChanges(const QMailAccountId id);

    /**
     * Get the accountids for all enabled accounts
     * @returns accountidlist of enabled accounts
     */
    QMailAccountIdList accountIdList() const;

    /**
     * Get the unread message count for folder
     * @returns the total number of unread messages currently known to messageserver for folder
     */
    uint folderUnreadCount (const QMailFolderId &id);

    /**
     * Get the total message count for folder
     * @returns the total number of messages currently known to messageserver for folder
     */
    uint folderTotalCount (const QMailFolderId &id);

    /**
     * Get the most recent email timestamp from sender
     * @returns the most recent email timestamp from send
     */
    QString getRecentEmailTimeStamp (const QString &sender);

private slots:
    void progressChanged(uint value, uint total);
    void activityChanged(QMailServiceAction::Activity  activity);
    void exportActivityChanged(QMailServiceAction::Activity  activity);
    void exportAccounts();

private:
    explicit MailManager(QObject *parent = 0);

    QTimer m_exportTimer;
    bool m_retrieving;
    bool m_transmitting;
    bool m_exporting;
    bool m_cancelling;
    QMailAccountIdList m_retrieveAccounts;
    QMailAccountIdList m_transmitAccounts;
    QMailAccountIdList m_exportAccounts;
    QMailRetrievalAction *const m_retrievalAction;
    QMailStorageAction *const m_storageAction;
    QMailTransmitAction *const m_transmitAction;
    QMailRetrievalAction *const m_exportAction;
    static MailManager *m_instance;
};
#endif // MAILMANAGER_H
