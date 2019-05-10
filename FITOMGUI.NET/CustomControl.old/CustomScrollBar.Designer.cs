namespace JDC.CustomControl
{
    partial class CustomScrollBar
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
            this.trackBox = new System.Windows.Forms.PictureBox();
            this.colorButton_Down = new JDC.CustomControl.ColorButton();
            this.colorButton_Up = new JDC.CustomControl.ColorButton();
            ((System.ComponentModel.ISupportInitialize)(this.trackBox)).BeginInit();
            this.SuspendLayout();
            // 
            // trackBox
            // 
            this.trackBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.trackBox.Location = new System.Drawing.Point(0, 41);
            this.trackBox.Margin = new System.Windows.Forms.Padding(0);
            this.trackBox.Name = "trackBox";
            this.trackBox.Size = new System.Drawing.Size(56, 119);
            this.trackBox.TabIndex = 2;
            this.trackBox.TabStop = false;
            this.trackBox.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox1_Paint);
            this.trackBox.MouseDown += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseDown);
            this.trackBox.MouseMove += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseMove);
            this.trackBox.MouseUp += new System.Windows.Forms.MouseEventHandler(this.pictureBox1_MouseUp);
            // 
            // colorButton_Down
            // 
            this.colorButton_Down.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.colorButton_Down.AutoToggle = false;
            this.colorButton_Down.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(157)))), ((int)(((byte)(167)))));
            this.colorButton_Down.DisabledForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(176)))), ((int)(((byte)(204)))), ((int)(((byte)(207)))));
            this.colorButton_Down.EnabledForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.colorButton_Down.Font = new System.Drawing.Font("メイリオ", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.colorButton_Down.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.colorButton_Down.Location = new System.Drawing.Point(0, 160);
            this.colorButton_Down.Margin = new System.Windows.Forms.Padding(0);
            this.colorButton_Down.Name = "colorButton_Down";
            this.colorButton_Down.PushedForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.colorButton_Down.RoundFrame = false;
            this.colorButton_Down.Selected = false;
            this.colorButton_Down.SelectedForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.colorButton_Down.Size = new System.Drawing.Size(56, 41);
            this.colorButton_Down.TabIndex = 1;
            this.colorButton_Down.Text = "▼";
            this.colorButton_Down.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.colorButton_Down.MouseUp += new System.Windows.Forms.MouseEventHandler(this.colorButton_Down_MouseClick);
            // 
            // colorButton_Up
            // 
            this.colorButton_Up.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.colorButton_Up.AutoToggle = false;
            this.colorButton_Up.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(60)))), ((int)(((byte)(157)))), ((int)(((byte)(167)))));
            this.colorButton_Up.DisabledForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(176)))), ((int)(((byte)(204)))), ((int)(((byte)(207)))));
            this.colorButton_Up.EnabledForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.colorButton_Up.Font = new System.Drawing.Font("メイリオ", 18F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.colorButton_Up.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.colorButton_Up.Location = new System.Drawing.Point(0, 0);
            this.colorButton_Up.Margin = new System.Windows.Forms.Padding(0);
            this.colorButton_Up.Name = "colorButton_Up";
            this.colorButton_Up.PushedForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.colorButton_Up.RoundFrame = false;
            this.colorButton_Up.Selected = false;
            this.colorButton_Up.SelectedForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.colorButton_Up.Size = new System.Drawing.Size(56, 41);
            this.colorButton_Up.TabIndex = 1;
            this.colorButton_Up.Text = "▲";
            this.colorButton_Up.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.colorButton_Up.MouseUp += new System.Windows.Forms.MouseEventHandler(this.colorButton_Up_MouseClick);
            // 
            // CustomScrollBar
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.trackBox);
            this.Controls.Add(this.colorButton_Down);
            this.Controls.Add(this.colorButton_Up);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "CustomScrollBar";
            this.Size = new System.Drawing.Size(56, 202);
            this.Load += new System.EventHandler(this.CustomScrollBar_Load);
            ((System.ComponentModel.ISupportInitialize)(this.trackBox)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private ColorButton colorButton_Up;
        private ColorButton colorButton_Down;
        private System.Windows.Forms.PictureBox trackBox;
    }
}
