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
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MLabel>
#include <MComboBox>
#include <MDialog>
#include <MButton>
#include <MSeparator>
#include <QStringList>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>

#include <qmailstore.h>
#include <qmailfolder.h>
#include <qmailserviceaction.h>

#include "emailapplication.h"
#include "emailsettingspage.h"
#include "emailserviceconfiguration.h"
#include "accountinfo.h"
#include "accountsetuppage.h"
#include "mailmanager.h"

EmailSettingsPage *EmailSettingsPage::m_instance = 0;
QStringList EmailSettingsPage::m_frequencyList;
QStringList EmailSettingsPage::m_inServerTypes;
QStringList EmailSettingsPage::m_outServerTypes;
QStringList EmailSettingsPage::m_securityTypes;
QStringList EmailSettingsPage::m_authTypes;

EmailSettingsPage *EmailSettingsPage::instance()
{
    if (!m_instance)
    {
        // Initialize some of the strings used in account settings here. 
        // This makes the i18n work go easier.

        //% "Every 5 minutes"
        m_frequencyList.append(qtTrId("xx_frequency_5_minutes"));
        //% "Every 15 minutes"
        m_frequencyList.append(qtTrId("xx_frequency_15_minutes"));
        //% "Every 30 minutes"
        m_frequencyList.append(qtTrId("xx_frequency_30_minutes"));
        //% "Every hour"
        m_frequencyList.append(qtTrId("xx_frequency_hour"));
        //% "Manual checking only"
        m_frequencyList.append(qtTrId("xx_frequency_manual"));

        //% "None"
        m_inServerTypes.append(qtTrId("xx_none"));
        //% "IMAP"
        m_inServerTypes.append(qtTrId("xx_imap"));
        //% "POP"
        m_inServerTypes.append(qtTrId("xx_pop"));

        m_outServerTypes.append(qtTrId("xx_none"));
        //% "SMTP"
        m_outServerTypes.append(qtTrId("xx_smtp"));

        m_securityTypes.append(qtTrId("xx_none"));
        //% "SSL"
        m_securityTypes.append(qtTrId("xx_ssl"));
        //% "TLS"
        m_securityTypes.append(qtTrId("xx_tls"));

        m_authTypes.append(qtTrId("xx_none"));
        //% "Login"
        m_authTypes.append(qtTrId("xx_login"));
        //% "Plain"
        m_authTypes.append(qtTrId("xx_plain"));


        m_instance = new EmailSettingsPage();
    }

    return m_instance;
}

