/* ============================================================================
* Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
* BlueQuartz Software nor the names of its contributors may be used to endorse
* or promote products derived from this software without specific prior written
* permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*  This code was written under United States Air Force Contract number
*                           FA8650-07-D-5800
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "ReadMicData.h"

#include <limits>
#include <sstream>
#include <vector>

#include <QtCore/QFileInfo>

#include "EbsdLib/EbsdLib.h"
#include "HEDMAnalysisFilters/HEDM/MicFields.h"
#include "HEDMAnalysisFilters/HEDM/MicReader.h"

#include "SIMPLib/FilterParameters/AbstractFilterParametersReader.h"
#include "SIMPLib/FilterParameters/InputFileFilterParameter.h"
#include "SIMPLib/FilterParameters/SeparatorFilterParameter.h"
#include "SIMPLib/FilterParameters/StringFilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"

#include "HEDMAnalysis/HEDMAnalysisConstants.h"


/* ############## Start Private Implementation ############################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class ReadMicDataPrivate
{
  Q_DISABLE_COPY(ReadMicDataPrivate)
  Q_DECLARE_PUBLIC(ReadMicData)
  ReadMicData* const q_ptr;
  ReadMicDataPrivate(ReadMicData* ptr);

  Mic_Private_Data m_Data;

  QString m_InputFile_Cache;
  QDateTime m_TimeStamp_Cache;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadMicDataPrivate::ReadMicDataPrivate(ReadMicData* ptr)
: q_ptr(ptr)
, m_InputFile_Cache("")
, m_TimeStamp_Cache(QDateTime())
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadMicData::ReadMicData()
: m_DataContainerName(SIMPL::Defaults::ImageDataContainerName)
, m_CellEnsembleAttributeMatrixName(SIMPL::Defaults::CellEnsembleAttributeMatrixName)
, m_CellAttributeMatrixName(SIMPL::Defaults::CellAttributeMatrixName)
, m_FileWasRead(false)
, m_PhaseNameArrayName("")
, m_MaterialNameArrayName(SIMPL::EnsembleData::PhaseName)
, m_InputFile("")
, m_CellEulerAnglesArrayName(SIMPL::CellData::EulerAngles)
, m_CellPhasesArrayName(SIMPL::CellData::Phases)
, m_CrystalStructuresArrayName(SIMPL::EnsembleData::CrystalStructures)
, m_LatticeConstantsArrayName(SIMPL::EnsembleData::LatticeConstants)
, m_RefFrameZDir(SIMPL::RefFrameZDir::UnknownRefFrameZDirection)
, m_Manufacturer(Ebsd::OEM::Unknown)
, d_ptr(new ReadMicDataPrivate(this))
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ReadMicData::~ReadMicData() = default;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
SIMPL_PIMPL_PROPERTY_DEF(ReadMicData, Mic_Private_Data, Data)
SIMPL_PIMPL_PROPERTY_DEF(ReadMicData, QString, InputFile_Cache)
SIMPL_PIMPL_PROPERTY_DEF(ReadMicData, QDateTime, TimeStamp_Cache)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadMicData::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(SIMPL_NEW_INPUT_FILE_FP("Input File", InputFile, FilterParameter::Parameter, ReadMicData, "*.mic"));
  parameters.push_back(SIMPL_NEW_STRING_FP("Data Container", DataContainerName, FilterParameter::CreatedArray, ReadMicData));
  parameters.push_back(SeparatorFilterParameter::New("Cell Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Attribute Matrix", CellAttributeMatrixName, FilterParameter::CreatedArray, ReadMicData));
  parameters.push_back(SeparatorFilterParameter::New("Cell Ensemble Data", FilterParameter::CreatedArray));
  parameters.push_back(SIMPL_NEW_STRING_FP("Cell Ensemble Attribute Matrix", CellEnsembleAttributeMatrixName, FilterParameter::CreatedArray, ReadMicData));
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadMicData::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setDataContainerName(reader->readString("DataContainerName", getDataContainerName()));
  setCellAttributeMatrixName(reader->readString("CellAttributeMatrixName", getCellAttributeMatrixName()));
  setCellEnsembleAttributeMatrixName(reader->readString("CellEnsembleAttributeMatrixName", getCellEnsembleAttributeMatrixName()));
  setInputFile(reader->readString("InputFile", getInputFile()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadMicData::flushCache()
{
  setInputFile_Cache("");
  setTimeStamp_Cache(QDateTime());
  setData(Mic_Private_Data());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadMicData::populateMicData(MicReader* reader, DataContainer::Pointer m, QVector<size_t> dims, MIC_READ_FLAG flag)
{
  QFileInfo fi(m_InputFile);
  QDateTime timeStamp(fi.lastModified());
  if(flag == MIC_FULL_FILE)
  {
    setInputFile_Cache(""); // We need something to trigger the file read below
  }
  // Drop into this if statement if we need to read from a file
  if(m_InputFile != getInputFile_Cache() || !getTimeStamp_Cache().isValid() || getTimeStamp_Cache() < timeStamp)
  {
    int zDim = 1;
    float zStep = 1.0, xOrigin = 0.0f, yOrigin = 0.0f, zOrigin = 0.0f;

    reader->setFileName(m_InputFile);

    if(flag == MIC_HEADER_ONLY)
    {
      int err = reader->readHeaderOnly();
      if(err < 0)
      {
        setErrorCondition(err);
        notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), err);
        notifyErrorMessage(getHumanLabel(), "MicReader could not read the .mic file header.", getErrorCondition());
        m_FileWasRead = false;
        return;
      }

        m_FileWasRead = true;
    }
    else
    {
      int err = reader->readFile();
      if(err < 0)
      {
        setErrorCondition(err);
        notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), err);
        notifyErrorMessage(getHumanLabel(), "MicReader could not read the .mic file.", getErrorCondition());
        return;
      }
    }

    dims[0] = reader->getXDimension();
    dims[1] = reader->getYDimension();
    dims[2] = zDim; // We are reading a single slice

    // Set cache with values from file
    {
      Mic_Private_Data data;
      data.dims = dims;
      data.resolution.push_back(reader->getXStep());
      data.resolution.push_back(reader->getYStep());
      data.resolution.push_back(zStep);
      data.origin.push_back(xOrigin);
      data.origin.push_back(yOrigin);
      data.origin.push_back(zOrigin);
      data.phases = reader->getPhaseVector();
      setData(data);

      setInputFile_Cache(m_InputFile);

      QFileInfo newFi(m_InputFile);
      QDateTime timeStamp(newFi.lastModified());
      setTimeStamp_Cache(timeStamp);
    }
  }
  else
  {
    m_FileWasRead = false;
  }

  // Read from cache
  {
    dims[0] = getData().dims[0];
    dims[1] = getData().dims[1];
    dims[2] = getData().dims[2];
    m->getGeometryAs<ImageGeom>()->setDimensions(dims[0], dims[1], dims[2]);
    m->getGeometryAs<ImageGeom>()->setResolution(getData().resolution[0], getData().resolution[1], getData().resolution[2]);
    m->getGeometryAs<ImageGeom>()->setOrigin(getData().origin[0], getData().origin[1], getData().origin[2]);
  }

  if(flag == MIC_FULL_FILE)
  {
    loadMaterialInfo(reader);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadMicData::initialize()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadMicData::dataCheck()
{
  // Reset FileWasRead flag
  m_FileWasRead = false;

  DataArrayPath tempPath;
  setErrorCondition(0);
  setWarningCondition(0);

  DataContainer::Pointer m = getDataContainerArray()->createNonPrereqDataContainer<AbstractFilter>(this, getDataContainerName());
  if(getErrorCondition() < 0)
  {
    return;
  }

  // Create the Image Geometry
  ImageGeom::Pointer image = ImageGeom::CreateGeometry(SIMPL::Geometry::ImageGeometry);
  m->setGeometry(image);

  QVector<size_t> tDims(3, 0);
  AttributeMatrix::Pointer cellAttrMat = m->createNonPrereqAttributeMatrix(this, getCellAttributeMatrixName(), tDims, AttributeMatrix::Type::Cell);
  if(getErrorCondition() < 0)
  {
    return;
  }
  tDims.resize(1);
  tDims[0] = 0;
  AttributeMatrix::Pointer cellEnsembleAttrMat = m->createNonPrereqAttributeMatrix(this, getCellEnsembleAttributeMatrixName(), tDims, AttributeMatrix::Type::CellEnsemble);
  if(getErrorCondition() < 0)
  {
    return;
  }

  QFileInfo fi(m_InputFile);
  if(!fi.exists())
  {
    QString ss = QObject::tr("The input file does not exist: '%1'").arg(getInputFile());
    setErrorCondition(-388);
    notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }

  if(m_InputFile.isEmpty() && m_Manufacturer == Ebsd::OEM::Unknown)
  {
    QString ss = QObject::tr("The input file must be set for property %1").arg("InputFile");
    setErrorCondition(-1);
    notifyErrorMessage(getHumanLabel(), ss, -1);
  }

  if(!m_InputFile.isEmpty()) // User set a filename, so lets check it
  {
    QVector<size_t> dims(3, 0);

    QString ext = fi.suffix();
    QVector<QString> names;
    if(ext.compare(Mic::FileExt) == 0)
    {
      MicReader* reader = new MicReader();

      populateMicData(reader, m, dims, MIC_HEADER_ONLY);

      // Update the size of the Cell Attribute Matrix now that the dimensions of the volume are known
      cellAttrMat->resizeAttributeArrays(dims);
      MicFields micfeatures;
      names = micfeatures.getFilterFeatures<QVector<QString>>();
      QVector<size_t> dims(1, 1);
      for(qint32 i = 0; i < names.size(); ++i)
      {
        if(reader->getPointerType(names[i]) == Ebsd::Int32)
        {
          cellAttrMat->createAndAddAttributeArray<DataArray<int32_t>, AbstractFilter, int32_t>(this, names[i], 0, dims);
        }
        else if(reader->getPointerType(names[i]) == Ebsd::Float)
        {
          cellAttrMat->createAndAddAttributeArray<DataArray<float>, AbstractFilter, float>(this, names[i], 0, dims);
        }
      }

      delete reader;
    }
    else
    {
      setErrorCondition(-997);
      QString ss = QObject::tr("The File extension '%1' was not recognized. The reader only recognizes the .mic file extension").arg(ext);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
    }

    QVector<size_t> dim(1, 3);
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getCellEulerAnglesArrayName());
    m_CellEulerAnglesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0, dim);                     /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CellEulerAnglesPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellEulerAngles = m_CellEulerAnglesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    dim[0] = 1;
    tempPath.update(getDataContainerName(), getCellAttributeMatrixName(), getCellPhasesArrayName());
    m_CellPhasesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<int32_t>, AbstractFilter, int32_t>(
        this, tempPath, 0, dim);                /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CellPhasesPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CellPhases = m_CellPhasesPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    // typedef DataArray<unsigned int> XTalStructArrayType;
    tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getCrystalStructuresArrayName());
    m_CrystalStructuresPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<uint32_t>, AbstractFilter, uint32_t>(
        this, tempPath, Ebsd::CrystalStructure::UnknownCrystalStructure, dim); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_CrystalStructuresPtr.lock())                               /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */
    dim[0] = 6;
    tempPath.update(getDataContainerName(), getCellEnsembleAttributeMatrixName(), getLatticeConstantsArrayName());
    m_LatticeConstantsPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(
        this, tempPath, 0.0, dim);                    /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
    if(nullptr != m_LatticeConstantsPtr.lock())       /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
    {
      m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
    } /* Now assign the raw pointer to data from the DataArray<T> object */

    StringDataArray::Pointer materialNames = StringDataArray::CreateArray(cellEnsembleAttrMat->getNumberOfTuples(), SIMPL::EnsembleData::PhaseName);
    cellEnsembleAttrMat->addAttributeArray(SIMPL::EnsembleData::PhaseName, materialNames);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadMicData::preflight()
{
  setInPreflight(true);
  Q_EMIT preflightAboutToExecute();
  Q_EMIT updateFilterParameters(this);
  dataCheck();
  Q_EMIT preflightExecuted();
  setInPreflight(false);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadMicData::execute()
{
  int err = 0;
  QString ss;
  setErrorCondition(err);

  dataCheck();
  if(getErrorCondition() < 0)
  {
    return;
  }

  QFileInfo fi(getInputFile());
  QString ext = fi.suffix();

  readMicFile();

  // Set the file name and time stamp into the cache, if we are reading from the file and after all the reading has been done
  {
    QFileInfo newFi(m_InputFile);
    QDateTime timeStamp(newFi.lastModified());

    if(m_InputFile == getInputFile_Cache() && getTimeStamp_Cache().isValid() && getTimeStamp_Cache() >= timeStamp)
    {
      setTimeStamp_Cache(timeStamp);
      setInputFile_Cache(m_InputFile);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReadMicData::readMicFile()
{
  int err = 0;
  std::shared_ptr<MicReader> reader(new MicReader());
  reader->setFileName(m_InputFile);
  err = reader->readFile();
  if(err < 0)
  {
    setErrorCondition(err);
    notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), getErrorCondition());
    return;
  }
  DataContainer::Pointer m = getDataContainerArray()->getDataContainer(getDataContainerName());
  AttributeMatrix::Pointer cellAttrMat = m->getAttributeMatrix(getCellAttributeMatrixName());

  int64_t dims[3];
  dims[0] = reader->getXDimension();
  dims[1] = reader->getYDimension();
  dims[2] = 1; // We are reading a single slice
  m->getGeometryAs<ImageGeom>()->setDimensions(dims[0], dims[1], dims[2]);
  m->getGeometryAs<ImageGeom>()->setResolution(reader->getXStep(), reader->getYStep(), 1.0);
  m->getGeometryAs<ImageGeom>()->setOrigin(0.0f, 0.0f, 0.0f);

  err = loadMaterialInfo(reader.get());

  float* f1 = nullptr;
  float* f2 = nullptr;
  float* f3 = nullptr;
  int* phasePtr = nullptr;

  FloatArrayType::Pointer fArray = FloatArrayType::NullPointer();
  Int32ArrayType::Pointer iArray = Int32ArrayType::NullPointer();
  size_t totalPoints = m->getGeometryAs<ImageGeom>()->getNumberOfElements();
  // Prepare the Cell Attribute Matrix with the correct number of tuples based on the total points being read from the file.
  QVector<size_t> tDims(3, 0);
  tDims[0] = m->getGeometryAs<ImageGeom>()->getXPoints();
  tDims[1] = m->getGeometryAs<ImageGeom>()->getYPoints();
  tDims[2] = m->getGeometryAs<ImageGeom>()->getZPoints();
  cellAttrMat->resizeAttributeArrays(tDims);

  float x, y;
  float xMin = 10000000;
  float yMin = 10000000;
  f1 = reinterpret_cast<float*>(reader->getPointerByName(Mic::X));
  f2 = reinterpret_cast<float*>(reader->getPointerByName(Mic::Y));
  for(size_t i = 0; i < totalPoints; i++)
  {
    x = f1[i];
    y = f2[i];
    if(x < xMin)
    {
      xMin = x;
    }
    if(y < yMin)
    {
      yMin = y;
    }
  }
  m->getGeometryAs<ImageGeom>()->setOrigin(xMin, yMin, 0.0);

  {
    phasePtr = reinterpret_cast<int*>(reader->getPointerByName(Mic::Phase));
    for(size_t i = 0; i < totalPoints; i++)
    {
      if(phasePtr[i] < 1)
      {
        phasePtr[i] = 1;
      }
    }
    iArray = Int32ArrayType::CreateArray(totalPoints, SIMPL::CellData::Phases);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMat->addAttributeArray(SIMPL::CellData::Phases, iArray);
  }

  QVector<size_t> compDims(1, 3); // Initially set this up for the Euler Angle 1x3
  {
    //  radianconversion = M_PI / 180.0;
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Mic::Euler1));
    f2 = reinterpret_cast<float*>(reader->getPointerByName(Mic::Euler2));
    f3 = reinterpret_cast<float*>(reader->getPointerByName(Mic::Euler3));
    fArray = FloatArrayType::CreateArray(totalPoints, compDims, SIMPL::CellData::EulerAngles);
    float* cellEulerAngles = fArray->getPointer(0);
    for(size_t i = 0; i < totalPoints; i++)
    {
      cellEulerAngles[3 * i] = f1[i];
      cellEulerAngles[3 * i + 1] = f2[i];
      cellEulerAngles[3 * i + 2] = f3[i];
    }
    cellAttrMat->addAttributeArray(SIMPL::CellData::EulerAngles, fArray);
  }

  compDims[0] = 1; // Now reset the size of the first dimension to 1
  {
    phasePtr = reinterpret_cast<int*>(reader->getPointerByName(Mic::Phase));
    iArray = Int32ArrayType::CreateArray(totalPoints, compDims, SIMPL::CellData::Phases);
    ::memcpy(iArray->getPointer(0), phasePtr, sizeof(int32_t) * totalPoints);
    cellAttrMat->addAttributeArray(SIMPL::CellData::Phases, iArray);
  }

  {
    f1 = reinterpret_cast<float*>(reader->getPointerByName(Mic::Confidence));
    fArray = FloatArrayType::CreateArray(totalPoints, compDims, Mic::Confidence);
    ::memcpy(fArray->getPointer(0), f1, sizeof(float) * totalPoints);
    cellAttrMat->addAttributeArray(Mic::Confidence, fArray);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer ReadMicData::newFilterInstance(bool copyFilterParameters) const
{
  ReadMicData::Pointer filter = ReadMicData::New();
  if(copyFilterParameters)
  {
    filter->setFilterParameters(getFilterParameters());
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadMicData::getCompiledLibraryName() const
{
  return HEDMAnalysisConstants::HEDMAnalysisBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadMicData::getGroupName() const
{
  return SIMPL::FilterGroups::Unsupported;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QUuid ReadMicData::getUuid()
{
  return QUuid("{5a2b714e-bae9-5213-8171-d1e190609e2d}");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadMicData::getSubGroupName() const
{
  return SIMPL::FilterSubGroups::InputFilters;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString ReadMicData::getHumanLabel() const
{
  return "Import HEDM Data (.mic)";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int ReadMicData::loadMaterialInfo(MicReader* reader)
{
  QVector<MicPhase::Pointer> phases = getData().phases;
  if(phases.empty())
  {
    setErrorCondition(reader->getErrorCode());
    notifyErrorMessage(getHumanLabel(), reader->getErrorMessage(), getErrorCondition());
    return getErrorCondition();
  }

  DataArray<unsigned int>::Pointer crystalStructures = DataArray<unsigned int>::CreateArray(phases.size() + 1, getCrystalStructuresArrayName());
  StringDataArray::Pointer materialNames = StringDataArray::CreateArray(phases.size() + 1, getMaterialNameArrayName());
  QVector<size_t> dims(1, 6);
  FloatArrayType::Pointer latticeConstants = FloatArrayType::CreateArray(phases.size() + 1, dims, getLatticeConstantsArrayName());

  // Initialize the zero'th element to unknowns. The other elements will
  // be filled in based on values from the data file
  crystalStructures->setValue(0, Ebsd::CrystalStructure::UnknownCrystalStructure);
  materialNames->setValue(0, "Invalid Phase");
  latticeConstants->setComponent(0, 0, 0.0f);
  latticeConstants->setComponent(0, 1, 0.0f);
  latticeConstants->setComponent(0, 2, 0.0f);
  latticeConstants->setComponent(0, 3, 0.0f);
  latticeConstants->setComponent(0, 4, 0.0f);
  latticeConstants->setComponent(0, 5, 0.0f);

  for(size_t i = 0; i < phases.size(); i++)
  {
    int phaseID = phases[i]->getPhaseIndex();
    crystalStructures->setValue(phaseID, phases[i]->determineLaueGroup());
    materialNames->setValue(phaseID, phases[i]->getMaterialName());
    QVector<float> lc = phases[i]->getLatticeConstants();

    latticeConstants->setComponent(phaseID, 0, lc[0]);
    latticeConstants->setComponent(phaseID, 1, lc[1]);
    latticeConstants->setComponent(phaseID, 2, lc[2]);
    latticeConstants->setComponent(phaseID, 3, lc[3]);
    latticeConstants->setComponent(phaseID, 4, lc[4]);
    latticeConstants->setComponent(phaseID, 5, lc[5]);
  }
  DataContainer::Pointer vdc = getDataContainerArray()->getDataContainer(getDataContainerName());
  if(nullptr == vdc)
  {
    return -1;
  }
  AttributeMatrix::Pointer attrMatrix = vdc->getAttributeMatrix(getCellEnsembleAttributeMatrixName());
  if(nullptr == attrMatrix.get())
  {
    return -2;
  }

  // Resize the AttributeMatrix based on the size of the crystal structures array
  QVector<size_t> tDims(1, crystalStructures->getNumberOfTuples());
  attrMatrix->resizeAttributeArrays(tDims);
  // Now add the attributeArray to the AttributeMatrix
  attrMatrix->addAttributeArray(SIMPL::EnsembleData::CrystalStructures, crystalStructures);
  attrMatrix->addAttributeArray(SIMPL::EnsembleData::PhaseName, materialNames);
  attrMatrix->addAttributeArray(SIMPL::EnsembleData::LatticeConstants, latticeConstants);

  // Now reset the internal ensemble array references to these new arrays
  m_CrystalStructuresPtr = crystalStructures;
  if(nullptr != m_CrystalStructuresPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_CrystalStructures = m_CrystalStructuresPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */

  m_LatticeConstantsPtr = latticeConstants;
  if(nullptr != m_LatticeConstantsPtr.lock()) /* Validate the Weak Pointer wraps a non-nullptr pointer to a DataArray<T> object */
  {
    m_LatticeConstants = m_LatticeConstantsPtr.lock()->getPointer(0);
  } /* Now assign the raw pointer to data from the DataArray<T> object */
  return 0;
}
