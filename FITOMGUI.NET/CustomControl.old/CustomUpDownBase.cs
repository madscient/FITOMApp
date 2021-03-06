﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using JDC.CustomControl;
using JDC.CustomDialog;
using JDC.ProcClass;

namespace JDC.CustomControl
{
    /// <summary>
    /// アップダウン付き数値テキストボックスのベースクラス
    /// 表示以外の制御部分を実装した派生用クラス。フォームから直接使用しない。
    /// </summary>
    public partial class CustomUpDownBase : UserControl
    {
        private Control _buttonText;
        private Control _buttonUp;
        private Control _buttonDown;
        private double internalValue;
        private string paramName = "数値";
        private bool allowNone = false;

        /// <summary>
        /// 値が変わったことを通知するイベント
        /// </summary>
        public event EventHandler ValueChanged;
        /// <summary>
        /// 無効値を入力可能にする
        /// </summary>
        public bool AllowNone
        {
            get
            {
                return allowNone;
            }
            set
            {
                allowNone = value;
            }
        }
        /// <summary>
        /// 項目名
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
        /// 表示テキスト（読み出し専用）
        /// </summary>
        public override string Text
        {
            get
            {
                return (_buttonText != null) ? _buttonText.Text : "";
            }
        }
        /// <summary>
        /// 真の値
        /// </summary>
        public double Value
        {
            get
            {
                return internalValue;
            }
            set
            {
                if (double.IsNaN(value))
                {
                    _buttonText.Text = "";  //NaN
                    internalValue = double.NaN;
                }
                else
                {
                    internalValue = value + (Math.Sign(value) * CommonProc.GetAdjustDouble(StringForm)); //丸め精度補正
                    try
                    {
                        _buttonText.Text = internalValue.ToString(StringForm);
                    }
                    catch (Exception e)
                    {
                        LogProc.WriteLog(LogProc.LOG_KIND_DEBUG, "例外(回復可能)", e);
                        _buttonText.Text = internalValue.ToString();
                    }
                }
            }
        }
        /// <summary>
        /// 最小値
        /// </summary>
        public double RangeMin
        {
            get;
            set;
        }
        /// <summary>
        /// 最大値
        /// </summary>
        public double RangeMax
        {
            get;
            set;
        }
        /// <summary>
        /// 増分
        /// </summary>
        public double Step
        {
            get;
            set;
        }
        /// <summary>
        /// 書式文字列
        /// </summary>
        public string StringForm
        {
            get;
            set;
        }
        /// <summary>
        /// 文字色
        /// </summary>
        public override Color ForeColor
        {
            get
            {
                return (_buttonText != null) ? _buttonText.ForeColor : base.ForeColor;
            }
            set
            {
                if (_buttonText != null)
                {
                    _buttonText.ForeColor = value;
                }
                else
                {
                    base.ForeColor = value;
                }
            }
        }
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public CustomUpDownBase()
        {
            InitializeComponent();
        }
        /// <summary>
        /// メンバー設定（継承先で必ず設定する）
        /// </summary>
        /// <param name="tb">テキストボックス部分のコントロール</param>
        /// <param name="ub">上ボタン部分のコントロール</param>
        /// <param name="db">下ボタン部分のコントロール</param>
        protected void Init(Control tb, Control ub, Control db)
        {
            _buttonText = tb;
            _buttonDown = db;
            _buttonUp = ub;
            _buttonText.ForeColor = base.ForeColor;

            _buttonText.Click +=new EventHandler(buttonText_Click);
            _buttonUp.MouseUp +=new MouseEventHandler(buttonUp_Click);
            _buttonDown.MouseUp += new MouseEventHandler(buttonDown_Click);
        }
        private void buttonUp_Click(object sender, MouseEventArgs e)
        {
            if (!double.IsNaN(Value))
            {
                if ((Value + Step) > RangeMax)
                {
                    Value = RangeMax;
                }
                else
                {
                    Value += Step;
                }
                if (ValueChanged != null)
                {
                    ValueChanged(sender, e);
                }
                Invalidate();
            }
        }

        private void buttonDown_Click(object sender, MouseEventArgs e)
        {
            if (!double.IsNaN(Value))
            {

                if ((Value - Step) < RangeMin)
                {
                    Value = RangeMin;
                }
                else
                {
                    Value -= Step;
                }
                if (ValueChanged != null)
                {
                    ValueChanged(sender, e);
                }
                Invalidate();
            }
        }

        private void buttonText_Click(object sender, EventArgs e)
        {
            CustomDialog.KeyboardDialog kdlg = new CustomDialog.KeyboardDialog();
            kdlg.ValLower = RangeMin;
            kdlg.ValUpper = RangeMax;
            kdlg.DispMode = KeyboardDialog.KB_MODE.NUMBER;  //数値
            string strrange = RangeMax.ToString(StringForm);
            int digits = strrange.IndexOf('.');
            kdlg.LengthInt = (digits < 0) ? strrange.Length : digits;
            kdlg.LengthDec = (digits < 0) ? 0 : (strrange.Length - digits - 1);
            kdlg.AllLength = Math.Max(strrange.Length, RangeMin.ToString(StringForm).Length);
            string msgfmt = CommonProc.MessageText("G001");
            msgfmt = msgfmt.Replace("%param", paramName);
            msgfmt = msgfmt.Replace("%min", RangeMin.ToString(StringForm));
            msgfmt = msgfmt.Replace("%max", RangeMax.ToString(StringForm));
            kdlg.Message_Text = msgfmt;
            kdlg.InputArea = _buttonText.Text;
            kdlg.AllowNone = allowNone;
            if (kdlg.ShowDialog() == DialogResult.OK)
            {
                if (allowNone && kdlg.InputArea == "")
                {
                    Value = double.NaN;
                }
                else
                {
                    Value = double.Parse(kdlg.InputArea);
                }
                if (ValueChanged != null)
                {
                    ValueChanged(sender, e);
                }
            }
            kdlg.Dispose();
            Invalidate();
        }
        /// <summary>
        /// Enableフラグ変更イベント
        /// </summary>
        /// <param name="e"></param>
        protected override void OnEnabledChanged(EventArgs e)
        {
            base.OnEnabledChanged(e);
            if (_buttonText != null)
            {
                _buttonText.Enabled = Enabled;
            }
            if (_buttonUp != null)
            {
                _buttonUp.Enabled = Enabled;
            }
            if (_buttonDown != null)
            {
                _buttonDown.Enabled = Enabled;
            }
        }

    }
}
