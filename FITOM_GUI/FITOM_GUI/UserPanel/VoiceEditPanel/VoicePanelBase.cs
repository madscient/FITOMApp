﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using FITOM_GUI.CustomControl;

namespace FITOM_GUI.UserPanel.VoiceEditPanel
{
    public partial class VoicePanelBase : UserControl
    {
        public VoicePanelBase()
        {
            InitializeComponent();
            DoubleBuffered = true;
        }
        [Category("Appearance")]
        [Browsable(true)]
        public string Caption { get; set; }

        /// <summary>
        /// バックグラウンド描画オーバーライド
        /// </summary>
        /// <param name="pevent"></param>
        protected override void OnPaintBackground(PaintEventArgs pevent)
        {//塗りつぶしのみとする（イメージ等非対応）
            Graphics g = pevent.Graphics;
            g.FillRectangle(new SolidBrush(BackColor), DisplayRectangle);
        }
        /// <summary>
        /// 親のバックカラー変更イベント（無効化する）
        /// </summary>
        /// <param name="e"></param>
        protected override void OnParentBackColorChanged(EventArgs e)
        {
            //base.OnParentBackColorChanged(e);
        }
        /// <summary>
        /// 親のバックイメージ変更イベント（無効化する）
        /// </summary>
        /// <param name="e"></param>
        protected override void OnParentBackgroundImageChanged(EventArgs e)
        {
            //base.OnParentBackgroundImageChanged(e);
        }
    }
}
