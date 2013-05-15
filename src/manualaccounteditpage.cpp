/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <MLabel>
#include <MButton>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <QGraphicsLinearLayout>
#include <MMessageBox>
#include <MTextEdit>
#include <qmailstore.h>
#include <qmailaccountkey.h>
#include <QtDebug>

#include "accountinfo.h"
#include "emailsettingspage.h"
#include "manualaccounteditpage.h"

static void creatLabel (MLinearLayoutPolicy *policy, QString text)
{
    MLabel *label = new MLabel (text);
    label->setWordWrap(true);
    label->setObjectName ("ManualAccountEditPageLabel");
    policy->addItem(label);
}

ManualAccountEditPage::ManualAccountEditPage(AccountInfo *account)
    : MApplicationPage(),
      m_account (account),
      m_sendSettingsWidget(NULL),
      m_receiveSettingsWidget(NULL)
{
}

ManualAccountEditPage::~ManualAccountEditPage()
{
}

void ManualAccountEditPage::createContent()
{
    MApplicationPage::createContent();
    setContentsMargins(0, 0, 0, 0);
    //% "Mail"
    setTitle(qtTrId("xx_page_title"));
    setPannable(true);

    MWidget *panel = (MWidget *)centralWidget();
    MLayout *layout = new MLayout(panel);
    m_policy = new MLinearLayoutPolicy(layout, Qt::Vertical);

    //% "New mail account"
    creatLabel (m_policy, ("<b>" + qtTrId("xx_new_mail_account") +"</b>"));
    //% "Sorry, we can't automatically setup your account. You may need to contact your email provider for these settings."
    creatLabel (m_policy, qtTrId("xx_cannot_auto_setup_account"));

    displayReceiveSettings();
}

void ManualAccountEditPage::displayReceiveSettings()
{
    if (m_sendSettingsWidget)
    {
        m_policy->removeItem(m_sendSettingsWidget);
    }

    if (!m_receiveSettingsWidget)
    {
        m_receiveSettingsWidget = new MWidget();
        MLayout *layout = new MLayout(m_receiveSettingsWidget);
        MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);

        //% "Receiving settings"
        creatLabel (policy, ("<b>" + qtTrId("xx_receiving_settings") + "</b>"));

        m_inServerType = new MComboBox(m_receiveSettingsWidget);
        //% "Server type"
        m_inServerType->setTitle(qtTrId("xx_server_type"));
        QStringList inServerTypes = EmailSettingsPage::instance()->incomingServerTypes();
        m_inServerType->addItems(inServerTypes);
        m_inServerType->setCurrentIndex(inServerTypes.indexOf(m_account->inServerType()));
        m_inServerType->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(m_inServerType, SIGNAL(currentIndexChanged(int)), this, SLOT(onInServerTypeChanged(int)));
        policy->addItem(m_inServerType);

        //% "Server address"
        creatLabel (policy, qtTrId("xx_server_address"));
        m_inServerAddress = new MTextEdit(MTextEditModel::SingleLine, "", m_receiveSettingsWidget);
        m_inServerAddress->setText (m_account->inServerAddress());
        m_inServerAddress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy->addItem(m_inServerAddress);

        //% "Port"
        creatLabel (policy, qtTrId("xx_port"));
        m_inServerPort = new MTextEdit(MTextEditModel::SingleLine, "", m_receiveSettingsWidget);
        m_inServerPort->setText (QString::number(m_account->inPort()));
        m_inServerPort->setContentType(M::NumberContentType);
        m_inServerPort->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy->addItem(m_inServerPort);

        m_inServerSecurity = new MComboBox (m_receiveSettingsWidget);;
        //% "Security"
        m_inServerSecurity->setTitle(qtTrId("xx_security"));
        QStringList securityTypes = EmailSettingsPage::instance()->securityTypes();
        m_inServerSecurity->addItems(securityTypes);
        m_inServerSecurity->setCurrentIndex(securityTypes.indexOf(m_account->inSecurity()));
        m_inServerSecurity->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(m_inServerSecurity, SIGNAL(currentIndexChanged(int)), this, SLOT(onInServerSecurityChanged(int)));
        policy->addItem(m_inServerSecurity);

        //% "Username"
        creatLabel (policy, qtTrId("xx_username"));
        m_inServerUsername = new MTextEdit(MTextEditModel::SingleLine, "", m_receiveSettingsWidget);
        m_inServerUsername->setText (m_account->inUsername());
        m_inServerUsername->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy->addItem(m_inServerUsername);

        //% "Password"
        creatLabel (policy, qtTrId("xx_password"));
        m_inServerPassword = new MTextEdit(MTextEditModel::SingleLine, "", m_receiveSettingsWidget);
        m_inServerPassword->setText (m_account->inPassword());
        m_inServerPassword->setEchoMode(MTextEditModel::Password);
        m_inServerPassword->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy->addItem(m_inServerPassword);

        policy->setItemSpacing(policy->indexOf(m_inServerPassword), 4);
        //% "Next: Sending"
        MButton *button = new MButton(qtTrId("xx_next_sending"), this);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(button, SIGNAL(clicked(bool)), this, SLOT(displaySendSettings()));
        policy->addItem(button);
    }

    m_policy->addItem(m_receiveSettingsWidget);
}

