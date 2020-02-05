namespace FITOM_GUI.CustomControl
{
    partial class CustomInputBox
    {
        /// <summary> 
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージド リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region コンポーネント デザイナーで生成されたコード

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
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.customFastLabel1 = new FITOM_GUI.CustomControl.CustomFastLabel();
            this.customColorButton1 = new FITOM_GUI.CustomControl.CustomColorButton();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 1;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle());
            this.tableLayoutPanel1.Controls.Add(this.customFastLabel1, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.customColorButton1, 0, 1);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 40F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.tableLayoutPanel1.Size = new System.Drawing.Size(108, 100);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // customFastLabel1
            // 
            this.customFastLabel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.customFastLabel1.Font = new System.Drawing.Font("Meiryo UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.customFastLabel1.Location = new System.Drawing.Point(0, 0);
            this.customFastLabel1.Margin = new System.Windows.Forms.Padding(0);
            this.customFastLabel1.Name = "customFastLabel1";
            this.customFastLabel1.Size = new System.Drawing.Size(108, 40);
            this.customFastLabel1.TabIndex = 0;
            this.customFastLabel1.Text = "ParamName";
            this.customFastLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // customColorButton1
            // 
            this.customColorButton1.AutoToggle = false;
            this.customColorButton1.DisabledBackColor = System.Drawing.SystemColors.ControlLight;
            this.customColorButton1.DisabledBorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(118)))), ((int)(((byte)(123)))), ((int)(((byte)(127)))));
            borderWidths1.Bottom = 2;
            borderWidths1.Left = 2;
            borderWidths1.Right = 2;
            borderWidths1.Top = 2;
            this.customColorButton1.DisabledBorderWidth = borderWidths1;
            this.customColorButton1.DisabledForeColor = System.Drawing.SystemColors.GrayText;
            this.customColorButton1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.customColorButton1.EnabledBackColor = System.Drawing.SystemColors.Control;
            this.customColorButton1.EnabledBorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            borderWidths2.Bottom = 2;
            borderWidths2.Left = 2;
            borderWidths2.Right = 2;
            borderWidths2.Top = 2;
            this.customColorButton1.EnabledBorderWidth = borderWidths2;
            this.customColorButton1.EnabledForeColor = System.Drawing.SystemColors.ControlText;
            this.customColorButton1.Font = new System.Drawing.Font("メイリオ", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.customColorButton1.Location = new System.Drawing.Point(0, 40);
            this.customColorButton1.Margin = new System.Windows.Forms.Padding(0);
            this.customColorButton1.Name = "customColorButton1";
            this.customColorButton1.PushedBackColor = System.Drawing.SystemColors.ControlDarkDark;
            this.customColorButton1.PushedBorderColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            borderWidths3.Bottom = 2;
            borderWidths3.Left = 2;
            borderWidths3.Right = 2;
            borderWidths3.Top = 2;
            this.customColorButton1.PushedBorderWidth = borderWidths3;
            this.customColorButton1.PushedForeColor = System.Drawing.SystemColors.HighlightText;
            this.customColorButton1.RoundFrame = false;
            this.customColorButton1.Selected = false;
            this.customColorButton1.SelectedBackColor = System.Drawing.SystemColors.ControlDark;
            this.customColorButton1.SelectedBorderColor = System.Drawing.SystemColors.ControlDark;
            borderWidths4.Bottom = 2;
            borderWidths4.Left = 2;
            borderWidths4.Right = 2;
            borderWidths4.Top = 2;
            this.customColorButton1.SelectedBorderWidth = borderWidths4;
            this.customColorButton1.SelectedForeColor = System.Drawing.SystemColors.HighlightText;
            this.customColorButton1.Size = new System.Drawing.Size(108, 60);
            this.customColorButton1.TabIndex = 1;
            this.customColorButton1.Text = "000";
            this.customColorButton1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // CustomInputBox
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.tableLayoutPanel1);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "CustomInputBox";
            this.Size = new System.Drawing.Size(108, 100);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private CustomFastLabel customFastLabel1;
        private CustomColorButton customColorButton1;
    }
}
