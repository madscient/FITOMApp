using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FITOM_GUI.CustomControl
{
    /// <summary>
    /// カスタムテキストボックスのベースクラス
    /// 自動的に入力ダイアログ制御をするテキストボックス
    /// 入力値の属性によって派生クラスで実装する
    /// フォームから直接使用しない
    /// </summary>
    public partial class CustomTextBoxBase : Label
    {
        /// <summary>
        /// 値が変更されたことを通知するイベント
        /// フォーム側でイベントを受け取るデリゲートを登録する
        /// </summary>
        public event EventHandler ValueChanged;
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public CustomTextBoxBase()
        {
            InitializeComponent();
            refreshBackColor();
        }
        private Color enabledBackColor = SystemColors.Control;
        private Color disabledBackColor = SystemColors.ControlDark;
        private Color pressedBackColor = SystemColors.ControlLight;
        private Color enabledForeColor = SystemColors.ControlText;
        private Color disabledForeColor = SystemColors.GrayText;
        private Color borderColor = SystemColors.ActiveBorder;

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
                refreshBackColor();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 非活性時の背景色
        /// </summary>
        public Color DisabledBackColor
        {
            get
            {
                return disabledBackColor;
            }
            set
            {
                disabledBackColor = value;
                refreshBackColor();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 活性時の文字色
        /// </summary>
        public Color EnabledForeColor
        {
            get
            {
                return enabledForeColor;
            }
            set
            {
                enabledForeColor = value;
                refreshBackColor();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 非活性時の文字色
        /// </summary>
        public Color DisabledForeColor
        {
            get => disabledBackColor;
            set
            {
                disabledForeColor = value;
                refreshBackColor();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 押下時の背景色
        /// </summary>
        public Color PressedBackColor
        {
            get => PressedBackColor;
            set
            {
                pressedBackColor = value;
                refreshBackColor();
            }
        }
        [Category("CustomColor")]
        /// <summary>
        /// 枠線の描画色
        /// </summary>
        public Color BorderColor
        {
            get => borderColor;
            set
            {
                borderColor = value;
                Invalidate();
            }
        }
        /// <summary>
        /// 描画オーバーライド
        /// </summary>
        /// <param name="pe"></param>
        protected override void OnPaint(PaintEventArgs pe)
        {
            if (Enabled)
            {
                base.OnPaint(pe);
            }
            else
            {//非活性のときだけ自分で描く・・・
                pe.Graphics.FillRectangle(new SolidBrush(disabledBackColor), DisplayRectangle);
                StringFormat strform = CommonProc.ConvertAlignmentProperty(TextAlign);
                pe.Graphics.DrawString(Text, Font, new SolidBrush(disabledForeColor), DisplayRectangle, strform);
            }
            if (BorderStyle == BorderStyle.None)
            {//BorderStyle != Noneのときは描かない
                //Panelの形を角丸長方形に変更
                Rectangle dstRect = new Rectangle(0, 0, this.Width - 1, this.Height - 1);
                pe.Graphics.SmoothingMode = SmoothingMode.AntiAlias;
                GraphicsPath path = CommonProc.GetRoundRect(dstRect, 5);
                pe.Graphics.DrawPath(new Pen(borderColor, 1.5f), path);
            }
        }
        /// <summary>
        /// 押下フラグ（内部）
        /// </summary>
        protected bool pressed = false;
        /// <summary>
        /// パラメータ名（内部）
        /// </summary>
        protected string paramName = "テキスト";
        /// <summary>
        /// 読み取り専用フラグ（内部）
        /// </summary>
        protected bool readOnly = false;
        /// <summary>
        /// 項目名（入力ダイアログに表示）
        /// </summary>
        public string ParamName
        {
            get
            {
                return paramName;
            }
            set
            {
                paramName = value;
            }
        }
        /// <summary>
        /// 読み取り専用フラグ
        /// </summary>
        public bool ReadOnly
        {
            get
            {
                return readOnly;
            }
            set
            {
                readOnly = value;
            }
        }
        private void refreshBackColor()
        {
            BackColor = pressed ? PressedBackColor : (Enabled ? EnabledBackColor : DisabledBackColor);
            ForeColor = Enabled ? EnabledForeColor : DisabledForeColor;
            Invalidate();
        }
        /// <summary>
        /// マウスダウンイベント
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void CustomControl_TextBoxBase_MouseDown(object sender, MouseEventArgs e)
        {
            pressed = true;
            refreshBackColor();
        }
        /// <summary>
        /// マウスアップイベント
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void CustomControl_TextBoxBase_MouseUp(object sender, MouseEventArgs e)
        {
            pressed = false;
            refreshBackColor();
        }

        private void CustomControl_TextBoxBase_EnabledChanged(object sender, EventArgs e)
        {
            refreshBackColor();
        }

        /// <summary>
        /// マウスクリックイベント（オーバーライド用）
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void OnMouseClick(object sender, EventArgs e)
        {
            if (!readOnly)
            {
                if (InputValue() && ValueChanged != null)
                {
                    ValueChanged(sender, e);
                }
            }
        }
        /// <summary>
        /// 派生クラスで値を入力する
        /// </summary>
        /// <returns>true:入力値が確定 false:入力をキャンセル</returns>
        protected virtual bool InputValue()
        {
            //中身は派生クラスで実装する
            return false;
        }
        private void CustomControl_TextBoxBase_SizeChanged(object sender, EventArgs e)
        {
            //Panelの形を角丸長方形に変更
            GraphicsPath rpath = CommonProc.GetRoundRect(DisplayRectangle, 5);
            this.Region = new Region(rpath);
        }


    }
}
