using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace JDC.CustomControl
{
    /// <summary>
    /// スクロールバー
    /// </summary>
    public partial class CustomScrollBar : CustomFastControl
    {
        //サムネイル最小値
        private const int THUMB_MIN = 36;
        //ボタンサイズ（垂直の場合は高さ、水平の場合は幅）
        private const int BUTTON_SIZE = 53;
        /// <summary>
        /// 見えている行数
        /// </summary>
        public int ViewSize
        {
            get;
            set;
        }
        /// <summary>
        /// 全体の最小値
        /// </summary>
        public int RangeMin
        {
            get;
            set;
        }
        /// <summary>
        /// 全体の最大値
        /// </summary>
        public int RangeMax
        {
            get;
            set;
        }
        /// <summary>
        /// クリック1回の移動量
        /// </summary>
        public int Step
        {
            get;
            set;
        }
        /// <summary>
        /// １ページの移動量
        /// </summary>
        public int Page
        {
            get;
            set;
        }
        private int position;
        /// <summary>
        /// サムネイルポジション
        /// </summary>
        public int Position
        {
            get
            {
                return position;
            }
            set
            {
                position = (value <= (RangeMax - ViewSize)) ? value : (RangeMax - ViewSize);
                position = (position >= RangeMin) ? position : RangeMin;
                trackBox.Invalidate();
                if (ValueChanged != null)
                {
                    ValueChanged();
                }
            }
        }
        /// <summary>
        /// ポジションが変わったときに発生するイベントハンドラ
        /// </summary>
        public Action ValueChanged
        {
            get;
            set;
        }
        /// <summary>
        /// スクロールバーの向き
        /// </summary>
        public ScrollOrientation Orientation
        {
            get;
            set;
        }
        private bool tracking = false;
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public CustomScrollBar()
        {
            InitializeComponent();
        }

        private void colorButton_Up_MouseClick(object sender, MouseEventArgs e)
        {
            Position -= Step;
        }

        private void colorButton_Down_MouseClick(object sender, MouseEventArgs e)
        {
            Position += Step;
        }

        private Rectangle calcThumbRect()
        {
            if (Orientation == ScrollOrientation.VerticalScroll)
            {//垂直スクロールバー
                double fullHeight = RangeMax - RangeMin;
                if (fullHeight > 0)
                {
                    double thumbHeight = Math.Round(((double)trackBox.Height * (double)ViewSize) / fullHeight);
                    thumbHeight = (thumbHeight < THUMB_MIN) ? THUMB_MIN : thumbHeight;
                    thumbHeight = (thumbHeight > trackBox.Height) ? trackBox.Height : thumbHeight;
                    double viewrange = (fullHeight - ViewSize);
                    if (viewrange < 1.0)
                    {
                        viewrange = 1.0;
                    }
                    double thumbPosition = Math.Round(((double)(trackBox.Height - thumbHeight) * (double)(position - RangeMin) / viewrange));
                    int thumbTop = (thumbPosition >= 0) ? (int)thumbPosition : 0;
                    return new Rectangle(0, (int)thumbPosition, trackBox.Width, (int)thumbHeight);
                }
            }
            if (Orientation == ScrollOrientation.HorizontalScroll)
            {//水平スクロールバー
                double fullWidth = RangeMax - RangeMin;
                if (fullWidth > 0)
                {
                    double thumbWidth = Math.Round(((double)trackBox.Width * (double)ViewSize) / fullWidth);
                    thumbWidth = (thumbWidth < THUMB_MIN) ? THUMB_MIN : thumbWidth;
                    thumbWidth = (thumbWidth > trackBox.Width) ? trackBox.Width : thumbWidth;
                    double viewrange = (fullWidth - ViewSize);
                    if (viewrange < 1.0)
                    {
                        viewrange = 1.0;
                    }
                    double thumbPosition = Math.Round(((double)(trackBox.Width - thumbWidth) * (double)(position - RangeMin) / viewrange));
                    int thumbTop = (thumbPosition >= 0) ? (int)thumbPosition : 0;
                    return new Rectangle((int)thumbPosition, 0, (int)thumbWidth, trackBox.Height);
                }
            }
            return trackBox.DisplayRectangle;
        }

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            Rectangle thumbRect = calcThumbRect();

            //スクロールバー背景
            e.Graphics.FillRectangle(new SolidBrush(ConstDef.Color_Button_Pushed_Back), trackBox.DisplayRectangle);
            //サムネイル
            e.Graphics.FillRectangle(new SolidBrush(ConstDef.Color_Button_Back), thumbRect);
            if (Orientation == ScrollOrientation.VerticalScroll)
            {//垂直スクロールバー
                e.Graphics.DrawRectangle(new Pen(Color.Black, 1.0f), 0, thumbRect.Top, thumbRect.Width - 1, thumbRect.Height - 1);
            }
            if (Orientation == ScrollOrientation.HorizontalScroll)
            {//水平スクロールバー
                e.Graphics.DrawRectangle(new Pen(Color.Black, 1.0f), thumbRect.Left, 0, thumbRect.Width - 1, thumbRect.Height - 1);
            }
        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            Rectangle thumbRect = calcThumbRect();
            if (thumbRect.Contains(e.Location))
            { //トラック開始
                tracking = true;
            }
            else
            {
                if (Orientation == ScrollOrientation.VerticalScroll)
                {
                    if (e.Location.Y < thumbRect.Top)
                    { //サムネイルより上
                        Position -= Page;
                    }
                    if (e.Location.Y > thumbRect.Bottom)
                    { //サムネイルより下
                        Position += Page;
                    }
                }
                if (Orientation == ScrollOrientation.HorizontalScroll)
                {
                    if (e.Location.X < thumbRect.Left)
                    { //サムネイルより左
                        Position -= Page;
                    }
                    if (e.Location.X > thumbRect.Right)
                    { //サムネイルより右
                        Position += Page;
                    }
                }
            }
        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (tracking)
            {//トラック中
                double pos = 0;
                if (Orientation == ScrollOrientation.VerticalScroll)
                {//垂直
                    pos = (double)e.Location.Y / (double)trackBox.Height;
                }
                if (Orientation == ScrollOrientation.HorizontalScroll)
                {//水平
                    pos = (double)e.Location.X / (double)trackBox.Width;
                }
                pos *= (double)(RangeMax - RangeMin);
                pos += (double)RangeMin;
                Position = (int)Math.Round(pos);
            }
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            tracking = false;
        }

        private void CustomScrollBar_Load(object sender, EventArgs e)
        {
            if (Orientation == ScrollOrientation.VerticalScroll)
            {//垂直スクロールバー
                colorButton_Up.Width = Width;
                colorButton_Up.Height = BUTTON_SIZE;
                colorButton_Up.Location = new Point(0, 0);
                colorButton_Up.Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right;
                colorButton_Up.Text = "▲";
                colorButton_Down.Width = Width;
                colorButton_Down.Height = BUTTON_SIZE;
                colorButton_Down.Location = new Point(0, Height - BUTTON_SIZE);
                colorButton_Down.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
                colorButton_Down.Text = "▼";
                trackBox.Width = Width;
                trackBox.Height = Height - (BUTTON_SIZE * 2);
                trackBox.Location = new Point(0, BUTTON_SIZE);
            }
            if (Orientation == ScrollOrientation.HorizontalScroll)
            {//水平スクロールバー
                colorButton_Up.Width = BUTTON_SIZE;
                colorButton_Up.Height = Height;
                colorButton_Up.Location = new Point(0, 0);
                colorButton_Up.Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right;
                colorButton_Up.Text = "◀";
                colorButton_Down.Width = BUTTON_SIZE;
                colorButton_Down.Height = Height;
                colorButton_Down.Location = new Point(Width - BUTTON_SIZE, 0);
                colorButton_Down.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Right;
                colorButton_Down.Text = "▶";
                trackBox.Width = Width - (BUTTON_SIZE * 2);
                trackBox.Height = Height;
                trackBox.Location = new Point(BUTTON_SIZE, 0);
            }
        }
    }
}
