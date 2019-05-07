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
    /// パスワード用テキストボックス
    /// </summary>
    public partial class PasswordTextBox : CustomControl.CustomTextBoxBase
    {
        private string password = "";
        /// <summary>
        /// パスワード文字列
        /// </summary>
        public string Value
        {
            get
            {
                return password;
            }
            set
            {
                password = value;
                Text = "****";
            }
        }
        /// <summary>
        /// 最大文字数
        /// </summary>
        public int MaxLength
        {
            get;
            set;
        }
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public PasswordTextBox()
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
                CustomDialog.KeyboardDialog kdlg = new CustomDialog.KeyboardDialog();
                kdlg.InputArea = Value;
                string msgfmt = CommonProc.MessageText("G007");
                msgfmt = msgfmt.Replace("%param", paramName);
                msgfmt = msgfmt.Replace("%length", MaxLength.ToString());
                kdlg.Message_Text = msgfmt;
                kdlg.DispMode = KeyboardDialog.KB_MODE.MASK;  //パスワード
                kdlg.AllLength = MaxLength;
                if (kdlg.ShowDialog() == DialogResult.OK)
                {
                    Value = kdlg.InputArea;
                    res = true;
                }
                kdlg.Dispose();
            }
            return res;
        }
    }
}
