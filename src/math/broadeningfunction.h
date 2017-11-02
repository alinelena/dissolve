/*
	*** Broadening Function
	*** src/math/broadeningfunction.h
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_BROADENINGFUNCTION_H
#define DUQ_BROADENINGFUNCTION_H

#define MAXFUNCTIONPARAMS 6

#include "base/charstring.h"

// Forward Declarations
class LineParser;
class ProcessPool;

// Broadening Function
class BroadeningFunction
{
	public:
	// Constructor
	BroadeningFunction();
	// Destructor
	~BroadeningFunction();
	// Assignment Operator
	void operator=(const BroadeningFunction& source);
	// Function Types
	enum FunctionType { UnityFunction, GaussianFunction, QDependentGaussianFunction, GaussianC2Function, nFunctionTypes };
	// Return FunctionType from supplied string
	static FunctionType functionType(const char* s);
	// Return FunctionType name
	static const char* functionType(FunctionType func);
	// Return number of parameters needed to define FunctionType
	static int nFunctionParameters(FunctionType func);
	// Return description for FunctionType
	static const char* functionDescription(FunctionType func);


	/*
	 * Function Data
	 */
	private:
	// Function Type
	FunctionType function_;
	// Parameters
	double parameters_[MAXFUNCTIONPARAMS];
	// Whether function is inverted - y returns yFT, and vice versa
	bool inverted_;

	public:
	// Set function data
	void set(FunctionType function, double p1 = 0.0, double p2 = 0.0, double p3 = 0.0, double p4 = 0.0, double p5 = 0.0, double p6 = 0.0);
	// Set function data from LineParser source
	bool set(LineParser& parser, int startArg);
	// Return function type
	FunctionType function() const;
	// Return short summary of function parameters
	CharString parameterSummary() const;
	// Set inversion state
	void setInverted(bool state);
	// Return value of function given parameters x and Q
	double y(double x, double Q) const;
	// Return value of Fourier transform of function, given parameters x and Q
	double yFT(double x, double Q) const;
	// Return value of function given parameters x and Q, regardless of inversion state
	double yActual(double x, double Q) const;
	// Return value of Fourier transform of function, given parameters x and Q, regardless of inversion state
	double yFTActual(double x, double Q) const;
	// Return unity function
	static BroadeningFunction& unity();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, int root = 0);
};

#endif
