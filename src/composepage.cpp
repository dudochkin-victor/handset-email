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
#include <MPopupList>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <MWidgetCreator>
#include <MStylableWidget>
#include <MLabel>
#include <MTextEdit>
#include <MButton>
#include <MMessageBox>
#include <QFileDialog>
#include <QStringListModel>

#include <qmailstore.h>
#include <qmailfolder.h>
#include <qmailaddress.h>
#include <qmailnamespace.h>

#include "composepage.h"
#include "emailutils.h"
#include "mailmanager.h"
#include "contactpage.h"

Attachment::Attachment(const QString &fileName) : m_fileName(fileName)
{
    QFileInfo fi(m_fileName);
    m_displayName = fi.fileName();
}

Attachment::Attachment(const QMailMessagePart &messagePart) : m_mailPart(messagePart)
{
    m_displayName = m_mailPart.displayName();
}


DeleteButton::DeleteButton(EmailAddressType type, MStylableWidget* parent, const QMailAddress &address)
    : m_type (type), m_parent (parent), m_address (address)
{
    setIconID ("icon-m-common-remove");
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

DeleteButton::DeleteButton(MStylableWidget* parent, Attachment *attachment)
    : m_parent(parent), m_attachment(attachment)
{
    setIconID ("icon-m-common-remove");
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

ComposePage::ComposePage(const QMailMessageId &id, QMailMessage::ResponseType type)
    : MApplicationPage(), m_type(type), m_id (id)
{
    setContentsMargins(0, 0, 0, 0);
    setPannable(true);

    createActions();
    //% "Mail"
    setTitle(qtTrId("xx_page_title"));

    MLayout *layout = new MLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAnimation(0);

    m_policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    m_policy->setObjectName("VerticalPolicy");
    m_policy->setContentsMargins(0, 0, 0, 0);
    m_policy->setSpacing(0);

    // "From:" row
    QMailAccountKey emailKey(QMailAccountKey::messageType(QMailMessage::Email));
    m_mailAccounts = QMailStore::instance()->queryAccounts(emailKey);

    m_from = new MComboBox;
    //% "From"
    m_from->setTitle(qtTrId("xx_from"));

    int ci = 0;
    for (int i = 0; i < m_mailAccounts.size(); i++)
    {
        QMailAccountId id = m_mailAccounts[i];
        m_from->addItem(QMailStore::instance()->account(id).name());
        QMailAccount account(id);
        if (account.status() & QMailAccount::PreferredSender)
            ci = i;
    }
    m_from->setCurrentIndex(ci);
    m_policy->addItem(m_from);

    // "To" row
    m_toRow = new MStylableWidget();
    m_toRow->setObjectName("entryBox");
    QGraphicsLinearLayout *entryLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    m_toRow->setLayout(entryLayout);
    //% "To"
    MLabel *label = new MLabel(qtTrId("xx_to") + " ");
    entryLayout->addItem(label);
    m_to = new MTextEdit(MTextEditModel::SingleLine, QString());
    m_to->setObjectName ("textEdit");
    m_to->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QMailMessage msg(id);
    QMailAddress replyTo = msg.replyTo();

    if (type != QMailMessage::Forward && type != QMailMessage::NoResponse)
    {
        if (!replyTo.isNull())
        {
            // Use replyTo as the default recipient.
            m_toEmailAddresses << replyTo;
            if (type == QMailMessage::ReplyToAll)
                m_ccEmailAddresses << msg.from();
        }
        else
        {
            m_toEmailAddresses << msg.from();
        }
    }

    //% "Enter email address here"
    m_to->setPrompt(qtTrId("xx_enter_email_address_here"));
    m_to->setObjectName("textEntry");
    entryLayout->addItem(m_to);

    MButton *emailAddButton =  new MButton(m_toRow);
    emailAddButton->setIconID ("icon-m-common-add");
    emailAddButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (emailAddButton, SIGNAL(clicked(bool)), SLOT(chooseToContactFromList()));

    entryLayout->addItem(emailAddButton);
    m_policy->addItem(m_toRow);

    // Insert list of "To" recipients here, if any.
    foreach (QMailAddress address, m_toEmailAddresses)
        insertEmailAddressRow (ADDRESSTYPE_TO, m_toRow, address);

    // "Add Bcc" and "Add Cc" buttons
    m_ccButtonsRow = new MStylableWidget();
    m_ccButtonsRow->setObjectName("entryBox");
    entryLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    m_ccButtonsRow->setLayout(entryLayout);
    m_ccButtonsRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    //% "Add Cc"
    m_ccButton = new MButton(qtTrId("xx_add_cc"));
    connect (m_ccButton, SIGNAL(clicked(bool)), SLOT(addCcRow()));
    entryLayout->addItem(m_ccButton);

    //% "Add Bcc"
    m_bccButton = new MButton(qtTrId("xx_add_bcc"));
    connect (m_bccButton, SIGNAL(clicked(bool)), SLOT(addBccRow()));
    entryLayout->addItem(m_bccButton);

    m_policy->addItem(m_ccButtonsRow);
  
    m_cc = new MTextEdit(MTextEditModel::SingleLine, QString());
    m_bcc = new MTextEdit(MTextEditModel::SingleLine, QString());

    if (type == QMailMessage::ReplyToAll)
    {
        m_ccEmailAddresses << msg.cc();
        m_bccEmailAddresses << msg.bcc();
    }

    // "Subject" row
    MStylableWidget *entryBox = new MStylableWidget();
    entryBox->setObjectName("entryBox");
    entryLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    entryBox->setLayout(entryLayout);

    m_subject = new MTextEdit(MTextEditModel::SingleLine, QString());
    if (type == QMailMessage::Reply || type == QMailMessage::ReplyToAll)
    {
        //% "Re: %1"
        m_subject->setText(qtTrId("xx_reply_subject_line").arg(msg.subject()));
        m_subject->setCursorPosition(0);
    }
    else if (type == QMailMessage::Forward)
    {
        //% "Fwd: %1"
        m_subject->setText(qtTrId("xx_fwd_subject_line").arg(msg.subject()));
        m_subject->setCursorPosition(0);
    }
    else
        //% "Enter subject here"
        m_subject->setPrompt(qtTrId("xx_enter_subject_here"));

    m_subject->setObjectName("textEntry");
    m_subject->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    entryLayout->addItem(m_subject);
    
    m_policy->addItem(entryBox);

    // "Message Body" row
    entryBox = new MStylableWidget();
    entryBox->setObjectName("entryBox");
    entryLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    entryBox->setLayout(entryLayout);
    m_body= new MTextEdit(MTextEditModel::MultiLine, QString());
    if (type == QMailMessage::NoResponse)
        //% "Enter mail message here"
        m_body->setPrompt(qtTrId("xx_enter_mail_message_here"));
    else
    {
        QString bodyText("\n");
        if (type == QMailMessage::Reply || type == QMailMessage::ReplyToAll)
        {
            //% "On %1, %2 wrote:"
            bodyText.append(qtTrId("xx_message_header").arg(msg.receivedDate().toString())
                                                       .arg(msg.from().name()));
            QString text = "\n>" + Utils::bodyPlainText(&msg).replace('\n', "\n>");
            text.truncate(text.size() - 1);  // remove the extra ">" put there by QString.replace
            m_body->setText(bodyText + text);
        }
        else if (type == QMailMessage::Forward)
        {
            //% "------------ Forwarded Message ------------\n"
            bodyText.append(qtTrId("xx_forwarded_message"));
            QString date = msg.receivedDate().toString();
            //% "Date: %1\n"
            bodyText.append(qtTrId("xx_date_text").arg(msg.receivedDate().toString()));
            //% "From: %1\n"
            bodyText.append(qtTrId("xx_from_text").arg(msg.from().address()));
            QStringList tolist = QMailAddress::toStringList (msg.to());
            //% "To: %1\n"
            bodyText.append(qtTrId("xx_to_list").arg(tolist.join(";")));
            //% "Subject: %1\n\n"
            bodyText.append(qtTrId("xx_subject_text").arg(msg.subject()));
            m_body->setText(bodyText + Utils::bodyPlainText(&msg));
        }
        m_body->setCursorPosition(0);
    }
    entryLayout->addItem(m_body);

    m_policy->addItem(entryBox);

    // if forward, need to extract attachments from previous mail message and attach to this one
    if (type == QMailMessage::Forward)
        extractAttachmentsFromOldMail();


    // Add a "Send" button to the bottom of the page.
    //% "Send"
    m_sendButton = new MButton(qtTrId("xx_send"));
    connect(m_sendButton, SIGNAL(clicked(bool)), this, SLOT(sendMail()));
    m_policy->addItem(m_sendButton);
    centralWidget()->setLayout(layout);
}

ComposePage::~ComposePage()
{
}

void ComposePage::setRecipient(const QString & recipient)
{
    m_to->setText(recipient);
}

void ComposePage::createActions()
{
    // "send" action item
    //% "Send"
    MAction *action = new MAction(qtTrId("xx_send"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(sendMail()));

    // "save draft" action item
    //% "Save draft"
    action = new MAction(qtTrId("xx_save_draft"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(saveDraft()));

    // "attach file" action item
    //% "Attach file"
    action = new MAction(qtTrId("xx_attach_file"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(attachFile()));

    // "discard" action item
    //% "Discard message"
    action = new MAction(qtTrId("xx_discard_message"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(discard()));
}

void ComposePage::addCcRow()
{
   // "Cc" row
    m_ccRow = new MStylableWidget();
    m_ccRow->setObjectName("entryBox");
    QGraphicsLinearLayout *entryLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    m_ccRow->setLayout(entryLayout);
    m_ccRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //% "Cc "
    MLabel *label = new MLabel(qtTrId("xx_cc_label"));
    entryLayout->addItem(label);
    //% "Enter email address here"
    m_cc->setPrompt(qtTrId("xx_enter_email_address_here"));
    m_cc->setObjectName("textEntry");
    m_cc->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    entryLayout->addItem(m_cc);
    MButton *emailAddButton =  new MButton(m_ccRow);
    emailAddButton->setIconID ("icon-m-common-add");
    connect (emailAddButton, SIGNAL(clicked(bool)), SLOT(chooseCcContactFromList()));
    entryLayout->addItem(emailAddButton);
    m_policy->insertItem(m_policy->indexOf(m_ccButtonsRow), m_ccRow);

    delete m_ccButton;
    m_ccButton = NULL;

    if (m_bccButton == NULL)
        delete m_ccButtonsRow;

    // Insert list of "Cc" recipients here, if any.
    foreach (QMailAddress address, m_ccEmailAddresses)
        insertEmailAddressRow (ADDRESSTYPE_CC, m_ccRow, address);

}

void ComposePage::addBccRow()
{
    // "Bcc" row
    m_bccRow = new MStylableWidget();
    m_bccRow->setObjectName("entryBox");
    QGraphicsLinearLayout *entryLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    m_bccRow->setLayout(entryLayout);
    m_bccRow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //% "Bcc"
    MLabel *label = new MLabel(qtTrId("xx_bcc_label"));
    entryLayout->addItem(label);
    m_bcc = new MTextEdit(MTextEditModel::SingleLine, QString());
    //% "Enter email address here"
    m_bcc->setPrompt(qtTrId("xx_enter_email_address_here"));
    m_bcc->setObjectName("textEntry");
    m_bcc->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    entryLayout->addItem(m_bcc);
    MButton *emailAddButton =  new MButton(m_bccRow);
    emailAddButton->setIconID ("icon-m-common-add");
    emailAddButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect (emailAddButton, SIGNAL(clicked(bool)), SLOT(chooseBccContactFromList()));
    entryLayout->addItem(emailAddButton);
    m_policy->insertItem(m_policy->indexOf(m_ccButtonsRow), m_bccRow);

    delete m_bccButton;
    m_bccButton = NULL;

    if (m_ccButton == NULL)
        delete m_ccButtonsRow;

    // Insert list of "Bcc" recipients here, if any.
    foreach (QMailAddress address, m_bccEmailAddresses)
        insertEmailAddressRow (ADDRESSTYPE_BCC, m_bccRow, address);

}

void ComposePage::sendMail()
{
    QMailMessage message;

    QMailAccountId id = m_mailAccounts[m_from->currentIndex()];

    // set Destinations
    QStringList addresses = m_to->text().split(";", QString::SkipEmptyParts);
    foreach (QString address, addresses)
    {
        QMailAddress newAddress(address);
        m_toEmailAddresses.prepend(newAddress);
    }
    message.setTo(m_toEmailAddresses);

    addresses = m_cc->text().split(";", QString::SkipEmptyParts);
    foreach (QString address, addresses)
    {
        QMailAddress newAddress (address);
        m_ccEmailAddresses.prepend(newAddress);
    }
    message.setCc(m_ccEmailAddresses);

    addresses = m_bcc->text().split(";", QString::SkipEmptyParts);
    foreach (QString address, addresses)
    {
        QMailAddress newAddress (address);
        m_bccEmailAddresses.prepend(newAddress);
    }
    message.setBcc(m_bccEmailAddresses);

    // Set subject
    message.setSubject(m_subject->text());
    
    // Set message body
    QString messageText(m_body->text());
    QString signature = QMailAccount(id).signature();
    if (signature.size() > 0)
        messageText.append("\n" + signature);

    QMailMessageContentType type("text/plain; charset=UTF-8");

    if (m_attachments.length() == 0)
        message.setBody(QMailMessageBody::fromData(messageText, type, QMailMessageBody::Base64));
    else
    {
        QMailMessagePart body;
        body.setBody(QMailMessageBody::fromData(messageText.toUtf8(), type, QMailMessageBody::Base64));
        message.setMultipartType(QMailMessagePartContainer::MultipartMixed);
        message.appendPart(body);
    }

    // set message basic attributes
    message.setDate(QMailTimeStamp::currentDateTime());
    message.setStatus(QMailMessage::Outgoing, true);
    message.setStatus(QMailMessage::ContentAvailable, true);
    message.setStatus(QMailMessage::PartialContentAvailable, true);
    message.setStatus(QMailMessage::Read, true);
    message.setStatus((QMailMessage::Outbox | QMailMessage::Draft), true);

    message.setParentFolderId(QMailFolder::LocalStorageFolderId);

    if (!message.id().isValid())
        message.setStatus(QMailMessage::LocalOnly, true);

    if (id.isValid())
    {
        message.setParentAccountId(id);
        message.setFrom(QMailAccount(id).fromAddress());
    }

    // Set priority
    // TODO: set priority to priorityNormal for now. Need UI design to support the
    //       priority setting.
    message.appendHeaderField("X-Priority", "3");
    message.appendHeaderField("X-MSMail-Priority", "Normal");

    // Final touch for local only
    message.setMessageType(QMailMessage::Email);
    message.setSize(message.indicativeSize() * 1024);

    message = processAttachments(message);

    QMailAccount account (id);
        if ((account.status() & (QMailAccount::CanReferenceExternalData | QMailAccount::CanTransmitViaReference)) &&
        account.standardFolder(QMailFolder::SentFolder).isValid()) {
        message.setStatus(QMailMessage::TransmitFromExternal, true);
    }

    QList<QMailAddress> destinations(message.to());
    destinations.append(message.cc());
    destinations.append(message.bcc());

    if (destinations.count() > 0) {
        foreach (const QMailAddress dest, destinations) {
            if (!dest.isEmailAddress()) {
                //% "One or more destinations is invalid, send aborting"
                MMessageBox(qtTrId("xx_invalid_email_address")).exec();
                return;
            }
        }
    }
    else {
        //% "No destination specified, send aborting"
        MMessageBox(qtTrId("xx_no_destination_specified")).exec();
        return;
    }

    if (m_subject->text().size() == 0)
    {
        //% "The subject field is empty. Do you want to continue?"
        MMessageBox msg(qtTrId("xx_subject_empty"), M::YesButton | M::NoButton);
        if (msg.exec() == QMessageBox::No)
            return;
    }

    if (m_body->text().size() == 0)
    {
        //% "The message is empty. Do you want to send an empty message?"
        MMessageBox msg(qtTrId("xx_message_empty"), M::YesButton | M::NoButton);
        if (msg.exec() == QMessageBox::No)
            return;
    }

    bool stored = false;

    if (!message.id().isValid())
        stored = QMailStore::instance()->addMessage(&message);
    else
        stored = QMailStore::instance()->updateMessage(&message);

    MailManager *mailManager = MailManager::instance();

    if (stored && !mailManager->isSynchronizing())
    {
        connect(MailManager::instance(), SIGNAL(sendCompleted()), this, SLOT(onSendCompleted()));
        mailManager->sendMessages(message.parentAccountId());
    }
    else
        qDebug() << "Error queuing message, stored: " << stored << "isSynchronising: " << mailManager->isSynchronizing();
    emit mailSent();
    dismiss();
}

void ComposePage::onSendCompleted()
{
    MBanner *infoBanner = new MBanner();
    //% "Message sent"
    infoBanner->setSubtitle (qtTrId("xx_message_sent"));
    infoBanner->appear(MSceneWindow::DestroyWhenDone);
}

void ComposePage::saveDraft()
{
    // TODO:  Not implemented yet.
}

void ComposePage::attachFile()
{
    QDir homeDir = QDir::home();
    QStringList folders= homeDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    MPopupList *popupList = new MPopupList();
    QStringListModel *model = new QStringListModel(this);
    model->setStringList(folders);
    popupList->setItemModel(model);

    //% "Select a folder"
    popupList->setTitle(qtTrId("xx_select_a_folder"));
    popupList->appear();
    connect(popupList, SIGNAL(clicked(const QModelIndex&)), SLOT(folderSelected(const QModelIndex &)));
}

void ComposePage::folderSelected(const QModelIndex & index)
{
    MPopupList *popupList = qobject_cast<MPopupList *>(sender());
    if (!popupList)
        return;
  
    QStringListModel *model = (QStringListModel *) popupList->itemModel();
    QStringList folders = model->stringList();

    m_currentFolder = QDir::homePath() + "/" + folders[index.row()];
    QDir aDir(m_currentFolder);

    QStringList files = aDir.entryList(QDir::Files | QDir::Readable);

    MPopupList *filePopupList = new MPopupList();
    model = new QStringListModel(this);
    model->setStringList(files);
    filePopupList->setItemModel(model);

    //% "Select a file"
    filePopupList->setTitle(qtTrId("xx_select_a_file"));
    filePopupList->appear();
    connect(filePopupList, SIGNAL(clicked(const QModelIndex&)), SLOT(fileSelected(const QModelIndex &)));
}

void ComposePage::fileSelected(const QModelIndex & index)
{
    MPopupList *popupList = qobject_cast<MPopupList *>(sender());
    if (!popupList)
        return;
  
    QStringListModel *model = (QStringListModel *) popupList->itemModel();
    QStringList fileList = model->stringList();

    QString filePath = m_currentFolder + "/" + fileList[index.row()];
    insertAttachment(new Attachment(filePath));
}

void ComposePage::discard()
{
    //% "Do you want to save a draft?"
    MMessageBox msg(qtTrId("xx_save_draft_question"), M::YesButton | M::NoButton);

    if (msg.exec() == QMessageBox::Yes)
        saveDraft();;

    dismiss();
}

void ComposePage::chooseToContactFromList()
{
   ContactPage *contact = new ContactPage;
   connect (contact, SIGNAL(emailsSelected(QStringList)), this, SLOT(toContactClicked(QStringList)));
   contact->appear(MSceneWindow::DestroyWhenDone);
}

void ComposePage::chooseCcContactFromList()
{
   ContactPage *contact = new ContactPage;
   connect (contact, SIGNAL(emailsSelected(QStringList)), this, SLOT(ccContactClicked(QStringList)));
   contact->appear(MSceneWindow::DestroyWhenDone);
}

void ComposePage::chooseBccContactFromList()
{
   ContactPage *contact = new ContactPage;
   connect (contact, SIGNAL(emailsSelected(QStringList)), this, SLOT(bccContactClicked(QStringList)));
   contact->appear(MSceneWindow::DestroyWhenDone);
}

void ComposePage::insertEmailAddressRow(EmailAddressType type, 
                                        MStylableWidget* parent, const QMailAddress &address)
{
    MStylableWidget *entryBox = new MStylableWidget();
    entryBox->setObjectName("entryBox");
    QGraphicsGridLayout *grid = new QGraphicsGridLayout;
    entryBox->setLayout(grid);
    entryBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    MLabel *label = new MLabel("   ");
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(label, 0, 0, Qt::AlignLeft);
    label = new MLabel(QMailAddress::removeComments(address.address()));
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    grid->addItem(label, 0, 1, Qt::AlignLeft);
    DeleteButton *deleteButton = new DeleteButton(type, entryBox, address);
    connect (deleteButton, SIGNAL(clicked(bool)), SLOT(deleteContact()));
    grid->addItem(deleteButton, 0, 2, Qt::AlignRight);
    m_policy->insertItem(m_policy->indexOf(parent)+1, entryBox);
}

void ComposePage::deleteContact()
{
    DeleteButton *button = qobject_cast<DeleteButton *>(sender());
    if (button)
    {
        delete button->parent();
        EmailAddressType type = button->type();
        if (type == ADDRESSTYPE_TO)
            m_toEmailAddresses.removeOne(button->address());
        else if (type == ADDRESSTYPE_CC)
            m_ccEmailAddresses.removeOne(button->address());
        else if (type == ADDRESSTYPE_BCC)
            m_bccEmailAddresses.removeOne(button->address());
    }
}

void ComposePage::toContactClicked(const QStringList& emails)
{
    if (emails.size() > 1)
    {
          MPopupList *popuplist = new MPopupList();
          QStringListModel *model = new QStringListModel(this);
          model->setStringList(emails);
          popuplist->setItemModel(model);
          //% "Select an email"
          popuplist->setTitle(qtTrId("xx_select_an_email"));
          popuplist->appear();
          connect(popuplist, SIGNAL(clicked(const QModelIndex&)), SLOT(addToContact(const QModelIndex &)));
    }
    else
    {
        QMailAddress address(emails[0]);
        m_toEmailAddresses.prepend(address);
        insertEmailAddressRow (ADDRESSTYPE_TO, m_toRow, address);
    }
}

void ComposePage::addToContact(const QModelIndex& index)
{
    MPopupList *popupList = qobject_cast<MPopupList *>(sender());
    if (!popupList)
        return;
    
    QStringListModel *model = (QStringListModel *) popupList->itemModel();
    QStringList emails = model->stringList();
    
    QMailAddress address(emails[index.row()]);
    m_toEmailAddresses.prepend(address);
    insertEmailAddressRow (ADDRESSTYPE_TO, m_toRow, address);
}

void ComposePage::ccContactClicked(const QStringList& emails)
{
    if (emails.size() > 1)
    {
          MPopupList *popuplist = new MPopupList();
          QStringListModel *model = new QStringListModel(this);
          model->setStringList(emails);
          popuplist->setItemModel(model);
          //% "Select an email"
          popuplist->setTitle(qtTrId("xx_select_an_email"));
          popuplist->appear();
          connect(popuplist, SIGNAL(clicked(const QModelIndex&)), SLOT(addCcContact(const QModelIndex &)));
    }
    else
    {
        QMailAddress address(emails[0]);
        m_toEmailAddresses.prepend(address);
        insertEmailAddressRow (ADDRESSTYPE_CC, m_ccRow, address);
    }
}

void ComposePage::addCcContact(const QModelIndex& index)
{
    MPopupList *popupList = qobject_cast<MPopupList *>(sender());
    if (!popupList)
        return;

    QStringListModel *model = (QStringListModel *) popupList->itemModel();
    QStringList emails = model->stringList();

    QMailAddress address(emails[index.row()]);
    m_toEmailAddresses.prepend(address);
    insertEmailAddressRow (ADDRESSTYPE_CC, m_ccRow, address);
}

void ComposePage::bccContactClicked(const QStringList& emails)
{
    if (emails.size() > 1)
    {
          MPopupList *popuplist = new MPopupList();
          QStringListModel *model = new QStringListModel(this);
          model->setStringList(emails);
          popuplist->setItemModel(model);
          //% "Select an email"
          popuplist->setTitle(qtTrId("xx_select_an_email"));
          popuplist->appear();
          connect(popuplist, SIGNAL(clicked(const QModelIndex&)), SLOT(addBccContact(const QModelIndex &)));
    }
    else
    {
        QMailAddress address(emails[0]);
        m_toEmailAddresses.prepend(address);
        insertEmailAddressRow (ADDRESSTYPE_BCC, m_bccRow, address);
    }
}

void ComposePage::addBccContact(const QModelIndex& index)
{
    MPopupList *popupList = qobject_cast<MPopupList *>(sender());
    if (!popupList)
        return;

    QStringListModel *model = (QStringListModel *) popupList->itemModel();
    QStringList emails = model->stringList();

    QMailAddress address(emails[index.row()]);
    m_toEmailAddresses.prepend(address);
    insertEmailAddressRow (ADDRESSTYPE_BCC, m_bccRow, address);
}

void ComposePage::insertAttachment(Attachment *attachment)
{
    MStylableWidget *entryBox = new MStylableWidget();
    entryBox->setObjectName("entryBox");
    QGraphicsGridLayout *grid = new QGraphicsGridLayout;
    entryBox->setLayout(grid);
    entryBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //% "Attachment: "
    MLabel *label = new MLabel(qtTrId("xx_attachment"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem(label, 0, 0, Qt::AlignLeft);

    label = new MLabel(attachment->displayName());
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    grid->addItem(label, 0, 1, Qt::AlignLeft);
    DeleteButton *deleteButton = new DeleteButton(entryBox, attachment);
    connect (deleteButton, SIGNAL(clicked(bool)), SLOT(deleteAttachment()));
    grid->addItem(deleteButton, 0, 2, Qt::AlignRight);
    m_policy->insertItem(m_policy->indexOf(m_sendButton), entryBox);
    m_attachments << attachment;
}

void ComposePage::insertAttachment(const QString &file)
{
    insertAttachment(new Attachment(file));
}

void ComposePage::deleteAttachment()
{
    DeleteButton *button = qobject_cast<DeleteButton *>(sender());
    if (button)
    {
        delete button->parent();
        Attachment *attachment = button->attachment();
        m_attachments.removeOne(attachment);
        delete attachment;
    }
}

QMailMessage ComposePage::processAttachments(QMailMessage message)
{
    // Add attachment(s) as necessary
    QMailMessagePart attachmentpart;

    foreach (Attachment *attachment, m_attachments) {
        if (!attachment->isFile())
        {
            // Attaching a file
            QFileInfo fi(attachment->fileName());

            // Just in case..
            if (!fi.isFile())
                continue;

            QMailMessageContentType attachmenttype(QMail::mimeTypeFromFileName(attachment->fileName()).toLatin1());
            attachmenttype.setName(fi.fileName().toLatin1());

            QMailMessageContentDisposition disposition(QMailMessageContentDisposition::Attachment);
            disposition.setFilename(fi.fileName().toLatin1());
            disposition.setSize(fi.size());

            attachmentpart = QMailMessagePart::fromFile(attachment->fileName(),
                                                        disposition,
                                                        attachmenttype,
                                                        QMailMessageBody::Base64,
                                                        QMailMessageBody::RequiresEncoding);
            message.appendPart(attachmentpart);
        }    
        else
        { 
            // Forwarding a messagepart
            QMailMessage oldMail(m_id);
            QMailAccount originallyFrom(oldMail.parentAccountId());
            QMailAccount nowFrom(message.parentAccountId());
            bool viaReference = ((originallyFrom.status() & QMailAccount::CanReferenceExternalData) &&
                                 nowFrom.status() & QMailAccount::CanTransmitViaReference);

            QMailMessageContentDisposition disposition(QMailMessageContentDisposition::Attachment);
            const QMailMessagePart &sourcePart(attachment->messagePart());

            qDebug() << "Examining source part " << sourcePart.location().toString(true);

            if (viaReference) {
                qDebug() << "Appending " << sourcePart.body().data() << " by reference";
                message.appendPart(QMailMessagePart::fromPartReference(sourcePart.location(),
                                                                       disposition,
                                                                       sourcePart.contentType(),
                                                                       sourcePart.transferEncoding()));
            } else {
                QByteArray partData(sourcePart.body().data(QMailMessageBody::Decoded));

                qDebug() << "Appending data length " << partData.length() << " non-reference, " << sourcePart.contentType().toString();
                message.appendPart(QMailMessagePart::fromData(partData,
                                                              disposition,
                                                              sourcePart.contentType(),
                                                              QMailMessageBody::Base64));
            }
        }    
    }
    return message;
}

void ComposePage::extractAttachmentsFromOldMail()
{
    QMailMessage message(m_id);

    if (!message.status() & QMailMessageMetaData::HasAttachments)
        return;

    for (uint i = 1; i < message.partCount(); ++i)
    {
        QMailMessagePart sourcePart = message.partAt(i);
        if (!(sourcePart.multipartType() == QMailMessagePartContainer::MultipartNone))
            continue;
        insertAttachment(new Attachment(sourcePart));
    }
}
