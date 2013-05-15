/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef MESSAGELISTPAGE_H
#define MESSAGELISTPAGE_H

#include <MApplicationPage>
#include <MList>
#include <qmailfolder.h>
#include <qmailmessage.h>

#include "messageitemmodel.h"

class MessageListPage : public MApplicationPage
{
    Q_OBJECT

public:
    MessageListPage();
    MessageListPage(QMailFolderId folderId);
    virtual ~MessageListPage();
    virtual void createContent();

signals:
    void showMessageBody(QMailMessageId);
    void composeMail();
    void composeReply(QMailMessageId);
    void composeReplyAll(QMailMessageId);
    void composeForward(QMailMessageId);

private slots:
    void composeActionTriggered();
    void refreshActionTriggered();
    void getMoreMessages();
    void update();

    void messageItemClicked(const QModelIndex &);
    void deleteMessages(const QMailMessageIdList &);
    void addMessages(const QMailMessageIdList &);

private:
    void createActions();

private:
    QMailFolderId    m_folderId;
    MessageItemModel *m_model;
    QModelIndex      m_currentIndex;
};

#endif // MESSAGELISTPAGE_H
