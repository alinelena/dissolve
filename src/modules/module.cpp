/*
	*** dUQ Module Interface
	*** src/modules/module.cpp
	Copyright T. Youngs 2012-2016

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

#include "modules/module.h"
#include "classes/configuration.h"
#include "base/sysfunc.h"

// Static Members
int Module::staticLogPoint_ = 0;
int Module::staticBroadcastPoint_ = 0;

// Constructor
Module::Module()
{
	frequency_ = 1;
	enabled_ = true;

	// LogPoints
	logPoint_ = 0;
	broadcastPoint_ = 0;
}

// Destructor
Module::~Module()
{
}

/*
 * Instances
 */

// Delete all instances of this Module
void Module::deleteInstances()
{
	while (instances().first()) instances().removeFirst();
}

/*
 * Definition
 */

// Return unique name of Module
const char* Module::uniqueName()
{
	return uniqueName_.get();
}

// Add dependent Module to this Module
void Module::addDependentModule(Module* module, bool autoAdded)
{
	dependentModules_.add(module, autoAdded);
}

// Return pointer for specified dependent Module
Module* Module::dependentModule(const char* name)
{
	RefListIterator<Module,bool> iterator(dependentModules_);
	while (Module* module = iterator.iterate()) if (DUQSys::sameString(name, module->name())) return module;

	return NULL;
}

// Update targets for any auto-added dependent Modules with those of this Module
void Module::updateDependentTargets()
{
	RefListIterator<Module,bool> iterator(dependentModules_);
	while (Module* module = iterator.iterate())
	{
		// If the Module was *not* added automatically, then do not update the targets
		if (!iterator.currentData()) continue;

		module->copyTargetSamples(this);
		module->copyTargetConfigurations(this);

		// Also may need to update dependent targets of this Module...
		module->updateDependentTargets();
	}
}

/*
 * Basic Control
 */

// Frequency with which to run Module (relative to master simulation loop counter)
void Module::setFrequency(int freq)
{
	frequency_ = freq;
}

// Frequency with which to run Module (relative to master simulation loop counter)
int Module::frequency()
{
	return frequency_;
}

// Return whether the Module should run this iteration
bool Module::runThisIteration(int iteration)
{
	if ((frequency_ < 1) || (!enabled_)) return false;
	else if ((iteration%frequency_) == 0) return true;
	else return false;
}

// Return short descriptive text relating frequency to supplied iteration number
const char* Module::frequencyDetails(int iteration)
{
	static Dnchar result;

	if (frequency_ < 0) return "NEGATIVE?";
	else if (frequency_ == 0) return "disabled";
	else if (frequency_ == 1) return "every time";
	else if ((iteration%frequency_) == 0) return "this iteration";
	else
	{
		// Calculate number of steps necessary to get to next multiple of the frequency_
		int nToGo = frequency_ - (iteration - frequency_*(iteration/frequency_));
		if (nToGo == 1) return "next iteration";

		result.sprintf("in %i steps time", nToGo);
		return result.get();
	}
}

// Set whether the Module is enabled
void Module::setEnabled(bool b)
{
	enabled_ = b;
}

// Return whether the Module is enabled
bool Module::enabled()
{
	return enabled_;
}

/*
 * Targets
 */

// Add Configuration target
bool Module::addConfigurationTarget(Configuration* cfg)
{
	// Check how many Configurations we accept before we do anything else
	if ((nTargetableConfigurations() == -1) || (targetConfigurations_.nItems() < nTargetableConfigurations()))
	{
		targetConfigurations_.add(cfg);
		return true;
	}
	else
	{
		if (nTargetableConfigurations() == 0) Messenger::error("Can't add Configuration '%s' as a target to Module '%s' since it doesn't accept any such targets.\n", cfg->name(), name());
		else Messenger::error("Can't add Configuration '%s' as a target to Module '%s' since the maximum number (%i) has already been reached.\n", cfg->name(), name(), nTargetableConfigurations());
	}

	return false;
}

// Return number of targeted Configurations
int Module::nConfigurationTargets()
{
	return targetConfigurations_.nItems();
}

// Return first targeted Configuration
RefList<Configuration,bool>& Module::targetConfigurations()
{
	return targetConfigurations_;
}

// Copy Configuration targets from specified Module
void Module::copyTargetConfigurations(Module* sourceModule)
{
	// First, check if this module actually accepts target Configurations
	if ((nTargetableConfigurations() < sourceModule->nConfigurationTargets()) && (nTargetableConfigurations() != -1))
	{
		Messenger::warn("Dependent Module '%s' does not accept Configuration targets, but the source Module '%s' lists %i.\n", name(), sourceModule->name());
		return;
	}
	RefListIterator<Configuration,bool> configIterator(sourceModule->targetConfigurations());
	while (Configuration* cfg = configIterator.iterate()) addConfigurationTarget(cfg);
}

