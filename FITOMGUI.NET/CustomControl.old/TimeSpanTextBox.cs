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
    /// 時間用テキストボックス
    /// </summary>
    public partial class TimeSpanTextBox : CustomTextBoxBase
    {
        private TimeSpan internalTimeSpan;
        private string strForm = "yyyy/MM/dd HH:mm:ss";
        private bool valueIsNone = false;
        private bool nosec = false;
        /// <summary>
        /// 真の値
        /// </summary>
        public TimeSpan Value
        {
            get
            {
                return internalTimeSpan;
            }
            set
            {
                internalTimeSpan = value;
                if (allowNone && value == TimeSpan.MinValue)
                {
                    Text = "";
                    valueIsNone = true;
                }
                else
                {
                    Text = internalTimeSpan.ToString(strForm);
                }
            }
        }
        /// <summary>
        /// 最小値
        /// </summary>
        public TimeSpan RangeMin
        {
            get;
            set;
        }
        /// <summary>
        /// 最大値
        /// </summary>
        public TimeSpan RangeMax
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
        public TimeSpanTextBox()
        {
            InitializeComponent();
            this.paramName = "後時間";
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
        /// マウスクリックイベントオーバーライド
        /// </summary>
        protected override bool InputValue()
        {
            bool res = false;
            if (!readOnly)
            {
                CustomDialog.KeyboardDialog kdlg = new CustomDialog.KeyboardDialog();
                kdlg.DispMode = KeyboardDialog.KB_MODE.TIME;
                string msgfmt = CommonProc.MessageText(nosec ? "G009" : "G008");
                msgfmt = msgfmt.Replace("%param", paramName);
                kdlg.Message_Text = msgfmt;
                if (allowNone && Text == "")
                {
                    kdlg.InputArea = "";
                }
                else
                {
                    kdlg.InputArea = internalTimeSpan.ToString(nosec ? "HHmm" : "HHmmss");
                }
                kdlg.AllowNone = allowNone;
                if (kdlg.ShowDialog() == DialogResult.OK)
                {
                    if (allowNone && kdlg.InputArea == "")
                    {
                        Text = "";
                        valueIsNone = true;
                        internalTimeSpan = TimeSpan.MinValue;
                    }
                    else
                    {
                        Text = CommonProc.DatetimeString(kdlg.InputArea);
                        internalTimeSpan = TimeSpan.Parse(Text);
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
