using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using SCCISharp;

namespace FITOM_config
{
    public partial class Form_DeviceSetting : Form
    {
        private SoundInterfaceManager SIManager = SoundInterfaceManager.GetInstance();
        struct FITOMDeviceConfigure
        {
            int SlotID;
            string DeviceSymbol;
            int Mode;
            int Clock;
        };
        struct FITOMInterfaceConfigure
        {
            string InterfaceManager;
            int InterfaceID;
            List<FITOMDeviceConfigure> DeviceCongires;
        };
        private List<FITOMInterfaceConfigure> InterfaceConfigures;
        public Form_DeviceSetting()
        {
            InitializeComponent();
        }

        private void Form_DeviceSetting_Load(object sender, EventArgs e)
        {
            listBox1.Items.Clear();
            if (SIManager != null)
            {
                SIManager.Reset();
                for (int i=0; i<SIManager.InterfaceCount; i++)
                {
                    InterfaceInfo SIInfo = SIManager.GetInterfaceInfo(i);
                    listBox1.Items.Add(SIInfo.ToString());
                    for (int j=0; j<SIInfo.SoundChipCount; j++)
                    {

                    }
                }
            }
        }
    }
}
