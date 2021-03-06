using System;
using Extensibility;
using EnvDTE;
using EnvDTE80;
using Microsoft.VisualStudio.CommandBars;
using Microsoft.VisualStudio.VCProjectEngine;
using System.Resources;
using System.Reflection;
using System.Globalization;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Xml;
using System.Xml.Xsl;
using System.Xml.XPath;
using System.Collections;

namespace PseudoUnitTestAddin
{
    public class Connect : IDTExtensibility2, IDTCommandTarget
    {
        // How to Debug
        // ------------
        //
        // 1. Recompile debug version of project
		// 2. Close all IDE's
		// 3. Run "Scripts\DeployPseudoAddin.csr Debug"
		// 4. Open new IDE
		// 5. Set start project for PseudoUnitTestAdding to external, find devenv.exe.
		// 6. Set breakpoints, hit F5
        //
		// Use /resetaddin RazzleBuildAddin.Connect to debug setup problems.

        #region Private Fields
        private DTE2 dte;
        private AddIn addin;
        private OutputWindowPane outputWindowPane;
        private Command runTestsCommand;

        #endregion
        
        #region IDTExtensibility2 Implementation
        public void OnConnection(
            object application, ext_ConnectMode connectMode, object addInInst, ref Array custom)
        {
            this.dte = (DTE2)application;
            this.addin = (AddIn)addInInst;

            if (connectMode == ext_ConnectMode.ext_cm_UISetup || 
                connectMode == ext_ConnectMode.ext_cm_Startup ||
                connectMode == ext_ConnectMode.ext_cm_AfterStartup)
            {
                try
                {
                    // TODO-johnls-12/30/2008: Should be localized
                    string runPseudoUnitTestsCaption = "Run Pseudo Unit Tests";

                    runTestsCommand = AddNamedCommand(
                        "RunPseudoUnitTests", runPseudoUnitTestsCaption, "Runs Pseudo based unit test projects", 0, null);
                    
                    CommandBars commandBars = (CommandBars)dte.Application.CommandBars;
                    CommandBar testCommandBar = FindCommandBarPopup(commandBars["MenuBar"], "Test");
    
                    // Did we find the test command bar?
                    if (testCommandBar != null)
                    {
                        CommandBarControl runTestsControl = FindCommandBarControl(testCommandBar, runPseudoUnitTestsCaption);

                        // Did we not yet add a command bar control for the command?
                        if (runTestsControl == null)
                        {
                            runTestsControl = (CommandBarButton)runTestsCommand.AddControl(testCommandBar, 1);

                            // Put a bar over the next item in the menu    
                            testCommandBar.Controls[runTestsControl.Index + 1].BeginGroup = true;
                        }
                    }
                }
                catch (Exception e)
                {
                    WriteOutput("ERROR: Unable to initialize Pseudo unit test add-in: "+ e.Message);
                }
            }
        }
        
        public void OnDisconnection(ext_DisconnectMode disconnectMode, ref Array custom)
        {
        }

        public void OnAddInsUpdate(ref Array custom)
        {
        }

        public void OnStartupComplete(ref Array custom)
        {
        }

        public void OnBeginShutdown(ref Array custom)
        {
        }

        #endregion

        #region IDTCommandTarget Implementation
        public void QueryStatus(string commandName, vsCommandStatusTextWanted neededText, ref vsCommandStatus status, ref object commandText)
        {
            if (neededText == vsCommandStatusTextWanted.vsCommandStatusTextWantedNone)
            {
                if (commandName == MakeFullCommandName("RunPseudoUnitTests"))
                {
                    status = (vsCommandStatus)vsCommandStatus.vsCommandStatusSupported | vsCommandStatus.vsCommandStatusEnabled;
                }
            }
        }

        public void Exec(string commandName, vsCommandExecOption executeOption, ref object varIn, ref object varOut, ref bool handled)
        {
            handled = false;

            if (executeOption == vsCommandExecOption.vsCommandExecOptionDoDefault)
            {
                if (commandName == MakeFullCommandName("RunPseudoUnitTests"))
                {
                    ClearOutput();

                    WriteOutput("Searching for VC++ projects...");

                    Projects projects = (Projects)dte.GetObject("VCProjects");
                    
                    if (projects == null)
                    {
                        WriteOutput("ERROR: Unable to get collection VCProjects");
                        return;
                    }

                    WriteOutput(String.Format("{0} VC++ projects found", projects.Count));
                        
                    for (int i = 1; i <= projects.Count; i++)
                    {
                        Project project = projects.Item(i);
                    
						try
						{
							ProcessVCProject(project.ConfigurationManager.ActiveConfiguration, (VCProject)project.Object);
						}
						catch (InvalidCastException)
						{
							WriteOutput("ERROR: Unable to get VC project interface");
						}
                    }
                    
                    handled = true;
                }
            }
        }

