/**
 * @file    Simple.h
 * @author  Adam O'Brien <obrienadam89@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This file contains the interface for class Simple, which contains
 * schemes for using the SIMPLE method for solving incompressible flow
 * problems.
 */

#ifndef SIMPLE_H
#define SIMPLE_H

#include "FvScheme.h"
#include "Tensor3D.h"

#include "SparseMatrix.h"
#include "SparseVector.h"
#include "IndexMap.h"

enum FlowBoundary{INLET, OUTLET, WALL};

class Simple : public FvScheme
{
protected:

    Field<Vector3D>* uFieldPtr_;
    Field<double>* pFieldPtr_;
    Field<double>* rhoFieldPtr_;
    Field<double>* muFieldPtr_;
    Field<double>* massFlowFieldPtr_;

    Field<Vector3D> uField0_;
    Field<Vector3D> uFieldStar_;
    Field<double> a0P_, aP_, aE_, aW_, aN_, aS_, aT_, aB_;
    Field<double> dE_, dW_, dN_, dS_, dT_, dB_;
    Field<Vector3D> cE_, cW_, cN_, cS_, cT_, cB_;
    Field<Vector3D> bP_;
    Field<Vector3D> hField_;
    Field<double> dField_;

    Field<double> pCorr_;
    Field<Vector3D> gradPCorr_;

    Field<Tensor3D> gradUField_;
    Field<Vector3D> gradPField_;

    Field<Tensor3D> gradVecField_;
    Field<Vector3D> gradScalarField_;

    IndexMap indexMap;

    bool timeAccurate_;

    double relaxationFactorMomentum_, relaxationFactorPCorr_;
    int gradReconstructionMethod_;

    int maxInnerIters_, momentumGmresIters_, pCorrGmresIters_;

    Vector3D momentumResidual_;

    int uxStartI_, uyStartI_, uzStartI_, pStartI_;
    int uxEndI_, uyEndI_, uzEndI_, pEndI_;

    void setConstantFields(Input &input);

    /**
     * @brief Solve the momentum equation using the latest available velocity and pressure fields.
     * @param rhoField A reference to the density field.
     * @param muField A reference to the viscosity field.
     * @param sFieldPtr A pointer to any additional source terms to be included in the momentum computation.
     * @param timeStep The time step.
     * @param uField A reference to the velocity field.
     * @param pField A reference to the pressure field.
     */
    void computeMomentum(Field<double> &rhoField, Field<double> &muField, Field<double> &massFlowField, Field<Vector3D>* sFieldPtr, double timeStep, Field<Vector3D>& uField, Field<double>& pField);

    /**
     * @brief Interpolate the face centered velocities using the Rhie-Chow interpolation method
     * @param uField A reference to the velocity field.
     * @param pField A reference to the pressure field.
     * @param dField A reference to the D field.
     */
    void rhieChowInterpolateInteriorFaces(Field<Vector3D>& uField, Field<double>& pField);

    /**
     * @brief Compute the mass fluxes at the interior faces from the known velocity field.
     * @param rhoField A reference to the density field.
     * @param uField A reference to the velocity field.
     */
    virtual void computeMassFlowFaces(Field<double> &rhoField, Field<Vector3D>& uField, Field<double> &massFlowField);

    /**
     * @brief Compute the pressure corrections.
     */
    void computePCorr(Field<double> &rhoField, Field<double> &massFlow, Field<Vector3D>& uField, Field<double> &pField);

    /**
     * @brief Correct the mass flow, pressure and velocity fields using the computed pressure corrections.
     * @param uField A reference to the velocity field.
     * @param pField A reference to the pressure field.
     */
    void correctContinuity(Field<double> &rhoField, Field<double> &massFlowField, Field<Vector3D>& uField, Field<double>& pField);

    Vector3D computeResidual(Field<Vector3D>& uField);

public:

    Simple();

    virtual void initialize(Input &input, HexaFvmMesh &mesh);
    void setBoundaryConditions(Input& input);

    int nConservedVariables();

    void storeUField(Field<Vector3D>& uField, Field<Vector3D>& uFieldOld);

    virtual void discretize(double timeStep, std::vector<double>& timeDerivatives);
    void copySolution(std::vector<double>& original);
    void updateSolution(std::vector<double>& update, int method);

    void displayUpdateMessage();
};

#endif
