using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace JDC.CustomControl
{
    /// <summary>
    /// 描画改善版UserControl
    /// </summary>
    public class CustomFastControl : UserControl
    {
        const int WS_EX_COMPOSITED = 0x2000000;
        /// <summary>
        /// ちらつき低減のためWS_EX_COMPOSITEDスタイルを強制する
        /// </summary>
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams c = base.CreateParams;
                if (!DesignMode)
                {
                    c.ExStyle |= WS_EX_COMPOSITED;
                }
                return c;
            }
        }
        /// <summary>
        /// 背景描画は最小限にする
        /// </summary>
        /// <param name="e"></param>
        protected override void OnPaintBackground(PaintEventArgs e)
        {
            //base.OnPaintBackground(e);
            e.Graphics.FillRectangle(new SolidBrush(BackColor), DisplayRectangle);
            if (BackgroundImage != null)
            {
                e.Graphics.DrawImage(BackgroundImage, DisplayRectangle);
            }
        }
        /// <summary>
        /// 表に出た時に強制再描画する（Windows8以降のWS_EX_COMPOSITED対策）
        /// </summary>
        /// <param name="e"></param>
        protected override void OnEnter(EventArgs e)
        {
            base.OnEnter(e);
            Invalidate();
        }
    }
}
