﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using JDC.CustomDialog;
using JDC.ProcClass;

namespace JDC.CustomControl
{
    /// <summary>
    /// 指数表記数値用入力ボックス
    /// </summary>
    class ExponentInputArea : InputAreaBase
    {
        public ExponentInputArea()
        {
        }
        private double internalValue;
        private double mantissa;
        private int exponent;
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
                refreshText();  //再表示
            }
        }
        protected override void refreshText()
        {
            Value = Value;
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
                        frontend.Text = strmant + "×10" + CommonProc.Superscripts(strexp);
                    }
                    else
                    {
                        frontend.Text = etext;
                    }
                }
                catch (Exception e)
                {
                    LogProc.WriteLog(LogProc.LOG_KIND_DEBUG, "例外(回復可能)", e);
                    internalValue = double.NaN;
                    mantissa = double.NaN;
                    exponent = 0;
                    frontend.Text = "";
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
                    frontend.Text = "";
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
        protected override bool DoInput()
        {
            bool ret = false;
            InputExponentialDialog edlg = new InputExponentialDialog();
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
            msgfmt = msgfmt.Replace("%param", ParamName);
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
                ret = true;
            }
            edlg.Dispose();
            return ret;
        }
    }
}
