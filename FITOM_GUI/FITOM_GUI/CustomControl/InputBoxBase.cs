using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace FITOM_GUI.CustomControl
{
    public partial class InputBoxBase : UserControl
    {
        public InputBoxBase()
        {
            InitializeComponent();
        }

        public override string Text => paramTitle.Text;

        protected ColorSet enabledTitleColor;
        protected ColorSet disabledTitleColor;
        protected ColorSet selectedTitleColor;

    }
}
