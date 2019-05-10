using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using JDC.CustomDialog;

namespace JDC.CustomControl
{
    /// <summary>
    /// 文字列用入力ボックス
    /// </summary>
    public class TextInputArea : InputAreaBase
    {
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public TextInputArea()
        {
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
        /// 日本語を許可する
        /// </summary>
        public bool AllowJapanese
        {
            get;
            set;
        }
        /// <summary>
        /// 入力ダイアログ
        /// </summary>
        /// <returns></returns>
        protected override bool DoInput()
        {
            bool ret = false;
            string msgfmt = CommonProc.MessageText("G003");
            msgfmt = msgfmt.Replace("%param", ParamName);
            msgfmt = msgfmt.Replace("%length", MaxLength.ToString());
            if (AllowJapanese)
            {
                IMEInputTextDialog tdlg = new IMEInputTextDialog();
                tdlg.InputText = frontend.Text;
                tdlg.Caption = msgfmt;
                tdlg.MaxLength = MaxLength;
                if (tdlg.ShowDialog() == DialogResult.OK)
                {
                    frontend.Text = tdlg.InputText;
                    ret = true;
                }
                tdlg.Dispose();
            }
            else
            {
                KeyboardDialog kdlg = new KeyboardDialog();
                kdlg.InputArea = frontend.Text;
                kdlg.Message_Text = msgfmt;
                kdlg.DispMode = 0;  //テキスト
                if (kdlg.ShowDialog() == DialogResult.OK)
                {
                    frontend.Text = kdlg.InputArea;
                    ret = true;
                }
                kdlg.Dispose();
            }
            return ret;
        }
    }
}
