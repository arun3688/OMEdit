/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR 
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2. 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE. 
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */
/*
 * 
 * @author Adeel Asghar <adeel.asghar@liu.se>
 *
 * RCS: $Id$
 *
 */

#include "NotificationsDialog.h"

/*!
  \class NotificationsDialog
  \brief Creates a notification dialog.

  Define a new notification type if you want to add a new Dialog.\n
  Also add the notification to NotificationPage class.\n
  Save the notification in omedit.ini settings file by defining a new key in notifications section.
  */

/*!
  \param notificationType - specifies what type of notification dialog should be created.
  \param notificationIcon - specifies which icon should be used.
  \param pMainWindow - pointer to MainWindow.
  */
NotificationsDialog::NotificationsDialog(NotificationType notificationType, NotificationIcon notificationIcon, MainWindow *pMainWindow)
  : QDialog(pMainWindow, Qt::WindowTitleHint)
{
  mpMainWindow = pMainWindow;
  mNotificationType = notificationType;
  setAttribute(Qt::WA_DeleteOnClose);
  setWindowTitle(QString(Helper::applicationName).append(" - ").append(getNotificationTitleString()));
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  // Notification Icon
  QPixmap pixmap(getNotificationIcon(notificationIcon));
  Label *pPixmapLabel = new Label;
  pPixmapLabel->setPixmap(pixmap);
  // Notificaiton Label
  mpNotificationLabel = new Label(getNotificationLabelString());
  mpNotificationLabel->setTextFormat(Qt::RichText);
  mpNotificationLabel->setTextInteractionFlags(mpNotificationLabel->textInteractionFlags() | Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
  mpNotificationLabel->setOpenExternalLinks(true);
  // Notificaiton Checkbox
  mpNotificationCheckBox = new QCheckBox(getNotificationCheckBoxString());
  // Notificaiton buttons
  mpOkButton = new QPushButton(Helper::ok);
  mpOkButton->setAutoDefault(true);
  connect(mpOkButton, SIGNAL(clicked()), SLOT(saveNotification()));
  mpCancelButton = new QPushButton(Helper::cancel);
  mpCancelButton->setAutoDefault(false);
  connect(mpCancelButton, SIGNAL(clicked()), SLOT(reject()));
  mpButtonBox = new QDialogButtonBox(Qt::Horizontal);
  mpButtonBox->addButton(mpOkButton, QDialogButtonBox::ActionRole);
  mpButtonBox->addButton(mpCancelButton, QDialogButtonBox::ActionRole);
  // horizontal layout
  QHBoxLayout *pHorizontalLayout = new QHBoxLayout;
  pHorizontalLayout->addWidget(pPixmapLabel, 0, Qt::AlignTop);
  pHorizontalLayout->addWidget(mpNotificationLabel, 0, Qt::AlignTop);
  // main layout
  QGridLayout *pMainLayout = new QGridLayout;
  pMainLayout->addLayout(pHorizontalLayout, 0, 0, 1, 2, Qt::AlignTop | Qt::AlignLeft);
  pMainLayout->addWidget(mpNotificationCheckBox, 1, 0, Qt::AlignLeft | Qt::AlignBottom);
  pMainLayout->addWidget(mpButtonBox, 1, 1, Qt::AlignRight | Qt::AlignBottom);
  setLayout(pMainLayout);
}

/*!
  Sets the notification dialog message.
  \param label - the message to set.
  */
void NotificationsDialog::setNotificationLabelString(QString label)
{
  mpNotificationLabel->setText(label);
}

/*!
  Returns the notification icon.
  \param notificationIcon - the notificaiton icon to set.
  \return the notificaiton icon as QPixmap
  */
QPixmap NotificationsDialog::getNotificationIcon(NotificationsDialog::NotificationIcon notificationIcon)
{
  QStyle *pStyle = this->style();
  int iconSize = pStyle->pixelMetric(QStyle::PM_MessageBoxIconSize, 0, this);
  QIcon tmpIcon;
  switch (notificationIcon)
  {
    case NotificationsDialog::InformationIcon:
      tmpIcon = pStyle->standardIcon(QStyle::SP_MessageBoxInformation, 0, this);
      break;
    case NotificationsDialog::WarningIcon:
      tmpIcon = pStyle->standardIcon(QStyle::SP_MessageBoxWarning, 0, this);
      break;
    case NotificationsDialog::CriticalIcon:
      tmpIcon = pStyle->standardIcon(QStyle::SP_MessageBoxCritical, 0, this);
      break;
    case NotificationsDialog::QuestionIcon:
      tmpIcon = pStyle->standardIcon(QStyle::SP_MessageBoxQuestion, 0, this);
    default:
      break;
  }
  if (!tmpIcon.isNull())
    return tmpIcon.pixmap(iconSize, iconSize);
  return QPixmap();
}

/*!
  Returns the notification dialog title.
  \return the title string.
  */
QString NotificationsDialog::getNotificationTitleString()
{
  switch (mNotificationType)
  {
    case NotificationsDialog::QuitApplication:
      return tr("Confirm Quit");
    case NotificationsDialog::ItemDroppedOnItself:
    case NotificationsDialog::ReplaceableIfPartial:
    case NotificationsDialog::InnerModelNameChanged:
    case NotificationsDialog::SaveModelForBitmapInsertion:
    case NotificationsDialog::ReleaseInformation:
      return Helper::information;
    case NotificationsDialog::CrashReport:
      return Helper::error;
    default:
      // should never be reached
      return "No String is defined for your notification type in NotificationsDialog::getNotificationTitleString()";
  }
}

/*!
  Returns the notification dialog message.
  \return the message string.
  */
QString NotificationsDialog::getNotificationLabelString()
{
  QString tmpPath, OMCCommandsLogFilePath, OMCOutputFile, stackTraceFile;
#ifndef WIN32
  char *user;
#endif
  switch (mNotificationType)
  {
    case NotificationsDialog::QuitApplication:
      return tr("Are you sure you want to quit?");
    case NotificationsDialog::ItemDroppedOnItself:
      return GUIMessages::getMessage(GUIMessages::ITEM_DROPPED_ON_ITSELF);
    case NotificationsDialog::ReplaceableIfPartial:
    case NotificationsDialog::InnerModelNameChanged:
      return "this string needs argument and will be set via NotificationsDialog::setNotificationLabelString(QString label)";
    case NotificationsDialog::SaveModelForBitmapInsertion:
      return tr("You must save the class before referencing a bitmap from local directory.");
    case NotificationsDialog::ReleaseInformation:
      return tr("Welcome to new enhanced OMEdit - OpenModelica Connection Editor.\n"
                "This version includes a lot of improvements and bug fixes. Check release notes for more details.<br /><br />"
                "It is highly recommended to delete the old OMEdit settings file. Click \"OK\" to delete.<br />"
                "Contact us [OpenModelica@ida.liu.se] or Adeel Asghar [adeel.asghar@liu.se] with any comments, suggestions or problems.");
    case NotificationsDialog::CrashReport:
      tmpPath = QDir::tempPath() + "/OpenModelica/OMEdit/";
      tmpPath.remove("\"");
#ifdef WIN32 // Win32
       OMCCommandsLogFilePath = QString("%1omeditcommands.log").arg(tmpPath);
       OMCOutputFile = QString("%1openmodelica.omc.output.%2").arg(tmpPath).arg(Helper::OMCServerName);
       stackTraceFile = QString("%1openmodelica.stacktrace.%2").arg(tmpPath).arg(Helper::OMCServerName);
#else // UNIX environment
      user = getenv("USER");
      if (!user) { user = "nobody"; }
      OMCCommandsLogFilePath = QString("%1omeditcommands.%2.log").arg(tmpPath).arg(QString(user));
      OMCOutputFile = QString("%1openmodelica.%2.omc.output.%3").arg(tmpPath).arg(QString(user)).arg(Helper::OMCServerName);
      stackTraceFile = QString("%1openmodelica.%2.stacktrace.%3").arg(tmpPath).arg(QString(user)).arg(Helper::OMCServerName);
#endif
      return QString(tr("Opps! Something went wrong. Click \"OK\" to send the crash report.<br /><br />")
                     .append(tr("Please attach the following files alongwith your bug description in your crash report,")).append("<br /><br />")
                     .append("1. " + OMCCommandsLogFilePath + "<br />")
                     .append("2. " + OMCOutputFile + "<br />")
                     .append("3. " + stackTraceFile));
    default:
      // should never be reached
      return "No String is defined for your notification type in NotificationsDialog::getNotificationLabelString()";
  }
}

/*!
  Returns the notification dialog checkbox.
  \return the checkbox.
  */
QCheckBox* NotificationsDialog::getNotificationCheckBox()
{
  return mpNotificationCheckBox;
}

/*!
  Returns the notification dialog checkbox message.
  \return the checkbox message string.
  */
QString NotificationsDialog::getNotificationCheckBoxString()
{
  switch (mNotificationType)
  {
    case NotificationsDialog::QuitApplication:
      return tr("Always quit without prompt");
    case NotificationsDialog::ItemDroppedOnItself:
    case NotificationsDialog::ReplaceableIfPartial:
    case NotificationsDialog::InnerModelNameChanged:
    case NotificationsDialog::SaveModelForBitmapInsertion:
      return Helper::dontShowThisMessageAgain;
    case NotificationsDialog::ReleaseInformation:
    case NotificationsDialog::CrashReport:
    default:
      // should never be reached
      return "No String is defined for your notification type in NotificationsDialog::getNotificationCheckBoxString()";
  }
}

/*!
  Saves the promptQuitApplication key settings to omedit.ini file.
  */
void NotificationsDialog::saveQuitNotificationSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, Helper::organization, Helper::application);
  settings.setValue("notifications/promptQuitApplication", true);
}

