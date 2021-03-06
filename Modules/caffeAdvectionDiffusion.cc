#include <iostream>

#include "Output.h"
#include "Input.h"
#include "RunControl.h"

#include "SolverIncludes.h"
#include "HexaFvmMesh.h"
#include "Diffusion.h"
#include "LinearAdvection.h"

int main(int argc, const char* argv[])
{
    using namespace std;

    Output::printCaffeHeader();

    try
    {
        // Declare the basic program objects

        Input input("case/case.in");
        RunControl runControl;
        Euler solver;
        HexaFvmMesh mesh;
        Diffusion diffusion;
        LinearAdvection linearAdvection;

        mesh.addScalarField("phi", CONSERVED);
        mesh.addScalarField("mu", AUXILLARY);
        mesh.addVectorField("v", AUXILLARY);
        mesh.addScalarField("rho", AUXILLARY);

        // Initialize objects

        runControl.initialize(input);
        solver.initialize(input);
        mesh.initialize(input);
        diffusion.initialize(input, mesh, "phi");
        linearAdvection.initialize(input, mesh, "phi", "v");

        // Set the boundary conditions

        mesh.findScalarField("phi").setAllBoundaries(FIXED, 0.,
                                                     FIXED, 0.,
                                                     FIXED, 1.,
                                                     FIXED, 1.,
                                                     FIXED, 1.,
                                                     FIXED, 0.);

        runControl.displayStartMessage();

        while(runControl.continueRun())
        {
            runControl.residualNorm = solver.solve(runControl.timeStep(), diffusion);

            if(runControl.writeToScreen())
                runControl.displayUpdateMessage();

            if(runControl.writeToFile())
                mesh.writeTec360(runControl.simTime(), "solution");
        }

        // Display end message, the run ended normally

        runControl.displayEndMessage();
    }

    // Catch any exceptions thrown during the run

    catch(const char* errorMessage)
    {
        cerr << "Error: " << errorMessage << endl;
    }

    return 0;
}
