namespace FITOM_GUI.CustomControl
{
    partial class InputBoxBase
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
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.paramTitle = new FITOM_GUI.CustomControl.CustomFastLabel();
            this.colorButton1 = new FITOM_GUI.CustomControl.ColorButton();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.CellBorderStyle = System.Windows.Forms.TableLayoutPanelCellBorderStyle.Single;
            this.tableLayoutPanel1.ColumnCount = 1;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Controls.Add(this.paramTitle, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.colorButton1, 0, 1);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Margin = new System.Windows.Forms.Padding(0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 30F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 70F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(150, 150);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // paramTitle
            // 
            this.paramTitle.AutoSize = true;
            this.paramTitle.Dock = System.Windows.Forms.DockStyle.Fill;
            this.paramTitle.Font = new System.Drawing.Font("Meiryo UI", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.paramTitle.Location = new System.Drawing.Point(4, 1);
            this.paramTitle.Name = "paramTitle";
            this.paramTitle.Size = new System.Drawing.Size(142, 44);
            this.paramTitle.TabIndex = 0;
            this.paramTitle.Text = "Title";
            this.paramTitle.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // colorButton1
            // 
            this.colorButton1.AutoToggle = false;
            this.colorButton1.BackColor = System.Drawing.Color.WhiteSmoke;
            this.colorButton1.DisabledBorderWidth = 1F;
            this.colorButton1.DisabledForeColor = System.Drawing.Color.Gray;
            this.colorButton1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.colorButton1.EnabledBorderWidth = 1F;
            this.colorButton1.EnabledForeColor = System.Drawing.Color.White;
            this.colorButton1.Font = new System.Drawing.Font("Meiryo UI", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.colorButton1.ForeColor = System.Drawing.Color.White;
            this.colorButton1.Location = new System.Drawing.Point(1, 46);
            this.colorButton1.Margin = new System.Windows.Forms.Padding(0);
            this.colorButton1.Name = "colorButton1";
            this.colorButton1.PushedForeColor = System.Drawing.Color.Black;
            this.colorButton1.RoundFrame = false;
            this.colorButton1.Selected = false;
            this.colorButton1.SelectedForeColor = System.Drawing.Color.LightGray;
            this.colorButton1.Size = new System.Drawing.Size(148, 103);
            this.colorButton1.TabIndex = 1;
            this.colorButton1.Text = "colorButton1";
            this.colorButton1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // InputBoxBase
            // 
            this.Controls.Add(this.tableLayoutPanel1);
            this.Name = "InputBoxBase";
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private CustomFastLabel paramTitle;
        private ColorButton colorButton1;
    }
}