void ManualAccountEditPage::displaySendSettings()
{
    m_policy->removeItem(m_receiveSettingsWidget);

    // saves the incoming settings
    m_account->setInUsername (m_inServerUsername->text());
    m_account->setInPassword (m_inServerPassword->text());
    m_account->setInPort (m_inServerPort->text().toInt());
    m_account->setInServerType (m_inServerType->currentText());
    m_account->setInServerAddress (m_inServerAddress->text());
    m_account->setInSecurity (m_inServerSecurity->currentText());

    if (!m_sendSettingsWidget)
    {
        m_sendSettingsWidget = new MWidget();
        MLayout *layout = new MLayout(m_sendSettingsWidget);
        MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);

        //% "Sending settings"
        creatLabel (policy, "<b>" + qtTrId("xx_sending_settings") + "</b>");

        m_outServerType = new MComboBox(m_sendSettingsWidget);
        //% "Server type"
        m_outServerType->setTitle(qtTrId("xx_Server type"));
        QStringList outServerTypes = EmailSettingsPage::instance()->sendServerTypes();
        m_outServerType->addItems(outServerTypes);
        m_outServerType->setCurrentIndex(outServerTypes.indexOf(m_account->outServerType().toUpper()));
        m_outServerType->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(m_outServerType, SIGNAL(currentIndexChanged(int)), this, SLOT(onOutServerTypeChanged(int)));
        policy->addItem(m_outServerType);

        //% "From"
        creatLabel (policy, qtTrId("xx_from"));
        m_senderName = new MTextEdit(MTextEditModel::SingleLine, "", m_sendSettingsWidget);
        m_senderName->setText(m_account->displayName());
        m_senderName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy->addItem(m_senderName);

        //% "Email address"
        creatLabel (policy, qtTrId("xx_email_address"));
        m_emailAddress = new MTextEdit(MTextEditModel::SingleLine, "", m_sendSettingsWidget);
        m_emailAddress->setText(m_account->emailAddress());
        m_emailAddress->setContentType(M::EmailContentType);
        m_emailAddress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy->addItem(m_emailAddress);

        //% "Server address"
        creatLabel (policy, qtTrId("xx_server_address"));
        m_outServerAddress = new MTextEdit(MTextEditModel::SingleLine, "", m_sendSettingsWidget);
        m_outServerAddress->setText(m_account->outServerAddress());
        m_outServerAddress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy->addItem(m_outServerAddress);

        //% "Port"
        creatLabel (policy, qtTrId("xx_port"));
        m_outServerPort = new MTextEdit(MTextEditModel::SingleLine, "", m_sendSettingsWidget);
        m_outServerPort->setText(QString::number(m_account->outPort()));
        m_outServerPort->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_outServerPort->setContentType(M::NumberContentType);
        policy->addItem(m_outServerPort);

        m_outServerSecurity = new MComboBox (m_sendSettingsWidget);;
        //% "Security"
        m_outServerSecurity->setTitle(qtTrId("xx_security"));
        QStringList securityTypes = EmailSettingsPage::instance()->securityTypes();
        m_outServerSecurity->addItems(securityTypes);
        m_outServerSecurity->setCurrentIndex(securityTypes.indexOf(m_account->outSecurity()));
        m_outServerSecurity->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(m_outServerSecurity, SIGNAL(currentIndexChanged(int)), this, SLOT(onOutServerSecurityChanged(int)));
        policy->addItem(m_outServerSecurity);

        m_outAuthentication = new MComboBox (m_sendSettingsWidget);;
        //% "Authentication"
        m_outAuthentication->setTitle(qtTrId("xx_authentication"));
        QStringList authTypes = EmailSettingsPage::instance()->authTypes();
        m_outAuthentication->addItems(authTypes);
        m_outAuthentication->setCurrentIndex(m_account->outAuthentication());
        m_outAuthentication->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(m_outAuthentication, SIGNAL(currentIndexChanged(int)), this, SLOT(onOutAuthenticationChanged(int)));
        policy->addItem(m_outAuthentication);

        //% "Username"
        creatLabel (policy, qtTrId("xx_username"));
        m_outServerUsername = new MTextEdit(MTextEditModel::SingleLine, "", m_sendSettingsWidget);
        if (m_account->outAuthentication() == 0)
            m_account->setOutUsername("");
        else
            m_account->setOutUsername(m_account->inUsername());
        m_outServerUsername->setText(m_account->outUsername());
        m_outServerUsername->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy->addItem(m_outServerUsername);

        //% "Password"
        creatLabel (policy, qtTrId("xx_password"));
        m_outServerPassword = new MTextEdit(MTextEditModel::SingleLine, "", m_sendSettingsWidget);
        m_outServerPassword->setEchoMode(MTextEditModel::Password);
        if (m_account->outAuthentication() == 0)
            m_account->setOutPassword("");
        else
            m_account->setOutPassword(m_account->inPassword());
        m_outServerPassword->setText(m_account->outPassword());
        m_outServerPassword->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy->addItem(m_outServerPassword);

        // create the "next: review" and "back: receiving" buttons.
        MWidget *buttonRow = new MWidget();
        QGraphicsLinearLayout *buttonLayout = new QGraphicsLinearLayout(Qt::Horizontal);
        buttonRow->setLayout(buttonLayout);

        //% "Back: Receiving"
        MButton *button = new MButton(qtTrId("xx_back_to_receiving"), this);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(button, SIGNAL(clicked(bool)), this, SLOT(displayReceiveSettings()));
        buttonLayout->addItem(button);

        //% "Next: Review"
        button= new MButton(qtTrId("xx_next_review"), this);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(button, SIGNAL(clicked(bool)), this, SLOT(reviewAccountSettings()));
        buttonLayout->addItem (button);

        policy->addItem(buttonRow);

    }

    m_policy->addItem(m_sendSettingsWidget);

}


