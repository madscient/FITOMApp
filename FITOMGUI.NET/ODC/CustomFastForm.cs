using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace JDC.CustomControl
{
    /// <summary>
    /// チラツキ防止フォーム
    /// </summary>
    public class CustomFastForm : Form
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
        /// 表に出た時に強制再描画する（Windows8以降のWS_EX_COMPOSITED対策）
        /// </summary>
        /// <param name="e"></param>
        protected override void OnEnter(EventArgs e)
        {
            base.OnEnter(e);
            Invalidate();
        }
        /// <summary>
        /// 背景画像の透過処理
        /// </summary>
        /// <param name="e"></param>
        protected override void OnPaintBackground(PaintEventArgs e)
        {
            if (BackgroundImage == null)
            {
                base.OnPaintBackground(e);
            }
            else
            {
                //e.Graphics.FillRectangle(new SolidBrush(BackColor), DisplayRectangle);
                e.Graphics.CopyFromScreen(DesktopLocation, Location, Size);
                e.Graphics.DrawImage(BackgroundImage, DisplayRectangle);
            }
        }
    }
}
