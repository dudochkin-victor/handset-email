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
#include <MComboBox>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MMessageBox>
#include <QGraphicsLinearLayout>
#include <MLabel>
#include <MTextEdit>
#include <MButton>

#include <qmailstore.h>
#include <qmailaccount.h>
#include <qmailnamespace.h>
#include <qmailtransport.h>
#include <qmailserviceconfiguration.h>

#include "mailmanager.h"
#include "editexistingaccountpage.h"
#include "emailserviceconfiguration.h"
#include "emailsettingspage.h"

static const QString imapServiceKey("imap4");
static const QString popServiceKey("pop3");
static const QString smtpServiceKey("smtp");
static const QString storageServiceKey("qtopiamailfile");

static MTextEdit *creatLabelWithEdit (MLinearLayoutPolicy *mainPolicy, QString labelText, QString editText = "")
{
    MWidgetController *w = new MWidgetController;
    w->setViewType("background");
    MLayout *layout = new MLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    policy->setSpacing(0);

    MLabel *label = new MLabel (labelText);
    policy->addItem(label);

    MTextEdit *edit = new MTextEdit(MTextEditModel::SingleLine, editText, w);
    policy->addItem(edit);
    mainPolicy->addItem (w);
    return edit;
}

static MComboBox *creatComboBoxEntry (MLinearLayoutPolicy *policy, QString title, 
                                      QStringList items, int index)
{

    MComboBox *cBox = new MComboBox();
    cBox->addItems(items);
    cBox->setTitle(title);
    cBox->setCurrentIndex (index);
    policy->addItem(cBox);
    return cBox;
}

