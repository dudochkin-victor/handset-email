/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <MApplicationPage>
#include <qmailstore.h>

#include "accountsetuppage.h"
#include "emailapplication.h"
#include "emailsettingspage.h"
#include "servercontroller.h"
#include "editexistingaccountpage.h"
#include "folderpage.h"
#include "messagelistpage.h"
#include "conversationpage.h"
#include "manualaccounteditpage.h"
#include "reviewaccountpage.h"
#include "mailmanager.h"

EmailApplication::EmailApplication(int &argc, char **argv)
    : MApplication(argc, argv), m_folderPage(NULL), m_composePage(NULL),
      m_accountSetupPage(NULL)
{

    // Set the organization name and app name so QSettings will
    // will know where to look for our settings
    QCoreApplication::setOrganizationName("Meego");
    QCoreApplication::setApplicationName("MeegoHandsetEmail");

    // Make sure the message server is running
    if (!ServerController::instance()->isRunning()) {
        if (!ServerController::instance()->startServer()) {
            qFatal("Message Server could not be started!");
        }
    }

    m_Window = new MApplicationWindow();
    m_Window->show();
}

EmailApplication *EmailApplication::instance()
{
    return qobject_cast<EmailApplication*>(MApplication::instance());
}

void EmailApplication::showFolderPage()
{
    if (m_folderPage == NULL)
    {
        m_folderPage = new FolderPage();
        connect (m_folderPage, SIGNAL(composeMail()), this, 
                 SLOT(showComposePage()));
        connect (m_folderPage, SIGNAL(listMessagesByMailId(QMailFolderId)), this, 
                 SLOT(showMessageListPage(QMailFolderId)));
        connect (m_folderPage, SIGNAL(emailSettingsTriggered()), this, 
                 SLOT(showEmailSettingsPage()));
        m_folderPage->appear();
    }
}

void EmailApplication::showAccountSetupPage()
{
    m_accountSetupPage = new AccountSetupPage();
    connect (m_accountSetupPage, SIGNAL(manualAccountEdit(AccountInfo *)), this, 
             SLOT(showManualAccountEditPage(AccountInfo *)));
    connect (m_accountSetupPage, SIGNAL(verifyAccount(AccountInfo *)), this, 
             SLOT(showReviewAccountPage(AccountInfo *)));
    connect (m_accountSetupPage, SIGNAL(emailSettingsTriggered()), this, 
             SLOT(showEmailSettingsPage()));
    m_accountSetupPage->appear(MSceneWindow::DestroyWhenDismissed);
}


void EmailApplication::showComposePage(QString recipient, QStringList attachments)
{
    m_composePage = new ComposePage(QMailMessageId());
    m_composePage->setRecipient(recipient);
    foreach (QString attachment, attachments)
        m_composePage->insertAttachment(attachment);
    m_composePage->appear();
}

void EmailApplication::showMessageListPage(QMailFolderId folderId)
{
    MApplicationPage *messageListPage = new MessageListPage(folderId);
    connect (messageListPage, SIGNAL(composeMail()), this, 
             SLOT(showComposePage()));
    connect (messageListPage, SIGNAL(composeReply(QMailMessageId)), this, 
             SLOT(composeReply(QMailMessageId)));
    connect (messageListPage, SIGNAL(composeReplyAll(QMailMessageId)), this, 
             SLOT(composeReplyAll(QMailMessageId)));
    connect (messageListPage, SIGNAL(composeForward(QMailMessageId)), this, 
             SLOT(composeForward(QMailMessageId)));
    connect (messageListPage, SIGNAL(showMessageBody(QMailMessageId)), this, 
             SLOT(showConversationPage(QMailMessageId)));
    messageListPage->appear();
}

void EmailApplication::showConversationPage(QMailMessageId messageId)
{
    MApplicationPage *conversationPage = new ConversationPage(messageId);
    conversationPage->appear();
}

void EmailApplication::showManualAccountEditPage(AccountInfo *account)
{
    MApplicationPage *manualAccountEditPage = new ManualAccountEditPage(account);
    connect (manualAccountEditPage, SIGNAL(verifyAccount(AccountInfo *)), this, 
             SLOT(showReviewAccountPage(AccountInfo *)));
    manualAccountEditPage->appear();
}

void EmailApplication::showReviewAccountPage(AccountInfo *account)
{
    MApplicationPage *reviewAccountPage = new ReviewAccountPage (account);
    connect (account, SIGNAL(accountReady()), this, SLOT(showFolderPage()));
    connect (reviewAccountPage, SIGNAL(manualAccountEdit(AccountInfo *)), this, 
             SLOT(showManualAccountEditPage(AccountInfo *)));
    reviewAccountPage->appear(MSceneWindow::DestroyWhenDismissed);
}
void EmailApplication::showEditExistingAccountPage(QMailAccountId id)
{
    MApplicationPage *editExistingAccountPage = new EditExistingAccountPage (id);
    editExistingAccountPage->appear(MSceneWindow::DestroyWhenDismissed);
}

void EmailApplication::showEmailSettingsPage()
{
    if (!m_emailSettingsPage)
    {
        m_emailSettingsPage = EmailSettingsPage::instance ();
        connect (m_emailSettingsPage, SIGNAL(addNewAccount()), this, SLOT(showAccountSetupPage()));
        connect (m_emailSettingsPage, SIGNAL(editExistingAccount(QMailAccountId)), this, SLOT(showEditExistingAccountPage(QMailAccountId)));
    }
    m_emailSettingsPage->appear();
}

void EmailApplication::composeReply (QMailMessageId id)
{
    m_composePage = new ComposePage(id, QMailMessage::Reply);
    m_composePage->appear();
}

void EmailApplication::composeReplyAll (QMailMessageId id)
{
    m_composePage = new ComposePage(id, QMailMessage::ReplyToAll);
    m_composePage->appear();
}

void EmailApplication::composeForward (QMailMessageId id)
{
    m_composePage = new ComposePage(id, QMailMessage::Forward);
    m_composePage->appear();
}

void EmailApplication::addEmailAccount(AccountInfo *account)
{
    m_accountList << account;
}

void EmailApplication::deleteEmailAccount(AccountInfo *account)
{
    m_accountList.removeOne(account);
    delete account;
}
