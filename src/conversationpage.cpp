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
#include <MBanner>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MLabel>
#include <MPopupList>
#include <MSeparator>
#include <QDir>
#include <QGraphicsLinearLayout>
#include <QStringListModel>

#include <qmailstore.h>
#include <qmailfolder.h>
#include <qmailserviceaction.h>

#include "conversationpage.h"
#include "emailutils.h"
#include "mailmanager.h"
#include "emailapplication.h"

ConversationPage::ConversationPage(QMailMessageId id)
    : MApplicationPage(),
      m_id(id)
{
}

ConversationPage::~ConversationPage()
{
}

void ConversationPage::createContent()
{
    MApplicationPage::createContent();
    setContentsMargins(0, 0, 0, 0);
    setPannable(true);

    createActions();
    //% "Mail"
    setTitle(qtTrId("xx_page_title"));

    MLayout *layout = new MLayout(centralWidget());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAnimation(0);

    m_policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    m_policy->setObjectName("VerticalPolicy");
    m_policy->setContentsMargins(0, 0, 0, 0);
    m_policy->setSpacing(0);

    QMailMessage message(m_id);
    //% "From"
    QString body = qtTrId("xx_from") + ": " + message.from().address() + "\n";
    QStringList addresses = QMailAddress::toStringList(message.to());
    //% "To"
    body.append(qtTrId("xx_to") + ": "  + addresses.join("; ") + "\n");
    addresses = QMailAddress::toStringList(message.cc());
    if (addresses.size() > 0)
        //% "Cc"
        body.append(qtTrId("xx_cc" ) + ": " + addresses.join("; ") + "\n");
    addresses = QMailAddress::toStringList(message.bcc());
    if (addresses.size() > 0)
        //% "Bcc"
        body.append(qtTrId("xx_bcc" ) + ": " + addresses.join("; ") + "\n");

    QMailAddress replyTo = message.replyTo();
    if (!replyTo.isNull())
        //% "Reply-To"
        body.append(qtTrId("xx_reply_to") + ": " + replyTo.toString() + "\n");

    //% "Subject"
    body.append(qtTrId("xx_subject") + ": " + message.subject() + "\n");
    //% "Date: %1"
    body.append(qtTrId("xx_date").arg(message.date().toLocalTime().toString(Qt::SystemLocaleShortDate)));
    MLabel *messageBody = new MLabel(body);
    messageBody->setWordWrap(true);
    messageBody->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    messageBody->setObjectName("BodyText");
    m_policy->addItem(messageBody);

    MSeparator *separator = new MSeparator();
    separator->setObjectName("Separator");
    m_policy->addItem(separator);

    messageBody = new MLabel("\n" + Utils::bodyPlainText(&message));
    messageBody->setWordWrap(true);
    messageBody->setObjectName("BodyText");
    messageBody->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_policy->addItem (messageBody);
    processAttachments(message);

    centralWidget()->setLayout(layout);
}

