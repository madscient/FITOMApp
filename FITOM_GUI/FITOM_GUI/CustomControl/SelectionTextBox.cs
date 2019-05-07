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
    /// 択一式文字列用テキストボックス
    /// </summary>
    public partial class SelectionTextBox : CustomTextBoxBase
    {
        /// <summary>
        /// 選択肢（選択肢の数によって自動的に最適なダイアログが使われる）
        /// </summary>
        public string[] Items
        {
            get;
            set;
        }
        /// <summary>
        /// 選択肢の一覧
        /// </summary>
        public bool[] ItemValids
        {
            get;
            set;
        }
        /// <summary>
        /// 現在のテキストが選択肢の何番目かを返す（無ければ-1）
        /// </summary>
        public int ItemIndex
        {
            get
            {
                if (Items != null)
                {
                    return Array.IndexOf<string>(Items, Text);
                }
                return -1;
            }
        }
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public SelectionTextBox()
        {
            InitializeComponent();
        }
        /// <summary>
        /// マウスクリックイベントオーバーライド
        /// </summary>
        protected override bool InputValue()
        {
            bool res = false;
            if (!ReadOnly)
            {
                if (Items.Length == 2)
                {//トグルモード
                    Text = Items[(ItemIndex + 1) & 1];
                    res = true;
                }
                else
                {
                    CustomDialog.SelectItemDialog sdlg = null;
                    if (Items.Length <= 3)
                    {
                        sdlg = new CustomDialog.SelectItem3Dialog();
                    }
                    else if (3 < Items.Length && Items.Length <= 5)
                    {//4～5項目
                        int maxlen = 0;
                        foreach (string item in Items)
                        {
                            maxlen = Math.Max(maxlen, item.Length);
                        }
                        if (Items.Length == 4 && maxlen < 5)
                        {
                            sdlg = new CustomDialog.SelectItem4Dialog();
                        }
                        else
                        {
                            sdlg = new CustomDialog.SelectItem5Dialog();
                        }
                    }
                    else if (Items.Length <= 10)
                    {//6～10項目
                        sdlg = new CustomDialog.SelectItem10Dialog();
                    }
                    if (sdlg != null)
                    {
                        sdlg.Items = Items;
                        sdlg.Select(Text);
                        string msgfmt = CommonProc.MessageText("G005");
                        msgfmt = msgfmt.Replace("%param", paramName);
                        sdlg.Caption = msgfmt;
                        if (ItemValids != null)
                        {
                            for (int i = 0; i < ItemValids.Length; i++)
                            {
                                sdlg.ButtonEnable(i, ItemValids[i]);
                            }
                        }
                        if (sdlg.ShowDialog() == DialogResult.OK)
                        {
                            Text = sdlg.SelectedText;
                            res = true;
                        }
                        sdlg.Dispose();
                    }
                }
            }
            return res;
        }
    }
}
