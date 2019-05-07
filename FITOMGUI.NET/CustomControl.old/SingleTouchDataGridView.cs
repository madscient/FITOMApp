using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace JDC.CustomControl
{
    /// <summary>
    /// 指一本で複数選択できるDataGridView
    /// </summary>
    class SingleTouchDataGridView : DataGridView
    {
        const int WM_MOUSEMOVE = 0x0200;
        const int WM_MOUSELEAVE = 0x02A3;
        const int WM_LBUTTONDOWN = 0x0201;
        const int WM_LBUTTONUP = 0x0202;
        const int MK_CONTROL = 0x0008;
        /// <summary>
        /// WndProcオーバーライド
        /// </summary>
        /// <param name="m"></param>
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == WM_MOUSEMOVE)
            {//マウスが動いてる間は常にCTRL押しっぱなしにする
                CommonProc.keybd_event(Keys.ControlKey, 0, 0, 0);
                m.WParam = (IntPtr)MK_CONTROL;
            }
            if (m.Msg == WM_MOUSELEAVE)
            {
                CommonProc.keybd_event(Keys.ControlKey, 0, 2, 0);
            }
            base.WndProc(ref m);
        }
    }
}
