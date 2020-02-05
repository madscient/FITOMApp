using System;
using System.Collections.Generic;
using System.Linq;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

namespace FITOM_GUI.CustomControl
{
    /// <summary>
    /// タブ制御クラス
    /// タブボタンと対応するページを登録すると、自動的にタブボタンとパネルの表示制御をする
    /// </summary>
    partial class CustomTabBase : Component
    {
        private int selected = -1;
        [Category("Status")]
        /// <summary>
        /// 現在選択されているタブページ番号（ゼロ始まり）
        /// </summary>
        public int SelectedTab
        {
            get => selected;
        }
        [Category("Status")]
        public UserControl SelectedPanel
        {
            get
            {
                if (selected >= 0 && tabPanelList.Count > selected)
                {
                    return tabPanelList[selected][0];
                }
                return null;
            }
        }

        /// <summary>
        /// タブが変更されたことを通知するイベントハンドラ
        /// </summary>
        public event EventHandler TabChanged;
        /// <summary>
        /// こんすとらくた
        /// </summary>
        public CustomTabBase()
        {
        }
        private List<CustomControl.CustomButtonBase> tabButtons = new List<CustomControl.CustomButtonBase>();
        private List<List<UserControl>> tabPanelList = new List<List<UserControl>>();
        //private List<UserControl> tabPanels = new List<UserControl>();
        /// <summary>
        /// タブテーブルにタブボタンとパネルの組み合わせを登録
        /// </summary>
        /// <param name="button">ボタンオブジェクト</param>
        /// <param name="panel">パネルオブジェクト</param>
        public int AddPanel(CustomControl.CustomButtonBase button, UserControl panel)
        {
            if (button != null)
            {
                button.Click += new EventHandler(OnTabButtonClick);
            }
            List<UserControl> tabPanels = new List<UserControl>();
            tabPanels.Add(panel);
            tabPanelList.Add(tabPanels);
            tabButtons.Add(button);
            return tabButtons.Count - 1;
        }
        /// <summary>
        /// タブテーブルにタブボタンとパネルの組み合わせをまとめて登録
        /// </summary>
        /// <param name="buttons"></param>
        /// <param name="panels"></param>
        public void AddPanels(CustomButtonBase[] buttons, UserControl[] panels)
        {
            int count = 0;
            if (buttons != null)
            {
                count = buttons.Length;
            }
            if (panels != null)
            {
                count = Math.Max(count, panels.Length);
            }
            for (int i = 0; i < count; i++)
            {
                AddPanel((buttons != null) ? buttons[i] : null, (panels != null) ? panels[i] : null);
            }
        }
        /// <summary>
        /// すでに登録されているタブボタンにサブページを追加する（タブボタン指定）
        /// （一つのタブボタンの中でページ制御をする場合に使用する）
        /// （サブページの切替はフォーム側のプログラムで行う。）
        /// （タブボタンによる画面遷移では、サブページがあっても必ず最初のページを表示）
        /// </summary>
        /// <param name="button">すでに登録されているタブボタンオブジェクト</param>
        /// <param name="panel">サブページとして登録するパネルオブジェクト</param>
        /// <returns>実際に登録されたサブページ番号（エラー時は-1）</returns>
        public int AddSubPanel(CustomButtonBase button, UserControl panel)
        {
            return AddSubPanel(tabButtons.IndexOf(button), panel);
        }
        /// <summary>
        /// すでに登録されているタブボタンにサブページを追加する（インデックス指定）
        /// （一つのタブボタンの中でページ制御をする場合に使用する）
        /// （サブページの切替はフォーム側のプログラムで行う。）
        /// （タブボタンによる画面遷移では、サブページがあっても必ず最初のページを表示）
        /// </summary>
        /// <param name="index">サブページを登録するタブ番号</param>
        /// <param name="panel">サブページとして登録するパネルオブジェクト</param>
        /// <returns>実際に登録されたサブページ番号（エラー時は-1）</returns>
        public int AddSubPanel(int index, UserControl panel)
        {
            if (0 <= index && index < tabPanelList.Count)
            {
                if (tabPanelList[index] == null)
                {
                    tabPanelList[index] = new List<UserControl>();
                }
                tabPanelList[index].Add(panel);
                return tabPanelList[index].IndexOf(panel);
            }
            return -1;
        }
        /// <summary>
        /// タブテーブルをクリア
        /// </summary>
        public void Clear()
        {
            foreach (List<UserControl> tabPanels in tabPanelList)
            {
                tabPanels.Clear();
            }
            tabPanelList.Clear();
            tabButtons.Clear();
        }
        protected virtual void OnTabButtonClick(object sender, EventArgs e)
        {
            try
            {
                if (sender != null)
                {
                    CustomButtonBase button = (CustomButtonBase)sender;
                    int index = tabButtons.IndexOf(button);
                    if (0 <= index && index < tabPanelList.Count && !button.Selected)
                    {
                        SelectPanel(index);
                        if (TabChanged != null && selected >= 0)
                        {
                            TabChanged(sender, e);
                        }
                    }
                }
            }
            catch(Exception ex)
            {
                //const string THIS_PROC = "CustomTabBase.OnTabButtonClick";

                //ログ出力
                //LogProc.WriteLog(LogProc.LOG_KIND_DEBUG, THIS_PROC, ex);
            }
        }
        /// <summary>
        /// パネルを選択する
        /// </summary>
        /// <param name="index">選択するパネルのインデックス</param>
        /// <returns>実際に選択されたパネルのインデックス</returns>
        public int SelectPanel(int index)
        {
            if (index < tabButtons.Count)
            {
                selected = index;
                if (tabPanelList[index] != null && tabPanelList[index].Count > 0 && tabPanelList[index][0] != null)
                {
                    SelectSubPanel(0);
                }
                for (int i = 0; i < tabButtons.Count; i++)
                {
                    if (tabButtons[i] != null)
                    {
                        if (i == index)
                        {
                            tabButtons[i].Selected = true;
                            tabButtons[i].BringToFront();
                        }
                        else
                        {
                            tabButtons[i].Selected = false;
                        }
                    }
                }
            }
            return selected;
        }
        /// <summary>
        /// サブパネルを切り替える
        /// </summary>
        /// <param name="index">サブページ番号（ゼロ始まり）</param>
        public int SelectSubPanel(int index)
        {
            if (selected >= 0 && tabPanelList.Count > 0 && tabPanelList[selected] != null && 0 <= index && index < tabPanelList[selected].Count)
            {
                for (int i = 0; i < tabPanelList[selected].Count; i++)
                {
                    if (i == index)
                    {
                        tabPanelList[selected][i].Visible = true;
                        tabPanelList[selected][i].BringToFront();
                        tabPanelList[selected][i].Select();
                        tabPanelList[selected][i].Focus();
                    }
                    else
                    {
                        tabPanelList[selected][i].Visible = false;
                    }
                }
                if (tabButtons[selected] != null)
                {
                    tabButtons[selected].Selected = true;
                    tabButtons[selected].BringToFront();
                }
            }
            return -1;
        }
        /// <summary>
        /// 現在選択されているタブ以外をロックする
        /// </summary>
        public void Lock()
        {
            foreach (CustomButtonBase button in tabButtons)
            {
                if (button != null && !button.Selected)
                {
                    button.Enabled = false;
                }
            }
        }
        /// <summary>
        /// タブのロックを解除する
        /// </summary>
        public void Unlock()
        {
            foreach (CustomButtonBase button in tabButtons)
            {
                if (button != null)
                {
                    button.Enabled = true;
                }
            }
        }
    }
}
