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
#include <MButton>
#include <MMessageBox>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MMessageBox>
#include <MTextEdit>
#include <QGraphicsLinearLayout>
#include <qmailstore.h>
#include <qmailaccountkey.h>
#include <QtDebug>
#include <qmailserviceaction.h>
#include <qmailaccount.h>

#include "reviewaccountpage.h"
#include "accountinfo.h"
#include "emailapplication.h"

static void creatLabel (MLinearLayoutPolicy *policy, QString text)
{
    MLabel *label = new MLabel (text);
    label->setWordWrap(true);
    label->setObjectName ("ReviewAccountPageLabel");
    policy->addItem(label);
}

ReviewAccountPage::ReviewAccountPage(AccountInfo *account)
    : MApplicationPage(),
      m_account (account)
{
}

ReviewAccountPage::~ReviewAccountPage()
{
}

void ReviewAccountPage::createContent()
{
    MApplicationPage::createContent();
    setContentsMargins(0, 0, 0, 0);
    //% "Mail"
    setTitle(qtTrId("xx_page_title"));
    setPannable(true);

    MLayout *layout = new MLayout(centralWidget());
    m_policy = new MLinearLayoutPolicy(layout, Qt::Vertical);

    //% "You entered the following details, please check they're correct."
    creatLabel (m_policy, QString(qtTrId("xx_check_for_account_details")));
    //% "Name: %1"
    creatLabel (m_policy, QString(qtTrId("xx_review_display_name").arg(m_account->displayName())));
    //% "Email address: %1"
    creatLabel (m_policy, QString(qtTrId("xx_review_email_address").arg(m_account->emailAddress())));

    // Receive settings.
    //% "Receiving"
    creatLabel (m_policy, QString("<b>" + qtTrId("xx_receiving_title") + "</b>"));
    //% "Server type: %1"
    creatLabel (m_policy, QString(qtTrId("xx_review_server_type").arg(m_account->inServerType())));
    //% "Server address: %1"
    creatLabel (m_policy, QString(qtTrId("xx_review_server_address").arg(m_account->inServerAddress())));
    //% "Port: %1"
    creatLabel (m_policy, QString(qtTrId("xx_review_port").arg(QString::number(m_account->inPort()))));
    //% "Security: %1"
    creatLabel (m_policy, QString(qtTrId("xx_review_security").arg(m_account->inSecurity())));
    //% "Username: %1"
    creatLabel (m_policy, QString(qtTrId("xx_review_username").arg(m_account->inUsername())));

    // Send settings.
    //% "Sending"
    creatLabel (m_policy, QString("<b>" + qtTrId("xx_sending_title") + "</b>"));
    creatLabel (m_policy, QString(qtTrId("xx_review_server_type").arg(m_account->outServerType())));
    creatLabel (m_policy, QString(qtTrId("xx_review_server_address").arg(m_account->outServerAddress())));
    creatLabel (m_policy, QString(qtTrId("xx_review_port").arg(QString::number(m_account->outPort()))));
    creatLabel (m_policy, QString(qtTrId("xx_review_security").arg(m_account->outSecurity())));
    if (m_account->outAuthentication() == AccountInfo::Authentication_None)
        //% "Authentication: None"
        creatLabel (m_policy, QString(qtTrId("xx_authentication_none")));
    else
    {
        if (m_account->outAuthentication() == AccountInfo::Authentication_Login)
            //% "Authentication: Login"
            creatLabel (m_policy, QString(qtTrId("xx_authentication_login")));
        else if (m_account->outAuthentication() == AccountInfo::Authentication_Plain)
            //% "Authentication: Plain"
            creatLabel (m_policy, QString(qtTrId("xx_authentication_plain")));

        creatLabel (m_policy, QString(qtTrId("xx_review_username").arg(m_account->outUsername())));
    }

    // create the buttons "Save and continue" and "Manual edit"
    MWidget *buttonRow = new MWidget();
    QGraphicsLinearLayout *buttonLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    buttonRow->setLayout(buttonLayout);

   //% "Save and continue"
    MButton *button = new MButton(qtTrId("xx_save_and_continue"), this);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(saveAndContinue()));
    buttonLayout->addItem(button);

    //% "Manual edit"
    button= new MButton(qtTrId("xx_manual_edit"), this);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(manualEdit()));
    buttonLayout->addItem (button);

    m_policy->addItem(buttonRow);
}


void ReviewAccountPage::saveAndContinue()
{
    // Make sure the account is valid before emit the accountReady signal.

    if (!m_account->accountIsValid())
    {
        //% "Warning: Can't validate your account at this time. You might not be able to send or recieve email."
        MMessageBox message(qtTrId("xx_error_account_invalid"));
        message.exec();
    }
    EmailApplication::instance()->addEmailAccount(m_account);
    dismiss();
}

void ReviewAccountPage::manualEdit()
{
    dismiss();
    emit manualAccountEdit(m_account);
}
