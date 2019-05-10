using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace JDC.CustomControl
{
    /// <summary>
    /// カラーボタン
    /// プログラムで描画するため、描画品質が悪い場合があるので注意
    /// </summary>
    public partial class ColorButton : CustomButtonBase
    {
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public ColorButton()
        {
            InitializeComponent();
        }

        private Color enabledBackColor = ConstDef.Color_Button_Back;
        private Color disabledBackColor = ConstDef.Color_Button_Dissable_Back;
        private Color selectedBackColor = ConstDef.Color_Button_Slected_Back;
        private Color pushedBackColor = ConstDef.Color_Button_Pushed_Back;
        private bool roundFrame = false;
        /// <summary>
        /// 丸み枠をつける
        /// </summary>
        public bool RoundFrame
        {
            get
            {
                return roundFrame;
            }
            set
            {
                roundFrame = value;
                refreshFrame();
            }
        }
        /// <summary>
        /// 状態変更時イベントオーバーライド
        /// </summary>
        protected override void OnStatusChanged()
        {
            if (Enabled)
            {
                if (pushed)
                {//押下状態
                    BackColor = pushedBackColor;
                    ForeColor = pressedForeColor;
                }
                else
                {//非押下
                    if (selected)
                    {//選択状態
                        BackColor = selectedBackColor;
                        ForeColor = selectedForeColor;
                    }
                    else
                    {//非選択状態
                        BackColor = enabledBackColor;
                        ForeColor = enabledForeColor;
                    }
                }
            }
            else
            {//無効状態
                BackColor = disabledBackColor;
                ForeColor = disabledForeColor;
            }
        }
        /// <summary>
        /// 描画オーバーライド
        /// </summary>
        /// <param name="e"></param>
        protected override void OnPaint(PaintEventArgs e)
        {
            if (Enabled)
            {
                base.OnPaint(e);
            }
            else
            {
                e.Graphics.FillRectangle(new SolidBrush(disabledBackColor), DisplayRectangle);
                StringFormat strform = CommonProc.ConvertAlignmentProperty(TextAlign);
                e.Graphics.DrawString(Text, Font, new SolidBrush(disabledForeColor), DisplayRectangle, strform);
            }
            if (RoundFrame)
            {
                Rectangle rect = new Rectangle(0, 0, this.Width, this.Height);
                // 角丸長方形のGraphicsPathを作成します。
                GraphicsPath path = CommonProc.GetRoundRect(rect, 5);

                //Panelの形を角丸長方形に変更
                //this.Region = new Region(path);
                e.Graphics.DrawPath(new Pen(Color.Black, 2.0f), path);
            }
            else
            {
                Rectangle rect = new Rectangle(0, 0, this.Width, this.Height);
                e.Graphics.DrawRectangle(new Pen(Color.Black, 2.0f), rect);
            }

        }
        private void refreshFrame()
        {
            if (roundFrame)
            {
                // 角丸長方形のGraphicsPathを作成します。
                GraphicsPath path = CommonProc.GetRoundRect(DisplayRectangle, 5);

                //形を角丸長方形に変更
                this.Region = new Region(path);
            }
            else
            {//普通の長方形
                this.Region = new Region(DisplayRectangle);
            }
        }
        private void CustomControl_ColorButton_SizeChanged(object sender, EventArgs e)
        {
            refreshFrame();
        }
    }
}