/*!
  Saves the notifications/itemDroppedOnItself key settings to omedit.ini file.\n
  Sets the notifications/itemDroppedOnItself notification checkbox on the NotificationsPage.
  */
void NotificationsDialog::saveItemDroppedOnItselfNotificationSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, Helper::organization, Helper::application);
  settings.setValue("notifications/itemDroppedOnItself", false);
  mpMainWindow->getOptionsDialog()->getNotificationsPage()->getItemDroppedOnItselfCheckBox()->setChecked(false);
}

/*!
  Saves the notifications/replaceableIfPartial key settings to omedit.ini file.\n
  Sets the notifications/replaceableIfPartial notification checkbox on the NotificationsPage.
  */
void NotificationsDialog::saveReplaceableIfPartialNotificationSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, Helper::organization, Helper::application);
  settings.setValue("notifications/replaceableIfPartial", false);
  mpMainWindow->getOptionsDialog()->getNotificationsPage()->getReplaceableIfPartialCheckBox()->setChecked(false);
}

/*!
  Saves the notifications/innerModelNameChanged key settings to omedit.ini file.\n
  Sets the notifications/innerModelNameChanged notification checkbox on the NotificationsPage.
  */
void NotificationsDialog::saveInnerModelNameChangedNotificationSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, Helper::organization, Helper::application);
  settings.setValue("notifications/innerModelNameChanged", false);
  mpMainWindow->getOptionsDialog()->getNotificationsPage()->getInnerModelNameChangedCheckBox()->setChecked(false);
}

