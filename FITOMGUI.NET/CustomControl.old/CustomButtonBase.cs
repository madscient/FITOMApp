using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace JDC.CustomControl
{
    /// <summary>
    /// カスタムボタンのベースクラス
    /// 通常はフォームから直接使用しない
    /// </summary>
    public partial class CustomButtonBase : CustomFastLabel
    {
        /// <summary>
        /// 押下状態
        /// </summary>
        protected bool pushed = false;
        /// <summary>
        /// 選択状態
        /// </summary>
        protected bool selected = false;
        /// <summary>
        /// 選択状態を設定する
        /// </summary>
        public bool Selected
        {
            get
            {
                return selected;
            }
            set
            {
                selected = value;
                OnStatusChanged();
            }
        }
        /// <summary>
        /// 選択状態を「選択」にする
        /// </summary>
        /// <param name="condition"></param>
        public void Select(bool condition = true)
        {
            Selected = condition;
        }
        /// <summary>
        /// 自動トグルモード
        /// </summary>
        public bool AutoToggle
        {
            get;
            set;
        }
        /// <summary>
        /// 活性時の文字色（継承用）
        /// </summary>
        protected Color enabledForeColor = ConstDef.Color_Button_Text;
        /// <summary>
        /// 非活性時の文字色（継承用）
        /// </summary>
        protected Color disabledForeColor = ConstDef.Color_Button_Dissable_Text;
        /// <summary>
        /// 選択時の文字色（継承用）
        /// </summary>
        protected Color selectedForeColor = ConstDef.Color_Button_Slected_Text;
        /// <summary>
        /// 押下時の文字色（継承用）
        /// </summary>
        protected Color pressedForeColor = ConstDef.Color_Button_Pushed_Text;
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
                OnStatusChanged();
            }
        }
        /// <summary>
        /// 非活性時の文字色
        /// </summary>
        public Color DisabledForeColor
        {
            get
            {
                return disabledForeColor;
            }
            set
            {
                disabledForeColor = value;
                OnStatusChanged();
            }
        }
        /// <summary>
        /// 選択時の文字色
        /// </summary>
        public Color SelectedForeColor
        {
            get
            {
                return selectedForeColor;
            }
            set
            {
                selectedForeColor = value;
                OnStatusChanged();
            }
        }
        /// <summary>
        /// 押下時の文字色
        /// </summary>
        public Color PushedForeColor
        {
            get
            {
                return pressedForeColor;
            }
            set
            {
                pressedForeColor = value;
                OnStatusChanged();
            }
        }
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public CustomButtonBase()
        {
            InitializeComponent();
            this.DoubleBuffered = true;
        }
        /// <summary>
        /// 状態変更イベント（継承用）
        /// </summary>
        protected virtual void OnStatusChanged()
        {
            ForeColor = Enabled ? (pushed ? pressedForeColor : (Selected ? selectedForeColor : enabledForeColor)) : disabledForeColor;
            Invalidate();
        }

        private void CustomControl_CustomButton_MouseDown(object sender, MouseEventArgs e)
        {
            pushed = true;
            OnStatusChanged();
        }

        private void CustomControl_CustomButton_MouseUp(object sender, MouseEventArgs e)
        {
            pushed = false;
            OnStatusChanged();
        }
        /// <summary>
        /// マウスクリックイベント（継承用）
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void OnMouseClick(object sender, EventArgs e)
        {
            if (AutoToggle)
            {
                Selected = !Selected;
            }
        }

        private void CustomControl_CustomButton_EnabledChanged(object sender, EventArgs e)
        {
            OnStatusChanged();
        }
        /// <summary>
        /// 背景描画イベント（高速化）
        /// </summary>
        /// <param name="pevent"></param>
        protected override void OnPaintBackground(PaintEventArgs pevent)
        {
            pevent.Graphics.FillRectangle(new SolidBrush(BackColor), DisplayRectangle);
            //if (Image != null)
            //{
            //    pevent.Graphics.DrawImage(Image, DisplayRectangle);
            //}
        }
    }
}
