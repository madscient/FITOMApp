using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace FITOM_GUI.CustomControl
{
    /// <summary>
    /// 数値用テキストボックス
    /// </summary>
    public partial class DoubleTextBox : CustomControl.CustomTextBoxBase
    {
        private double internalValue;
        private string strForm;
        private int significants;
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
                }
                else
                {
                    strForm = "0";
                }
                Value = Value;  //再表示
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
                value = Math.Min(value, MaxValue);
                value = Math.Max(value, MinValue);
                internalValue = value + CommonProc.GetAdjustDouble(strForm); //丸め精度補正
                Text = internalValue.ToString(strForm);
            }
        }

        /// <summary>
        /// 最小値
        /// </summary>
        public double MinValue
        {
            get;
            set;
        }
        /// <summary>
        /// 最大値
        /// </summary>
        public double MaxValue
        {
            get;
            set;
        }
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public DoubleTextBox()
        {
            InitializeComponent();
        }
        /// <summary>
        /// マウスクリックイベントオーバーライド
        /// </summary>
        protected override bool InputValue()
        {
            bool res = false;
            KeyboardDialog kdlg = new KeyboardDialog();
            kdlg.ValLower = MinValue;
            kdlg.ValUpper = MaxValue;
            kdlg.DispMode = KeyboardDialog.KB_MODE.NUMBER;
            string strrange = MaxValue.ToString(strForm);
            int digits = strrange.IndexOf('.');
            kdlg.LengthInt = (digits < 0) ? strrange.Length : digits;
            kdlg.LengthDec = (digits < 0) ? 0 : (strrange.Length - digits - 1);
            kdlg.AllLength = strrange.Length;
            string msgfmt = CommonProc.MessageText("G001");
            msgfmt = msgfmt.Replace("%param", paramName);
            msgfmt = msgfmt.Replace("%min", MinValue.ToString(strForm));
            msgfmt = msgfmt.Replace("%max", MaxValue.ToString(strForm));
            kdlg.Message_Text = msgfmt;
            kdlg.InputArea = Text;
            if (kdlg.ShowDialog() == DialogResult.OK)
            {
                Value = double.Parse(kdlg.InputArea);
                res = true;
            }
            kdlg.Dispose();
            Invalidate();
            return res;
        }
    }
}