// Add Sample target
bool Module::addSampleTarget(Sample* sam)
{
	// Check how many Samples we accept before we do anything else
	if ((nTargetableSamples() == -1) || (targetSamples_.nItems() < nTargetableSamples()))
	{
		targetSamples_.add(sam);
		return true;
	}
	else
	{
		if (nTargetableSamples() == 0) Messenger::error("Can't add Sample '%s' as a target to Module '%s' since it doesn't accept any such targets.\n", sam->name(), name());
		else Messenger::error("Can't add Sample '%s' as a target to Module '%s' since the maximum number (%i) has already been reached.\n", sam->name(), name(), nTargetableSamples());
	}

	return false;
}

// Return number of targeted Samples
int Module::nSampleTargets()
{
	return targetSamples_.nItems();
}

// Return first targeted Sample
RefList<Sample,bool>& Module::targetSamples()
{
	return targetSamples_;
}

// Copy Sample targets from specified Module
void Module::copyTargetSamples(Module* sourceModule)
{
	// First, check if this module actually accepts target Samples
	if ((nTargetableSamples() < sourceModule->nSampleTargets()) && (nTargetableSamples() != -1))
	{
		Messenger::warn("Dependent Module '%s' does not accept Sample targets, but the source Module '%s' lists %i.\n", name(), sourceModule->name());
		return;
	}
	RefListIterator<Sample,bool> sampleIterator(sourceModule->targetSamples());
	while (Sample* sample = sampleIterator.iterate()) addSampleTarget(sample);
}

/*
 * Variables
 */

// Add Variable to Module
void Module::addVariable(const char* varName, VariableValue defaultValue, const char* description)
{
	variables_.setVariable(varName, defaultValue, description, uniqueName());
}

// Retrieve variable from Module (bool)
bool Module::variableAsBool(const char* varName)
{
	return variables_.variableAsBool(varName);
}

// Retrieve variable from Module (int)
int Module::variableAsInt(const char* varName)
{
	return variables_.variableAsInt(varName);
}

// Retrieve variable from Module (double)
double Module::variableAsDouble(const char* varName)
{
	return variables_.variableAsDouble(varName);
}

// Retrieve variable from Module (char)
const char* Module::variableAsChar(const char* varName)
{
	return variables_.variableAsChar(varName);
}

// Retrieve variable from Module (Array<int>)
Array<int>& Module::variableAsIntArray(const char* varName)
{
	return variables_.variableAsIntArray(varName);
}

// Retrieve variable from Module (Array<double>)
Array<double>& Module::variableAsDoubleArray(const char* varName)
{
	return variables_.variableAsDoubleArray(varName);
}

// Retrieve Module variable from supplied Configuration, or get default value (bool)
bool Module::variableAsBool(Configuration* cfg, const char* varName)
{
	Variable* var = cfg->moduleVariable(varName, uniqueName());
	return (var ? var->asBool() : variableAsBool(varName));
}

// Retrieve Module variable from supplied Configuration, or get default value (int)
int Module::variableAsInt(Configuration* cfg, const char* varName)
{
	Variable* var = cfg->moduleVariable(varName, uniqueName());
	return (var ? var->asInt() : variableAsInt(varName));
}

// Retrieve Module variable from supplied Configuration, or get default value (double)
double Module::variableAsDouble(Configuration* cfg, const char* varName)
{
	Variable* var = cfg->moduleVariable(varName, uniqueName());
	return (var ? var->asDouble() : variableAsDouble(varName));
}

// Retrieve Module variable from supplied Configuration, or get default value (char)
const char* Module::variableAsChar(Configuration* cfg, const char* varName)
{
	Variable* var = cfg->moduleVariable(varName, uniqueName());
	return (var ? var->asChar() : variableAsChar(varName));
}

// Retrieve Module variable in supplied Configuration (Array<int>)
Array<int>& Module::variableAsIntArray(Configuration* cfg, const char* varName)
{
	Variable* var = cfg->moduleVariable(varName, uniqueName());
	return (var ? var->asIntArray() : variableAsIntArray(varName));
}

// Retrieve Module variable in supplied Configuration (Array<double>)
Array<double>& Module::variableAsDoubleArray(Configuration* cfg, const char* varName)
{
	Variable* var = cfg->moduleVariable(varName, uniqueName());
	return (var ? var->asDoubleArray() : variableAsDoubleArray(varName));
}

// Retrieve Module variable from supplied Configuration, or get default value
void Module::setVariable(Configuration* cfg, const char* varName, VariableValue value)
{
	cfg->setModuleVariable(varName, value, "", uniqueName());
}

// Append value to Module array variable in supplied Configuration
bool Module::appendVariable(Configuration* cfg, const char* varName, VariableValue value)
{
	cfg->appendModuleVariable(varName, value, "", uniqueName());
}

// Search for named variable in Module
Variable* Module::findVariable(const char* varName)
{
	return variables_.variable(varName, uniqueName());
}

// Return first defined Variable
Variable* Module::variables()
{
	return variables_.variables();
}

/*
 * Parallel Comms
 */

// Broadcast module variables
bool Module::broadcastVariables(ProcessPool& procPool)
{
	return variables_.broadcast(procPool);
}

// Broadcast data associated to module
bool Module::broadcastData(DUQ& duq, ProcessPool& procPool)
{
	return true;
}