EditExistingAccountPage::EditExistingAccountPage(const QMailAccountId& id)
    :m_accountId (id)
{
    setContentsMargins(0, 0, 0, 0);
    setPannable(true);
    //% "Mail"
    setTitle(qtTrId("xx_page_title"));

    MLayout *layout = new MLayout(centralWidget());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAnimation(0);

    m_policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    m_policy->setSpacing(5);

    QMailAccount mailAccount(id);
    //% "Your name"
    m_displayName = creatLabelWithEdit(m_policy, qtTrId("xx_your_name"), mailAccount.name());

    QMailAddress from = mailAccount.fromAddress();
    //% "Email address"
    m_emailAddress = creatLabelWithEdit(m_policy, qtTrId("xx_email_address"), from.toString());

    // Recieving Settings start here
    MWidgetController *container = new MWidgetController;
    container->setObjectName("settigCategory");
    container->setViewType ("background");
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout = new MLayout (container);
    layout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    policy->setSpacing(5);
    //% "Receiving settings"
    policy->addItem(new MLabel(QString("<b>" + qtTrId("xx_receiving_settings") + "</b>")));

    QMailAccountConfiguration config(id);
    m_services = config.services();
    
    //% "None"
    m_inServiceKey = qtTrId("xx_none");
    int sIndex = 0;
    if (m_services.contains(imapServiceKey))
    {
        m_inServiceKey = imapServiceKey;
        sIndex = 1;
    }
    else if (m_services.contains(popServiceKey))
    {
        m_inServiceKey = popServiceKey;
        sIndex = 2;
    }

    QMailAccountConfiguration::ServiceConfiguration &cfg (config.serviceConfiguration(m_inServiceKey));
    m_inSvcCfgValues = cfg.values();   // saves a copy of the previous service configuration settings

    EmailServiceConfiguration svcCfg(&config, m_inServiceKey);

    QStringList inServerTypes = EmailSettingsPage::instance()->incomingServerTypes();
    //% "Server type"
    m_inServerType = creatComboBoxEntry (policy, qtTrId("xx_server_type"), inServerTypes, sIndex);
    connect(m_inServerType, SIGNAL(currentIndexChanged(int)), this, SLOT(onInServerTypeChanged(int)));
    //% "Server address"
    m_inServerAddress = creatLabelWithEdit(policy, qtTrId("xx_server_address"), svcCfg.value("server"));
    //% "Port"
    m_inServerPort = creatLabelWithEdit(policy, qtTrId("xx_port"), svcCfg.value("port"));
    QStringList securityTypes = EmailSettingsPage::instance()->securityTypes();
    //% "Security"
    m_inServerSecurity = creatComboBoxEntry (policy, qtTrId("xx_security"), securityTypes, 
                                                       svcCfg.value("encryption").toInt());
    connect(m_inServerSecurity, SIGNAL(currentIndexChanged(int)), this, SLOT(onInServerSecurityChanged(int)));

    //% "Username"
    m_inUsername = creatLabelWithEdit(policy, qtTrId("xx_username"), svcCfg.value("username"));
    //% "Password"
    m_inPassword = creatLabelWithEdit(policy, qtTrId("xx_password"), svcCfg.decode(svcCfg.value("password")));
    m_inPassword->setEchoMode(MTextEditModel::Password);

    m_policy->addItem (container);

    // Sending Settings start here
    container = new MWidgetController;
    container->setObjectName("settigCategory");
    container->setViewType ("background");
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout = new MLayout (container);
    layout->setContentsMargins(0, 0, 0, 0);
    policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    policy->setSpacing(5);
    //% "Sending settings"
    policy->addItem(new MLabel(QString("<b>" + qtTrId("xx_sending_settings") + "</b>")));

    // for this relesae, we only support smtp
    sIndex = 0;
    //% "None"
    m_outServiceKey = qtTrId("xx_none");
    if (m_services.contains(smtpServiceKey))
    {
         m_outServiceKey = smtpServiceKey;
         sIndex = 1;
    }

    QMailAccountConfiguration::ServiceConfiguration &outCfg (config.serviceConfiguration(m_outServiceKey));
    m_outSvcCfgValues = outCfg.values();   // saves a copy of the previous service configuration settings

    EmailServiceConfiguration outSvcCfg(&config, m_outServiceKey);

    QStringList outServerTypes = EmailSettingsPage::instance()->sendServerTypes();
    //% "Server type"
    m_outServerType = creatComboBoxEntry (policy, qtTrId("xx_server_type"), outServerTypes, sIndex);
    connect(m_outServerType, SIGNAL(currentIndexChanged(int)), this, SLOT(onOutServerTypeChanged(int)));

    //% "Server address"
    m_outServerAddress = creatLabelWithEdit(policy, qtTrId("xx_server_address"), outSvcCfg.value("server"));
    //% "port"
    m_outServerPort = creatLabelWithEdit(policy, qtTrId("xx_port"), outSvcCfg.value("port"));
    QStringList authTypes = EmailSettingsPage::instance()->authTypes();
    //% "Authentication"
    m_outAuthentication = creatComboBoxEntry (policy, qtTrId("xx_authentication"), authTypes, 
                                                       outSvcCfg.value("authentication").toInt());

    securityTypes = EmailSettingsPage::instance()->securityTypes();
    //% "Security"
    m_outServerSecurity = creatComboBoxEntry (policy, qtTrId("xx_security"), securityTypes, 
                                                       outSvcCfg.value("encryption").toInt());
    connect(m_outServerSecurity, SIGNAL(currentIndexChanged(int)), this, SLOT(onOutServerSecurityChanged(int)));

    //% "Username"
    m_outUsername = creatLabelWithEdit(policy, qtTrId("xx_username"), outSvcCfg.value("smtpusername"));
    //% "Password"
    m_outPassword = creatLabelWithEdit(policy, qtTrId("xx_password"), outSvcCfg.decode(outSvcCfg.value("smtppassword")));
    m_outPassword->setEchoMode(MTextEditModel::Password);

    m_policy->addItem (container);

    // Create the Update and Delete buttons
    MWidget *buttonRow = new MWidget();
    QGraphicsLinearLayout *buttonLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    buttonRow->setLayout(buttonLayout);

    //% "Update"
    MButton *button = new MButton(qtTrId("xx_update"), this);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onUpdateAccount()));
    buttonLayout->addItem(button);

    //% "Delete account"
    button= new MButton(qtTrId("xx_delete_account"), this);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onDeleteAccount()));
    buttonLayout->addItem (button);

    m_policy->addItem(buttonRow);

    MAction *action = new MAction (qtTrId("xx_update"),this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect (action, SIGNAL(triggered()), this, SLOT(onUpdateAccount()));

    action = new MAction (qtTrId("xx_delete_account"),this);
    action->setLocation(MAction::ApplicationMenuLocation);
    addAction(action);
    connect (action, SIGNAL(triggered()), this, SLOT(onDeleteAccount()));
}

EditExistingAccountPage::~EditExistingAccountPage()
{
}

