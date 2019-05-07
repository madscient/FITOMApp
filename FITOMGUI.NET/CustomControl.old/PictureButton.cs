using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace JDC.CustomControl
{
    /// <summary>
    /// ピクチャーボタン
    /// </summary>
    public partial class PictureButton : CustomButtonBase
    {
        private Image backgroundImage;
        private Image pressedImage;
        private Image disabledImage;
        private Image selectedImage;
        private string textJapanese;
        private string toggleJapanese;

        /// <summary>
        /// コンストラクタ
        /// </summary>
        public PictureButton()
        {
            InitializeComponent();
        }
        /// <summary>
        /// 活性時の背景画像
        /// </summary>
    	override public Image BackgroundImage
		{
			get
			{
				return this.backgroundImage;
			}
			set
			{
				this.backgroundImage = value;
			}
		}
        /// <summary>
        /// 押下時の背景画像
        /// </summary>
		public Image PressedImage
		{
			get
			{
				return this.pressedImage;
			}
			set
			{
				this.pressedImage = value;
			}
		}
        /// <summary>
        /// 非活性時の背景画像
        /// </summary>
        public Image DisabledImage
        {
            get
            {
                return this.disabledImage;
            }
            set
            {
                this.disabledImage = value;
            }
        }
        /// <summary>
        /// 選択時の背景画像
        /// </summary>
        public Image SelectedImage
        {
            get
            {
                return this.selectedImage;
            }
            set
            {
                this.selectedImage = value;
            }
        }
        /// <summary>
        /// ボタンテキスト
        /// </summary>
        public override string Text
        {
            get
            {
                //return Selected ? toggleJapanese : textJapanese;
                return base.Text;
            }
            set
            {
                base.Text = value;
                if (!Selected)
                {
                    textJapanese = value;
                }
                else
                {
                    toggleJapanese = value;
                }
                OnStatusChanged();
            }
        }
        /// <summary>
        /// 初期設定（イメージ指定）
        /// </summary>
        /// <param name="imgEnabled">活性時の背景画像</param>
        /// <param name="imgPressed">押下時の背景画像</param>
        /// <param name="imgSelected">選択時の背景画像</param>
        /// <param name="imgDisabled">非活性の背景画像</param>
        /// <param name="jText">活性時の表示テキスト</param>
        /// <param name="jToggle">選択時の表示テキスト</param>
        public void Init(Image imgEnabled, Image imgPressed, Image imgSelected, Image imgDisabled, string jText, string jToggle="")
        {
            backgroundImage = imgEnabled;
            pressedImage = imgPressed;
            selectedImage = imgSelected;
            disabledImage = imgDisabled;
            textJapanese = jText;
            toggleJapanese = jToggle;
        }
        /// <summary>
        /// ボタン状態変更時のオーバーライド
        /// </summary>
        protected override void OnStatusChanged()
        {
            if (selected && toggleJapanese != null && toggleJapanese != "")
            {
                base.Text = toggleJapanese;
            }
            else
            {
                base.Text = textJapanese;
            }
            if (Enabled && pushed && pressedImage != null)
            {
                Image = pressedImage;
            }
            else
            {
                if (Enabled && Selected && selectedImage != null)
                {
                    Image = selectedImage;
                }
                else
                {
                    Image = backgroundImage;
                }
            }
            if (!Enabled && disabledImage != null)
            {
                Image = disabledImage;
            }
            base.OnStatusChanged();
        }
        /// <summary>
        /// 初期設定（ファイル名指定）
        /// </summary>
        /// <param name="enafile">活性時の背景画像（ファイル名）</param>
        /// <param name="prsfile">押下時の背景画像（ファイル名）</param>
        /// <param name="selfile">選択時の背景画像（ファイル名）</param>
        /// <param name="disfile">非活性時の背景画像（ファイル名）</param>
        /// <param name="jText">活性時の表示テキスト</param>
        /// <param name="jToggle">選択時の表示テキスト</param>
        public void Init(string enafile, string prsfile, string selfile, string disfile, string jText, string jToggle)
        {
            if (enafile != null && enafile != "")
            {
                backgroundImage = CommonProc.ImageFromFile(enafile);
            }
            else
            {
                backgroundImage = null;
            }
            if (prsfile != null && prsfile != "")
            {
                pressedImage = CommonProc.ImageFromFile(prsfile);
            }
            else
            {
                pressedImage = null;
            }
            if (selfile != null && selfile != "")
            {
                selectedImage = CommonProc.ImageFromFile(selfile);
            }
            else
            {
                selectedImage = null;
            }
            if (disfile != null && disfile != "")
            {
                disabledImage = CommonProc.ImageFromFile(disfile);
            }
            else
            {
                disabledImage = null;
            }
            textJapanese = jText;
            toggleJapanese = jToggle;
            ImageAlign = ContentAlignment.MiddleCenter;
            TextAlign = ContentAlignment.MiddleCenter;
            Image = backgroundImage;
            base.Text = textJapanese;
            //Region = CommonProc.CreateRegionFromImage((Bitmap)backgroundImage);
            OnStatusChanged();
            Invalidate();
        }
        /// <summary>
        /// 初期設定（ハッシュテーブル指定）
        /// </summary>
        /// <param name="initrec">ボタン定義を含むハッシュテーブルオブジェクト</param>
        public void Init(Hashtable initrec)
        {
            if (initrec != null)
            {
                Init((string)initrec["Enabled"], (string)initrec["Pushed"], (string)initrec["Selected"], (string)initrec["Disabled"],
                    (string)initrec["Jtext"], (string)initrec["Jtoggle"]);
            }
        }
        /// <summary>
        /// 初期設定（ボタンID指定）
        /// </summary>
        /// <param name="buttonID">ボタン定義ファイルのボタンID</param>
        public void Init(string buttonID)
        {
            Hashtable ht = null;
            try
            {
                ht = (Hashtable)CommonData.Info.tblButton[buttonID];
            }
            catch
            {//握りつぶす（デザイナー対策）
            }
            if (ht != null)
            {
                Init(ht);
            }
        }
        /// <summary>
        /// 描画オーバーライド
        /// </summary>
        /// <param name="e"></param>
        protected override void OnPaint(PaintEventArgs e)
        {
            if (Image != null)
            {
                Graphics g = e.Graphics;
                g.DrawImage(Image, DisplayRectangle);
                StringFormat fmt = CommonProc.ConvertAlignmentProperty(TextAlign);
                fmt.FormatFlags |= StringFormatFlags.MeasureTrailingSpaces;
                g.DrawString(base.Text, Font, new SolidBrush(ForeColor), DisplayRectangle, fmt);
            }
            else
            {
                base.OnPaint(e);
            }
        }
    }
}
