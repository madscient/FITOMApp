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
    /// 文字列用カスタムテキストボックス
    /// </summary>
    public partial class StringTextBox : CustomControl.CustomTextBoxBase
    {
        /// <summary>
        /// 最大文字数
        /// </summary>
        public int MaxLength
        {
            get;
            set;
        }
        /// <summary>
        /// 日本語を許可する
        /// </summary>
        public bool AllowJapanese
        {
            get;
            set;
        }
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public StringTextBox()
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
                string msgfmt = CommonProc.MessageText("G003");
                msgfmt = msgfmt.Replace("%param", paramName);
                msgfmt = msgfmt.Replace("%length", MaxLength.ToString());
                if (AllowJapanese)
                {
                    CustomDialog.IMEInputTextDialog tdlg = new CustomDialog.IMEInputTextDialog();
                    tdlg.InputText = Text;
                    tdlg.Caption = msgfmt;
                    tdlg.MaxLength = MaxLength;
                    if (tdlg.ShowDialog() == DialogResult.OK)
                    {
                        Text = tdlg.InputText;
                        res = true;
                    }
                    tdlg.Dispose();
                }
                else
                {
                    CustomDialog.KeyboardDialog kdlg = new CustomDialog.KeyboardDialog();
                    kdlg.InputArea = Text;
                    kdlg.Message_Text = msgfmt;
                    kdlg.DispMode = 0;  //テキスト
                    kdlg.AllLength = MaxLength;
                    if (kdlg.ShowDialog() == DialogResult.OK)
                    {
                        Text = kdlg.InputArea;
                        res = true;
                    }
                    kdlg.Dispose();
                }
            }
            return res;
        }
    }
}