void EditExistingAccountPage::onUpdateAccount()
{
    // check the Receiving settings for any changes
    // check for changes in server type.  
    int index = m_inServerType->currentIndex();
    //% "None"
    QString serviceKey = qtTrId("xx_none");
    if (index == 1)
        serviceKey = imapServiceKey;
    else if (index == 2)
        serviceKey = popServiceKey;

    QMailAccount account (m_accountId);
    account.setName (m_displayName->text());
    QMailAccountConfiguration config(m_accountId);

    bool newService = false;
    if (serviceKey != m_inServiceKey)
    {
        // we need to delete the previous service configuration and create a new one
        config.removeServiceConfiguration(m_inServiceKey);
        config.addServiceConfiguration(serviceKey);

        m_inServiceKey = serviceKey;
        newService = true;
    }

    if (m_inServiceKey == imapServiceKey)
    {
        EmailServiceConfiguration imapConfig(&config, imapServiceKey);
        if (newService)
        {
           imapConfig.setVersion(100);
           imapConfig.setType(QMailServiceConfiguration::Source);
           imapConfig.setIntValue("autoDownload", true);
           imapConfig.setValue("textSubtype", "plain");
           imapConfig.setIntValue("pushEnabled", true);
           imapConfig.setIntValue("maxSize", 30);
           imapConfig.setIntValue("canDelete", true);
           imapConfig.setIntValue("intervalCheckRoamingEnabled", true);

           imapConfig.setValue("pushFolders", "INBOX");
           account.setStatus(QMailAccount::CanCreateFolders, true);
           account.setStatus(QMailAccount::CanRetrieve, true);
        }
        imapConfig.setValue ("username", m_inUsername->text());
        imapConfig.setValue ("password", imapConfig.encode(m_inPassword->text()));
        imapConfig.setValue ("server", m_inServerAddress->text());
        imapConfig.setValue ("port",  m_inServerPort->text());

        QMailTransport::EncryptType encrypType = QMailTransport::Encrypt_NONE;
        //% "SSL"
        if (QString::compare(m_inServerSecurity->currentText(), qtTrId("xx_ssl"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_SSL;
        //% "TLS"
        else if (QString::compare(m_inServerSecurity->currentText(), qtTrId("xx_tls"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_TLS;

        imapConfig.setIntValue ("encryption", encrypType);

        int interval = EmailSettingsPage::instance()->frequency();
        imapConfig.setIntValue("checkInterval", interval);
    }
    else if (m_inServiceKey == popServiceKey)
    {
        EmailServiceConfiguration popConfig (&config, popServiceKey);
        if (newService)
        {
            popConfig.setVersion(100);
            popConfig.setType(QMailServiceConfiguration::Source);

            popConfig.setIntValue("autoDownload", true);
            popConfig.setValue("textSubtype", "plain");
            popConfig.setIntValue("pushEnabled", true);
            popConfig.setIntValue("maxSize", 30);
            popConfig.setIntValue("canDelete", true);
            popConfig.setIntValue("intervalCheckRoamingEnabled", true);
    
            account.setStatus(QMailAccount::CanRetrieve, true);
        }
        popConfig.setValue ("username", m_inUsername->text());
        popConfig.setValue ("password", popConfig.encode(m_inPassword->text()));
        popConfig.setValue ("server", m_inServerAddress->text());
        popConfig.setValue ("port",  m_inServerPort->text());

        QMailTransport::EncryptType encrypType = QMailTransport::Encrypt_NONE;
        //% "SSL"
        if (QString::compare(m_inServerSecurity->currentText(), qtTrId("xx_ssl"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_SSL;
        //% "TLS"
        else if (QString::compare(m_inServerSecurity->currentText(), qtTrId("xx_tls"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_TLS;

        popConfig.setIntValue ("encryption", encrypType);

        int interval = EmailSettingsPage::instance()->frequency();
        popConfig.setIntValue("checkInterval", interval);
    }

    // Now lets take care of send settings
    index = m_outServerType->currentIndex();

    //% "None"
    serviceKey = qtTrId("xx_none");
    if (index == 1)
        serviceKey = smtpServiceKey;

    newService = false;
    if (serviceKey != m_outServiceKey)
    {
        // we need to delete the previous service configuration and create a new one
        
        //% "None"
        if (serviceKey == qtTrId("xx_none"))
            config.removeServiceConfiguration(m_outServiceKey);
        else
            config.addServiceConfiguration(serviceKey);

        m_outServiceKey = serviceKey;
        newService = true;
    }

    //% "None"
    if (serviceKey != qtTrId("xx_none"))
    {
        EmailServiceConfiguration smtpConfig (&config, smtpServiceKey);
        if (newService)
        {
            smtpConfig.setVersion(100);
            smtpConfig.setType(QMailServiceConfiguration::Sink);
            QString signature = EmailSettingsPage::instance()->signature();
            if (signature.size() > 0)
            {
                account.setStatus(QMailAccount::AppendSignature, true);
                account.setSignature(signature);
            }
            else
            {
                account.setStatus(QMailAccount::AppendSignature, false);
                account.setSignature("");
            }

            account.setStatus(QMailAccount::MessageSink, true);
            account.setStatus(QMailAccount::CanTransmit, true);
        }

        smtpConfig.setValue("address", m_emailAddress->text());
        smtpConfig.setValue("server", m_outServerAddress->text());
        smtpConfig.setValue("port", m_outServerPort->text());

        smtpConfig.setIntValue("authentication", m_outAuthentication->currentIndex());

        if (m_outAuthentication->currentIndex() != 0)
        {
            smtpConfig.setValue("smtpusername", m_outUsername->text());
            smtpConfig.setValue("smtppassword", smtpConfig.encode(m_outPassword->text()));
        }
        else
        {
            smtpConfig.setValue("smtpusername", "");
            smtpConfig.setValue("smtppassword", "");
        }

        QMailTransport::EncryptType encrypType = QMailTransport::Encrypt_NONE;
        //% "SSL"
        if (QString::compare(m_outServerSecurity->currentText(), qtTrId("xx_ssl"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_SSL;
        //% "TLS"
        else if (QString::compare(m_outServerSecurity->currentText(), qtTrId("xx_tls"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_TLS;
        smtpConfig.setIntValue("encryption", encrypType);
    }

    QMailStore::instance()->updateAccount(&account, &config);

    m_retrievalAction = new QMailRetrievalAction(this);
    m_transmitAction = new QMailTransmitAction(this);

    connect(m_retrievalAction, SIGNAL(progressChanged(uint, uint)),
            this, SLOT(displayRetrieveProgress(uint, uint)));
    connect(m_retrievalAction, SIGNAL(activityChanged(QMailServiceAction::Activity)),
            this, SLOT(activityChanged(QMailServiceAction::Activity)));

    connect(m_transmitAction, SIGNAL(progressChanged(uint, uint)),
            this, SLOT(displayTransmitProgress(uint, uint)));
    connect(m_transmitAction, SIGNAL(activityChanged(QMailServiceAction::Activity)),
            this, SLOT(activityChanged(QMailServiceAction::Activity)));

    QTimer::singleShot(10, this, SLOT(testAccountConfiguration()));
    dismiss();
}

void EditExistingAccountPage::onDeleteAccount()
{
    QMailStore::instance()->removeAccount(m_accountId);
    dismiss();
}

void EditExistingAccountPage::testAccountConfiguration()
{
    QMailAccount account (m_accountId);
    if (account.status() & (QMailAccount::MessageSource | QMailAccount::MessageSink))
    {
        if (account.status() & QMailAccount::MessageSource) {
            m_retrievalAction->retrieveFolderList(m_accountId, QMailFolderId(), true);
        } else if (account.status() & QMailAccount::MessageSink) {
            m_transmitAction->transmitMessages(m_accountId);
        }
    }
}
void EditExistingAccountPage::activityChanged(QMailServiceAction::Activity activity)
{
    if (sender() == static_cast<QObject*>(m_retrievalAction)) {
        const QMailServiceAction::Status status(m_retrievalAction->status());
        if (status.accountId.isValid()) {
            QMailAccount account(status.accountId);

            if (activity == QMailServiceAction::Successful) {
                if (account.status() & QMailAccount::MessageSink) {
                    m_transmitAction->transmitMessages(account.id());
                } else {
                    emit accountReady();
                }
            } else if (activity == QMailServiceAction::Failed) {
                //% "Retrieve Failure"
                QString caption(qtTrId("xx_retrieve_failure"));
                //% "%1 - Error retrieving folders: %2"
                QString errMsg(qtTrId("xx_retrieval_failure_error").arg(account.name()).arg(status.text));

                MMessageBox message(errMsg);
                message.exec();
            }
        }
    } else if (sender() == static_cast<QObject*>(m_transmitAction)) {
        if (activity == QMailServiceAction::Successful) {
            emit accountReady();
        } else if (activity == QMailServiceAction::Failed) {
            const QMailServiceAction::Status status(m_transmitAction->status());
            QMailAccount account(status.accountId);

            //% "Transmission Failure"
            QString caption(qtTrId("xx_tranmit_failed"));
            //% "%1 - Error testing connection: %2"
            QString errMsg(qtTrId("xx_connection_failure_error").arg(account.name()).arg(status.text));

            MMessageBox message(errMsg);
            message.exec();
        }
    }
}

void EditExistingAccountPage::displayRetrieveProgress(uint value, uint range)
{
    Q_UNUSED (value);
    Q_UNUSED (range);
    // TODO:  might want to put up a progress dialog in future
}

void EditExistingAccountPage::displayTransmitProgress(uint value, uint range)
{
    Q_UNUSED (value);
    Q_UNUSED (range);
    // TODO:  might want to put up a progress dialog in future
}

void EditExistingAccountPage::onInServerTypeChanged(int index)
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

void EditExistingAccountPage::onInServerSecurityChanged(int index)
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

void EditExistingAccountPage::onOutServerTypeChanged(int index)
{
    int securityType = m_outServerSecurity->currentIndex();

    QString port = "-1";
    if (index ==  1)  // smtp
    {
        if (securityType == 0) // none
            port = "25";
        else if (securityType == 1) // ssl
            port = "465";
        else if (securityType == 2) // tls
            port = "587";
    }
    m_outServerPort->setText(port);
}

void EditExistingAccountPage::onOutServerSecurityChanged(int index)
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
