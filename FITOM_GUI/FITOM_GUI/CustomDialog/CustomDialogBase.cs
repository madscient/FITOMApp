using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using FITOM_GUI.CustomControl;

namespace FITOM_GUI.CustomDialog
{
    /// <summary>
    /// カスタムダイアログのベースクラス
    /// これを直接使用することはないはず。
    /// 
    /// </summary>
    public partial class CustomDialogBase : CustomFastForm
    {
        /// <summary>
        /// ダイアログタイトル
        /// </summary>
        public string Caption => labelCaption.Text;
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public CustomDialogBase()
        {
            InitializeComponent();
        }
        /// <summary>
        /// ボタンモード
        /// Q:はい/いいえ
        /// それ以外:OK/キャンセル
        /// </summary>
        public string ButtonMode
        {
            get
            {
                return mode;
            }
            set
            {
                mode = value;
            }
        }
        private string mode = "";
        private void CustomDialogBase_Load(object sender, EventArgs e)
        {
            //背景色
            BackColor = SystemColors.Window;
            //BackgroundImage = CommonProc.ImageFromFile(ConstDef.IMG_PANEL_DIALOG_A_BLANK);
            //pictureBox_OK.Init((mode == "Q") ? "MB02" : "MB00");
            //pictureBox_CANCEL.Init((mode == "Q") ? "MB03" : "MB01");
        }

        /// <summary>
        /// OK（またはYES）ボタン押下
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void pictureBox_OK_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }
        /// <summary>
        /// CANCEL（またはNO）ボタン押下
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void pictureBox_CANCEL_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }
    }
}
