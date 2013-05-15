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
#include <MButton>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MMessageBox>
#include <MTextEdit>
#include <qmailstore.h>
#include <qmailaccountkey.h>
#include <QtDebug>

#include "accountsetuppage.h"
#include "accountinfo.h"

static void displayError (QString msg)
{
    MMessageBox message(msg);
    message.exec();
}

static void creatLabel (MLinearLayoutPolicy *policy, QString text, bool addSpacing=false)
{
    MLabel *label = new MLabel (text);
    label->setWordWrap(true);
    label->setObjectName ("AccountSetupPageLabel");
    policy->addItem(label);
    if (addSpacing)
        policy->setItemSpacing(policy->indexOf(label), 5);
}

AccountSetupPage::AccountSetupPage(QGraphicsItem *parent)
    : MApplicationPage(parent),
      m_account(new AccountInfo())
{
}

AccountSetupPage::~AccountSetupPage()
{
}

void AccountSetupPage::createContent()
{
    MApplicationPage::createContent();
    setContentsMargins(0, 0, 0, 0);
    setPannable(true);
    //% "Mail"
    setTitle(qtTrId("xx_page_title"));

    MLayout *layout = new MLayout(centralWidget());

    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);

    //% "New mail account"
    creatLabel (policy, ("<b>" + qtTrId("xx_new_mail_account") + "<b>"), true);

    //% "We need to take some details to setup your account."
    creatLabel (policy, qtTrId("xx_need_account_detail"), true);
     
    // Prompt the user for name
    //% "What's your name?"
    creatLabel(policy, qtTrId("xx_enter_your_name"));
    m_username = new MTextEdit(MTextEditModel::SingleLine, "", this);
    m_username->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    policy->addItem(m_username);
    policy->setItemSpacing(policy->indexOf(m_username), 5);

    // Prompt the user for email address
    //% "Email address"
    creatLabel (policy, qtTrId("xx_email_address"));
    m_emailAddress = new MTextEdit(MTextEditModel::SingleLine, "", this);
    m_emailAddress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    policy->addItem(m_emailAddress);
    policy->setItemSpacing(policy->indexOf(m_emailAddress), 5);

    // Prompt the user for password
    //% "Password"
    creatLabel(policy, qtTrId("xx_password"));
    m_password = new MTextEdit(MTextEditModel::SingleLine, "", this);
    m_password->setEchoMode(MTextEditModel::Password);
    m_password->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    policy->addItem(m_password);
    policy->setItemSpacing(policy->indexOf(m_password), 5);

    // Setup button
    //% "Setup account"
    MButton *button = new MButton(qtTrId("xx_setup_account_button_label"), this);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(setupAccount()));
    policy->addItem(button);
    policy->addStretch();

    // Settings menu
    //% "Settings"
    MAction *action = new MAction(qtTrId("xx_settings_actions"), this);
    action->setLocation(MAction::ApplicationMenuLocation);
    connect(action, SIGNAL(triggered(bool)), this, SIGNAL(emailSettingsTriggered()));
    addAction(action);
}