/*!
  Saves the notifications/saveModelForBitmapInsertion key settings to omedit.ini file.\n
  Sets the notifications/saveModelForBitmapInsertion notification checkbox on the NotificationsPage.
  */
void NotificationsDialog::saveModelForBitmapInsertionNotificationSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, Helper::organization, Helper::application);
  settings.setValue("notifications/saveModelForBitmapInsertion", false);
  mpMainWindow->getOptionsDialog()->getNotificationsPage()->getSaveModelForBitmapInsertionCheckBox()->setChecked(false);
}

/*!
  Saves the notifications/releaseInformation key settings to omedit.ini file.
  */
void NotificationsDialog::saveReleaseInformationNotificationSettings()
{
  QSettings settings(QSettings::IniFormat, QSettings::UserScope, Helper::organization, Helper::application);
  settings.clear();
  settings.setValue("notifications/releaseInformation", false);
}

/*!
  Sends the crash report.
  */
void NotificationsDialog::sendCrashReport()
{
  QString OMCCommandsLogFilePath, OMCOutputFile, stackTraceFile;
  OMCCommandsLogFilePath = QString(QDir::tempPath()).append("/OpenModelica/OMEdit/omeditcommands.log");
#ifdef WIN32 // Win32
  OMCOutputFile = QString(QDir::tempPath()).append("/OpenModelica/OMEdit/openmodelica.omc.output.").append(Helper::OMCServerName);
#else // UNIX environment
  char *user = getenv("USER");
  if (!user) { user = "nobody"; }
  OMCOutputFile = QString(QDir::tempPath()).append("/OpenModelica/OMEdit/openmodelica.").append(*(new QString(user))).append(".omc.output.").append(Helper::OMCServerName);
#endif
  stackTraceFile = QString(QDir::tempPath()).append("/OpenModelica/OMEdit/openmodelica.stacktrace.").append(Helper::OMCServerName);
  QString body = QString("Please attach the following files alongwith your bug description in your crash report,\n\n")
      .append("1. " + OMCCommandsLogFilePath + "\n")
      .append("2. " + OMCOutputFile + "\n")
      .append("3. " + stackTraceFile);
  QUrl mailToUrl ("mailto:openmodelicacrashreports@ida.liu.se?CC=adeel.asghar@liu.se&subject=OpenModelica Crash Report&body=" + body);
  QDesktopServices::openUrl(mailToUrl);
}

/*!
  Slot activated when mpOkButton clicked signal is raised.\n
  Checks the notification type and calls the appropriate method.
  */
void NotificationsDialog::saveNotification()
{
  if (mpNotificationCheckBox->isChecked())
  {
    switch (mNotificationType)
    {
      case NotificationsDialog::QuitApplication:
        saveQuitNotificationSettings();
        break;
      case NotificationsDialog::ItemDroppedOnItself:
        saveItemDroppedOnItselfNotificationSettings();
        break;
      case NotificationsDialog::ReplaceableIfPartial:
        saveReplaceableIfPartialNotificationSettings();
        break;
      case NotificationsDialog::InnerModelNameChanged:
        saveInnerModelNameChangedNotificationSettings();
        break;
      case NotificationsDialog::SaveModelForBitmapInsertion:
        saveModelForBitmapInsertionNotificationSettings();
        break;
      case NotificationsDialog::ReleaseInformation:
        saveReleaseInformationNotificationSettings();
        break;
      case NotificationsDialog::CrashReport:
        sendCrashReport();
        break;
      default:
        // should never be reached
        break;
    }
  }
  else
  {
    switch (mNotificationType)
    {
      case NotificationsDialog::ReleaseInformation:
        saveReleaseInformationNotificationSettings();
        break;
      case NotificationsDialog::CrashReport:
        sendCrashReport();
        break;
      default:
        // should never be reached
        break;
    }
  }
  accept();
}