EmailSettingsPage::EmailSettingsPage()
    : m_accountsContainer(NULL), 
      m_frequency(-1),
      m_frequencyModified(false),
      m_emailAlert(true),
      m_askBeforeDeleting (true),
      m_signature (""),
      m_signatureModified(false)
{
    MApplicationPage::createContent();
    setContentsMargins(0, 0, 0, 0);
    setPannable(true);
    //% "Mail"
    setTitle(qtTrId("xx_page_title"));

    MLayout *layout = new MLayout(centralWidget());
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAnimation(0);

    m_policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    m_policy->setSpacing(5);

    MWidgetController *container = new MWidgetController;
    container->setViewType ("background");
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    layout = new MLayout (container);
    layout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *containerPolicy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    containerPolicy->setSpacing(5);
    MSeparator *separator = new MSeparator();
    separator->setObjectName("Separator");
    m_policy->addItem(separator);
    //% "Settings:"
    containerPolicy->addItem(new MLabel(qtTrId("xx_settings_label")));

    QMailAccountIdList ids = MailManager::instance()->accountIdList();
    int fIndex = 4;  // manual check by default
    if (ids.size() > 0)
    {
        // Hack- Since the settings were deisgn to be global, we can just pick up the preivous
        // settigns from one of the existing account.
        QMailAccountConfiguration config(ids[0]);
        QStringList services = config.services();

        foreach (QString service, services)
        {
            EmailServiceConfiguration svcCfg(&config, service);
            if (svcCfg.type() == QMailServiceConfiguration::Source)
            {
                m_frequency = svcCfg.value ("checkInterval").toInt();
                if  (m_frequency == 5)
                    fIndex = 0;
                else if  (m_frequency == 15)
                    fIndex = 1;
                else if  (m_frequency == 30)
                    fIndex = 2;
                else if  (m_frequency == 60)
                    fIndex = 3;
            }
            QMailAccount account (ids[0]);
            m_signature = account.signature();
        }
    }

    MComboBox *frequency = new MComboBox();
    frequency->addItems(m_frequencyList);

    //% "Check frequency"
    frequency->setTitle(qtTrId("xx_check_frequency"));
    frequency->setCurrentIndex (fIndex);
    connect(frequency, SIGNAL(currentIndexChanged(int)), this, SLOT(frequencyChanged(int)));
    containerPolicy->addItem(frequency);

    MWidgetController *widget = new MWidgetController;
    widget->setViewType("background");
    QGraphicsGridLayout *grid = new QGraphicsGridLayout;
    widget->setLayout(grid);
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Add new email alert setting
    //% "New email alert"
    MLabel *label = new MLabel(qtTrId("xx_new_email_alert"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem (label, 0, 0, Qt::AlignLeft);
    MButton *button = new MButton(this);
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setChecked(m_emailAlert);
    connect(button, SIGNAL(toggled(bool)), this, SLOT(emailAlertChanged(bool)));
    grid->addItem (button, 0, 1, Qt::AlignLeft);

    // Add Ask before deleting setting
    //% "Ask before deleting"
    label = new MLabel(qtTrId("xx_ask_before_deleting"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    grid->addItem (label, 1, 0, Qt::AlignLeft);
    button = new MButton(this);
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setChecked(m_askBeforeDeleting);
    connect(button, SIGNAL(toggled(bool)), this, SLOT(askBeforeDeletingChanged(bool)));
    grid->addItem (button, 1, 1, Qt::AlignLeft);

    containerPolicy->addItem(widget);

    //% "Edit signature"
    button = new MButton(qtTrId("xx_edit_signature"), this);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(editSignature()));
    containerPolicy->addItem(button);

    m_policy->addItem(container);

    connect (QMailStore::instance(), SIGNAL(accountsAdded(const QMailAccountIdList &)), this,
             SLOT(handleAccountsChanged(const QMailAccountIdList &)));
    connect (QMailStore::instance(), SIGNAL(accountsRemoved(const QMailAccountIdList &)), this,
             SLOT(handleAccountsChanged(const QMailAccountIdList &)));
    connect (QMailStore::instance(), SIGNAL(accountsUpdated(const QMailAccountIdList &)), this,
             SLOT(handleAccountsChanged(const QMailAccountIdList &)));
}

void EmailSettingsPage::appear(MSceneWindow::DeletionPolicy policy)
{
    updateAccountDisplayList();
    MApplicationPage::appear(policy);
}

EmailSettingsPage::~EmailSettingsPage()
{
}


void EmailSettingsPage::updateAccountDisplayList()
{
    if (m_accountsContainer)
    {
        m_policy->removeItem(m_accountsContainer);
        delete m_accountsContainer;
        m_accountsContainer = NULL;
        m_accountButtons.clear();
    }

    m_accountsContainer = new MWidgetController;
    m_accountsContainer->setViewType ("background");
    m_accountsContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    MLayout *layout = new MLayout (m_accountsContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    MLinearLayoutPolicy *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    policy->setSpacing(5);
    //% "Accounts:"
    policy->addItem(new MLabel(qtTrId("xx_accounts_label")));

    QMailAccountIdList accounts = MailManager::instance()->accountIdList();

    foreach (QMailAccountId id, accounts)
    {
        QMailAccount mailAccount(id);

        //% "Edit %1"
        MButton *button = new MButton(qtTrId("xx_edit").arg(mailAccount.name()));
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        policy->addItem(button);
        m_accountButtons.insert(button, id);
        connect(button, SIGNAL(clicked()), this, SLOT(handleAccountSelection()));
    }

    //% "Add new account"
    MButton *button = new MButton(qtTrId("xx_add_new_account"), m_accountsContainer);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(button, SIGNAL(clicked(bool)), this, SIGNAL(addNewAccount()));
    policy->addItem(button);

    m_policy->insertItem(0, m_accountsContainer);
}

void EmailSettingsPage::handleAccountsChanged(const QMailAccountIdList & ids)
{
    Q_UNUSED(ids);
    // just update the account list view.
    updateAccountDisplayList();
}

void EmailSettingsPage::handleAccountSelection()
{
    QObject *sender_object = sender();

    if (!sender_object)
        return;

    MButton *button = qobject_cast<MButton *>(sender_object);

    if (button) 
    {
        QMailAccountId id = m_accountButtons.value(button);
        emit editExistingAccount(id);
    }
}

void EmailSettingsPage::emailAlertChanged(bool checked)
{
    m_emailAlert = checked;
}

void EmailSettingsPage::askBeforeDeletingChanged(bool checked)
{
    m_askBeforeDeleting = checked;
}

void EmailSettingsPage::editSignature()
{
    // create an entry dialog to prompt the user for the signature

    MWidget *centralWidget = new MWidget;
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);

    MTextEdit *edit = new MTextEdit(MTextEditModel::MultiLine, QString(), centralWidget);
    if (m_signature.size())
        edit->setText(m_signature);
    centralWidget->setLayout(layout);
    layout->addItem(edit);

    //% "Edit signature"
    MDialog dialog(qtTrId("xx_edit_signature_dialog_title"), M::OkButton | M::CancelButton);
    dialog.setCentralWidget(centralWidget);
    dialog.appear(MSceneWindow::DestroyWhenDone);
    if (dialog.exec() == MDialog::Accepted)
    {
        QString newSignature = edit->text();
        if (m_signature != newSignature)
        {
            m_signature = newSignature;
            m_signatureModified = true;
            updateAccounts();
        }
    }
}

void EmailSettingsPage::frequencyChanged(int index)
{
    if (index == 0)
        m_frequency = 5;
    else if (index == 1)
        m_frequency = 15;
    else if (index == 2)
        m_frequency = 30;
    else if (index == 3)
        m_frequency = 60;
    else
        m_frequency = -1;

    m_frequencyModified = true;
    updateAccounts();
}

void EmailSettingsPage::updateAccounts()
{
    // update all accounts with the new settings
    QMailAccountIdList ids = MailManager::instance()->accountIdList();

    foreach (QMailAccountId id, ids)
    {
        QMailAccountConfiguration config = QMailStore::instance()->accountConfiguration(id);

        QMailAccount account(id);
        if (m_signatureModified)
        {
            if (m_signature.size() > 0)
                account.setStatus(QMailAccount::AppendSignature, true);
            else
                account.setStatus(QMailAccount::AppendSignature, false);
            account.setSignature(m_signature);
            QMailStore::instance()->updateAccount(&account, &config);
        }

        if (m_frequencyModified)
        {
            QStringList services = config.services();

            foreach (QString service, services)
            {
                EmailServiceConfiguration svcCfg(&config, service);
                if (svcCfg.type() == QMailServiceConfiguration::Source)
                {
                    svcCfg.setIntValue ("checkInterval", m_frequency);
                    QMailStore::instance()->updateAccount(&account, &config);
                }
            }
        }
    }

    m_signatureModified = false;
    m_frequencyModified = false;
}
