/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef COMPOSEPAGE_H
#define COMPOSEPAGE_H

#include <MApplicationPage>
#include <MLinearLayoutPolicy>
#include <MStylableWidget>
#include <MButton>
#include <MTextEdit>
#include <MComboBox>

#include <qmailstore.h>
#include <qmailmessage.h>
#include <qmailaccount.h>

class Attachment;

typedef enum _EmailAddressType
{
    ADDRESSTYPE_TO  = 0,
    ADDRESSTYPE_CC  = 1,
    ADDRESSTYPE_BCC = 2,
} EmailAddressType;

class ComposePage : public MApplicationPage
{
    Q_OBJECT

public:
    explicit ComposePage(const QMailMessageId &, QMailMessage::ResponseType type=QMailMessage::NoResponse);
    virtual ~ComposePage();

    void setRecipient(const QString & emailAddress);
    void insertAttachment(const QString &file);
    void insertAttachment(Attachment *attachment);

signals:
    void mailSent();

private slots:
    void addBccRow();
    void addCcRow();
    void attachFile();
    void addToContact(const QModelIndex &);
    void addCcContact(const QModelIndex &);
    void addBccContact(const QModelIndex &);
    void chooseToContactFromList();
    void chooseCcContactFromList();
    void chooseBccContactFromList();
    void toContactClicked(const QStringList &);
    void ccContactClicked(const QStringList &);
    void bccContactClicked(const QStringList &);
    void deleteContact();
    void discard();
    void saveDraft();
    void sendMail();
    void deleteAttachment();
    void folderSelected(const QModelIndex &);
    void fileSelected(const QModelIndex &);
    void onSendCompleted();

private:
    void createActions();
    void insertEmailAddressRow(EmailAddressType type, MStylableWidget* parent, const QMailAddress& address);
    QMailMessage processAttachments(QMailMessage message);
    void extractAttachmentsFromOldMail();

    QMailMessage::ResponseType m_type;
    QMailMessageId m_id;
    QMailAccountIdList m_mailAccounts;

    MLinearLayoutPolicy *m_policy;
    MStylableWidget *m_toRow;
    MStylableWidget *m_ccRow;
    MStylableWidget *m_bccRow;
    MStylableWidget *m_ccButtonsRow;

    QList<QMailAddress>  m_toEmailAddresses;
    QList<QMailAddress>  m_ccEmailAddresses;
    QList<QMailAddress>  m_bccEmailAddresses;
    QList<Attachment *>  m_attachments;
    QString  m_currentFolder;

    MButton   *m_ccButton;
    MButton   *m_bccButton;
    MButton   *m_sendButton;
    MTextEdit *m_to;
    MTextEdit *m_bcc;
    MTextEdit *m_cc;
    MTextEdit *m_subject;
    MTextEdit *m_body;
    MComboBox *m_from;
};

class Attachment
{
public:
    Attachment (const QString &fileName);
    Attachment (const QMailMessagePart & messagePart);
    ~Attachment() {}

    QString &displayName() { return m_displayName; }
    bool isFile()          { return (m_fileName.isEmpty() ? true : false); }
    QString &fileName()    { return m_fileName; }
    QMailMessagePart messagePart() { return m_mailPart; }

private:
    QString m_fileName;
    QMailMessagePart m_mailPart;
    QString m_displayName;
};

//helper class to handle the delete Button on the email address row.
class DeleteButton : public MButton
{
    Q_OBJECT
public :

    DeleteButton(EmailAddressType type, MStylableWidget* parent, const QMailAddress &address);
    DeleteButton(MStylableWidget* parent, Attachment *file);
    virtual ~DeleteButton() {}

    EmailAddressType type()       { return m_type; }
    MStylableWidget *parent()     { return m_parent; }
    QMailAddress     address()    { return m_address; }
    Attachment       *attachment() { return m_attachment; }
    
private:
    EmailAddressType m_type;
    MStylableWidget *m_parent;
    QMailAddress     m_address;
    Attachment      *m_attachment;
};

#endif // COMPOSEPAGE_H
