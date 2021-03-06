/*
	*** Dissolve Main Window
	*** src/gui/gui.h
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_MAINWINDOW_H
#define DISSOLVE_MAINWINDOW_H

#include "gui/ui_gui.h"
#include "gui/widgets/subwindow.h"
#include "gui/guide.h"
#include "gui/outputhandler.hui"
#include "gui/thread.hui"
#include "gui/maintab.h"
#include "base/charstring.h"
#include "templates/list.h"

// Forward Declarations
class BrowserWidget;
class Configuration;
class ConfigurationTab;
class Dissolve;
class ForcefieldTab;
class ModuleLayerTab;
class QMdiSubWindow;
class Species;
class SpeciesTab;
class ModuleTab;
class WorkspaceTab;

class DissolveWindow : public QMainWindow
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	DissolveWindow(Dissolve& dissolve);
	~DissolveWindow();
	// Main form declaration
	Ui::DissolveWindow ui;

	protected:
	void closeEvent(QCloseEvent* event);
	void resizeEvent(QResizeEvent* event);


	/*
	 * Dissolve Integration
	 */
	private:
	// Dissolve reference
	Dissolve& dissolve_;
	// Whether any data has been modified
	bool modified_;
	// Whether window is currently refreshing
	bool refreshing_;
	// Whether window has been shown
	bool shown_;
	// Output handler for messaging in GUI
	GUIOutputHandler outputHandler_;
	// Whether the current simulation is on the local machine
	bool localSimulation_;

	public slots:
	// Flag that data has been modified via the GUI
	void setModified();
	// Flag that data has been modified via the GUI, and whether this invalidates the current setup
	void setModified(bool invalidatesSetUp);
	// Flag that data has been modified via the GUI, and that the setup is now invalid
	void setModifiedAndInvalidated();

	public:
	// Return reference to Dissolve
	Dissolve& dissolve();
	// Link the Messenger in to the GUI output device
	void addOutputHandler();


	/*
	 * File
	 */
	public:
	// Open specified input file from the CLI
	bool openFileFromCLI(const char* inputFile, const char* restartFile, bool ignoreRestartFile, bool ignoreLayoutFile);


	/*
	 * Update Functions
	 */
	public slots:
	// Update all tabs
	void updateTabs();
	// Update window title
	void updateWindowTitle();
	// Update controls frame
	void updateControlsFrame();
	// Perform full update of the GUI, including tab reconciliation
	void fullUpdate();


	/*
	 * Main Menu
	 */
	private:
	// Check whether current input needs to be saved and, if so, if it saved successfully
	bool checkSaveCurrentInput();
	// Clear all data and start new simulation afresh
	void startNew();

	private slots:
	// Session
	void on_SessionNewAction_triggered(bool checked);
	void on_SessionSetupWizardAction_triggered(bool checked);
	void on_SessionOpenLocalAction_triggered(bool checked);
	void on_SessionOpenRemoteAction_triggered(bool checked);
	void on_SessionOpenRecentAction_triggered(bool checked);
	void on_SessionCloseAction_triggered(bool checked);
	void on_SessionSaveAction_triggered(bool checked);
	void on_SessionSaveAsAction_triggered(bool checked);
	void on_SessionQuitAction_triggered(bool checked);
	// Simulation
	void on_SimulationAddSpeciesAction_triggered(bool checked);
	void on_SimulationAddConfigurationAction_triggered(bool checked);
	void on_SimulationAddProcessingLayerAction_triggered(bool checked);
	void on_SimulationAddForcefieldTermsAction_triggered(bool checked);
	void on_SimulationSetRandomSeedAction_triggered(bool checked);
	// Control
	void on_SimulationRunAction_triggered(bool checked);
	void on_SimulationStepAction_triggered(bool checked);
	void on_SimulationStepFiveAction_triggered(bool checked);
	void on_SimulationPauseAction_triggered(bool checked);
	// Workspace
	void on_WorkspaceAddNewAction_triggered(bool checked);
	// Help
	void on_HelpViewQuickStartGuideAction_triggered(bool checked);
	void on_HelpRunATutorialAction_triggered(bool checked);


	/*
	 * Main Stack
	 */
	private:
	// Stack Pages Enum
	enum MainStackPage
	{
		StartStackPage,		/* Start Page - Routes to load, create, and monitor simulations */
		WizardStackPage,	/* Wizard Page - Simulation creation wizard */
		SimulationStackPage,	/* Simulation Page - Controls for current simulation */
		nStackPages
	};

	private:
	// Set currently-visible main stack page
	void showMainStackPage(MainStackPage page);


	/*
	 * 'Start' Stack Page
	 */
	private:
	// Guide object for QuickStart
	Guide quickStartGuide_;

	private slots:
	// 'Create' Group
	void on_StartCreateNewButton_clicked(bool checked);
	void on_StartSetupWizardButton_clicked(bool checked);
	// 'Open / Connect' Group
	void on_StartOpenLocalButton_clicked(bool checked);
	void on_StartOpenRemoteButton_clicked(bool checked);
	void on_StartOpenRecentButton_clicked(bool checked);
	// 'Learn' Group
	void on_StartQuickStartButton_clicked(bool checked);
	void on_StartRunTutorialButton_clicked(bool checked);


	/*
	 * 'Simulation' Stack Page - Run Control
	 */
	public:
	// Dissolve State Enum
	enum DissolveState {
		EditingState,		/* Dissolve is currently editing a file in the GUI */
		RunningState,		/* Dissolve is currently running in the GUI */
		MonitoringState,	/* Dissolve is running in the background, and we are monitoring it via the restart file */
		nDissolveStates
	};

	private:
	// Thread controller
	DissolveThreadController threadController_;
	// Current state of Dissolve
	DissolveState dissolveState_;

	public:
	// Return current state of Dissolve
	DissolveState dissolveState() const;

	private slots: 
	void on_ControlSetUpButton_clicked(bool checked);
	void on_ControlRunButton_clicked(bool checked);
	void on_ControlPauseButton_clicked(bool checked);
	void on_ControlReloadButton_clicked(bool checked);

	public slots:
	// Set widgets ready for the main code to be run
	void setWidgetsForRun();
	// Set widgets after the main code has been run
	void setWidgetsAfterRun();
	// All iterations requested are complete
	void iterationsComplete();

	signals:
	void iterate(int);
	void stopIterating();


	/*
	 * 'Simulation' Stack Page - Tabs
	 */
	private:
	// Pointer to Forcefield tab
	ForcefieldTab* forcefieldTab_;
	// List of Species tabs
	List<SpeciesTab> speciesTabs_;
	// List of Configuration tabs
	List<ConfigurationTab> configurationTabs_;
	// List of processing layer tabs
	List<ModuleLayerTab> processingLayerTabs_;
	// List of Module tabs
	List<ModuleTab> moduleTabs_;
	// List of Workspace tabs
	List<WorkspaceTab> workspaceTabs_;

	private slots:
	void on_MainTabs_currentChanged(int index);
	void mainTabsDoubleClicked(int index);

	private:
	// Remove tabs related to the current data
	void clearTabs();
	// Reconcile tabs, making them consistent with the current data
	void reconcileTabs();
	// Add core (permanent) tabs
	void addCoreTabs();
	// Add on an empty workspace tab
	MainTab* addWorkspaceTab(const char* title);
	// Find tab with title specified
	MainTab* findTab(const char* title);
	// Find tab with specified page widget
	MainTab* findTab(QWidget* page);
	// Find SpeciesTab containing specified page widget
	SpeciesTab* speciesTab(QWidget* page);
	// Find ConfigurationTab containing specified page widget
	ConfigurationTab* configurationTab(QWidget* page);
	// Find ModuleLayerTab containing specified page widget
	ModuleLayerTab* processingLayerTab(QWidget* page);
	// Find ModuleTab containing specified page widget
	ModuleTab* moduleTab(QWidget* page);
	// Find ModuleTab containing specified Module
	ModuleTab* moduleTab(Module* module);
	// Find WorkspaceTab containing specified page widget
	WorkspaceTab* workspaceTab(QWidget* page);
	// Return current tab
	MainTab* currentTab();
	// Make specified tab the current one
	void setCurrentTab(MainTab* tab);
	// Make specified tab the current one (by index)
	void setCurrentTab(int tabIndex);
	// Show forcefield tab
	void showForcefieldTab();
	// Make specified Species tab the current one
	void setCurrentTab(Species* species);
	// Make specified Configuration tab the current one
	void setCurrentTab(Configuration* cfg);
	// Make specified processing layer tab the current one
	void setCurrentTab(ModuleLayer* layer);
	// Return reference list of all current tabs
	RefList<MainTab,bool> allTabs() const;

	public:
	// Add tab for specified Module target
	MainTab* addModuleTab(Module* module);

	public slots:
	// Remove tab containing the specified page widget
	void removeTab(QWidget* page);


	/*
	 * 'Simulation' Stack Page - Guide
	 */
	private slots:
	void guideWidgetCanceled();
	void guideWidgetFinished();


	/*
	 * 'Simulation' Stack Page - State I/O
	 */
	private:
	// Filename containing current window layout
	CharString windowLayoutFilename_;

	public:
	// Save current window layout
	bool saveWindowLayout();
	// Load window state
	bool loadWindowLayout();
};

#endif
