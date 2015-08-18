/*
 * Stellarium
 * Copyright (C) 2015 Guillaume Chereau
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

#include "StelGui.hpp"
#include "StelActionMgr.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelFileMgr.hpp"
#include "StelLocaleMgr.hpp"
#include "StelLocation.hpp"
#include "StelModuleMgr.hpp"
#include "StelObject.hpp"
#include "StelObjectMgr.hpp"
#include "StelUtils.hpp"

#include <QSettings>

StelGui::StelGui():
	  autoHideHorizontalButtonBar(true)
	, autoHideVerticalButtonBar(true)
	, helpDialogVisible(false)
	, configurationDialogVisible(false)
	, searchDialogVisible(false)
	, viewDialogVisible(false)
	, dateTimeDialogVisible(false)
	, locationDialogVisible(false)
	, shortcutsDialogVisible(false)
{
	QSettings* conf = StelApp::getInstance().getSettings();
	QString objectInfo = conf->value("gui/selected_object_info", "all").toString();

	if (objectInfo == "all")
	{
		infoTextFilters = StelObject::InfoStringGroup(StelObject::AllInfo);
	}
	else if (objectInfo == "short")
	{
		infoTextFilters = StelObject::InfoStringGroup(StelObject::ShortInfo);
	}
	else if (objectInfo == "none")
	{
		infoTextFilters = StelObject::InfoStringGroup(0);
	}
	else if (objectInfo == "custom")
	{
		infoTextFilters = StelObject::InfoStringGroup(0);
		
		conf->beginGroup("custom_selected_info");
		if (conf->value("flag_show_name", false).toBool())
			infoTextFilters |= StelObject::Name;
		if (conf->value("flag_show_catalognumber", false).toBool())
			infoTextFilters |= StelObject::CatalogNumber;
		if (conf->value("flag_show_magnitude", false).toBool())
			infoTextFilters |= StelObject::Magnitude;
		if (conf->value("flag_show_absolutemagnitude", false).toBool())
			infoTextFilters |= StelObject::AbsoluteMagnitude;
		if (conf->value("flag_show_radecj2000", false).toBool())
			infoTextFilters |= StelObject::RaDecJ2000;
		if (conf->value("flag_show_radecofdate", false).toBool())
			infoTextFilters |= StelObject::RaDecOfDate;
		if (conf->value("flag_show_hourangle", false).toBool())
			infoTextFilters |= StelObject::HourAngle;
		if (conf->value("flag_show_altaz", false).toBool())
			infoTextFilters |= StelObject::AltAzi;
		if (conf->value("flag_show_distance", false).toBool())
			infoTextFilters |= StelObject::Distance;
		if (conf->value("flag_show_size", false).toBool())
			infoTextFilters |= StelObject::Size;
		if (conf->value("flag_show_extra", false).toBool())
			infoTextFilters |= StelObject::Extra;
		if (conf->value("flag_show_type", false).toBool())
			infoTextFilters |= StelObject::ObjectType;
		if (conf->value("flag_show_galcoord", false).toBool())
			infoTextFilters |= StelObject::GalacticCoord;
		if (conf->value("flag_show_eclcoord", false).toBool())
			infoTextFilters |= StelObject::EclipticCoord;
		conf->endGroup();
	}
	else
	{
		qWarning() << "config.ini option gui/selected_object_info is invalid, using \"all\"";
		infoTextFilters = StelObject::InfoStringGroup(StelObject::AllInfo);
	}

	QString miscGroup = N_("Miscellaneous");
	StelActionMgr *actionMgr = StelApp::getInstance().getStelActionManager();
	actionMgr->addAction("actionAutoHideHorizontalButtonBar", miscGroup, N_("Auto hide horizontal button bar"), this, "autoHideHorizontalButtonBar");
	actionMgr->addAction("actionAutoHideVerticalButtonBar", miscGroup, N_("Auto hide vertical button bar"), this, "autoHideVerticalButtonBar");

	// Register all default buttons.
	// Maybe this should be done in the different modules directly.
	addButton("qrc:///graphicGui/btConstellationLines-on.png",
			  "qrc:///graphicGui/btConstellationLines-off.png",
			  "actionShow_Constellation_Lines",
			  "010-constellationsGroup");

	addButton("qrc:///graphicGui/btConstellationLabels-on.png",
			  "qrc:///graphicGui/btConstellationLabels-off.png",
			  "actionShow_Constellation_Labels",
			  "010-constellationsGroup");

	addButton("qrc:///graphicGui/btConstellationArt-on.png",
			  "qrc:///graphicGui/btConstellationArt-off.png",
			  "actionShow_Constellation_Art",
			  "010-constellationsGroup");

	addButton("qrc:///graphicGui/btEquatorialGrid-on.png",
			  "qrc:///graphicGui/btEquatorialGrid-off.png",
			  "actionShow_Equatorial_Grid",
			  "020-gridsGroup");

	addButton("qrc:///graphicGui/btAzimuthalGrid-on.png",
			  "qrc:///graphicGui/btAzimuthalGrid-off.png",
			  "actionShow_Azimuthal_Grid",
			  "020-gridsGroup");

	addButton("qrc:///graphicGui/btGround-on.png",
			  "qrc:///graphicGui/btGround-off.png",
			  "actionShow_Ground",
			  "030-landscapeGroup");

	addButton("qrc:///graphicGui/btCardinalPoints-on.png",
			  "qrc:///graphicGui/btCardinalPoints-off.png",
			  "actionShow_Cardinal_Points",
			  "030-landscapeGroup");

	addButton("qrc:///graphicGui/btAtmosphere-on.png",
			  "qrc:///graphicGui/btAtmosphere-off.png",
			  "actionShow_Atmosphere",
			  "030-landscapeGroup");

	addButton("qrc:///graphicGui/btNebula-on.png",
			  "qrc:///graphicGui/btNebula-off.png",
			  "actionShow_Nebulas",
			  "030-nebulaeGroup");

	addButton("qrc:///graphicGui/btPlanets-on.png",
			  "qrc:///graphicGui/btPlanets-off.png",
			  "actionShow_Planets_Labels",
			  "030-nebulaeGroup");

	addButton("qrc:///graphicGui/btGotoSelectedObject-on.png",
			  "qrc:///graphicGui/btGotoSelectedObject-off.png",
			  "actionShow_Planets_Labels",
			  "060-othersGroup");

	addButton("qrc:///graphicGui/btNightView-on.png",
			  "qrc:///graphicGui/btNightView-off.png",
			  "actionShow_Night_Mode",
			  "060-othersGroup");

	addButton("qrc:///graphicGui/btFullScreen-on.png",
			  "qrc:///graphicGui/btFullScreen-off.png",
			  "actionSet_Full_Screen_Global",
			  "060-othersGroup");

	addButton("qrc:///graphicGui/btTimeRewind-on.png",
			  "qrc:///graphicGui/btTimeRewind-off.png",
			  "actionDecrease_Time_Speed",
			  "070-timeGroup");

	addButton("qrc:///graphicGui/btTimeRealtime-on.png",
			  "qrc:///graphicGui/btTimeRealtime-off.png",
			  "actionSet_Real_Time_Speed",
			  "070-timeGroup");

	addButton("qrc:///graphicGui/btTimeNow-on.png",
			  "qrc:///graphicGui/btTimeNow-off.png",
			  "actionReturn_To_Current_Time",
			  "070-timeGroup");

	addButton("qrc:///graphicGui/btTimeForward-on.png",
			  "qrc:///graphicGui/btTimeForward-off.png",
			  "actionIncrease_Time_Speed",
			  "070-timeGroup");

	// Add all the windows bar actions and buttons.
	// actionMgr->addAction("actionAutoHideHorizontalButtonBar", miscGroup, N_("Auto hide horizontal button bar"), this, "autoHideHorizontalButtonBar");
	// actionMgr->addAction("actionAutoHideVerticalButtonBar", miscGroup, N_("Auto hide vertical button bar"), this, "autoHideVerticalButtonBar");

	QString windowsGroup = N_("Windows");
	actionMgr->addAction("actionShow_Help_Window_Global", windowsGroup, N_("Help window"), this, "helpDialogVisible", "F1", "", true);
	actionMgr->addAction("actionShow_Configuration_Window_Global", windowsGroup, N_("Configuration window"), this, "configurationDialogVisible", "F2", "", true);
	actionMgr->addAction("actionShow_Search_Window_Global", windowsGroup, N_("Search window"), this, "searchDialogVisible", "F3", "Ctrl+F", true);
	actionMgr->addAction("actionShow_SkyView_Window_Global", windowsGroup, N_("Sky and viewing options window"), this, "viewDialogVisible", "F4", "", true);
	actionMgr->addAction("actionShow_DateTime_Window_Global", windowsGroup, N_("Date/time window"), this, "dateTimeDialogVisible", "F5", "", true);
	actionMgr->addAction("actionShow_Location_Window_Global", windowsGroup, N_("Location window"), this, "locationDialogVisible", "F6", "", true);
	actionMgr->addAction("actionShow_Shortcuts_Window_Global", windowsGroup, N_("Shortcuts window"), this, "shortcutsDialogVisible", "F7", "", true);

	addButton("qrc:///graphicGui/1-on-time.png",
			  "qrc:///graphicGui/1-off-time.png",
			  "actionShow_DateTime_Window_Global",
			  "win-bar");

	addButton("qrc:///graphicGui/2-on-location.png",
			  "qrc:///graphicGui/2-off-location.png",
			  "actionShow_Location_Window_Global",
			  "win-bar");

	addButton("qrc:///graphicGui/5-on-labels.png",
			  "qrc:///graphicGui/5-off-labels.png",
			  "actionShow_SkyView_Window_Global",
			  "win-bar");

	addButton("qrc:///graphicGui/8-on-settings.png",
			  "qrc:///graphicGui/8-off-settings.png",
			  "actionShow_Configuration_Window_Global",
			  "win-bar");

	addButton("qrc:///graphicGui/6-on-search.png",
			  "qrc:///graphicGui/6-off-search.png",
			  "actionShow_Search_Window_Global",
			  "win-bar");
}

void StelGui::addButton(QString pixOn, QString pixOff,
                        QString action, QString groupName,
                        QString beforeActionName)
{
	QVariantMap button;
	button["pixOn"]     = pixOn;
	button["pixOff"]    = pixOff;
	button["action"]    = action;
	button["groupName"] = groupName;
	button["beforeActionName"] = beforeActionName;
	buttons << button;
	emit changed();
}

QString StelGui::getLocationName() const
{
	StelCore* core = StelApp::getInstance().getCore();
	const StelLocation* loc = &core->getCurrentLocation();
	const StelLocaleMgr& locmgr = StelApp::getInstance().getLocaleMgr();
	const StelTranslator& trans = locmgr.getSkyTranslator();
	if (!loc->name.isEmpty())
	{
		return trans.qtranslate(loc->planetName) +", "+loc->name + ", "+q_("%1m").arg(loc->altitude);
	}
	else
	{
		return trans.qtranslate(loc->planetName)+", "+StelUtils::decDegToDmsStr(loc->latitude)+", "+StelUtils::decDegToDmsStr(loc->longitude);
	}
}

QStringList StelGui::getLanguages() const
{
	QStringList ret = StelTranslator::globalTranslator->getAvailableLanguagesNamesNative(StelFileMgr::getLocaleDir());
	ret.sort();
	return ret;
}

QString StelGui::getLanguage() const
{
	QString appLang = StelApp::getInstance().getLocaleMgr().getAppLanguage();
	return StelTranslator::iso639_1CodeToNativeName(appLang);
}

void StelGui::setLanguage(QString v)
{
	QString code = StelTranslator::nativeNameToIso639_1Code(v);
	StelApp::getInstance().getLocaleMgr().setAppLanguage(code);
	emit changed();
}

QString StelGui::getSkyLanguage() const
{
	QString appLang = StelApp::getInstance().getLocaleMgr().getSkyLanguage();
	return StelTranslator::iso639_1CodeToNativeName(appLang);
}

void StelGui::setSkyLanguage(QString v)
{
	QString code = StelTranslator::nativeNameToIso639_1Code(v);
	StelApp::getInstance().getLocaleMgr().setSkyLanguage(code);
	emit changed();
}

void StelGui::update()
{
	QList<StelObjectP> selected = GETSTELMODULE(StelObjectMgr)->getSelectedObject();
	if (selected.isEmpty())
	{
		selectionInfoText.clear();
	}
	else
	{
		StelObject::InfoStringGroup flags = (StelObject::InfoStringGroup)infoTextFilters;
		selectionInfoText = selected[0]->getInfoString(StelApp::getInstance().getCore(), flags);
	}
	emit updated();
}