// Callback function for the "Setup Acount" buttion.
void AccountSetupPage::setupAccount()
{
    QString emailAddress = m_emailAddress->text();
    QMailAddress address(emailAddress);
    if (!address.isEmailAddress())
    {
        //% "Please enter a valid email address."
        displayError (qtTrId("xx_enter_valid_email_address"));
        return;
    }

    QString name = m_username->text();
    if (name == "")
    {
        //% "Please enter your user name."
        displayError (qtTrId("xx_enter_user_name"));
        return;
    }

    QString password = m_password->text();
    if (password == "")
    {
        //% "Please enter your password."
        displayError (qtTrId("xx_enter_your_password"));
        return;
    }

    m_account->setEmailAddress (emailAddress);
    m_account->setDisplayName(name);
    m_account->setInUsername (emailAddress);
    m_account->setInPassword (password);
    m_account->setOutUsername (emailAddress);
    m_account->setOutPassword (password);

    // TODO: We should get the mail server configuration from a file or database.

    // check to see if the email server is one that we know about
    if (emailAddress.endsWith ("@linux.intel.com", Qt::CaseInsensitive))
    {
        // FIX ME:  linux.intel.com is only tempoary for testing purpose. It should be removed 
        //        from the production build.
        int index = emailAddress.indexOf("@");
        QString login = emailAddress.left(index);
        m_account->setInUsername(login);
        m_account->setInPort (993);
        //% "SSL"
        m_account->setInSecurity (qtTrId("xx_ssl"));
        m_account->setInServerAddress ("linux.intel.com");
        //% "IMAP"
        m_account->setInServerType (qtTrId("xx_imap"));
        m_account->setOutAuthentication (AccountInfo::Authentication_None);
        //% "SMTP"
        m_account->setOutServerType (qtTrId("xx_smtp"));
        m_account->setOutServerAddress ("smtp.intel.com");
        m_account->setOutPort (25);
        //% "None"
        m_account->setOutSecurity(qtTrId("xx_none"));
        m_account->setOutUsername("");
        m_account->setOutPassword("");
    }
    else if (emailAddress.endsWith ("@gmail.com", Qt::CaseInsensitive))
    {
        m_account->setInPort (993);
        //% "SSL"
        m_account->setInSecurity (qtTrId("xx_ssl"));
        m_account->setInServerAddress ("imap.gmail.com");
        //% "IMAP"
        m_account->setInServerType (qtTrId("xx_imap"));
        m_account->setOutAuthentication (AccountInfo::Authentication_Login);
        m_account->setOutPort (465);
        //% "SMTP"
        m_account->setOutServerType (qtTrId("xx_smtp"));
        m_account->setOutServerAddress ("smtp.gmail.com");
        //% "SSL"
        m_account->setOutSecurity (qtTrId("xx_ssl"));
    }
    else if (emailAddress.endsWith ("@yahoo.com", Qt::CaseInsensitive))
    {
        m_account->setInPort (110);
        //% "SSL"
        m_account->setInSecurity (qtTrId("xx_ssl"));
        m_account->setInServerAddress ("pop.mail.yahoo.com");
        //% "POP"
        m_account->setInServerType (qtTrId("xx_pop"));
        m_account->setOutAuthentication (AccountInfo::Authentication_Login);
        m_account->setOutPort (25);
        //% "SMTP"
        m_account->setOutServerType (qtTrId("xx_smtp"));
        m_account->setOutServerAddress ("smtp.mail.yahoo.com");
        //% "SSL"
        m_account->setOutSecurity (qtTrId("xx_ssl"));
    }
    else if (emailAddress.endsWith ("@aol.com", Qt::CaseInsensitive))
    {
        // aol's login name is just the screen name without "@aol.com"
        int index = emailAddress.indexOf("@");
        QString login = emailAddress.left(index);
        m_account->setInUsername(login);

        m_account->setInPort (993);
        //% "SSL"
        m_account->setInSecurity (qtTrId("xx_ssl"));
        m_account->setInServerAddress ("imap.aol.com");
        //% "IMAP"
        m_account->setInServerType (qtTrId("xx_imap"));
        m_account->setOutAuthentication (AccountInfo::Authentication_Plain);
        m_account->setOutPort (587);
        //% "SMTP"
        m_account->setOutServerType (qtTrId("xx_smtp"));
        m_account->setOutServerAddress ("smtp.aol.com");
        //% "TLS"
        m_account->setOutSecurity(qtTrId("xx_tls"));
        m_account->setOutUsername(login);
    }
    else
    {
        int index = emailAddress.lastIndexOf("@");
        emailAddress.remove(0, index+1);
        m_account->setInServerType (qtTrId("xx_imap"));
        m_account->setInSecurity (qtTrId("xx_none"));
        m_account->setInServerAddress("imap." + emailAddress);
        m_account->setInPort (143);

        m_account->setOutServerType (qtTrId("xx_smtp"));
        m_account->setOutSecurity(qtTrId("xx_none"));
        m_account->setOutServerAddress("smtp." + emailAddress);
        m_account->setOutAuthentication (AccountInfo::Authentication_None);
        m_account->setOutPort (25);
        emit manualAccountEdit(m_account);
        dismiss();
        return;
    }

    emit verifyAccount(m_account);
    dismiss();
}
