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
    /// バーグラフ付き数値ラベル
    /// </summary>
    public partial class BarLabel : CustomFastLabel
    {
        private int range = 0;
        private int count = 0;
        private Color barColor = Color.Green;
        private bool gridline = true;
        /// <summary>
        /// バーの色
        /// </summary>
        public Color BarColor
        {
            get
            {
                return barColor;
            }
            set
            {
                barColor = value;
                Invalidate();
            }
        }  
        /// <summary>
        /// レンジ
        /// </summary>
        public int Range
        {
            get
            {
                return range;
            }
            set
            {
                range = value;
                Invalidate();
            }
        }
        /// <summary>
        /// カウント
        /// </summary>
        public int Count
        {
            get
            {
                return count;
            }
            set
            {
                count = value;
                Text = (count < 999999) ? count.ToString() : "999999";
                Invalidate();
            }
        }
        /// <summary>
        /// 目盛線フラグ
        /// </summary>
        public bool GridLine
        {
            get
            {
                return gridline;
            }
            set
            {
                gridline = value;
                Invalidate();
            }
        }
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public BarLabel()
        {
            InitializeComponent();
        }
        /// <summary>
        /// 描画オーバーライド
        /// </summary>
        /// <param name="pe"></param>
        protected override void OnPaint(PaintEventArgs pe)
        {
            //目盛り線を描く
            if (gridline)
            {
                for (double i = 0.0; i < Width; i += (Width / 10.0))
                {
                    int x = (int)Math.Round(i);
                    pe.Graphics.DrawLine(new Pen(Color.FromArgb(160, 160, 160), 1.0f), new Point(x, 0), new Point(x, Height));
                }
            }
            if (Enabled)
            {
                //base.OnPaint(pe);
                //バーを描く
                double ratio = Width * (double)Count / (double)Range;
                int barheight = Height / 2;
                Rectangle barrect = new Rectangle(0, barheight / 2, (int)Math.Round(ratio), barheight);
                pe.Graphics.FillRectangle(new SolidBrush(BarColor), barrect);

                //テキストを描く
                StringFormat strform = new StringFormat();
                strform.Alignment = StringAlignment.Far;
                strform.LineAlignment = StringAlignment.Center;
                pe.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
                GraphicsPath gp = new GraphicsPath();
                gp.AddString(Text, Font.FontFamily, (int)Font.Style, Font.Height, DisplayRectangle, strform);
                pe.Graphics.DrawPath(new Pen(BackColor, 4.0f), gp);
                pe.Graphics.FillPath(new SolidBrush(ForeColor), gp);
                //pe.Graphics.DrawString(Text, Font, new SolidBrush(ForeColor), DisplayRectangle, strform);
            }
            if (BorderStyle != BorderStyle.None)
            {
                //枠線を描く
                pe.Graphics.DrawRectangle(new Pen(Color.Black, 2.0f), DisplayRectangle);
            }
        }
    }
}
