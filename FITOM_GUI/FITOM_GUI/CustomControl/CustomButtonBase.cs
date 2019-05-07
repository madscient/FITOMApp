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
    public partial class CustomButtonBase : CustomFastLabel
    {
        public CustomButtonBase()
        {
            InitializeComponent();
            AutoSize = false;
        }

        /// <summary>
        /// 押下状態
        /// </summary>
        protected bool pushed = false;
        /// <summary>
        /// 選択状態
        /// </summary>
        protected bool selected = false;

        [Browsable(false)]
        public override bool AutoSize
        {
            set
            {
                base.AutoSize = false;
            }
            get => base.AutoSize;
        }

        [Category("Status")]
        /// <summary>
        /// 選択状態を設定する
        /// </summary>
        public bool Selected
        {
            get => selected;
            set
            {
                selected = value;
                Invalidate();
            }
        }
        [Category("Action")]
        /// <summary>
        /// 自動トグルモード
        /// </summary>
        public bool AutoToggle { get; set; } = false;

        protected override void OnMouseDown(MouseEventArgs e)
        {
            pushed = true;
            base.OnMouseDown(e);
            Invalidate();
        }
        protected override void OnMouseUp(MouseEventArgs e)
        {
            pushed = false;
            base.OnMouseUp(e);
            Invalidate();
        }
        /// <summary>
        /// マウスクリックイベント（継承用）
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected override void OnMouseClick(MouseEventArgs e)
        {
            if (AutoToggle)
            {
                Selected = !Selected;
            }
            base.OnMouseClick(e);
        }

    }
}
