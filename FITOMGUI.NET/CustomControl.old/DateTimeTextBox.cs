using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using JDC.CustomDialog;

namespace JDC.CustomControl
{
    /// <summary>
    /// 日付時刻用テキストボックス
    /// </summary>
    public partial class DateTimeTextBox : CustomTextBoxBase
    {
        private DateTime internalDateTime;
        private string strForm = "yyyy/MM/dd HH:mm:ss";
        private bool valueIsNone = false;
        private bool dateOnly = false;
        private bool nosec = false;
        /// <summary>
        /// 真の値
        /// </summary>
        public DateTime Value
        {
            get
            {
                return internalDateTime;
            }
            set
            {
                internalDateTime = value;
                if (allowNone && value == DateTime.MinValue)
                {
                    Text = "";
                    valueIsNone = true;
                }
                else
                {
                    Text = internalDateTime.ToString(strForm);
                }
            }
        }
        /// <summary>
        /// 最小値
        /// </summary>
        public DateTime RangeMin
        {
            get;
            set;
        }
        /// <summary>
        /// 最大値
        /// </summary>
        public DateTime RangeMax
        {
            get;
            set;
        }
        /// <summary>
        /// 書式文字列
        /// </summary>
        public string StringForm
        {
            get
            {
                return strForm;
            }
            set
            {
                strForm = value;
            }
        }
        /// <summary>
        /// 値が無効であることを示す
        /// </summary>
        public bool ValueIsNone
        {
            get
            {
                return valueIsNone;
            }
        }
        /// <summary>
        /// 秒なし
        /// </summary>
        public bool NoSecond
        {
            get
            {
                return nosec;
            }
            set
            {
                nosec = value;
            }
        }
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public DateTimeTextBox()
        {
            InitializeComponent();
            this.paramName = "日付時刻";
        }
        /// <summary>
        /// 描画オーバーライド
        /// </summary>
        /// <param name="pe"></param>
        protected override void OnPaint(PaintEventArgs pe)
        {
            base.OnPaint(pe);
        }
        private bool allowNone = false;
        /// <summary>
        /// 空欄を許可する
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
        /// 日付のみ
        /// </summary>
        public bool DateOnly
        {
            get
            {
                return dateOnly;
            }
            set
            {
                dateOnly = value;
            }
        }
        /// <summary>
        /// マウスクリックイベントオーバーライド
        /// </summary>
        protected override bool InputValue()
        {
            bool res = false;
            if (!readOnly)
            {
                CustomDialog.KeyboardDialog kdlg = new CustomDialog.KeyboardDialog();
                kdlg.DispMode = dateOnly ? KeyboardDialog.KB_MODE.DATEONLY : KeyboardDialog.KB_MODE.DATE;
                kdlg.TimeResolution = nosec ? 1 : 0;
                kdlg.TimeMin = RangeMin;
                kdlg.TimeMax = RangeMax;
                string msgfmt = CommonProc.MessageText(dateOnly ? "G006" : (nosec ? "G011" : "G002"));
                msgfmt = msgfmt.Replace("%param", paramName);
                kdlg.Message_Text = msgfmt;
                if (allowNone && Text == "")
                {
                    kdlg.InputArea = "";
                }
                else
                {
                    kdlg.InputArea = internalDateTime.ToString(dateOnly ? "yyyyMMdd" : (nosec ? "yyyyMMddHHmm" : "yyyyMMddHHmmss"));
                }
                kdlg.AllowNone = allowNone;
                if (kdlg.ShowDialog() == DialogResult.OK)
                {
                    if (allowNone && kdlg.InputArea == "")
                    {
                        Text = "";
                        valueIsNone = true;
                        internalDateTime = DateTime.MinValue;
                    }
                    else
                    {
                        Text = dateOnly ? CommonProc.DateString(kdlg.InputArea) : CommonProc.DatetimeString(kdlg.InputArea);
                        internalDateTime = DateTime.Parse(Text);
                        valueIsNone = false;
                    }
                    res = true;
                }
                kdlg.Dispose();
            }
            return res;
        }
    }
}
