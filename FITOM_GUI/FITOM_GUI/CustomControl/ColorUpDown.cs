using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FITOM_GUI.CustomControl
{
    /// <summary>
    /// アップダウン付き数値テキストボックス（フリーサイズ版）
    /// プログラムによって描画するため、表示品質が悪い場合があるので注意
    /// </summary>
    public partial class ColorUpDown : CustomUpDownBase
    {
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public ColorUpDown()
        {
            InitializeComponent();
            base.Init(buttonText, buttonUp, buttonDown);
        }
        /// <summary>
        /// 数値表示部のテキストアライン
        /// </summary>
        public ContentAlignment TextAlign
        {
            get
            {
                return buttonText.TextAlign;
            }
            set
            {
                buttonText.TextAlign = value;
            }
        }
        private void buttonText_Paint(object sender, PaintEventArgs e)
        {
            Rectangle rect = new Rectangle(0, 0, this.Width, this.Height);
            GraphicsPath path = CommonProc.GetRoundRect(rect, 5);

            //ペン作成
            Pen linePen = new Pen(Color.Black, 2);

            // フォームに描画します。
            e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
            e.Graphics.DrawPath(linePen, path);

            //ペン削除
            linePen.Dispose();
        }

        private void buttonDown_Paint(object sender, PaintEventArgs e)
        {
            //ボタン用の長方形を作成
            Rectangle rect_button = buttonDown.DisplayRectangle;
            int radius = 5;

            GraphicsPath path = new GraphicsPath();
            //右上の角丸
            path.AddArc(rect_button.Right - radius * 2, rect_button.Top,
                radius * 2, radius * 2,
                270, 90);
            // 右下の角丸
            path.AddArc(rect_button.Right - radius * 2, rect_button.Bottom - radius * 2,
                radius * 2, radius * 2,
                0, 90);


            //ペン作成
            Pen linePen = new Pen(Color.Black, 2);

            // フォームに描画します。
            e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
            e.Graphics.DrawPath(linePen, path);

            //ペン削除
            linePen.Dispose();
        }

        private void CustomControl_UpDown_EnabledChanged(object sender, EventArgs e)
        {
            buttonText.BackColor = Enabled ? ConstDef.Color_TextBox_Back : ConstDef.Color_Button_Dissable_Back;
        }

        private void CustomControl_UpDown_SizeChanged(object sender, EventArgs e)
        {
            // 角丸長方形のGraphicsPathを作成します。
            GraphicsPath path = CommonProc.GetRoundRect(DisplayRectangle, 5);

            //Panelの形を角丸長方形に変更
            this.Region = new Region(path);
        }

        private void CustomControl_UpDown_Paint(object sender, PaintEventArgs e)
        {
            // フォームに描画します。
            e.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
            e.Graphics.DrawPath(new Pen(Color.Black, 2.0f), CommonProc.GetRoundRect(DisplayRectangle, 5));

        }
    }
}
