/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef CONVERSATIONPAGE_H
#define CONVERSATIONPAGE_H

#include <MApplicationPage>
#include <MLinearLayoutPolicy>
#include <MAction>
#include <MButton>

#include <qmailfolder.h>
#include <qmailmessage.h>
#include <qmailserviceaction.h>
#include <QModelIndex>

class ConversationPage : public MApplicationPage
{
    Q_OBJECT

public:
    ConversationPage(QMailMessageId messageId);
    virtual ~ConversationPage();
    virtual void createContent();

signals:
    void downloadCompleted();

private slots:
    void deleteActionTriggered();
    void forwardActionTriggered();
    void replyActionTriggered();
    void replyAllActionTriggered();
    void activityChanged(QMailServiceAction::Activity);

    void openAttachmentDownloadDialog();
    void openAttachmentOpenDialog();
    void folderSelected(const QModelIndex& index);
    void applicationSelected(const QModelIndex& index);
    void saveAttachment();


private:
    void createActions();
    void processAttachments(const QMailMessage &);

    MLinearLayoutPolicy *m_policy;
    QString m_downloadFolder;
    QMailMessageId m_id;
    QMailMessagePart m_selectedAttachment;
    QMailRetrievalAction  *m_retrievalAction;
};

// helper class to handle attachment
class AttachmentAction : public MAction
{
    Q_OBJECT

public:
    AttachmentAction (const QString& text, MButton *parent,  QMailMessagePart part)
        : MAction (text, parent)
    {
        m_part = part;
        setLocation(MAction::ObjectMenuLocation);
        parent->addAction(this);
    }

    QMailMessagePart part() { return m_part; }

private:
    QMailMessagePart m_part ;
};

#endif // CONVERSATIONPAGE_H
