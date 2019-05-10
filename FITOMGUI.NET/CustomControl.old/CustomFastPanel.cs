using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using JDC.CustomControl;

namespace JDC.CustomControl
{
    class CustomFastPanel : Panel
    {
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public CustomFastPanel()
        {
            this.DoubleBuffered = true;
        }
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
