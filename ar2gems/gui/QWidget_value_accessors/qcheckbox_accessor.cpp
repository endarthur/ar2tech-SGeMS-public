/* -----------------------------------------------------------------------------
** Copyright (c) 2012 Advanced Resources and Risk Technology, LLC
** All rights reserved.
**
** This file is part of Advanced Resources and Risk Technology, LLC (AR2TECH) 
** version of the open source software sgems.  It is a derivative work by 
** AR2TECH (THE LICENSOR) based on the x-free license granted in the original 
** version of the software (see notice below) and now sublicensed such that it 
** cannot be distributed or modified without the explicit and written permission 
** of AR2TECH.
**
** Only AR2TECH can modify, alter or revoke the licensing terms for this 
** file/software.
**
** This file cannot be modified or distributed without the explicit and written 
** consent of AR2TECH.
**
** Contact Dr. Alex Boucher (aboucher@ar2tech.com) for any questions regarding
** the licensing of this file/software
**
** The open-source version of sgems can be downloaded at 
** sourceforge.net/projects/sgems.
** ----------------------------------------------------------------------------*/



/**********************************************************************
** Author: Nicolas Remy
** Copyright (C) 2002-2004 The Board of Trustees of the Leland Stanford Junior
**   University
** All rights reserved.
**
** This file is part of the "gui" module of the Geostatistical Earth
** Modeling Software (GEMS)
**
** This file may be distributed and/or modified under the terms of the 
** license defined by the Stanford Center for Reservoir Forecasting and 
** appearing in the file LICENSE.XFREE included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
** Contact the Stanford Center for Reservoir Forecasting, Stanford University
** if any conditions of this licensing are not clear to you.
**
**********************************************************************/

#include <gui/QWidget_value_accessors/qcheckbox_accessor.h>
#include <utils/gstl_messages.h>
#include <utils/string_manipulation.h>

#include <qstring.h>
#include <qdom.h>


QCheckBox_accessor::QCheckBox_accessor( QWidget* widget ) 
    : checkbox_(dynamic_cast<QCheckBox*>(widget) ) {}

    
bool QCheckBox_accessor::initialize( QWidget* widget ) {

  checkbox_ = dynamic_cast<QCheckBox*>(widget);
  if( checkbox_ == 0 )
    return false;
  
  return true;
}


std::string QCheckBox_accessor::value() const {
  std::string widget_name = String_Op::qstring2string(checkbox_->objectName());
  std::string val;

  if( checkbox_->isChecked() )
    val = "1";
  else
    val = "0";

  return "<" + widget_name + "  value=\"" + val + "\"  /> \n";
}


bool QCheckBox_accessor::set_value( const std::string& str ) {

  QString qstr( str.c_str() );
  
  // str is just an element of an xml file, hence can not be parsed
  // by QDomDocument. We need to add a root element.
  qstr = "<root>" + qstr + "</root>";
  QDomDocument doc;
  bool parsed = doc.setContent( qstr );
  appli_assert( parsed );

  QDomElement root_element = doc.documentElement();
  QDomElement elem = root_element.firstChild().toElement();
  
  QString value = elem.attribute( "value" );

  if( value == "1")
    checkbox_->setChecked(true);
  else
    checkbox_->setChecked(false);

  return true;
}


void QCheckBox_accessor::clear() {
  checkbox_->setChecked(false);
}

Named_interface* QCheckBox_accessor::create_new_interface(std::string&) {
  return new QCheckBox_accessor(0);
}