        #endregion
        
        #region Private Methods
        private void ProcessVCProject(Configuration config, VCProject vcProj)
        {
            if (!vcProj.Name.EndsWith("UnitTest") && !vcProj.Name.EndsWith("UnitTests"))
            {
                WriteOutput("Project '{0}' is not a unit test", vcProj.Name);
                return;
            }
            
            String configName = config.ConfigurationName + "|" + config.PlatformName;
            VCConfiguration vcConfig = null;
            IVCCollection collection = (IVCCollection)vcProj.Configurations;

            for (int i = 1; i <= collection.Count; i++)
            {
                vcConfig = (VCConfiguration)collection.Item(i);

                if (vcConfig.Name == configName)
                {
                    break;
                }
            }

            if (vcConfig == null)
            {
                WriteOutput("No matching configuration for project '{0}'", vcProj.Name);
                return;
            }

            string exePath = vcConfig.PrimaryOutput;

            if (exePath != null)
            {
                WriteOutput("Attempting to run unit test project '{0}'", vcProj.Name);
                
                if (File.Exists(exePath) || Path.GetExtension(exePath).ToLower() != ".exe")
                {
                    string output;

                    if (Run(exePath, out output))
                    {
                        if (TransformOutput(ref output))
                        {
                            WriteOutput(output);
                        }
                        else
                        {
                            WriteOutput("ERROR: Unable to transform output for project '{0}'", vcProj.Name);
                        }
                    }
                    else
                    {
                        WriteOutput("ERROR: Could not run program '{0}'", exePath);
                    }
                }
                else
                {
                    WriteOutput("ERROR: Output executable '{0}' not found or not a .exe", exePath);
                }
            }
            else
            {
                WriteOutput("Project '{0}' has no output file specified", vcProj.Name);
            }
        }

        private bool TransformOutput(ref string output)
        {
            bool ok = true;
            StringBuilder sb = new StringBuilder();
            XmlWriterSettings settings = new XmlWriterSettings();

            settings.ConformanceLevel = ConformanceLevel.Auto;

            XmlWriter xw = XmlTextWriter.Create(sb, settings);

            try
            {
                // Load the source document (to be transformed)
                XslCompiledTransform transform = new XslCompiledTransform();

                transform.Load(XmlReader.Create(Assembly.GetExecutingAssembly().GetManifestResourceStream("PseudoUnitTestAddin.UnitTest.xslt")));
                transform.Transform(XmlReader.Create(new StringReader(output)), xw);
            }
            catch (Exception e)
            {
                WriteOutput("ERROR:\n" + e);
                ok = false;
            }
            finally
            {
                xw.Close();
            }

            output = sb.ToString();

            return ok;
        }

        private bool Run(string exePath, out string output)
        {
            int exitCode = -1; // -1 will indicate that the program didn't run
            StringBuilder outputString = new StringBuilder();
            StringWriter outputWriter = new StringWriter(outputString);
            System.Diagnostics.Process p = null;

            try
            {
                WriteOutput("Running '{0}'", exePath); 
                
                ProcessStartInfo info = new ProcessStartInfo(exePath);

                info.UseShellExecute = false;
                info.CreateNoWindow = true;
                info.RedirectStandardOutput = true;
                info.RedirectStandardError = true;
                info.WorkingDirectory = Path.GetDirectoryName(exePath);

                p = new System.Diagnostics.Process();

                p.StartInfo = info;

                StreamRedirectionHelper redirect = new StreamRedirectionHelper(p, outputWriter, outputWriter);

                bool ok = p.Start();
                
                if (ok)
                {
                    p.BeginOutputReadLine();
                    p.BeginErrorReadLine();

                    p.WaitForExit();

                    exitCode = p.ExitCode;
                }
                else
                {
                    WriteOutput("ERROR: Program did not start");
                }
            }
            catch (Exception e)
            {
                WriteOutput("ERROR: " + e);
            }
            finally
            {
                if (p != null)
                    p.Close();
                    
                outputWriter.Close();
            }

            output = outputString.ToString();

            return (exitCode != -1);
        }

