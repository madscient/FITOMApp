using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace FITOM_GUI.CustomControl
{
    public partial class CustomPictureButton : CustomButtonBase
    {
        public CustomPictureButton()
        {
            InitializeComponent();
        }

        private Image enabledImage = null;
        private Image disabledImage = null;
        private Image selectedImage = null;
        private Image pressedImage = null;

        [Category("CustomImage")]
        public Image EnabledImage
        {
            get => enabledImage;
            set
            {
                enabledImage = value;
                Invalidate();
            }
        }
        [Category("CustomImage")]
        public Image DisabledImage
        {
            get => disabledImage;
            set
            {
                disabledImage = value;
                Invalidate();
            }
        }
        [Category("CustomImage")]
        public Image SelectedImage
        {
            get => selectedImage;
            set
            {
                selectedImage = value;
                Invalidate();
            }
        }
        [Category("CustomImage")]
        public Image PressedImage
        {
            get => pressedImage;
            set
            {
                pressedImage = value;
                Invalidate();
            }
        }

        protected override void OnPaint(PaintEventArgs pe)
        {
            //base.OnPaint(pe);
            if (!Enabled)
            {
                base.Image = disabledImage;
            }
            else if (pushed)
            {
                base.Image = pressedImage;
            }
            else if (selected)
            {
                base.Image = selectedImage;
            }
            else
            {
                base.Image = enabledImage;
            }
            if (Image != null)
            {
                Graphics g = pe.Graphics;
                g.DrawImage(Image, DisplayRectangle);
                StringFormat fmt = CommonProc.ConvertAlignmentProperty(TextAlign);
                fmt.FormatFlags |= StringFormatFlags.MeasureTrailingSpaces;
                using (Brush forebrush = new SolidBrush(ForeColor))
                {
                    g.DrawString(base.Text, Font, forebrush, DisplayRectangle, fmt);
                }
            }
            else
            {
                base.OnPaint(pe);
            }
        }
    }
}
