using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using FITOM_GUI.CustomControl;
using FITOM_GUI.UserPanel;

namespace FITOM_GUI
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void customPictureButton3_Click(object sender, EventArgs e)
        {
            Close();
        }

        private CustomTabBase mainTab = new CustomTabBase();
        private void MainForm_Load(object sender, EventArgs e)
        {
            CustomButtonBase[] buttons = new CustomButtonBase[]
            {
                colorButton1, colorButton2, colorButton3, colorButton4, colorButton5, colorButton6,
            };
            UserPanelBase[] panels = new UserPanelBase[]
            {
                new HomePanel(), new MidiMonitorPanel(), new DeviceMonitorPanel(), new VoicePanel(), new DrumSetPanel(), new SystemSettingPanel(),
            };
            mainTab.AddPanels(buttons, panels);
            mainTab.SelectPanel(0);
        }
    }
}
