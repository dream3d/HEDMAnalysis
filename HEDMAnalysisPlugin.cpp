/*
 * Your License or Copyright Information can go here
 */

#include "HEDMAnalysisPlugin.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>

#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"

#include "HEDMAnalysis/HEDMAnalysisConstants.h"
#include "HEDMAnalysis/HEDMAnalysisVersion.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HEDMAnalysisPlugin::HEDMAnalysisPlugin()
: m_Version(HEDMAnalysis::Version::Package())
, // Initialize HEDMAnalysis's Version Number Here
    m_CompatibilityVersion(HEDMAnalysis::Version::Package())
, // Initialize HEDMAnalysis's Compatibility Version Number Here
    m_Vendor("Open-Source")
, m_URL("http://www.github.com/dream3d/HEDMAnalysisPlugin")
, // Initialize Company URL Here
    m_Location("")
, // Initialize Company Location Here
    m_Description("")
, // Initialize HEDMAnalysis's Description Here
    m_Copyright("")
, // Initialize HEDMAnalysis's Copyright Here
    m_Filters(QList<QString>())
, // Initialize HEDMAnalysis's List of Dependencies Here
    m_DidLoad(false)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
HEDMAnalysisPlugin::~HEDMAnalysisPlugin() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getPluginFileName()
{
  return HEDMAnalysisConstants::HEDMAnalysisPluginFile;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getPluginDisplayName()
{
  return HEDMAnalysisConstants::HEDMAnalysisPluginDisplayName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getPluginBaseName()
{
  return HEDMAnalysisConstants::HEDMAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getVersion()
{
  return m_Version;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getCompatibilityVersion()
{
  return m_CompatibilityVersion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getVendor()
{
  return m_Vendor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getURL()
{
  return m_URL;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getLocation()
{
  return m_Location;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getDescription()
{
  /* PLEASE UPDATE YOUR PLUGIN'S DESCRIPTION FILE.
  It is located at HEDMAnalysis/Resources/HEDMAnalysis/HEDMAnalysisDescription.txt */

  QFile licenseFile(":/HEDMAnalysis/HEDMAnalysisDescription.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--Description was not read-->>";

  if(licenseFileInfo.exists())
  {
    if(licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getCopyright()
{
  return m_Copyright;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString HEDMAnalysisPlugin::getLicense()
{
  /* PLEASE UPDATE YOUR PLUGIN'S LICENSE FILE.
  It is located at HEDMAnalysis/Resources/HEDMAnalysis/HEDMAnalysisLicense.txt */

  QFile licenseFile(":/HEDMAnalysis/HEDMAnalysisLicense.txt");
  QFileInfo licenseFileInfo(licenseFile);
  QString text = "<<--License was not read-->>";

  if(licenseFileInfo.exists())
  {
    if(licenseFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QTextStream in(&licenseFile);
      text = in.readAll();
    }
  }
  return text;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, QString> HEDMAnalysisPlugin::getThirdPartyLicenses()
{
  QMap<QString, QString> licenseMap;
  QList<QString> fileStrList;
  fileStrList.push_back(":/ThirdParty/HDF5.txt");

  fileStrList.push_back(":/ThirdParty/Qt.txt");
  fileStrList.push_back(":/ThirdParty/Qwt.txt");

  for(QList<QString>::iterator iter = fileStrList.begin(); iter != fileStrList.end(); iter++)
  {
    QFile file(*iter);
    QFileInfo licenseFileInfo(file);

    if(licenseFileInfo.exists())
    {
      if(file.open(QIODevice::ReadOnly | QIODevice::Text))
      {
        QTextStream in(&file);
        licenseMap.insert(licenseFileInfo.baseName(), in.readAll());
      }
    }
  }

  return licenseMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool HEDMAnalysisPlugin::getDidLoad()
{
  return m_DidLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HEDMAnalysisPlugin::setDidLoad(bool didLoad)
{
  m_DidLoad = didLoad;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HEDMAnalysisPlugin::setLocation(QString filePath)
{
  m_Location = filePath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HEDMAnalysisPlugin::writeSettings(QSettings& prefs)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HEDMAnalysisPlugin::readSettings(QSettings& prefs)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void HEDMAnalysisPlugin::registerFilterWidgets(FilterWidgetManager* fwm)
{
}

#include "HEDMAnalysisFilters/RegisterKnownFilters.cpp"
