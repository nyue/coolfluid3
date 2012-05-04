
// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "ui/core/SSHTunnel.hpp"

#include <QObject>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFormLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QSettings>
#include <QCheckBox>
#include <QPushButton>

#include <iostream>

//////////////////////////////////////////////////////////////////////////////

using namespace cf3::common;
using namespace cf3::common::XML;

//////////////////////////////////////////////////////////////////////////////

namespace cf3 {
namespace ui {
namespace core {

////////////////////////////////////////////////////////////////////////////

SSHTunnel::SSHTunnel(QObject* parent,quint16 local_port, quint16 distant_port
                     , QString gateway_host, QString gateway_user
                     , QString gateway_pass,QString distant_host)
  : QProcess(parent){
  QString filename=QDir::currentPath()+"/ssh_simple_tunnel.tcl";
  if (!QFile::exists(filename)){
    //some error
  }
  QString command=filename+" "+QString::number(local_port)+" "+QString::number(distant_port)+" "
      +gateway_host+" "+gateway_user+" "+gateway_pass+" "+distant_host;
  connect(this,SIGNAL(readyReadStandardOutput()),this,SLOT(process_sent_output()));
  connect(this,SIGNAL(readyReadStandardError()),this,SLOT(process_sent_error()));
  connect(this,SIGNAL(finished(int)),this,SLOT(process_end(int)));
  start(command);
  ssh_tunnel_is_open=true;
}

SSHTunnel::SSHTunnel(QObject* parent,QString local_host,QString gateway_host,QString distant_host
                     ,quint16 local_port,quint16 distant_port,QString local_user,QString gateway_user
                     ,QString distant_user,QString local_pass,QString gateway_pass,QString distant_pass)
  : QProcess(parent){
  QString filename=QDir::currentPath()+"/ssh_reverse_tunnel.tcl";
  if (!QFile::exists(filename)){
    //some error
  }
  QString command=filename+" "+local_host+" "+gateway_host+" "+distant_host+" "
      +QString::number(local_port)+" "+QString::number(distant_port)+" "
      +local_user+" "+gateway_user+" "+distant_user+" "+local_pass+" "+gateway_pass+" "+distant_pass;
  connect(this,SIGNAL(readyReadStandardOutput()),this,SLOT(process_sent_output()));
  connect(this,SIGNAL(readyReadStandardError()),this,SLOT(process_sent_error()));
  connect(this,SIGNAL(finished(int)),this,SLOT(process_end(int)));
  start(command);
  ssh_tunnel_is_open=true;
}

SSHTunnel::~SSHTunnel(){
  if (ssh_tunnel_is_open){
    kill();
    if (waitForFinished())
      std::cout << "ssh tunnel killed" << std::endl;
    else
      std::cout << "unable to kill the ssh tunnel" << std::endl;
  }
}

void SSHTunnel::process_sent_output(){
  QString output(readAllStandardOutput().data());
  std::cout << "SSH OUTPUT :" << output.toStdString() << std::endl;
}

void SSHTunnel::process_sent_error(){
  QString output(readAllStandardError().data());
  std::cout << "SSH ERROR :" << output.toStdString() << std::endl;
  //kill();
}

void SSHTunnel::process_end(int status){
  Q_UNUSED(status);
  ssh_tunnel_is_open=false;
  std::cout << "ssh tunnel ended" << std::endl;
}

SSHTunnel* SSHTunnel::simple_tunnel_popup(QWidget *parent){
  QSettings settings("vki.ac.be", "coolfluid-client");
  QDialog dialog(parent);
  dialog.setWindowTitle("Simple tunnel configuration");
  QFormLayout *main_layout=new QFormLayout();
  QLineEdit *gateway_hostname=new QLineEdit(settings.value("ssh_tunel/gateway_hostname").toString());
  QSpinBox *local_port=new QSpinBox();
  local_port->setRange(0,USHRT_MAX);
  local_port->setValue(settings.value("ssh_tunel/local_port").toInt());
  QLineEdit *gateway_username=new QLineEdit(settings.value("ssh_tunel/gateway_username").toString());
  QLineEdit *gateway_password=new QLineEdit();
  gateway_password->setEchoMode(QLineEdit::Password);
  QLineEdit *distant_hostname=new QLineEdit(settings.value("ssh_tunel/distant_hostname").toString());
  QSpinBox *distant_port=new QSpinBox();
  distant_port->setRange(0,USHRT_MAX);
  distant_port->setValue(settings.value("ssh_tunel/distant_port").toInt());
  QPushButton *cancel_button=new QPushButton("Cancel");
  QPushButton *ok_button=new QPushButton("Accept");
  main_layout->addRow("Gateway Hostname :", gateway_hostname);
  main_layout->addRow("Gateway Username :", gateway_username);
  main_layout->addRow("Gateway Password :", gateway_password);
  main_layout->addRow("Distant Hostname :", distant_hostname);
  main_layout->addRow("Local Port :", local_port);
  main_layout->addRow("Distant Port :", distant_port);
  main_layout->addRow(cancel_button, ok_button);
  dialog.setLayout(main_layout);
  connect(ok_button,SIGNAL(pressed()),&dialog,SLOT(accept()));
  connect(cancel_button,SIGNAL(pressed()),&dialog,SLOT(reject()));
  if (dialog.exec() == QDialog::Accepted){
    settings.setValue("ssh_tunel/gateway_hostname",gateway_hostname->text());
    settings.setValue("ssh_tunel/local_port",local_port->value());
    settings.setValue("ssh_tunel/gateway_username",gateway_username->text());
    settings.setValue("ssh_tunel/distant_hostname",distant_hostname->text());
    settings.setValue("ssh_tunel/distant_port",distant_port->value());
    settings.sync();
    return new SSHTunnel(parent
                         ,(quint16)local_port->value()
                         ,(quint16)distant_port->value()
                         ,gateway_hostname->text()
                         ,gateway_username->text()
                         ,gateway_password->text()
                         ,distant_hostname->text());
  }
  return NULL;
}

SSHTunnel* SSHTunnel::reverse_tunnel_popup(QWidget* parent){
  QSettings settings("vki.ac.be", "coolfluid-client");
  QDialog dialog(parent);
  dialog.setWindowTitle("Reverse tunnel configuration");
  QFormLayout *main_layout=new QFormLayout();
  QLineEdit *local_hostname=new QLineEdit(settings.value("ssh_tunel/local_hostname").toString());
  QLineEdit *local_username=new QLineEdit(settings.value("ssh_tunel/local_username").toString());
  QLineEdit *local_password=new QLineEdit();
  local_password->setEchoMode(QLineEdit::Password);
  QLineEdit *gateway_hostname=new QLineEdit(settings.value("ssh_tunel/gateway_hostname").toString());
  QLineEdit *gateway_username=new QLineEdit(settings.value("ssh_tunel/gateway_username").toString());
  QLineEdit *gateway_password=new QLineEdit();
  gateway_password->setEchoMode(QLineEdit::Password);
  QLineEdit *distant_hostname=new QLineEdit(settings.value("ssh_tunel/distant_hostname").toString());
  QLineEdit *distant_username=new QLineEdit(settings.value("ssh_tunel/distant_username").toString());
  QLineEdit *distant_password=new QLineEdit();
  distant_password->setEchoMode(QLineEdit::Password);
  QSpinBox *local_port=new QSpinBox();
  local_port->setRange(0,USHRT_MAX);
  local_port->setValue(settings.value("ssh_tunel/local_port").toInt());
  QSpinBox *distant_port=new QSpinBox();
  distant_port->setRange(0,USHRT_MAX);
  distant_port->setValue(settings.value("ssh_tunel/distant_port").toInt());
  QPushButton *cancel_button=new QPushButton("Cancel");
  QPushButton *ok_button=new QPushButton("Accept");
  main_layout->addRow("Local Hostname :", local_hostname);
  main_layout->addRow("Local Username :", local_username);
  main_layout->addRow("Local Password :", local_password);
  main_layout->addRow("Gateway Hostname :", gateway_hostname);
  main_layout->addRow("Gateway Username :", gateway_username);
  main_layout->addRow("Gateway Password :", gateway_password);
  main_layout->addRow("Distant Hostname :", distant_hostname);
  main_layout->addRow("Distant Username :", distant_username);
  main_layout->addRow("Distant Password :", distant_password);
  main_layout->addRow("Local Port :", local_port);
  main_layout->addRow("Distant Port :", distant_port);
  main_layout->addRow(cancel_button, ok_button);
  dialog.setLayout(main_layout);
  connect(ok_button,SIGNAL(pressed()),&dialog,SLOT(accept()));
  connect(cancel_button,SIGNAL(pressed()),&dialog,SLOT(reject()));
  if (dialog.exec() == QDialog::Accepted){
    settings.setValue("ssh_tunel/local_hostname",local_hostname->text());
    settings.setValue("ssh_tunel/local_username",local_username->text());
    settings.setValue("ssh_tunel/gateway_hostname",gateway_hostname->text());
    settings.setValue("ssh_tunel/gateway_username",gateway_username->text());
    settings.setValue("ssh_tunel/distant_hostname",distant_hostname->text());
    settings.setValue("ssh_tunel/distant_username",distant_username->text());
    settings.setValue("ssh_tunel/local_port",local_port->value());
    settings.setValue("ssh_tunel/distant_port",distant_port->value());
    settings.sync();
    return new SSHTunnel(parent
                         ,local_hostname->text()
                         ,gateway_hostname->text()
                         ,distant_hostname->text()
                         ,(quint16)local_port->value()
                         ,(quint16)distant_port->value()
                         ,local_username->text()
                         ,gateway_username->text()
                         ,distant_username->text()
                         ,local_password->text()
                         ,gateway_password->text()
                         ,distant_password->text());
  }
  return NULL;
}

} // Core
} // ui
} // cf3

//////////////////////////////////////////////////////////////////////////////
