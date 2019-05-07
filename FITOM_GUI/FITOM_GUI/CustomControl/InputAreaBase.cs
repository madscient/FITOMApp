using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;

namespace FITOM_GUI.CustomControl
{
    /// <summary>
    /// 入力ボックス制御クラス（任意のGUIパーツの上に入出力制御を乗せる）
    /// </summary>
    public class InputAreaBase
    {
        private bool readOnly = false;
        private string paramName = "";
        private List<Image> imageList = new List<Image>();
        /// <summary>
        /// GUIパーツ本体
        /// </summary>
        protected Control frontend = null;
        /// <summary>
        /// 入力値変更イベント
        /// </summary>
        public event EventHandler ValueChanged;
        /// <summary>
        /// 書式文字列
        /// </summary>
        protected string strForm = "";
        
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public InputAreaBase()
        {
        }
        /// <summary>
        /// 初期化
        /// </summary>
        /// <param name="ui"></param>
        public void Init(Control ui)
        {
            frontend = ui;
            ui.Click += new EventHandler(OnClick);
        }
        /// <summary>
        /// 書式文字列
        /// </summary>
        public string Formula
        {
            get => strForm;
            set
            {
                strForm = value;
                refreshText();
            }
        }
        /// <summary>
        /// 項目名（入力ダイアログに表示）
        /// </summary>
        public string ParamName => paramName;
        /// <summary>
        /// 読み取り専用フラグ
        /// </summary>
        public bool ReadOnly => readOnly;
        /// <summary>
        /// テキスト更新
        /// </summary>
        protected virtual void refreshText()
        {
            //中身は派生クラスで実装する
        }
        /// <summary>
        /// 入力実行
        /// </summary>
        /// <returns></returns>
        protected virtual bool DoInput()
        {
            //中身は派生クラスで実装する
            return false;
        }
        /// <summary>
        /// マウスクリックイベント
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        protected virtual void OnClick(object sender, EventArgs e)
        {
            if (!readOnly && DoInput())
            {
                if (ValueChanged != null)
                {
                    ValueChanged(this, new EventArgs());
                }
            }
        }
    }
}
