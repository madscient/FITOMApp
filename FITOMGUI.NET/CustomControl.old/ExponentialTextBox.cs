﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using JDC.ProcClass;

namespace JDC.CustomControl
{
    /// <summary>
    /// 指数表示数値用テキストボックス
    /// </summary>
    public partial class ExponentialTextBox : CustomTextBoxBase
    {
        private double internalValue;
        private double mantissa;
        private int exponent;
        private string strForm;
        private int significants;
        private bool allowNaN = false;
        /// <summary>
        /// 有効桁数(小数点以下)
        /// </summary>
        public int Significants
        {
            get
            {
                return significants;
            }
            set
            {
                significants = value;
                if (value > 0)
                {
                    strForm = "0.";
                    for (int i = 0; i < value; i++)
                    {
                        strForm += "0";
                    }
                    strForm += "E+0";
                }
                else
                {
                    strForm = "E";
                }
                Value = Value;  //再表示
            }
        }
        /// <summary>
        /// 仮数部最小値
        /// </summary>
        public double MinMantissa
        {
            get;
            set;
        }
        /// <summary>
        /// 仮数部最大値
        /// </summary>
        public double MaxMantissa
        {
            get;
            set;
        }
        /// <summary>
        /// 指数部最小値
        /// </summary>
        public int MinExponent
        {
            get;
            set;
        }
        /// <summary>
        /// 指数部最大値
        /// </summary>
        public int MaxExponent
        {
            get;
            set;
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
                try
                {
                    if (strForm == null || strForm.IndexOf("E") < 0)
                    {
                        strForm = "E";
                    }
                    value = Math.Min(value, MaxValue);
                    value = Math.Max(value, MinValue);
                    string etext = value.ToString(strForm);
                    string strmant = etext.Substring(0, etext.IndexOf("E"));
                    string strexp = etext.Substring(etext.IndexOf("E") + 1);
                    mantissa = double.Parse(strmant) + CommonProc.GetAdjustDouble(strForm.Substring(strForm.IndexOf("E")));
                    exponent = int.Parse(strexp);
                    internalValue = value;
                    if (Superscripts)
                    {
                        Text = strmant + "×10" + CommonProc.Superscripts(strexp);
                    }
                    else
                    {
                        Text = etext;
                    }
                }
                catch (Exception e)
                {
                    LogProc.WriteLog(LogProc.LOG_KIND_DEBUG, "例外(回復可能)", e);
                    internalValue = double.NaN;
                    mantissa = double.NaN;
                    exponent = 0;
                    Text = "";
                }
            }
        }
        /// <summary>
        /// 全体の最小値
        /// </summary>
        public double MinValue
        {
            get;
            set;
        }
        /// <summary>
        /// 全体の最大値
        /// </summary>
        public double MaxValue
        {
            get;
            set;
        }
        /// <summary>
        /// 仮数部
        /// </summary>
        public double Mantissa
        {
            set
            {
                if (allowNaN && double.IsNaN(value))
                {
                    internalValue = mantissa = double.NaN;
                    Text = "";
                }
                else
                {
                    double min = double.Parse(MinMantissa.ToString(strForm));
                    double max = double.Parse(MaxMantissa.ToString(strForm));
                    double val = double.Parse(value.ToString(strForm));
                    if (min <= val && val <= max)
                    {
                        mantissa = val;
                        Value = mantissa * Math.Pow(10.0, (double)exponent);
                    }
                }
            }
            get
            {
                return mantissa;
            }
        }
        /// <summary>
        /// 指数部
        /// </summary>
        public int Exponent
        {
            set
            {
                if (allowNaN && double.IsNaN(mantissa))
                {
                    exponent = 0;
                }
                else
                {
                    if (MinExponent <= value && value <= MaxExponent)
                    {
                        exponent = value;
                        Value = mantissa * Math.Pow(10.0, (double)exponent);
                    }
                }
            }
            get
            {
                return exponent;
            }
        }
        /// <summary>
        /// 指数を上付き数字で表示する
        /// </summary>
        public bool Superscripts
        {
            get;
            set;
        }
        /// <summary>
        /// 無効値（NaN）を許容する
        /// </summary>
        public bool AllowNaN
        {
            get
            {
                return allowNaN;
            }
            set
            {
                allowNaN = value;
            }
        }
        private bool InBound(double value)
        {
            bool ret = false;
            return ret;
        }
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public ExponentialTextBox()
        {
            InitializeComponent();
        }

        /// <summary>
        /// マウスクリックイベントオーバーライド
        /// </summary>
        protected override bool InputValue()
        {
            bool res = false;
            if (!readOnly)
            {
                CustomDialog.InputExponentialDialog edlg = new CustomDialog.InputExponentialDialog();
                edlg.MinExponent = MinExponent;
                edlg.MaxExponent = MaxExponent;
                edlg.MinMantissa = MinMantissa;
                edlg.MaxMantissa = MaxMantissa;
                edlg.Exponent = exponent;
                edlg.Mantissa = mantissa;
                edlg.MinValue = MinValue;
                edlg.MaxValue = MaxValue;
                edlg.Significants = Significants;
                edlg.AllowNone = allowNaN;
                string msgfmt = CommonProc.MessageText("G001");
                msgfmt = msgfmt.Replace("%param", paramName);
                msgfmt = msgfmt.Replace("%min", MinValue.ToString(strForm));
                msgfmt = msgfmt.Replace("%max", MaxValue.ToString(strForm));
                edlg.Caption = msgfmt;
                if (edlg.ShowDialog() == DialogResult.OK)
                {
                    if (edlg.ValueIsNone)
                    {
                        Exponent = 0;
                        Mantissa = double.NaN;
                    }
                    else
                    {
                        Exponent = edlg.Exponent;
                        Mantissa = edlg.Mantissa;
                    }
                    res = true;
                }
                edlg.Dispose();
            }
            return res;
        }
    }
}
