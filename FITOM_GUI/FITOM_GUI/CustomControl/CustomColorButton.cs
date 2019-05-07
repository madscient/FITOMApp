using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace FITOM_GUI.CustomControl
{
    public partial class CustomColorButton : CustomButtonBase
    {
        public CustomColorButton()
        {
            InitializeComponent();
            AutoSize = false;
        }

        [Browsable(false)]
        public override Color ForeColor => base.ForeColor;

        [Browsable(false)]
        public override Color BackColor => base.BackColor;

        /// <summary>
        /// 活性時の文字色（継承用）
        /// </summary>
        protected Color enabledForeColor = SystemColors.ControlText;
        /// <summary>
        /// 非活性時の文字色（継承用）
        /// </summary>
        protected Color disabledForeColor = SystemColors.GrayText;
        /// <summary>
        /// 選択時の文字色（継承用）
        /// </summary>
        protected Color selectedForeColor = SystemColors.HighlightText;
        /// <summary>
        /// 押下時の文字色（継承用）
        /// </summary>
        protected Color pressedForeColor = SystemColors.HighlightText;
        /// <summary>
        /// 活性時の背景色
        /// </summary>
        protected Color enabledBackColor = SystemColors.Control;
        /// <summary>
        /// 非活性時の背景色
        /// </summary>
        protected Color disabledBackColor = SystemColors.ControlLight;
        /// <summary>
        /// 選択時の背景色
        /// </summary>
        protected Color selectedBackColor = SystemColors.ControlDark;
        /// <summary>
        /// 押下時の背景色
        /// </summary>
        protected Color pressedBackColor = SystemColors.ControlDarkDark;
        /// <summary>
        /// 活性時のフレーム色
        /// </summary>
        protected Color enabledBorderColor = SystemColors.Control;
        /// <summary>
        /// 非活性時のフレーム色
        /// </summary>
        protected Color disabledBorderColor = SystemColors.ControlLight;
        /// <summary>
        /// 選択時のフレーム色
        /// </summary>
        protected Color selectedBorderColor = SystemColors.ControlDark;
        /// <summary>
        /// 押下時のフレーム色
        /// </summary>
        protected Color pressedBorderColor = SystemColors.ControlDarkDark;
        /// <summary>
        /// 活性時のフレーム色
        /// </summary>
        protected BorderWidths enabledBorderWidth = new BorderWidths();
        /// <summary>
        /// 非活性時のフレーム色
        /// </summary>
        protected BorderWidths disabledBorderWidth = new BorderWidths();
        /// <summary>
        /// 選択時のフレーム色
        /// </summary>
        protected BorderWidths selectedBorderWidth = new BorderWidths();
        /// <summary>
        /// 押下時のフレーム色
        /// </summary>
        protected BorderWidths pressedBorderWidth = new BorderWidths();

        protected bool roundFrame = false;

        [Category("CustomColor")]
        /// <summary>
        /// 活性時の文字色
        /// </summary>
        public Color EnabledForeColor
        {
            get => enabledForeColor;
            set
            {
                enabledForeColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 非活性時の文字色
        /// </summary>
        public Color DisabledForeColor
        {
            get => disabledForeColor;
            set
            {
                disabledForeColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 選択時の文字色
        /// </summary>
        public Color SelectedForeColor
        {
            get => selectedForeColor;
            set
            {
                selectedForeColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 押下時の文字色
        /// </summary>
        public Color PushedForeColor
        {
            get => pressedForeColor;
            set
            {
                pressedForeColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 活性時の背景色
        /// </summary>
        public Color EnabledBackColor
        {
            get => enabledBackColor;
            set
            {
                enabledBackColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 非活性時の背景色
        /// </summary>
        public Color DisabledBackColor
        {
            get => disabledBackColor;
            set
            {
                disabledBackColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 選択時の背景色
        /// </summary>
        public Color SelectedBackColor
        {
            get => selectedBackColor;
            set
            {
                selectedBackColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 押下時の背景色
        /// </summary>
        public Color PushedBackColor
        {
            get => pressedBackColor;
            set
            {
                pressedBackColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 活性時のフレーム色
        /// </summary>
        public Color EnabledBorderColor
        {
            get => enabledBorderColor;
            set
            {
                enabledBorderColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 非活性時のフレーム色
        /// </summary>
        public Color DisabledBorderColor
        {
            get => disabledBorderColor;
            set
            {
                disabledBorderColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 選択時のフレーム色
        /// </summary>
        public Color SelectedBorderColor
        {
            get => selectedBorderColor;
            set
            {
                selectedBorderColor = value;
                OnStatusChanged();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 押下時のフレーム色
        /// </summary>
        public Color PushedBorderColor
        {
            get => pressedBorderColor;
            set
            {
                pressedBorderColor = value;
                OnStatusChanged();
            }
        }
        [Category("Appearance")]
        /// <summary>
        /// 活性時のフレーム幅
        /// </summary>
        public BorderWidths EnabledBorderWidth
        {
            get => enabledBorderWidth;
            set
            {
                enabledBorderWidth = value;
                OnStatusChanged();
            }
        }
        [Category("Appearance")]
        /// <summary>
        /// 非活性時のフレーム幅
        /// </summary>
        public BorderWidths DisabledBorderWidth
        {
            get => disabledBorderWidth;
            set
            {
                disabledBorderWidth = value;
                OnStatusChanged();
            }
        }
        [Category("Appearance")]
        /// <summary>
        /// 選択時のフレーム幅
        /// </summary>
        public BorderWidths SelectedBorderWidth
        {
            get => selectedBorderWidth;
            set
            {
                selectedBorderWidth = value;
                OnStatusChanged();
            }
        }
        [Category("Appearance")]
        /// <summary>
        /// 押下時のフレーム幅
        /// </summary>
        public BorderWidths PushedBorderWidth
        {
            get => pressedBorderWidth;
            set
            {
                pressedBorderWidth = value;
                OnStatusChanged();
            }
        }

        [Category("Appearance")]
        /// <summary>
        /// 丸み枠をつける
        /// </summary>
        public bool RoundFrame
        {
            get => roundFrame;
            set
            {
                roundFrame = value;
                refreshFrame();
            }
        }
        /// <summary>
        /// 状態変更時イベントオーバーライド
        /// </summary>
        protected void OnStatusChanged()
        {
            Invalidate();
        }
        /// <summary>
        /// 描画オーバーライド
        /// </summary>
        /// <param name="e"></param>
        protected override void OnPaint(PaintEventArgs e)
        {
            Color fore = enabledForeColor;
            Color back = enabledBackColor;
            Color border = enabledBorderColor;
            BorderWidths bw = enabledBorderWidth;

            if (!Enabled)
            {
                fore = disabledForeColor;
                back = disabledBackColor;
                border = disabledBorderColor;
                bw = disabledBorderWidth;
            }
            else if (pushed)
            {
                fore = pressedForeColor;
                back = pressedBackColor;
                border = pressedBorderColor;
                bw = pressedBorderWidth;
            }
            else if (selected)
            {
                fore = selectedForeColor;
                back = selectedBackColor;
                border = selectedBorderColor;
                bw = selectedBorderWidth;
            }
            e.Graphics.FillRectangle(new SolidBrush(back), DisplayRectangle);
            StringFormat strform = CommonProc.ConvertAlignmentProperty(TextAlign);
            using (Brush forebrush = new SolidBrush(fore))
            {
                e.Graphics.DrawString(Text, Font, forebrush, DisplayRectangle, strform);
            }

            if (RoundFrame)
            {
                Rectangle rect = new Rectangle(0, 0, this.Width, this.Height);
                // 角丸長方形のGraphicsPathを作成します。
                using (GraphicsPath path = CommonProc.GetRoundRect(rect, 5))
                using (Pen borderpen = new Pen(border, 1.0f) { Alignment = PenAlignment.Inset })
                {
                    //Panelの形を角丸長方形に変更
                    //this.Region = new Region(path);
                    e.Graphics.DrawPath(borderpen, path);
                }
            }
            else
            {
                using (Brush borderbrush = new SolidBrush(border))
                {
                    if (bw.Top > 0)
                    {
                        e.Graphics.FillRectangle(borderbrush, DisplayRectangle.Left, DisplayRectangle.Top, DisplayRectangle.Width, bw.Top);
                    }
                    if (bw.Bottom > 0)
                    {
                        e.Graphics.FillRectangle(borderbrush, DisplayRectangle.Left, DisplayRectangle.Bottom - bw.Bottom, DisplayRectangle.Width, bw.Bottom);
                    }
                    if (bw.Left > 0)
                    {
                        e.Graphics.FillRectangle(borderbrush, DisplayRectangle.Left, DisplayRectangle.Top, bw.Left, DisplayRectangle.Height);
                    }
                    if (bw.Right > 0)
                    {
                        e.Graphics.FillRectangle(borderbrush, DisplayRectangle.Right - bw.Right, DisplayRectangle.Top, bw.Right, DisplayRectangle.Height);
                    }
                }
            }
        }
        private void refreshFrame()
        {
            if (roundFrame)
            {
                // 角丸長方形のGraphicsPathを作成します。
                using (GraphicsPath path = CommonProc.GetRoundRect(DisplayRectangle, 5))
                {
                    //形を角丸長方形に変更
                    this.Region = new Region(path);
                }
            }
            else
            {//普通の長方形
                this.Region = new Region(DisplayRectangle);
            }
            Invalidate();
        }
        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);
            refreshFrame();
        }
    }
}