        private EnvDTE.OutputWindowPane OutputPane
        {
            get
            {
                if (outputWindowPane == null)
                {
                    string name = "Pseudo Unit Tests";

                    // Retrieve handle to the VS.NET output window collection
                    OutputWindow outputWindow = (OutputWindow)dte.Windows.Item(Constants.vsWindowKindOutput).Object;

                    foreach (OutputWindowPane pane in outputWindow.OutputWindowPanes)
                    {
                        if (pane.Name == name)
                        {
                            outputWindowPane = pane;
                            break;
                        }
                    }

                    if (outputWindowPane == null)
                        outputWindowPane = outputWindow.OutputWindowPanes.Add(name);
                }

                return outputWindowPane;
            }
        }

        private void WriteOutput(string format, params object[] objs)
        {
            OutputPane.OutputString(String.Format(format + Environment.NewLine, objs));
        }

        private void ClearOutput()
        {
            OutputPane.Clear();
            OutputPane.Activate();
        }

        private CommandBar FindCommandBarPopup(CommandBar commandBar, string name)
        {
            for (int i = 1; i < commandBar.Controls.Count; i++)
            {
                CommandBarControl commandBarControl = commandBar.Controls[i];

                if (commandBarControl.Type == MsoControlType.msoControlPopup)
                {
                    CommandBarPopup commandBarPopup = (CommandBarPopup)commandBarControl;

                    if (commandBarPopup.CommandBar.Name == name)
                    {
                        return ((CommandBarPopup)commandBarControl).CommandBar;
                    }
                }
            }

            return null;
        }

        private CommandBarControl FindCommandBarControl(CommandBar commandBar, string caption)
        {
            for (int i = 1; i < commandBar.Controls.Count; i++)
            {
                CommandBarControl commandBarControl = commandBar.Controls[i];

                if (commandBarControl.Type == MsoControlType.msoControlButton && 
                    String.Compare(commandBarControl.Caption, caption, StringComparison.CurrentCultureIgnoreCase) == 0)
                {
                    return commandBarControl;
                }
            }

            return null;
        }

        private string MakeFullCommandName(string name)
        {
            return this.addin.ProgID + "." + name;
        }

        private Command FindCommand(string name)
        {
            IEnumerator e = dte.Commands.GetEnumerator();

            while (e.MoveNext())
            {
                Command command = (Command)e.Current;

                if (MakeFullCommandName(name) == command.Name)
                    return command;
            }

            return null;
        }

        private Command AddNamedCommand(
            string commandName, string caption, string description, int iconIndex, string hotkey)
        {
            // Valid contexts for the commands
            object[] contextGUIDS = new object[] { };
            Commands2 commands = (Commands2)dte.Commands;
            Command command = FindCommand(commandName);

            // If it doesn't already exist so create it
            if (command == null)
            {
                // Add a command to the Commands collection:
                command = commands.AddNamedCommand2(
                    addin, commandName, caption, description,
                    false, iconIndex, ref contextGUIDS,
                    (int)vsCommandStatus.vsCommandStatusSupported + (int)vsCommandStatus.vsCommandStatusEnabled,
                    (int)vsCommandStyle.vsCommandStyleText,
                    vsCommandControlType.vsCommandControlTypeButton);

                if (hotkey != null && hotkey != "")
                {
                    object[] bindings = (object[])command.Bindings;

                    if (bindings != null)
                    {
                        bindings = new object[1];
                        bindings[0] = (object)hotkey;

                        try
                        {
                            command.Bindings = (object)bindings;
                        }
                        catch { }
                    }
                }
            }

            return command;
        }

        #endregion
        
        #region Private/Internal Classes

        internal class StreamRedirectionHelper
        {
            private TextWriter errorWriter;
            private TextWriter outputWriter;

            public StreamRedirectionHelper(System.Diagnostics.Process p, TextWriter outputWriter, TextWriter errorWriter)
            {
                this.outputWriter = outputWriter;
                this.errorWriter = errorWriter;

                p.OutputDataReceived += new DataReceivedEventHandler(OnOutputDataReceived);
                p.ErrorDataReceived += new DataReceivedEventHandler(OnErrorDataReceived);
            }

            public void OnOutputDataReceived(object sendingProcess, DataReceivedEventArgs arg)
            {
                if (arg.Data != null)
                    outputWriter.WriteLine(arg.Data);
            }

            public void OnErrorDataReceived(object sendingProcess, DataReceivedEventArgs arg)
            {
                if (arg.Data != null)
                    errorWriter.WriteLine(arg.Data);
            }
        }
 
        #endregion    
    }
}