void ConversationPage::createActions()
{
    //% "Delete"
    MAction *action = new MAction(qtTrId("xx_delete_action"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(deleteActionTriggered()));

    //% "Reply"
    action = new MAction(qtTrId("xx_reply_action"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(replyActionTriggered()));

    //% "Reply to all"
    action = new MAction(qtTrId("xx_reply_all_action"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(replyAllActionTriggered()));

    //% "Forward"
    action = new MAction(qtTrId("xx_forward_action"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(forwardActionTriggered()));

    // "Delete" context action item
    action = new MAction(qtTrId("xx_delete_action"), this);
    action->setLocation(MAction::ObjectMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(deleteActionTriggered()));

    // "Reply" context action item
    action = new MAction(qtTrId("xx_reply_action"), this);
    action->setLocation(MAction::ObjectMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(replyActionTriggered()));

    // "Reply to all" context action item
    action = new MAction(qtTrId("xx_reply_all_action"), this);
    action->setLocation(MAction::ObjectMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(replyAllActionTriggered()));

    // "Forward" context action item
    action = new MAction(qtTrId("xx_forward_action"), this);
    action->setLocation(MAction::ObjectMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(forwardActionTriggered()));
}

void ConversationPage::processAttachments(const QMailMessage &message)
{
    if (!message.status() & QMailMessageMetaData::HasAttachments)
        return;

    connect(this, SIGNAL(downloadCompleted()), this, SLOT(saveAttachment()));

    bool oneTimeFlag = true;
    for (uint i = 1; i < message.partCount(); ++i)
    {
        QMailMessagePart sourcePart = message.partAt(i);
        if (!(sourcePart.multipartType() == QMailMessagePartContainer::MultipartNone))
            continue;

        if (oneTimeFlag)
        {
            MSeparator *separator = new MSeparator();
            separator->setObjectName("Separator");
            m_policy->addItem(separator);
            oneTimeFlag = false;
        }

        MStylableWidget *w = new MStylableWidget(this);
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Horizontal);
        w->setLayout(layout);
        //% "Attached: "
        MLabel *label = new MLabel(qtTrId("xx_attached"));
        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        label->setObjectName ("AttachmentText");
        layout->addItem(label);
        MButton *button = new MButton(sourcePart.displayName());
        button->setObjectName ("AttachmentButton");
        button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        layout->addItem(button);

        //% "Download..."
        AttachmentAction *action = new AttachmentAction(qtTrId("xx_download_context_menu"), button, sourcePart);
        connect(action, SIGNAL(triggered()), this, SLOT(openAttachmentDownloadDialog()));

        //% "Open..."
        action = new AttachmentAction(qtTrId("xx_open_context_menu"), button, sourcePart);
        connect(action, SIGNAL(triggered()), this, SLOT(openAttachmentOpenDialog()));
        m_policy->addItem (w);
    }
}

void ConversationPage::saveAttachment()
{
    QMailMessage message(m_id);
    for (uint i = 1; i < message.partCount(); ++i)
    {
        const QMailMessagePart &sourcePart(message.partAt(i));
        if (sourcePart.location().toString(false) == m_selectedAttachment.location().toString(false))
        {
           sourcePart.writeBodyTo(m_downloadFolder);
        }
    }
    MBanner *infoBanner = new MBanner();
    //% "Download completed"
    infoBanner->setSubtitle (qtTrId("xx_download_completed"));
    infoBanner->appear(MSceneWindow::DestroyWhenDone);
}

void ConversationPage::activityChanged(QMailServiceAction::Activity activity)
{
    if (QMailServiceAction *action = static_cast<QMailServiceAction*>(sender()))
    {
        if (activity == QMailServiceAction::Successful)
        {
            if (action == m_retrievalAction)
            {
                emit downloadCompleted();
            }
        }
    }
}

void ConversationPage::deleteActionTriggered()
{

    QMailMessageIdList ids;
    ids << m_id;
    MailManager::instance()->deleteMessages(ids);
    dismiss();
}

void ConversationPage::forwardActionTriggered()
{
    EmailApplication::instance()->composeForward(m_id);
}

void ConversationPage::replyActionTriggered()
{
    EmailApplication::instance()->composeReply(m_id);
}

void ConversationPage::replyAllActionTriggered()
{
    EmailApplication::instance()->composeReplyAll(m_id);
}

void ConversationPage::openAttachmentDownloadDialog()
{
    AttachmentAction *action = qobject_cast<AttachmentAction *>(sender());
    if (!action)
        return;

    m_selectedAttachment = action->part();  // saves the message part

    MPopupList *popuplist = new MPopupList();
    QStringListModel *model = new QStringListModel(this);

    QDir homeDir = QDir::home();
    QStringList folders= homeDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    model->setStringList(folders);
    popuplist->setItemModel(model);
    //% "Select a folder"
    popuplist->setTitle(qtTrId("xx_select_a_folder"));
    popuplist->appear();
    connect(popuplist, SIGNAL(clicked(const QModelIndex&)), SLOT(folderSelected(const QModelIndex &)));
}

void ConversationPage::folderSelected(const QModelIndex& index)
{
    MPopupList *popupList = qobject_cast<MPopupList *>(sender());
    if (!popupList)
        return;
   
    QStringListModel *model = (QStringListModel *) popupList->itemModel();
    QStringList folders = model->stringList();

    m_downloadFolder = QDir::homePath() + "/" + folders[index.row()];

    QMailMessagePart::Location location = m_selectedAttachment.location();
    location.setContainingMessageId(m_id);

    if (m_selectedAttachment.hasBody())
    {
        // The content has already been downloaded to local device. No need to retrieve part from server.
        emit downloadCompleted();
        return;
    }

    m_retrievalAction = new QMailRetrievalAction(this);
    connect(m_retrievalAction, SIGNAL(activityChanged(QMailServiceAction::Activity)), 
                               this, SLOT(activityChanged(QMailServiceAction::Activity)));
    m_retrievalAction->retrieveMessagePart(location);
}

void ConversationPage::openAttachmentOpenDialog()
{
    MPopupList *popuplist = new MPopupList();
    QStringListModel *model = new QStringListModel(this);

    QDir homeDir = QDir::home();

    // Hack: hardcoded the list of applications for the 1.1 release
    QStringList apps = QStringList()
                          //% "Music player"
                          << qtTrId("xx_music_player")
                          //% "Photo viewer"
                          << qtTrId("xx_photo_viewer")
                          //% "Video player"
                          << qtTrId("xx_video player");


    model->setStringList(apps);
    popuplist->setItemModel(model);
    //% "Select an application"
    popuplist->setTitle(qtTrId("xx_select_an_application"));
    popuplist->appear();
    connect(popuplist, SIGNAL(clicked(const QModelIndex&)), SLOT(applicationSelected(const QModelIndex &)));

}

void ConversationPage::applicationSelected(const QModelIndex &index)
{

    MPopupList *popupList = qobject_cast<MPopupList *>(sender());
    if (!popupList)
        return;

    int selection = index.row();
    if (selection == 0)  // music
    {
        //Todo:  add code to invoke music player
    }
    else if (selection == 1) // photo
    {
        //Todo:  add code to invoke photo viewer
    }
    else if (selection == 2) // video
    {
        //Todo:  add code to invoke video player
    }
}
