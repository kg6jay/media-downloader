/*
 *
 *  Copyright (c) 2021
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "translator.h"
#include "settings.h"
#include "locale_path.h"

#include <QCoreApplication>

translator::translator( settings& s,QApplication& app ) : m_qapp( app ),m_settings( s )
{
	this->addString( QObject::tr( "Polish (Poland)" ),"Polish (Poland)","pl_PL" ) ;
	this->addString( QObject::tr( "English (US)" ),"English (US)","en_US" ) ;
	this->addString( QObject::tr( "Spanish (Spain)" ),"Spanish (Spain)","es" ) ;
	this->addString( QObject::tr( "Chinese (China)" ),"Chinese (China)","zh_CN" ) ;

	this->setDefaultLanguage() ;
}

void translator::setLanguage( const QString& e )
{
	m_qapp.installTranslator( [ & ](){

		this->clear() ;

		m_translator = new QTranslator() ;

		m_translator->load( e,m_settings.localizationLanguagePath() ) ;

		return m_translator ;
	}() ) ;

	for( const auto& it : m_actions ){

		it.first->setText( QObject::tr( it.second.UINameUnTranslated ) ) ;
	}

	for( const auto& it : m_menus ){

		it.first->setTitle( QObject::tr( it.second.UINameUnTranslated ) ) ;
	}
}

void translator::setDefaultLanguage()
{
	this->setLanguage( m_settings.localizationLanguage() ) ;
}

translator::~translator()
{
}

const QString& translator::UIName( const QString& internalName )
{
	static QString s ;

	for( const auto& it : m_languages ){

		if( it.internalName == internalName ){

			s = QObject::tr( it.UINameUnTranslated ) ;

			return s ;
		}
	}

	s = internalName ;

	return s ;
}

const QString& translator::name( const QString& UIName )
{
	for( const auto& it : m_languages ){

		if( it.UINameTranslated == UIName ){

			return it.internalName ;
		}
	}

	static QString s ;
	return s ;
}

QString translator::translate( const QString& internalName )
{
	return QObject::tr( this->UINameUnTranslated( internalName ) ) ;
}

const char * translator::UINameUnTranslated( const QString& internalName )
{
	for( const auto& it : m_languages ){

		if( it.internalName == internalName ){

			return it.UINameUnTranslated ;
		}
	}

	return "" ;
}

void translator::addString( const QString& translatedString,
			    const char * untranslatedString,
			    const QString & internalName)
{
	m_languages.emplace_back( entry( translatedString,untranslatedString,internalName ) ) ;
}

QAction * translator::addAction( QMenu * m,translator::entry e,bool permanentEntry )
{
	auto ac = m->addAction( e.UINameTranslated ) ;

	ac->setObjectName( e.UINameUnTranslated ) ;

	if( permanentEntry ){

		m_actions.emplace_back( ac,std::move( e ) ) ;
	}

	return ac ;
}

QMenu * translator::addMenu( QMenu * m,translator::entry e,bool permanentEntry )
{
	auto menu = m->addMenu( e.UINameTranslated ) ;

	if( permanentEntry ){

		m_menus.emplace_back( m,std::move( e ) ) ;
	}

	return menu ;
}

void translator::clear()
{
	if( m_translator ){

		QCoreApplication::removeTranslator( m_translator ) ;
		delete m_translator ;
		m_translator = nullptr ;
	}
}

translator::entry::entry( const QString& a,const char * b,const QString& c ) :
	UINameTranslated( a ),UINameUnTranslated( b ),internalName( c )
{
}
