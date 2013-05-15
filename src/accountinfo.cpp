/*
 * meego-handset-email - Meego Handset Email application
 * Copyright © 2010, Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <qmailstore.h>
#include <qmailtransport.h>
#include <qmailaddress.h>
#include <qmailserviceaction.h>
#include <MMessageBox>
#include <QTimer>

#include "accountinfo.h"
#include "emailserviceconfiguration.h"
#include "emailsettingspage.h"

static const QString imapServiceKey("imap4");
static const QString popServiceKey("pop3");
static const QString smtpServiceKey("smtp");
static const QString storageServiceKey("qtopiamailfile");

AccountInfo::AccountInfo() :
    m_emailAddress (""),
    m_displayName (""),

    m_inPassword (""),
    m_inPort (-1),
    //% "None"
    m_inSecurity (qtTrId("xx_none")),
    m_inServerAddress (""),
    m_inServerType (qtTrId("xx_none")),
    m_inUsername (""),

    m_outAuthentication (AccountInfo::Authentication_None),
    m_outPassword (""),
    m_outPort (-1),
    m_outSecurity (qtTrId("xx_none")),
    m_outServerAddress (""),
    m_outServerType (qtTrId("xx_none")),
    m_outUsername ("")
{
    m_account = new QMailAccount();
    m_config = new QMailAccountConfiguration();
    m_account->setStatus(QMailAccount::UserEditable, true);
    m_account->setStatus(QMailAccount::UserRemovable, true);

    m_account->setMessageType(QMailMessage::Email);
    m_account->setStatus(QMailAccount::Enabled, true);

    m_account->setStatus(QMailAccount::MessageSource, true);
    m_account->setStatus(QMailAccount::MessageSink, true);
    m_account->setStatus(QMailAccount::CanTransmit, true);
    m_account->setStatus(QMailAccount::CanRetrieve, true);

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
}

AccountInfo::~AccountInfo()
{
    if (m_retrievalAction != NULL)
        delete m_retrievalAction;

    if (m_transmitAction != NULL)
        delete m_transmitAction;

    if (m_account != NULL)
        delete m_account;

    if (m_config != NULL)
        delete m_config;
}

bool AccountInfo::accountIsValid()
{
    m_account->setName (m_displayName);

    m_account->setStatus (QMailAccount::PreferredSender, true);

    //% "IMAP"
    if (m_inServerType.compare(qtTrId("xx_imap"), Qt::CaseInsensitive) == 0)
    {
        if (!m_config->services().contains(imapServiceKey))
            m_config->addServiceConfiguration(imapServiceKey);

        EmailServiceConfiguration imapConfig (m_config, imapServiceKey);
        imapConfig.setVersion(100);
        imapConfig.setType(QMailServiceConfiguration::Source);

        imapConfig.setValue ("username", m_inUsername);
        imapConfig.setValue ("password", imapConfig.encode(m_inPassword));
        imapConfig.setValue ("server", m_inServerAddress);
        imapConfig.setIntValue ("port",  (m_inPort == -1 ? 143 : m_inPort));

        QMailTransport::EncryptType encrypType = QMailTransport::Encrypt_NONE;
        //% "SSL"
        if (QString::compare(m_inSecurity, qtTrId("xx_ssl"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_SSL;
        //% "TLS"
        else if (QString::compare(m_inSecurity, qtTrId("xx_tls"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_TLS;

        imapConfig.setIntValue ("encryption", encrypType);
          
        int interval = EmailSettingsPage::instance()->frequency();
        imapConfig.setIntValue("checkInterval", interval);

        // Todo:  the following options will be handled by email settings.
        //        For now, just set them to a good known default value.
        imapConfig.setIntValue("autoDownload", true);
        imapConfig.setValue("textSubtype", "plain");
        imapConfig.setIntValue("pushEnabled", true);
        imapConfig.setIntValue("maxSize", 30);
        imapConfig.setIntValue("canDelete", true);
        imapConfig.setIntValue("intervalCheckRoamingEnabled", true);

        imapConfig.setValue("pushFolders", "INBOX");
        m_account->setStatus(QMailAccount::CanCreateFolders, true);
        m_account->setStatus(QMailAccount::CanRetrieve, true);
    } 
    //% "POP"
    else if (m_inServerType.compare(qtTrId("xx_pop"), Qt::CaseInsensitive) == 0)
    {
        if (!m_config->services().contains(popServiceKey))
            m_config->addServiceConfiguration(popServiceKey);

        EmailServiceConfiguration popConfig (m_config, popServiceKey);

        popConfig.setVersion(100);
        popConfig.setType(QMailServiceConfiguration::Source);

        popConfig.setValue ("username", m_inUsername);
        popConfig.setValue ("password", popConfig.encode(m_inPassword));
        popConfig.setValue ("server", m_inServerAddress);
        popConfig.setIntValue ("port",  (m_inPort == -1 ? 143 : m_inPort));

        QMailTransport::EncryptType encrypType = QMailTransport::Encrypt_NONE;
        //% "SSL"
        if (QString::compare(m_inSecurity, qtTrId("xx_ssl"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_SSL;
        //% "TLS"
        else if (QString::compare(m_inSecurity, qtTrId("xx_tls"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_TLS;

        popConfig.setIntValue ("encryption", encrypType);

        int interval = EmailSettingsPage::instance()->frequency();
        popConfig.setIntValue("checkInterval", interval);

        // Todo:  the following options will be handled by email settings.
        //        For now, just set them to a good known default value.
        popConfig.setIntValue("autoDownload", true);
        popConfig.setValue("textSubtype", "plain");
        popConfig.setIntValue("pushEnabled", true);
        popConfig.setIntValue("maxSize", 30);
        popConfig.setIntValue("canDelete", true);
        popConfig.setIntValue("intervalCheckRoamingEnabled", true);

        m_account->setStatus(QMailAccount::CanRetrieve, true);

    }

    //% "SMTP"
    if (m_outServerType.compare(qtTrId("xx_smtp"), Qt::CaseInsensitive) == 0)
    {
        m_account->setFromAddress(QMailAddress(m_emailAddress));

        if (!m_config->services().contains(smtpServiceKey))
            m_config->addServiceConfiguration(smtpServiceKey);

        EmailServiceConfiguration smtpConfig (m_config, smtpServiceKey);

        smtpConfig.setVersion(100);
        smtpConfig.setType(QMailServiceConfiguration::Sink);

        smtpConfig.setValue("address", m_emailAddress);
        smtpConfig.setValue("server", m_outServerAddress);
        smtpConfig.setIntValue("port", (m_outPort == -1 ? 25 : m_outPort));

        smtpConfig.setIntValue("authentication", m_outAuthentication);

        if (m_outAuthentication != 0)
        {
            smtpConfig.setValue("smtpusername", m_outUsername);
            smtpConfig.setValue("smtppassword", smtpConfig.encode(m_outPassword));
        }

        QMailTransport::EncryptType encrypType = QMailTransport::Encrypt_NONE;
        //% "SSL"
        if (QString::compare(m_outSecurity, qtTrId("xx_ssl"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_SSL;
        //% "TLS"
        else if (QString::compare(m_outSecurity, qtTrId("xx_tls"), Qt::CaseInsensitive) == 0)
           encrypType = QMailTransport::Encrypt_TLS;
        smtpConfig.setIntValue("encryption", encrypType);

        m_account->setStatus(QMailAccount::PreferredSender, true);

        QString signature = EmailSettingsPage::instance()->signature();

        if (signature.size() > 0)
        {
            m_account->setStatus(QMailAccount::AppendSignature, true);
            m_account->setSignature(signature);
        }
        else
        {
            m_account->setStatus(QMailAccount::AppendSignature, false);
            m_account->setSignature("");
        }

        m_account->setStatus(QMailAccount::MessageSink, true);

        if (!smtpConfig.value("server").isEmpty() && !smtpConfig.value("address").isEmpty())
            m_account->setStatus(QMailAccount::CanTransmit, true);
    }

    if (!m_config->services().contains(storageServiceKey))
        m_config->addServiceConfiguration(storageServiceKey);

    QMailAccountConfiguration::ServiceConfiguration &svcCfg(m_config->serviceConfiguration(storageServiceKey));
    svcCfg.setValue("version", "101");
    svcCfg.setValue("servicetype", "storage");

    svcCfg.setValue("basePath", "");

    if (m_account->id().isValid())
    {   // We are dealing with an existing account just update the account.
        QMailStore::instance()->updateAccount(m_account, m_config);
    }
    else
    {
        QMailStore::instance()->addAccount(m_account, m_config);
    }
    QTimer::singleShot(10, this, SLOT(testAccountConfiguration()));
    return true;
}

void AccountInfo::testAccountConfiguration()
{
    if (m_account->status() & (QMailAccount::MessageSource | QMailAccount::MessageSink)) 
    {
        if (m_account->status() & QMailAccount::MessageSource) {
            m_retrievalAction->retrieveFolderList(m_account->id(), QMailFolderId(), true);
        } else if (m_account->status() & QMailAccount::MessageSink) {
            m_transmitAction->transmitMessages(m_account->id());
        }
    }
}

void AccountInfo::activityChanged(QMailServiceAction::Activity activity)
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

void AccountInfo::displayRetrieveProgress(uint value, uint range)
{
    Q_UNUSED (value);
    Q_UNUSED (range);
    // TODO:  might want to put up a progress dialog in future
}

void AccountInfo::displayTransmitProgress(uint value, uint range)
{
    Q_UNUSED (value);
    Q_UNUSED (range);
    // TODO:  might want to put up a progress dialog in future
}
