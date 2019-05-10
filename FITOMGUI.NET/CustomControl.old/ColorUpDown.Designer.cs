namespace JDC.CustomControl
{
    partial class ColorUpDown
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

        #region コンポーネント デザイナーで生成されたコード

        /// <summary> 
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を 
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.buttonText = new System.Windows.Forms.Button();
            this.buttonDown = new JDC.CustomControl.ColorButton();
            this.buttonUp = new JDC.CustomControl.ColorButton();
            this.SuspendLayout();
            // 
            // buttonText
            // 
            this.buttonText.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonText.BackColor = System.Drawing.SystemColors.Window;
            this.buttonText.FlatAppearance.BorderColor = System.Drawing.Color.Black;
            this.buttonText.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.buttonText.Font = new System.Drawing.Font("ＭＳ ゴシック", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.buttonText.ForeColor = System.Drawing.SystemColors.WindowText;
            this.buttonText.Location = new System.Drawing.Point(0, 0);
            this.buttonText.Margin = new System.Windows.Forms.Padding(0);
            this.buttonText.Name = "buttonText";
            this.buttonText.Size = new System.Drawing.Size(122, 43);
            this.buttonText.TabIndex = 0;
            this.buttonText.Text = "button1";
            this.buttonText.UseVisualStyleBackColor = false;
            this.buttonText.Paint += new System.Windows.Forms.PaintEventHandler(this.buttonText_Paint);
            // 
            // buttonDown
            // 
            this.buttonDown.AutoToggle = false;
            this.buttonDown.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(157)))), ((int)(((byte)(167)))));
            this.buttonDown.Dock = System.Windows.Forms.DockStyle.Right;
            this.buttonDown.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.buttonDown.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonDown.Location = new System.Drawing.Point(165, 0);
            this.buttonDown.Margin = new System.Windows.Forms.Padding(0);
            this.buttonDown.Name = "buttonDown";
            this.buttonDown.RoundFrame = false;
            this.buttonDown.Selected = false;
            this.buttonDown.Size = new System.Drawing.Size(43, 43);
            this.buttonDown.TabIndex = 1;
            this.buttonDown.Text = "▼";
            this.buttonDown.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.buttonDown.Paint += new System.Windows.Forms.PaintEventHandler(this.buttonDown_Paint);
            // 
            // buttonUp
            // 
            this.buttonUp.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonUp.AutoToggle = false;
            this.buttonUp.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(157)))), ((int)(((byte)(167)))));
            this.buttonUp.Font = new System.Drawing.Font("MS UI Gothic", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.buttonUp.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonUp.Location = new System.Drawing.Point(122, 0);
            this.buttonUp.Margin = new System.Windows.Forms.Padding(0);
            this.buttonUp.Name = "buttonUp";
            this.buttonUp.RoundFrame = false;
            this.buttonUp.Selected = false;
            this.buttonUp.Size = new System.Drawing.Size(43, 43);
            this.buttonUp.TabIndex = 2;
            this.buttonUp.Text = "▲";
            this.buttonUp.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // CustomControl_UpDown
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.buttonDown);
            this.Controls.Add(this.buttonUp);
            this.Controls.Add(this.buttonText);
            this.Name = "CustomControl_UpDown";
            this.Size = new System.Drawing.Size(208, 43);
            this.EnabledChanged += new System.EventHandler(this.CustomControl_UpDown_EnabledChanged);
            this.SizeChanged += new System.EventHandler(this.CustomControl_UpDown_SizeChanged);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.CustomControl_UpDown_Paint);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button buttonText;
        private JDC.CustomControl.ColorButton buttonUp;
        private JDC.CustomControl.ColorButton buttonDown;
    }
}
