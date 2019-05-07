using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using JDC.ProcClass;
using JDC.CustomDialog;

namespace JDC.CustomControl
{
    /// <summary>
    /// 日付時刻用入力ボックス
    /// </summary>
    public class DateTimeInputArea : InputAreaBase
    {
        private DateTime internalDateTime;
        private bool valueIsNone = false;
        private bool dateOnly = false;
        private bool allowNone = false;
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public DateTimeInputArea()
        {
            strForm = "yyyy/MM/dd HH:mm:ss";
        }
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
                refreshText();
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
        /// テキスト更新
        /// </summary>
        protected override void refreshText()
        {
            if (allowNone && internalDateTime == DateTime.MinValue)
            {
                frontend.Text = "";
                valueIsNone = true;
            }
            else
            {
                frontend.Text = internalDateTime.ToString(strForm);
            }
        }
        /// <summary>
        /// 入力ダイアログ
        /// </summary>
        /// <returns></returns>
        protected override bool DoInput()
        {
            bool ret = false;
            CustomDialog.KeyboardDialog kdlg = new CustomDialog.KeyboardDialog();
            kdlg.DispMode = dateOnly ? KeyboardDialog.KB_MODE.DATEONLY : KeyboardDialog.KB_MODE.DATE;
            kdlg.TimeMin = RangeMin;
            kdlg.TimeMax = RangeMax;
            string msgfmt = CommonProc.MessageText(dateOnly ? "G006" : "G002");
            msgfmt = msgfmt.Replace("%param", ParamName);
            kdlg.Message_Text = msgfmt;
            if (allowNone && frontend.Text == "")
            {
                kdlg.InputArea = "";
            }
            else
            {
                kdlg.InputArea = internalDateTime.ToString(dateOnly ? "yyyyMMdd" : "yyyyMMddHHmmss");
            }
            kdlg.AllowNone = allowNone;
            if (kdlg.ShowDialog() == DialogResult.OK)
            {
                if (allowNone && kdlg.InputArea == "")
                {
                    frontend.Text = "";
                    valueIsNone = true;
                    internalDateTime = DateTime.MinValue;
                }
                else
                {
                    frontend.Text = dateOnly ? CommonProc.DateString(kdlg.InputArea) : CommonProc.DatetimeString(kdlg.InputArea);
                    internalDateTime = DateTime.Parse(frontend.Text);
                    valueIsNone = false;
                }
            }
            kdlg.Dispose();
            return ret;
        }
    }
}
