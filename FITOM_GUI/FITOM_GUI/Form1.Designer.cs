namespace FITOM_GUI
{
    partial class Form1
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースが破棄される場合 true、破棄されない場合は false です。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            FITOM_GUI.CustomControl.BorderWidths borderWidths1 = new FITOM_GUI.CustomControl.BorderWidths();
            FITOM_GUI.CustomControl.BorderWidths borderWidths2 = new FITOM_GUI.CustomControl.BorderWidths();
            FITOM_GUI.CustomControl.BorderWidths borderWidths3 = new FITOM_GUI.CustomControl.BorderWidths();
            FITOM_GUI.CustomControl.BorderWidths borderWidths4 = new FITOM_GUI.CustomControl.BorderWidths();
            this.colorButton2 = new FITOM_GUI.CustomControl.CustomColorButton();
            this.SuspendLayout();
            // 
            // colorButton2
            // 
            this.colorButton2.AutoToggle = false;
            this.colorButton2.BackColor = System.Drawing.Color.WhiteSmoke;
            this.colorButton2.DisabledBackColor = System.Drawing.SystemColors.ControlLight;
            this.colorButton2.DisabledBorderColor = System.Drawing.Color.Black;
            borderWidths1.Bottom = 1;
            borderWidths1.Left = 1;
            borderWidths1.Right = 1;
            borderWidths1.Top = 1;
            this.colorButton2.DisabledBorderWidth = borderWidths1;
            this.colorButton2.DisabledForeColor = System.Drawing.SystemColors.ControlText;
            this.colorButton2.EnabledBackColor = System.Drawing.SystemColors.Control;
            this.colorButton2.EnabledBorderColor = System.Drawing.Color.Maroon;
            borderWidths2.Bottom = 1;
            borderWidths2.Left = 1;
            borderWidths2.Right = 1;
            borderWidths2.Top = 1;
            this.colorButton2.EnabledBorderWidth = borderWidths2;
            this.colorButton2.EnabledForeColor = System.Drawing.SystemColors.ControlText;
            this.colorButton2.Location = new System.Drawing.Point(98, 46);
            this.colorButton2.Margin = new System.Windows.Forms.Padding(0);
            this.colorButton2.Name = "colorButton2";
            this.colorButton2.PushedBackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.colorButton2.PushedBorderColor = System.Drawing.SystemColors.ControlDarkDark;
            borderWidths3.Bottom = 0;
            borderWidths3.Left = 0;
            borderWidths3.Right = 0;
            borderWidths3.Top = 0;
            this.colorButton2.PushedBorderWidth = borderWidths3;
            this.colorButton2.PushedForeColor = System.Drawing.SystemColors.HighlightText;
            this.colorButton2.RoundFrame = false;
            this.colorButton2.Selected = false;
            this.colorButton2.SelectedBackColor = System.Drawing.SystemColors.ControlDark;
            this.colorButton2.SelectedBorderColor = System.Drawing.SystemColors.ControlDark;
            borderWidths4.Bottom = 0;
            borderWidths4.Left = 0;
            borderWidths4.Right = 0;
            borderWidths4.Top = 0;
            this.colorButton2.SelectedBorderWidth = borderWidths4;
            this.colorButton2.SelectedForeColor = System.Drawing.SystemColors.HighlightText;
            this.colorButton2.Size = new System.Drawing.Size(452, 260);
            this.colorButton2.TabIndex = 0;
            this.colorButton2.Text = "colorButton2";
            this.colorButton2.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(776, 454);
            this.Controls.Add(this.colorButton2);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);

        }

        #endregion

        private CustomControl.CustomColorButton colorButton2;
    }
}

