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
    /// 高速ラベル
    /// </summary>
    public class CustomFastLabel : Label
    {
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public CustomFastLabel()
        {
            this.DoubleBuffered = true;
        }
        /// <summary>
        /// バックグラウンド描画オーバーライド
        /// </summary>
        /// <param name="pevent"></param>
        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
            Graphics g = pevent.Graphics;
            g.FillRectangle(new SolidBrush(BackColor), DisplayRectangle);
        }
        /// <summary>
        /// 親のバックカラー変更イベント
        /// </summary>
        /// <param name="e"></param>
        protected override void OnParentBackColorChanged(EventArgs e)
        {
            //base.OnParentBackColorChanged(e);
        }
        /// <summary>
        /// 親のバックイメージ変更イベント
        /// </summary>
        /// <param name="e"></param>
        protected override void OnParentBackgroundImageChanged(EventArgs e)
        {
            //base.OnParentBackgroundImageChanged(e);
        }
    }
}