void ManualAccountEditPage::reviewAccountSettings()
{
    // Fix me: The following is a hack and for testing only. Should be removed for
    //         production release.

    if (m_inServerAddress->text() == "linxu.intel.com")
    {
        m_account->setEmailAddress(m_inServerUsername->text() + "@" + m_inServerAddress->text());
    }

    m_account->setOutUsername (m_outServerUsername->text());
    m_account->setOutPassword (m_outServerPassword->text());
    m_account->setOutPort (m_outServerPort->text().toInt());
    m_account->setOutServerType (m_outServerType->currentText());
    m_account->setOutServerAddress (m_outServerAddress->text());
    m_account->setOutSecurity (m_outServerSecurity->currentText());

    AccountInfo::AuthenticationType authType = AccountInfo::Authentication_None;
    if (m_outAuthentication->currentIndex() == 1)
       authType = AccountInfo::Authentication_Login;
    else if (m_outAuthentication->currentIndex() == 2)
       authType = AccountInfo::Authentication_Plain;
        
    m_account->setOutAuthentication(authType);

    m_account->setEmailAddress(m_emailAddress->text());
    m_account->setDisplayName(m_senderName->text());

    dismiss();
    emit verifyAccount(m_account);
}

void ManualAccountEditPage::onInServerTypeChanged(int index)
{
    int securityType = m_inServerSecurity->currentIndex();

    QString port = "-1";  // default to invalid port
    if (index == 1)  // imap
    {
        if (securityType == 0)  // none
            port = "143";
        else
            port = "993";
    } 
    else if (index == 2)  // pop
    {
        if (securityType == 0)  // none
            port = "110";
        else
            port = "995";
    }
    // update the port with the appropriate default value
    m_inServerPort->setText(port);
}

void ManualAccountEditPage::onInServerSecurityChanged(int index)
{
    int serverType = m_inServerType->currentIndex();

    QString port = "-1";  // default to invalid port
    if (index == 0)  // none
    {
        if (serverType == 1)  // imap
            port = "143";
        else if (serverType == 2)  // pop
            port = "110";
    } 
    else // SSL or TLS
    {
        if (serverType == 1)  // imap
            port = "993";
        else if (serverType == 2)  // pop
            port = "995";
    }
    // update the port with the appropriate default value
    m_inServerPort->setText(port);
}

void ManualAccountEditPage::onOutServerTypeChanged(int index)
{
    int securityType = m_outServerSecurity->currentIndex();
   
    QString port = "-1";
    if (index ==  1)  // smtp
    {
        if (securityType == 0) // none
            port = "25";
        else  if (securityType == 1) // ssl
            port = "465";
        else if (securityType == 2) // tls
            port = "587";
    }
    m_outServerPort->setText(port);
}

void ManualAccountEditPage::onOutServerSecurityChanged(int index)
{
    int serverType = m_outServerType->currentIndex();

    QString port = "-1";  // default to invalid port

    if (serverType ==  1)  // smtp
    {
        if (index == 0) // none
            port = "25";
        else if (index == 1) // ssl
            port = "465";
        else if (index == 2) // tls
            port = "587";
    }
    m_outServerPort->setText(port);
}

void ManualAccountEditPage::onOutAuthenticationChanged(int index)
{
    if (index == 0)
    {
        m_account->setOutUsername("");
        m_account->setOutPassword("");
    }
    else
    {
        m_account->setOutUsername(m_account->inUsername());
        m_account->setOutPassword(m_account->inPassword());
    }

    m_outServerUsername->setText(m_account->outUsername());
    m_outServerPassword->setText(m_account->outPassword());
}
