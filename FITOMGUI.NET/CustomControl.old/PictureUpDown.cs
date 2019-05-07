using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace JDC.CustomControl
{
    /// <summary>
    /// アップダウン付き数値テキストボックス（イメージファイル版）
    /// サイズのバリエーションは170x36, 150x53, 180x53, 230x53, 280x53の５種類
    /// </summary>
    public partial class PictureUpDown : CustomUpDownBase
    {
        private int hsize = 0;
        private int vsize = 0;
        private List<Image> imageList = new List<Image>();
        /// <summary>
        /// 横サイズ(縦サイズ=0のときは無効。常に170pxとなる）
        /// 0:150 / 1:180 2:230 3:280
        /// </summary>
        public int HSizeMode
        {
            get
            {
                return hsize;
            }
            set
            {
                hsize = value;
                refreshSize(null, null);
            }
        }
        /// <summary>
        /// 縦サイズ
        /// 0: 36 / 1:53
        /// </summary>
        public int VSizeMode
        {
            get
            {
                return vsize;
            }
            set
            {
                vsize = value;
                refreshSize(null, null);
            }
        }
        /// <summary>
        /// 数値表示部のテキストアライン
        /// </summary>
        public ContentAlignment TextAlign
        {
            get
            {
                return buttonText.TextAlign;
            }
            set
            {
                buttonText.TextAlign = value;
            }
        }
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public PictureUpDown()
        {
            InitializeComponent();
            Init(buttonText, buttonUp, buttonDown);
            imageList.Add(CommonProc.ImageFromFile(ConstDef.IMG_UPDOWN_INPUT_170E));
            imageList.Add(CommonProc.ImageFromFile(ConstDef.IMG_UPDOWN_INPUT_150E));
            imageList.Add(CommonProc.ImageFromFile(ConstDef.IMG_UPDOWN_INPUT_180E));
            imageList.Add(CommonProc.ImageFromFile(ConstDef.IMG_UPDOWN_INPUT_230E));
            imageList.Add(CommonProc.ImageFromFile(ConstDef.IMG_UPDOWN_INPUT_280E));
            imageList.Add(CommonProc.ImageFromFile(ConstDef.IMG_UPDOWN_INPUT_170D));
            imageList.Add(CommonProc.ImageFromFile(ConstDef.IMG_UPDOWN_INPUT_150D));
            imageList.Add(CommonProc.ImageFromFile(ConstDef.IMG_UPDOWN_INPUT_180D));
            imageList.Add(CommonProc.ImageFromFile(ConstDef.IMG_UPDOWN_INPUT_230D));
            imageList.Add(CommonProc.ImageFromFile(ConstDef.IMG_UPDOWN_INPUT_280D));
        }

        private void CustomControl_UpDown_Load(object sender, EventArgs e)
        {
            refreshSize(null, null);
        }
        /// <summary>
        /// Enable状態変更時イベント
        /// </summary>
        /// <param name="e"></param>
        protected override void OnEnabledChanged(EventArgs e)
        {
            base.OnEnabledChanged(e);
            refreshInput();
        }
        private void refreshInput()
        {
            int textimage = -1;
            if (vsize == 0)
            {
                textimage = 0;
            }
            else
            {
                textimage = hsize + 1;
            }
            textimage += (Enabled ? 0 : 5);
            if (imageList != null && imageList.Count > textimage && imageList[textimage] != null)
            {
                buttonText.Image = imageList[textimage];
                //buttonText.Size = imageList[textimage].Size;
            }
        }
        private void refreshSize(object sender, EventArgs e)
        {
            int height = 0;
            int width = 0;
            switch (vsize)
            {
                case 0: //36x36
                    buttonDown.Init("UD00");
                    buttonUp.Init("UD01");
                    height = 36;
                    width = 170;
                    break;
                case 1: //53x53
                    buttonDown.Init("UD02");
                    buttonUp.Init("UD03");
                    height = 53;
                    break;
            }
            if (width == 0)
            {
                switch (hsize)
                {
                    case 0: //150
                        width = 150;
                        break;
                    case 1: //180
                        width = 180;
                        break;
                    case 2: //230
                        width = 230;
                        break;
                    case 3: //280
                        width = 280;
                        break;
                }
            }
            this.Size = new Size(width, height);
            buttonText.Size = new Size(width - (height * 2), height);
            buttonUp.Size = new Size(height, height);
            buttonDown.Size = new Size(height, height);
            buttonDown.Location = new Point(width - height, 0);
            buttonUp.Location = new Point(width - (height * 2), 0);
            refreshInput();
        }
    }
}
