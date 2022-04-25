/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#pragma once

#include <vector>
#include <map>
#include <QtCore/QString>

#include "HEDMAnalysis/HEDMAnalysisConstants.h"
#include "OrientationLib/LaueOps/OrthoRhombicOps.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SIMPLib/DataArrays/StatsDataArray.h"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/FilterParameters/FileListInfoFilterParameter.h"
#include "SIMPLib/Filtering/AbstractFilter.h"
#include "SIMPLib/Geometry/ShapeOps/ShapeOps.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/StatsData/StatsData.h"

#include "HEDMAnalysis/HEDMAnalysisDLLExport.h"

/**
 * @class TesselateFarFieldGrains TesselateFarFieldGrains.h DREAM3DLib/SyntheticBuilderFilters/TesselateFarFieldGrains.h
 * @brief
 * @author
 * @date Nov 19, 2011
 * @version 1.0
 */
class HEDMAnalysis_EXPORT TesselateFarFieldGrains : public AbstractFilter
{
    Q_OBJECT
    PYB11_CREATE_BINDINGS(TesselateFarFieldGrains SUPERCLASS AbstractFilter)
    PYB11_PROPERTY(DataArrayPath OutputCellAttributeMatrixName READ getOutputCellAttributeMatrixName WRITE setOutputCellAttributeMatrixName)
    PYB11_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)
    PYB11_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)
    PYB11_PROPERTY(QString SlabIdArrayName READ getSlabIdArrayName WRITE setSlabIdArrayName)
    PYB11_PROPERTY(QString FeaturePhasesArrayName READ getFeaturePhasesArrayName WRITE setFeaturePhasesArrayName)
    PYB11_PROPERTY(QString FeatureEulerAnglesArrayName READ getFeatureEulerAnglesArrayName WRITE setFeatureEulerAnglesArrayName)
    PYB11_PROPERTY(QString ElasticStrainsArrayName READ getElasticStrainsArrayName WRITE setElasticStrainsArrayName)
    PYB11_PROPERTY(QString CentroidsArrayName READ getCentroidsArrayName WRITE setCentroidsArrayName)
    PYB11_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)
    PYB11_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)
    PYB11_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)
    PYB11_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)
    PYB11_PROPERTY(QString EquivalentDiametersArrayName READ getEquivalentDiametersArrayName WRITE setEquivalentDiametersArrayName)
    PYB11_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)
    PYB11_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)
    PYB11_PROPERTY(FileListInfo_t FeatureInputFileListInfo READ getFeatureInputFileListInfo WRITE setFeatureInputFileListInfo)
  public:
    SIMPL_SHARED_POINTERS(TesselateFarFieldGrains)
    SIMPL_FILTER_NEW_MACRO(TesselateFarFieldGrains)
    SIMPL_TYPE_MACRO_SUPER_OVERRIDE(TesselateFarFieldGrains, AbstractFilter)

    ~TesselateFarFieldGrains() override;
    SIMPL_FILTER_PARAMETER(DataArrayPath, OutputCellAttributeMatrixName)
    Q_PROPERTY(DataArrayPath OutputCellAttributeMatrixName READ getOutputCellAttributeMatrixName WRITE setOutputCellAttributeMatrixName)

    SIMPL_INSTANCE_STRING_PROPERTY(OutputCellFeatureAttributeMatrixName)
    Q_PROPERTY(QString OutputCellFeatureAttributeMatrixName READ getOutputCellFeatureAttributeMatrixName WRITE setOutputCellFeatureAttributeMatrixName)

    SIMPL_INSTANCE_STRING_PROPERTY(OutputCellEnsembleAttributeMatrixName)
    Q_PROPERTY(QString OutputCellEnsembleAttributeMatrixName READ getOutputCellEnsembleAttributeMatrixName WRITE setOutputCellEnsembleAttributeMatrixName)

    SIMPL_FILTER_PARAMETER(QString, FeatureIdsArrayName)
    Q_PROPERTY(QString FeatureIdsArrayName READ getFeatureIdsArrayName WRITE setFeatureIdsArrayName)

    SIMPL_FILTER_PARAMETER(QString, CellPhasesArrayName)
    Q_PROPERTY(QString CellPhasesArrayName READ getCellPhasesArrayName WRITE setCellPhasesArrayName)

    SIMPL_FILTER_PARAMETER(QString, SlabIdArrayName)

    SIMPL_FILTER_PARAMETER(QString, FeaturePhasesArrayName)
    Q_PROPERTY(QString FeaturePhasesArrayName READ getFeaturePhasesArrayName WRITE setFeaturePhasesArrayName)

    SIMPL_FILTER_PARAMETER(QString, FeatureEulerAnglesArrayName)
    Q_PROPERTY(QString FeatureEulerAnglesArrayName READ getFeatureEulerAnglesArrayName WRITE setFeatureEulerAnglesArrayName)

    SIMPL_FILTER_PARAMETER(QString, ElasticStrainsArrayName)
    Q_PROPERTY(QString ElasticStrainsArrayName READ getElasticStrainsArrayName WRITE setElasticStrainsArrayName)

    SIMPL_FILTER_PARAMETER(QString, CentroidsArrayName)
    Q_PROPERTY(QString CentroidsArrayName READ getCentroidsArrayName WRITE setCentroidsArrayName)

    SIMPL_FILTER_PARAMETER(QString, VolumesArrayName)
    Q_PROPERTY(QString VolumesArrayName READ getVolumesArrayName WRITE setVolumesArrayName)

    SIMPL_FILTER_PARAMETER(QString, AxisLengthsArrayName)
    Q_PROPERTY(QString AxisLengthsArrayName READ getAxisLengthsArrayName WRITE setAxisLengthsArrayName)

    SIMPL_FILTER_PARAMETER(QString, AxisEulerAnglesArrayName)
    Q_PROPERTY(QString AxisEulerAnglesArrayName READ getAxisEulerAnglesArrayName WRITE setAxisEulerAnglesArrayName)

    SIMPL_FILTER_PARAMETER(QString, Omega3sArrayName)
    Q_PROPERTY(QString Omega3sArrayName READ getOmega3sArrayName WRITE setOmega3sArrayName)

    SIMPL_FILTER_PARAMETER(QString, EquivalentDiametersArrayName)
    Q_PROPERTY(QString EquivalentDiametersArrayName READ getEquivalentDiametersArrayName WRITE setEquivalentDiametersArrayName)

    SIMPL_FILTER_PARAMETER(QString, CrystalStructuresArrayName)
    Q_PROPERTY(QString CrystalStructuresArrayName READ getCrystalStructuresArrayName WRITE setCrystalStructuresArrayName)

    typedef DataArray<float>::Pointer SharedFloatArray;
    typedef DataArray<int>::Pointer SharedIntArray;

    SIMPL_FILTER_PARAMETER(DataArrayPath, MaskArrayPath)
    Q_PROPERTY(DataArrayPath MaskArrayPath READ getMaskArrayPath WRITE setMaskArrayPath)

    SIMPL_FILTER_PARAMETER(FileListInfo_t, FeatureInputFileListInfo)
    Q_PROPERTY(FileListInfo_t FeatureInputFileListInfo READ getFeatureInputFileListInfo WRITE setFeatureInputFileListInfo)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    const QString getCompiledLibraryName() const override;

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    const QString getBrandingString() const override;

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    const QString getFilterVersion() const override;

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    const QString getGroupName() const override;

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    const QString getSubGroupName() const override;

    /**
     * @brief getUuid Return the unique identifier for this filter.
     * @return A QUuid object.
     */
    const QUuid getUuid() override;

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    const QString getHumanLabel() const override;

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    void setupFilterParameters() override;

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

    /**
     * @brief preflight
     */
    void preflight() override;

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    void execute() override;

  Q_SIGNALS:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    TesselateFarFieldGrains();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();

    void load_features();
    void merge_twins();
    void assign_voxels();
    void assign_gaps_only();
    void assign_orientations();

  private:
    int32_t* m_Neighbors;

    // Cell Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)
    DEFINE_DATAARRAY_VARIABLE(int32_t, CellPhases)
    DEFINE_DATAARRAY_VARIABLE(bool, Mask)
    int8_t*  m_BoundaryCells;

    // Feature Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeaturePhases)
    DEFINE_DATAARRAY_VARIABLE(int32_t, SlabId)
    DEFINE_DATAARRAY_VARIABLE(float, FeatureEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(float, ElasticStrains)
    DEFINE_DATAARRAY_VARIABLE(float, Centroids)
    DEFINE_DATAARRAY_VARIABLE(float, Volumes)
    DEFINE_DATAARRAY_VARIABLE(float, AxisLengths)
    DEFINE_DATAARRAY_VARIABLE(float, AxisEulerAngles)
    DEFINE_DATAARRAY_VARIABLE(float, Omega3s)
    DEFINE_DATAARRAY_VARIABLE(float, EquivalentDiameters)

    // Ensemble Data - make sure these are all initialized to nullptr in the constructor
    DEFINE_DATAARRAY_VARIABLE(unsigned int, CrystalStructures)

    // All other private variables
    ShapeOps::Pointer m_EllipsoidOps;

    OrthoRhombicOps::Pointer m_OrthoOps;

    unsigned long long int m_RandomSeed;

    float m_SizeX;
    float m_SizeY;
    float m_SizeZ;
    float m_TotalVol;

    std::vector<int> m_NewNames;
    std::vector<int> m_PackQualities;
    std::vector<int> m_GSizes;


    void updateFeatureInstancePointers();

    void updateEnsembleInstancePointers();

  public:
    TesselateFarFieldGrains(const TesselateFarFieldGrains&) = delete; // Copy Constructor Not Implemented
    TesselateFarFieldGrains(TesselateFarFieldGrains&&) = delete;      // Move Constructor Not Implemented
    TesselateFarFieldGrains& operator=(const TesselateFarFieldGrains&) = delete; // Copy Assignment Not Implemented
    TesselateFarFieldGrains& operator=(TesselateFarFieldGrains&&) = delete;      // Move Assignment Not Implemented
};




