using System;
using System.Collections;
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
    /// ラジオボタン制御クラス
    /// フォームに配置したボタンをこのクラスに登録しておくと、自動的に択一制御をする
    /// </summary>
    public partial class CustomRadioButton : Component
    {
        /// <summary>
        /// コンストラクタ
        /// </summary>
        public CustomRadioButton()
        {
        }
        /// <summary>
        /// 値が変わったことを通知するイベントハンドラ
        /// </summary>
        public EventHandler ValueChanged;
        private List<CustomButtonBase> buttons = new List<CustomButtonBase>();
        private int selected = 0;
        /// <summary>
        /// 選択されているアイテムのインデックス
        /// </summary>
        public int Selected
        {
            get
            {
                return selected;
            }
            set
            {
                selected = value;
                refreshSelection();
            }
        }
        /// <summary>
        /// 選択状態の再描画
        /// </summary>
        private void refreshSelection()
        {
            if (buttons != null && 0 <= selected && selected < buttons.Count)
            {
                foreach (CustomControl.CustomButtonBase button in buttons)
                {
                    button.Selected = false;
                }
                buttons[selected].Selected = true;
            }
        }
        /// <summary>
        /// ラジオボタンクリックイベント
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void selectionMouseClick(object sender, EventArgs e)
        {
            Control button = (Control)sender;
            int index = 0;
            if (int.TryParse((string)button.Tag, out index))
            {
                Selected = index;
                if (ValueChanged != null)
                {
                    ValueChanged(buttons[selected], new EventArgs());
                }
            }
        }
        /// <summary>
        /// ラジオボタン追加
        /// </summary>
        /// <param name="button"></param>
        public void AddButton(CustomButtonBase button)
        {
            button.Tag = buttons.Count.ToString();
            button.Click += new EventHandler(selectionMouseClick);
            buttons.Add(button);
        }
        /// <summary>
        /// ラジオボタンをロックする
        /// </summary>
        public void Lock()
        {
            if (buttons != null && 0 <= selected && selected < buttons.Count)
            {
                foreach (CustomControl.CustomButtonBase button in buttons)
                {
                    if (!button.Selected)
                    {//非選択ボタンだけ非活性化
                        button.Enabled = false;
                    }
                }
            }
        }
        /// <summary>
        /// ロックを解除する
        /// </summary>
        public void Unlock()
        {
            if (buttons != null && 0 <= selected && selected < buttons.Count)
            {
                foreach (CustomControl.CustomButtonBase button in buttons)
                {
                    button.Enabled = true;
                }
            }
        }
    }
}
